#include "imagelist.hpp"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QWindow>

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

  ImageList list;

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("list", &list);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  if(engine.rootObjects().isEmpty())
    return -1;

  list.setWindow(dynamic_cast<QWindow *>(engine.rootObjects().first()));
  list.build(parser.positionalArguments());

  return app.exec();
}
