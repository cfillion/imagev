#ifndef IMAGEV_IMAGELIST_HPP
#define IMAGEV_IMAGELIST_HPP

#include <QFileInfo>
#include <QObject>
#include <QSet>
#include <QUrl>

class QDir;

class ImageList : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString currentImageName READ currentImageName NOTIFY currentImageChanged)
  Q_PROPERTY(QString currentImagePath READ currentImagePath NOTIFY currentImageChanged)
  Q_PROPERTY(QUrl    currentImageUrl  READ currentImageUrl  NOTIFY currentImageChanged)

  Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged);
  Q_PROPERTY(int size         READ size         NOTIFY listChanged);

public:
  ImageList();
  int currentIndex() const { return m_currentIndex; }
  QString currentImagePath() const;
  QString currentImageName() const;
  QUrl currentImageUrl() const;
  Q_INVOKABLE int size() const { return m_images.size(); }

signals:
  void currentIndexChanged();
  void currentImageChanged();
  void listChanged();

public slots:
  void build(const QStringList &);
  void absoluteSeek(int);
  void relativeSeek(int);
  void randomSeek(int);

private:
  void append(const QFileInfo &, bool checkType = true);
  void scanDirectory(const QDir &);
  void sort(bool updateCurrent);

  QSet<QFileInfo> m_images;
  QList<const QFileInfo *> m_sorted;
  QList<int> m_random;
  int m_currentIndex;
};

inline uint qHash(const QFileInfo &key, uint seed = 0)
{
  return qHash(key.absoluteFilePath(), seed);
}

#endif
