#include <QGuiApplication>
#include "Application.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    Application application;
    application.run();

    return app.exec();
}
