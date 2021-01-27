# ImageV

This is a viewer for directories of images and animations.

It takes a list of files or directories as arguments. Images can be
browsed in sorted or random order.

Inspired by the venerable [feh](https://feh.finalrewind.org/) and the now
obsolete Xee2.

## Usage

    Usage: imagev [options] <files or directories...>
    Viewer for directories of images and animations.

    Options:
    -h, --help                      show this help and exit
    -s, --skip                      skip files that cannot be loaded

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

Depends on C++17, CMake, mpv and glfw3. ICU is required on Linux. Boost on macOS.

Windows is not supported at this time (todo: Win32 API collator for sorting,
check support for Unicode filenames, move to trash).

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target install
