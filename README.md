# Majora's Mask Recompiled DOOM Mod

## Disclaimer

This mod was very much made in jest and is mostly just intended as a proof of concept to showcase the ability of running native code inside [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp).

Expect plenty of bugs, softlocks and other buggy interactions if you mess around with it.

There's no pre-made binary available for MacOS at the moment but adding support for it is welcome.

## What is this?

This mod uses a modified version [doomgeneric](https://github.com/ozkl/doomgeneric) to create a native library that outputs the screen's contents to a destination buffer instead of a display.

The mod calls the exported functions from the native library to retrieve the game's next frame and displays it.

The game is played inside a desktop PC placed in the game world itself.

![Gaming](/img/clock_tower.gif)

## How to play

- Install the mod using the [latest release](https://github.com/DarioSamo/MMRecompDoomMod/releases/latest).
- Before running the mod, you must have a a `DOOM1.WAD` file next to the dynamic library (`doomgeneric.dll` or others) file in the mods.
  - If you don't own DOOM, you can just use the shareware version which is freely available online.
- Once you're in-game, you can just press `L + R` at any time and Link will spawn a desk and start playing DOOM. Press this combination again to stop playing.

## How to build

Instructions on how to build each component of the mod can be found in each of the project's folders. You'll have to build both components separately and copy them together to the mods folder.

## Special thanks

- [Wiseguy](https://github.com/Mr-Wiseguy) for providing general code assistance and material tweaking.
- [Reonu](https://github.com/Reonu) for providing the desk model and the animations for Link as well as adding Linux building.
