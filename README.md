Group 09
Game title: AIphaGo Away

TODO: set_up script:
under project dir run

# First, go to project dir where cmakeLists is

# Second go to 

## windows with vs:

### Install vcpkg

git clone https://github.com/microsoft/vcpkg

.\vcpkg\bootstrap-vcpkg.bat

### install package

.\vcpkg\vcpkg install entt

.\vcpkg\vcpkg install gl3w

.\vcpkg\vcpkg install imgui

.\vcpkg\vcpkg install nlohmann-json

### vcpkg Integration  with visual studio

.\vcpkg\vcpkg integrate install

Open the CMake Settings Editor, and under CMake toolchain file, add the path to the vcpkg toolchain file:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

## mac/linux with clion:

### Install vcpkg

git clone https://github.com/microsoft/vcpkg

./vcpkg/bootstrap-vcpkg.sh

### install package

./vcpkg/vcpkg install entt

./vcpkg/vcpkg install gl3w

./vcpkg/vcpkg install imgui

./vcpkg/vcpkg install nlohmann-json

### clion toolchain

Open the Toolchains settings (File > Settings Linux, CLion > Preferences on macOS), and go to the CMake settings (Build, Execution, Deployment > CMake). Finally, in CMake options, add the following line:

-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

jetbrains educational license

https://www.jetbrains.com/community/education/#students

## Mac with xCode

### Install vcpkg

git clone https://github.com/microsoft/vcpkg

./vcpkg/bootstrap-vcpkg.sh

### install packge

./vcpkg/vcpkg install entt

./vcpkg/vcpkg install gl3w

./vcpkg/vcpkg install imgui

./vcpkg/vcpkg install nlohmann-json

### Build file

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake .

### Run game:

./AlphaGo-Away

*Remark: if your mac is stopping you from running the game from terminal due to some permission issue, please go to the file finder and launch the executable from there. You might get aborted right away but don't worry. Close it and get back to the original termnial(which still should be in the project directory) and rerun "./AlphaGo-Away". Now it should work.

Credit:

- package manager

https://github.com/microsoft/vcpkg

- ECS:

https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system

- GUI:

https://github.com/ocornut/imgui

- Utils:

https://github.com/nlohmann/json

- Games:

https://github.com/guillaume-haerinck/imac-tower-defense

https://github.com/Kerndog73/EnTT-Pacman

- Mesh generator:
  
https://www.taketechease.com/

- Free texture:

https://www.kenney.nl/assets/toon-characters-1

https://itch.io/game-assets/free/tag-explosions

https://www.pinterest.ca/pin/42925002684118502/

https://www.clipartkey.com/view/iixxTxo_pixel-laser-beam-png/

https://www.pinterest.ca/pin/698269117220803900/

https://www.nicepng.com/ourpic/u2r5w7q8e6u2r5q8_laser-eyes-meme-maker-glowing-eyes-png-lens/

https://www.stockunlimited.com/vector-illustration/pixel-art-arrow_1957532.html


- Paid spritesheet:

https://tokegameart.net/item/rpg-hero-character-pack/

- Shaders:

https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

https://www.youtube.com/watch?v=RP1MVD4hAJM&list=WL&index=7&t=22s

https://www.shadertoy.com/view/lsS3zc

- Audio asset (paid):

《Huangwuzhishang》 & 《Jinguzhidi》 from the Donghua "Ling Long: Incarnation"

- Algorithm

Physics-based animation:

Fluid Simulation from code train

https://www.youtube.com/watch?v=alhpH6ECFvQ&t=523s

Swarm behavior:

https://eater.net/boids

https://github.com/beneater/boids

Advanced shader, geometry shader (should avoid):

https://www.youtube.com/watch?v=RP1MVD4hAJM&t=126s

https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

https://github.com/JoeyDeVries/LearnOpenGL/tree/master/src/4.advanced_opengl/9.1.geometry_shader_houses