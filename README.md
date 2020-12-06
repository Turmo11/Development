# Metamorphosis

## Description

Metamorphosis is a 2D platformer game developed by a team of two students<img align="right" width="128" height="128" src="https://github.com/Turmo11/Metamorphosis/blob/master/Screenshots/uwu_logo_black.png"> from the [Bachelor's degrees in Video Game Design and Development at CITM/TTC](<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>). 



This content was generated for the Game Development subject under the supervision of lecturer [Ramón Santamaria](<https://www.linkedin.com/in/raysan/>). 

## Context

<img align="right" width="64" height="128" src="https://github.com/Turmo11/Metamorphosis/blob/master/Screenshots/goal.gif">
You are a fallen greek soul attempting to escape the underworld, in order to do so you must ascend the God's Tower and fight your way into Elysium. To accomplish that, you must go through every level and complete an offering to the assigned god/dess. If you want to complete an offering you must first gather the different pieces which make the word for your offering, and then get to the top of the level to ascend to the next one. Good luck! 


## Key Features

Here are some of the main features of the game (oldest to newest version)

Alpha version (01/11/2020):
 - Jump and fall through Platforms
 - Four different Parallax layers (platforms, clouds, clouds2, tower)
 - Pickups implemented and necessary to complete the game
 - Animated static objects
 - Valuation on not only skill but also exploration (required to find the pieces)
 - All of the assets including all of the art and music have been created by Guillem Turmo and exclusively for this project

Beta version (06/12/2020):
 - FPS management
    - Game capped at 60 FPS
    - Movement normalized using deltaTime
    - Title displays FPS functionality
    
 - Added enemies
    - Flying enemies that can pathfind to the player avoiding non walkable areas if you are close enough
    - Enemies can be killed by the player
    - Enemies kill the player when colliding with it
    
 - Player mechanics
    - 3 extra lives before it dies
    - Shooting projectiles that kill enemies
    - Activation shield that protects you from enemies
    - Skills have cooldowns
    
 - Implemented GUI:
    - Displays PlayerLives left
    - Displays projectile cooldown (animated wheel)
    - Displays shield cooldown (animated wheel)
    
 - New Health pickup that adds +1 PlayerLife (unless it already has 3) 
 
 - Implemented Checkpoint System:
    - Autosaves when activating a checkpoint
    - Map teleportation to said checkpoint
    - Audio feedback when activating or spawning in a checkpoint
    
 - NEW map: Athena level totally playable (second level), including 4 new letter pickups (psi, omega, mu and iota) that must be collected in order to complete the level 
 
 - Added audio feedback (manually recorded):
    - PlayerJump
    - PlayerShoot (projectile)
    - PlayerDeath
    - LetterPickup
    - HealthPickup
    - Checkpoint activation / teleportation
    - ProjectileCollision
    
- Started score system:
    - Keeps track of Player's score
    - Add random amount of score (within defined boundaries) when:
        - Collecting a letter (100~300)
        - Picking up a health point (150~450)
        - Killing an enemy (450~1000)
   - Subtracts random amount of score (within defined boundaries) when:
        - Dying (300~550)
   - Displays score on the Application title

 
## Controls

### Player Movement
 - A/D - Horizontal Movement
 - Space - Jump
 - WASD - Movement when godmode activated
 - Q - Shoot projectile
 - E - Activate shield
 
  <img width="170" height="170" src="https://github.com/Turmo11/Metamorphosis/blob/master/Screenshots/player_run.gif"> <img width="170" height="170" src="https://github.com/Turmo11/Metamorphosis/blob/master/Screenshots/player_god.gif">





### Debug Keys
 - F1 - Start from the first level
 - F2 - Start from the second level
 - F3 - Start from the current level
 - F4 - Toggle Fullscreen
 - F5 - Save 
 - F6 - Load 
 - F7 - TitleScreen
 - F8 - GameOverScreen
 - F9 - Toggle DebugColliders
 - F10 - Godmode
 - F11 - Enable/Disable fps cap to 30
 - (-) Turn volume down
 - (+) Turn volume up
 - (*)  Automatically gather up all the pieces
 - (Arrow up) Add a live
 - (Arrow down) Remove a live
 - NUMPAD 1 - Teleport to the last checkpoint
 
## Developers

In this project we didn't have very defined roles, since we both worked on the project's design and programming, however we still wrote them in order to mention any additional roles

 - [Guillem Turmo](<https://github.com/Turmo11>) - Developer, Designer, Art, Music
 - [Òscar Tarrés](<https://github.com/oscarta3>) - Developer, Designer


## License

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

All of the assets including the sprites, UI and music are licensed under a [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/).

Author: [Turmo11](https://github.com/Turmo11)
