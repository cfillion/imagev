#include "window.hpp"

#include <AppKit/AppKit.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

void Window::setFilePath(const char *fn)
{
  NSWindow *nativeWindow { glfwGetCocoaWindow(m_handle) };
  nativeWindow.representedFilename = [NSString stringWithUTF8String:fn];
}
