> Project Working Time period: Jan-Apr 2021

# Video Game Programming Project: AlphaGo Away

## Team Members:
- Bill Zhou 		
- Jingbo Liu              
- Jasmine	Kwong	
- Yu Tian                   
- Wenxi Li                 
- Yuxin Wang
	
## Story/Gameplay:

 - **Genre**: auto-batte, strategy
 - **Theme/Background story**: 100 years in the future, AI (AlphaGo) has taken over the world. It’s up to the world’s last computer scientists to right their wrongs and save the world.
 - **Player goals/Basic game mechanics**: The goal is for the player to complete all levels of the game. Users place units onto the board at the start of a level, then press start to begin the level, which will cause the enemies and units to move towards one another and attack on collision. Each unit/enemy has a health bar and once it’s depleted, they die. The goal is to defeat all the enemies in one level.
 - **Game delay**: To account for time delay, the players will take turns placing units on the board, with the active player determined by a toggle switch. This is not time sensitive as the players have unlimited time to design their board. Once both players are satisfied, then they can press start to begin the game at which point players do not need to do anything else.

 ## Technical Elements:

  - **Rendering**: Enemies and map obstacles (ex. rivers, mountains, etc.) will be automatically rendered at the start of every level. Units will be rendered once the player places them on the map at the start of each level. We will also render the different levels in a level-select screen
Geometric/Sprite/Other assets: Units and enemies, the map, the grid map as a battlefield and gold/currency icon, health bars
  - **2D Geometry manipulation**: Units and enemies will be translated across the board. On collision, there will be attack animations such as particles and glowing effects. Translation, rotation, scaling, and reflection will be used to place units and implement attack animations
  - **Gameplay logic**:  All the units and enemies will be driven by AI once the fight starts. All units will search for the nearest enemy and try to avoid obstacles. A simple decision tree will be used to decide whether units stand still, search for the nearest enemies, or attack.
  - **Physics**:  The speed at which different units/enemies move across different terrains, acceleration rate of units/elements, knockback upon collision of units, and illumination of the board.
  - **Advanced**: path finding for enemies/units (please refer to the development spreadsheet)
  - **Audio**: There will be background music and sound effects will be implemented for when units/enemies attack

  - **Advanced Technical Elements**:
   - **Map generation**: obstacles will be generated randomly before each fight starts, level builder, and a sandbox to test units.
Add more extensive animations on the collider box and other attacks.

 - **Devices**:
Players will use the keyboard or mouse to control their movements.		
To begin with, both players will use the mouse to select and place the units, and a toggle switch will be used to determine which player is active.
An alternative plan is to allow the second player to use the keyboard, and removing the toggle switch:
      
      - left arrow = move left
	    - right arrow = move right
	    - up arrow = move upwards
	    - down arrow = move downwards
	    - spacebar = select & place the unit
 				 						
 - **Tools**:
C/C++ and OpenGL


 ## Development Plan:

