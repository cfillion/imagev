# ImageV

This is an image viewer. It displays images from the command line or current
directory. It provides quick access (in alphabetical or random order) to all
other images in the same directory when opening a single image file. Animated
images (gif) are supported.

Inspired by the venerable [feh](https://feh.finalrewind.org/) and the now
obsolete Xee2.

## Usage

    imagev [files or directories...]

## Shortcuts

Action                | Keys
--------------------- | -----------------------------
Next image            | Right, Down, Page Down or `.`
Previous image        | Left, Up, Page Up or `,`
Next random image     | `r`
Previous random image | `R`
First image           | Home
Last image            | End
Move to the trash     | Delete
Toggle fullscreen     | `f`
Quit                  | `q`

## Build from source

Depends on C++11, CMake and Qt 5.15+.

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target install
