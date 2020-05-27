#include "imagelist.hpp"

#include "comparators.hpp"

#include <QDebug>
#include <QDir>
#include <QMimeDatabase>

ImageList::ImageList()
  : m_currentIndex{0}
{
  connect(this, &ImageList::currentImageChanged, this, &ImageList::currentIndexChanged);
}

void ImageList::build(const QStringList &args)
{
  bool updateCurrent { false };

  for(const QString &fn : args) {
    QFileInfo info{fn};

    if(info.isFile())
      updateCurrent = true;

    append(info, false);
  }

  // didn't find a directory
  if(args.size() == m_images.size()) {
    // if the user didn't specify any input files
    if(args.empty())
      scanDirectory(QDir::current());
    else if(args.size() == 1)
      scanDirectory(QFileInfo{args[0]}.dir());
  }

  if(!m_images.empty()) {
    sort(updateCurrent);
    emit currentImageChanged();
  }

  emit listChanged();
}

void ImageList::scanDirectory(const QDir &dir)
{
  for(const QFileInfo &info : dir.entryInfoList(QDir::Files | QDir::Readable))
    append(info);
}

void ImageList::append(const QFileInfo &info, const bool checkType)
{
  if(!info.exists()) {
    qWarning() << "imagev: file not found: " << info.filePath();
    return;
  }
  else if(info.isDir()) {
    scanDirectory(info.filePath());
    return;
  }

  if(checkType) {
    static QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(info.filePath());
    if(!type.name().startsWith("image/"))
      return;
  }

  const int oldSize = m_images.size();
  auto it = m_images.insert(info);
  if(oldSize < m_images.size())
    m_sorted.append(&*it);
}

void ImageList::sort(const bool updateCurrent)
{
  const QFileInfo *current = m_sorted[m_currentIndex];
  std::sort(m_sorted.begin(), m_sorted.end(), NameComparator{});

  if(updateCurrent) {
    const int newIndex = m_sorted.indexOf(current);
    if(newIndex != m_currentIndex) {
      m_currentIndex = newIndex;
      // the index changed but the image stayed the same
      emit currentIndexChanged();
    }
  }
}

void ImageList::absoluteSeek(int newIndex)
{
  newIndex = qBound(0, newIndex, m_images.size() - 1);

  if(newIndex != m_currentIndex) {
    m_currentIndex = newIndex;
    emit currentImageChanged();
  }
}

void ImageList::relativeSeek(const int rel)
{
  absoluteSeek(m_currentIndex + rel);
}

QString ImageList::currentImageName() const
{
  return m_images.empty() ? QString{} : m_sorted[m_currentIndex]->fileName();
}

QString ImageList::currentImagePath() const
{
  return m_images.empty() ? QString{} : m_sorted[m_currentIndex]->filePath();
}

QUrl ImageList::currentImageUrl() const
{
  return m_images.empty() ? QUrl{} : QUrl::fromLocalFile(currentImagePath());
}
