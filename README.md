# NES-Emulator

Basic NES emulator that only supports NROM (mapper 0) games.

## PreMake Project Files
DISCLAIMER: I do not own the right nor contribute to the PreMake project.

You may use PreMake optionally to generate your project files you can also use the provided CMake files that are already provided by @filip-arch.

> 1. Download [PreMake5](https://premake.github.io/) and insert into the project root directory.
> 2. You may either run the bash script `GenerateProjectFiles.sh` as shown below:
> ```console
> ./GenerateProjectFiles.sh
> ```
>Or you may generate the project files using the PreMake executable directly as shown below:
> ```console
> ./premake5 gmake2
> ```

>### Deleting Project Files
>For any reason that you may need to delete the project files generated using PreMake, run the `DeleteProjectFiles.sh` script as shown below:
>```console
>./DeleteProjectFiles.sh
>```
> <br>

## Compilation
> ### CMake
> To compile, run:
> ```console
> cmake pathToSourceFolder/src
> make
> ```
> <br>

> ### PreMake
> If the project files have been generated using PreMake then just run in the project root directory:
> ```console
> make
> ```
> <br>

## Running

Emulator must be given one argument, that being the .nes file it is to run e.g. ``./NESEmulator`` game.nes

## Controls

|NES Controller|Keyboard|
|--------------|--------|
|Up | Up Arrow|
|Down | Down Arrow|
|Left | Left Arrow|
|Right | Right Arrow|
|A | Z|
|B | X|
|Start | Enter|
|Select | Backspace|

Currently no way to remap controls other than directly modifying the controls.cpp file.

## Changes
|Changes|Date|
|---|---|
|Added a PreMake5 lua script for project generation. | 08/12/2023 |
|Added complenting bash scripts for generating and deleting Makefiles of this project.| 08/12/2023 |
|Updated `README` with a better layout and PreMake instructions.| 08/12/2023 |
|Updated `.gitignore` to ignore Makefiles and premake5.lua | 08/12/2023 |
