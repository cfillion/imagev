import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.2
import ca.cfillion.imagev 1.0

ApplicationWindow {
  id: window
  color: 'black'
  visible: true
  width: screen.desktopAvailableWidth   * 0.8
  height: screen.desktopAvailableHeight * 0.8

  title:
    if(list.size == 0) 'No images [0 of 0]'
    else `${list.currentImageName} [${list.currentIndex + 1} of ${list.size}]`

  ImageList {
    id: list
    Component.onCompleted: build(files)
  }

  WindowHelper {
    window: window
    filePath: list.currentImagePath
  }

  Shortcut {
    sequences: ['Right', 'Down', 'PgDown', '.']
    onActivated: list.relativeSeek(+1)
  }

  Shortcut {
    sequences: ['Left', 'Up', 'PgUp', ',']
    onActivated: list.relativeSeek(-1)
  }

  Shortcut {
    sequences: ['R']
    onActivated: list.randomSeek(1)
  }

  Shortcut {
    sequences: ['Shift+R']
    onActivated: list.randomSeek(-1)
  }

  Shortcut {
    sequences: ['Home']
    onActivated: list.absoluteSeek(0)
  }

  Shortcut {
    sequences: ['End']
    onActivated: list.absoluteSeek(list.size);
  }

  Shortcut {
    sequences: [StandardKey.FullScreen, 'F']
    onActivated: {
      if(window.visibility == Window.FullScreen)
        showNormal()
      else
        showFullScreen()
    }
  }

  Shortcut {
    sequences: [StandardKey.Close, StandardKey.Quit, 'Q']
    onActivated: close()
  }

  Flickable {
    anchors.fill: parent
    contentWidth: image.width; contentHeight: image.height
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.horizontal: ScrollBar { id: hbar; active: vbar.active }
    ScrollBar.vertical:   ScrollBar { id: vbar; active: hbar.active }

    AnimatedImage {
      id: image

      autoTransform: true
      fillMode: Image.PreserveAspectFit
      mipmap: true
      source: list.currentImageUrl

      width: window.width
      height: window.height

      // restart after displaying a non-animated image
      onStatusChanged: playing = (status == AnimatedImage.Ready)
    }
  }

  MouseArea {
    anchors.fill: parent
    enabled: false
    cursorShape:
      window.visibility == Window.FullScreen ? Qt.BlankCursor : Qt.ArrowCursor
  }
}
