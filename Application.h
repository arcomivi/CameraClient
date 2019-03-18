#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QTimer>
#include <QQmlApplicationEngine>
#include <QImage>
#include <QQuickImageProvider>
#include <QtWebSockets/QtWebSockets>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QtMultimedia/QAbstractVideoSurface>

class FrameProvider : public QObject {
        Q_OBJECT
        Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)


    public:
        FrameProvider(QObject *parent = Q_NULLPTR) : QObject(parent) {

        }
        QAbstractVideoSurface *videoSurface() const { return m_surface; }

    private:
        QAbstractVideoSurface *m_surface = Q_NULLPTR;
        QVideoSurfaceFormat m_format;

    public:


        void setVideoSurface(QAbstractVideoSurface *surface) {
            if (m_surface && m_surface != surface  && m_surface->isActive()) {
                m_surface->stop();
            }

            m_surface = surface;

            if (m_surface && m_format.isValid()) {
                m_format = m_surface->nearestFormat(m_format);
                m_surface->start(m_format);

            }
        }

        void setFormat(int width, int heigth) { //}, int format = QVideoFrame::Format_RGB32) {
            QSize size(width, heigth);
            QVideoSurfaceFormat format(size, QVideoFrame::Format_RGB32);
            m_format = format;

            if (m_surface) {
                if (m_surface->isActive()) {
                    m_surface->stop();
                }
                m_format = m_surface->nearestFormat(m_format);
                m_surface->start(m_format);
            }
        }

    public slots:
        void onNewVideoContentReceived(const QVideoFrame &frame) {

            if (m_surface) {
                m_surface->present(frame);
            }
        }
};

class LiveImageProvider : public QObject, public QQuickImageProvider {
        Q_OBJECT
    public:
        LiveImageProvider();

        QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    public slots:
        void updateImage(QImage image);

    signals:
        void imageChanged();

    private:
        QImage image;
        QImage no_image;
};

class Application : public QObject {
        Q_OBJECT


    public:
        explicit Application(QObject *parent = nullptr);
        void run();
    signals:
        void imageChanged(QImage img);
    public Q_SLOTS:
        void clientConnected();
        void clientClosed();
        void onError(QAbstractSocket::SocketError error);
        void clientReceivedBinary(const QByteArray &message);
        void clientReceivedText(const QString &message);
        void pingServer();
        void pongReceived();
    private:
        FrameProvider *m_frameProvider;
        LiveImageProvider *m_imageProvider;
        QQmlApplicationEngine engine;
        QWebSocket *m_webSocket;
        QTimer *m_pingTimer;
        QImage m_image;
};

#endif // APPLICATION_H
