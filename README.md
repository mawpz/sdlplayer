# sdlplayer
A small audio player written in C++ with SDL

![screenshot](./screenshot.png)

# Usage
```
    ./sdlplayer <MP3 File>
    ./sdlplayer <playlist file>
```

A playlist file is just a text file (with the MIME type of `text/plain`), containing full paths to MP3 files

## Dependencies
* Arch Linux: `sudo pacman -S sdl2 sdl2_image sdl2_mixer sdl2_ttf taglib ttf-isoevka-nerd`

## LICENCE
MIT License

Copyright (c) 2026 mawpz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
