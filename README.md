# Stick Knights Online
Client source code for Stick Knights Online, an online side scrolling stickman
role playing game.

## Development Status
The codebase is currently undergoing heavy re-factoring in order to make it
easier to expand and improve the game, as well making it more approachable to
anybody who would wish to collaborate.

## Compiling

1. Open operating_system.h and uncomment whichever operating system you are using, and comment those you are not
..* Example: If compiling on a Linux distrobution such as Ubuntu, comment  out `#define WINDOWS_OS 1` and uncomment `//#define LINUX_OS 2` 

2. Copy contents of Makefile.<OS> into Makefile
..* Example: copy contents of operating_system.linux.h into operating_system.h
    if on Ubuntu or another Linux distro.

More thorough/in-depth compile guidelines are planned.

## Contact
Email: admin@optimuspi.com
Website: [Stick Knights Online](http://www.stickknightsonline.com/)
