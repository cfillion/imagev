#include "window.hpp"

#include <AppKit/AppKit.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

void Window::nativeInit()
{
  NSWindow *nativeWindow { glfwGetCocoaWindow(m_handle) };

  // enable moving the window by dragging its contents
  nativeWindow.movableByWindowBackground = true;
  [nativeWindow setOpaque:false];
}

void Window::setFilePath(const char *fn)
{
  NSWindow *nativeWindow { glfwGetCocoaWindow(m_handle) };
  nativeWindow.representedFilename = [NSString stringWithUTF8String:fn];
}
