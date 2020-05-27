#include "imagelist.hpp"
#include "windowhelper.hpp"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);
  QCoreApplication::setApplicationName("imagev");
  QCoreApplication::setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Image view");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("files", "Images ", "[files or directories...]");
  parser.process(app);

  qmlRegisterType<ImageList>("ca.cfillion.imagev", 1, 0, "ImageList");
  qmlRegisterType<WindowHelper>("ca.cfillion.imagev", 1, 0, "WindowHelper");

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("files", parser.positionalArguments());
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  if(engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