- **Skeletal Game**
    > Week: Feb 1
    - Set up part of the development environment and sandbox.
    - Set up the basic ECS system
    - Render the basic map layout.
    - Be able to render one unit onto the map at a position.
    - Be able to place unit on the map
    - Basic transformations: translation and reflection
    
    > Week: Feb 8
    - Basic fighting mechanisms. Each unit should have two states.
    - Basic collisions (player units with enemies) and penetration handling 
    - Basic character movement: Units in the sandbox should be able to attack, search for the nearest enemy, or stand still.
    - Observer Pattern: set up collisionObservers and keyboardObservers
    - Basic keyboard/mouse control: Control of one or more units.
    - Quality control: take care of the lagging problem and resolution consistency
    - Basic physics: acceleration rate of units/elements, knockback
    - textured units and tiles
    - Basic searching algorithm and pathfinding
    - Basic unit decision making
    - Basic decision making for a group of units of same class
    - Basic movement interpolation
    - basic movement sprite transition on turning.
    - Implemented ECS::view<template ...Components> that returns all entities that contain all the components
    - Audio feedback on unit death


 - **Minimal Playability** 
    > Week: Feb 22
    - Design the level loading screen. On level selection, the player will be sent to the sandbox where players can place units.
    - Do research on advanced collision detection
    - switch to vcpkg, entt, and integrate more libs
    - implement level builder + debugger 
    - Implement the decision making of the unit. In each loop, each unit will decide to search for enemies, stay still, or attack.
    - Add sprite for state transitions (haven’t done yet).
    - Store units stats as json files.
    - refactoring code to be more robust (haven’t done yet)
  
    > Week: March 1
    - Create a ranged unit and its assets.
    - Create projectile entities. (change the collide mechanism, don’t plan to involve the projectile at this stage)
    - Implement projectile path finding. (change the collide mechanism, don't plan to involve the projectile at this stage)
    - Implement advanced pathfinding so that units will be able to find nearest enemies.
    - Help: Use debugging graphics with some text to explain how to place units and how units fight enemies.
    - Implement sprite and decision making of the ranged unit.
    - Implement debugging graphics for units placement.
    - QA testing

 - **Playability**
  
    > Week: March 8
    - implement new features to achieve a minimum of 5 minutes of progressive, non-repetitive gameplay
    - introduce new units with different fighting or moving mechanism
    - add obstacles to the world such as walls, boulders, maybe even water if feasible
    - begin designing characters that are part of the game and start writing dialog for said characters
    - add background music and other sound effects
    - Add level descriptions.
  
    > Week: March 15
    - design narrative elements to increase users’ engagement
    - animations
    - cutscenes that introduces characters and introduces story of the game (potentially a cutscene after each level completion)
    - small pop ups of characters with tips and phrases to assist the player, as well as showcase personalities of in game characters. 
    - introducing more features means more potential for bugs and errors. Testing to ensure features introduced in previous milestones are working as intended, as well as new features are working

 - **Final Game**
  
    > Week: March 22
    - Extend the game: complete the implementation of at least 5 playable levels so that the player can interact with the game for more than 10minutes
    - Develop any potential advanced graphics in the shaders, for instance, fog effect or advanced textures on the grid map
    - Stability testing: make sure there’s no memory leak and fix bugs from prior stage
  
    > Week: March 29
    - Add delay-agnostic design to support remote control (zoom players)
    - Add more interesting physical-based animation
    - Finish the particle effect
    - user experience: 
conduct user experience testing and optimize the game. Report the feedback
start to create a comprehensive tutorial for first-time users

  
    >  Week: April 5
    - Final optimization of the game based on user experience testing result
    - Improve code robustness
    - final stability testing to check if there are any bugs or crashes
    - Finish the user tutorial and make sure every level is included


## Install Instruction

### Step 1: 
  Go to project dir where cmakeLists is

### Step2:

 > For windows with vs:

   - Install vcpkg: 
     1. `git clone https://github.com/microsoft/vcpkg`

     2. `.\vcpkg\bootstrap-vcpkg.bat`

   - install package

     1. `.\vcpkg\vcpkg install entt`

     2. `.\vcpkg\vcpkg install gl3w`

     3. `.\vcpkg\vcpkg install imgui`

     4. `.\vcpkg\vcpkg install nlohmann-json`

   - vcpkg Integration  with visual studio

     1. `.\vcpkg\vcpkg integrate install`

   - Open the CMake Settings Editor, and under CMake toolchain file, add the path to the vcpkg toolchain file:

       `-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake`

 > For Mac/linux with clion:

   - Install vcpkg

     1. `git clone https://github.com/microsoft/vcpkg`

     2. `./vcpkg/bootstrap-vcpkg.sh`

   - install package

     1. `./vcpkg/vcpkg install entt`

     2. `./vcpkg/vcpkg install gl3w`

     3. `./vcpkg/vcpkg install imgui`

     4. `./vcpkg/vcpkg install nlohmann-json`

   - clion toolchain

Open the Toolchains settings (File > Settings Linux, CLion > Preferences on macOS), and go to the CMake settings (Build, Execution, Deployment > CMake). Finally, in CMake options, add the following line:

`-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake`

   - jetbrains educational license

https://www.jetbrains.com/community/education/#students

 > Mac with xCode

   - Install vcpkg

`git clone https://github.com/microsoft/vcpkg`

`./vcpkg/bootstrap-vcpkg.sh`

   - install packge

`./vcpkg/vcpkg install entt`

`./vcpkg/vcpkg install gl3w`

`./vcpkg/vcpkg install imgui`

`./vcpkg/vcpkg install nlohmann-json`

   - Build file

`cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake` 

### Step3: Run game:

`./AlphaGo-Away`

**Remark**: if your mac is stopping you from running the game from terminal due to some permission issue, please go to the file finder and launch the executable from there. You might get aborted right away but don't worry. Close it and get back to the original termnial(which still should be in the project directory) and rerun "./AlphaGo-Away". Now it should work.

 ## Credit:

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
