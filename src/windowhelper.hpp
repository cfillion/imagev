#ifndef IMAGEV_WINOWHELPER_HPP
#define IMAGEV_WINOWHELPER_HPP

#include <QObject>

class QWindow;

class WindowHelper : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString filePath WRITE setFilePath)
  Q_PROPERTY(QWindow *window MEMBER m_window)

public:
  WindowHelper();
  void setFilePath(const QString &);

private:
  QWindow *m_window;
};

#endif
