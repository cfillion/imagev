#ifndef IMAGEV_IMAGELIST_HPP
#define IMAGEV_IMAGELIST_HPP

#include <QFileInfo>
#include <QObject>
#include <QSet>
#include <QUrl>

class QDir;
class QWindow;

class ImageList : public QObject {
  Q_OBJECT

  Q_PROPERTY(QUrl currentImage READ currentImage NOTIFY currentImageChanged)

public:
  ImageList();
  void setWindow(QWindow *);
  void build(const QStringList &);
  QUrl currentImage() const;

signals:
  void currentImageChanged();

public slots:
  void absoluteSeek(int);
  void relativeSeek(int);
  int size() const { return m_images.size(); }

private:
  void append(const QFileInfo &, bool checkType = true);
  void scanDirectory(const QDir &);
  void sort(bool updateCurrent);
  void updateWindowTitle();

  QSet<QFileInfo> m_images;
  QList<const QFileInfo *> m_sorted;
  int m_currentIndex;
  QWindow *m_window;
};

inline uint qHash(const QFileInfo &key, uint seed = 0)
{
  return qHash(key.absoluteFilePath(), seed);
}

#endif
