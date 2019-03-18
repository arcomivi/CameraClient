#include "Application.h"
#include <QCoreApplication>
#include <QImage>
#include <QQmlContext>
Application::Application(QObject *parent) : QObject(parent)
    , m_frameProvider(Q_NULLPTR)
    , m_imageProvider(Q_NULLPTR)
    , m_webSocket(Q_NULLPTR)
    , m_pingTimer(Q_NULLPTR) {

}

void Application::run() {

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    this, [url](QObject * obj, const QUrl & objUrl) {
        if (!obj && url == objUrl)
        { QCoreApplication::exit(-1); }
    }, Qt::QueuedConnection);
    //    engine.rootContext()->setContextProperty("liveImageProvider", liveImageProvider.data());
    m_imageProvider = new LiveImageProvider();
    engine.addImageProvider("live", m_imageProvider);
    engine.rootContext()->setContextProperty("$application", this);

    m_frameProvider = new FrameProvider(this);
    m_frameProvider->setFormat(200, 200);

    engine.rootContext()->setContextProperty("$provider", m_frameProvider);
    engine.load(url);
    m_pingTimer = new QTimer(this);
    m_pingTimer->setInterval(3000);
    qRegisterMetaType<QAbstractSocket::SocketState>();
    if(!m_webSocket)    {
        m_webSocket = new QWebSocket();
        connect(m_webSocket, &QWebSocket::connected, this, &Application::clientConnected);
        connect(m_webSocket, &QWebSocket::disconnected, this, &Application::clientClosed);
        connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &Application::clientReceivedBinary);
        connect(m_webSocket, &QWebSocket::textMessageReceived, this, &Application::clientReceivedText);
        connect(m_webSocket, &QWebSocket::pong, this, &Application::pongReceived);

        connect(m_pingTimer, &QTimer::timeout, this, &Application::pingServer);
        connect(m_webSocket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &Application::onError);
        m_webSocket->setParent(this);
    }
    connect(this, &Application::imageChanged, m_imageProvider, &LiveImageProvider::updateImage);
    m_webSocket->open(QUrl("ws://127.0.0.1:8080"));
}



void Application::clientReceivedBinary(const QByteArray &message) {
    qDebug() << __FUNCTION__;
    QImage m_lastFrame = QImage(640, 480, QImage::Format_RGB32);
    QByteArray foo(message);
    QDataStream ds(&foo, QIODevice::ReadWrite);
    ds.readRawData((char *)m_lastFrame.bits(), message.size());
    //    //empty image
    //    QImage img(100, 100, QImage::Format_RGB32);
    //    m_image.set
    //    //load pixel data into empty image from QByteArray
    m_image = QImage::fromData(reinterpret_cast<const uchar *>(message.data()), message.size());
    //    //m_frameProvider->onNewVideoContentReceived()

    emit imageChanged(m_lastFrame);
    QVideoFrame frame(m_lastFrame);
    m_frameProvider->onNewVideoContentReceived(frame);
}

void Application::clientReceivedText(const QString &message) {
    qDebug() << __FUNCTION__ << message;
}

void Application::pingServer() {
    m_webSocket->ping();
}

void Application::pongReceived() {
    qDebug() << __FUNCTION__;
}

void Application::clientConnected() {
    m_pingTimer->start();
    qDebug() << "clientConnected";
    m_webSocket->sendTextMessage(QStringLiteral("Hello, world!"));
}

void Application::clientClosed() {
    m_pingTimer->stop();
    qDebug() << "clientClosed";
}

void Application::onError(QAbstractSocket::SocketError error) {
    qDebug() << __FUNCTION__ << error << ((m_webSocket != Q_NULLPTR) ? m_webSocket->errorString() : "");
}



/**
* @brief Image provider that is used to handle the live image stream in the QML viewer.
 */
LiveImageProvider::LiveImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
    this->no_image = QImage(3, 3, QImage::Format_RGB32);
    this->blockSignals(false);
}

/**
 * @brief Delivers image. The id is not used.
 * @param id The id is the requested image source, with the "image:" scheme and provider identifier removed.
 * For example, if the image source was "image://myprovider/icons/home", the given id would be "icons/home".
 * @param size In all cases, size must be set to the original size of the image. This is used to set the
 * width and height of the relevant Image if these values have not been set explicitly.
 * @param requestedSize The requestedSize corresponds to the Image::sourceSize requested by an Image item.
 * If requestedSize is a valid size, the image returned should be of that size.
 * @return
 */
QImage LiveImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    qDebug() << __FUNCTION__ << id << size;
    QImage result = this->image;

    if(result.isNull()) {
        result = this->no_image;
    }

    if(size) {
        *size = result.size();
    }

    if(requestedSize.width() > 0 && requestedSize.height() > 0) {
        result = result.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
    }

    return result;
}

/**
 * @brief Update of the current image.
 * @param image The new image.
 */
void LiveImageProvider::updateImage(QImage image) {
    qDebug() << __FUNCTION__ << image;
    //    if(this->image != image) {
    this->image = image;
    emit imageChanged();
    //    }
}
