#include <QQuickStyle>
#ifdef Q_OS_WIN
#include <QGuiApplication>
#elif defined (Q_OS_LINUX)
#include <QApplication>
#endif
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "singletontypeexample.h"
#include "circlehead.h"
//static QObject* callBack(QQmlEngine*, QJSEngine*);
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    QGuiApplication app(argc, argv);
#elif defined (Q_OS_LINUX)
    QApplication app(argc, argv);
#endif
    qmlRegisterType<CircleHead>("CusModel", 1, 0, "CircleHead");
    QQuickStyle::setStyle("Fusion");
    QQmlApplicationEngine engine;
    //qmlRegisterSingletonType<SingletonTypeExample>("Qt.example.qobjectSingleton", 1, 0, "Example", callBack);
    QScopedPointer<SingletonTypeExample> example(new SingletonTypeExample);
    qmlRegisterSingletonInstance("Qt.example.qobjectSingleton", 1, 0, "Example", example.get());
    /*
    qmlRegisterSingletonType<SingletonTypeExample>("Qt.example.qobjectSingleton", 1, 0, "Example", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new SingletonTypeExample();
    });
    */
    engine.load("qrc:/main.qml");
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
/*
static QObject* callBack(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new SingletonTypeExample();
}
*/