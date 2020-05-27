#include "windowhelper.hpp"

#include <QWindow>

WindowHelper::WindowHelper()
  : m_window{}
{
}

void WindowHelper::setFilePath(const QString &filePath)
{
  m_window->setFilePath(filePath);
}
