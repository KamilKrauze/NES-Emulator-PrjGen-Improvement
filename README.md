# NES-Emulator

Basic NES emulator that only supports NROM (mapper 0) games.

# Compilation

To compile, run:

cmake pathToSourceFolder/src
make

# Running

Emulator must be given one argument, that being the .nes file it is to run e.g. ./nesEmu game.nes

# Controls

Up - Up Arrow
Down - Down Arrow
Left - Left Arrow
Right - Right Arrow
A - Z
B - X
Start - Enter
Select - Backspace

Currently no way to remap controls other than directly modifying the controls.cpp file.
