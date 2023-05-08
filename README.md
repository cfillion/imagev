# ImageV

[![Build status](https://github.com/cfillion/imagev/workflows/build/badge.svg)](https://github.com/cfillion/imagev/actions)

This is a viewer for directories of images and animations.

It takes a list of files or directories as arguments. Images can be
browsed in sorted or random order.

Inspired by the venerable [feh](https://feh.finalrewind.org/) and the now
obsolete Xee2.

## Usage

    Usage: imagev [options] <files or directories...>
    Viewer for directories of images and animations.

    Options:
      -a, --all                       do not ignore files starting with .
      -h, --help                      show this help and exit
      -r, --recursive                 scan directories recursively
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

Depends on C++17, Meson, mpv and glfw3. ICU is required on Linux.
Boost on platforms without `<filesystem>`.

Windows is not supported at this time (todo: Win32 API collator for sorting,
check support for Unicode filenames, move to trash).

    meson setup build --buildtype=release
    cd build
    ninja
    meson install
