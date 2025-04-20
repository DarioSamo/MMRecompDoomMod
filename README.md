# Majora's Mask Recompiled DOOM Mod

## Disclaimer

This mod was very much made in jest and is mostly just intended as a proof of concept to showcase the ability of running native code inside [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp).

Expect plenty of bugs, softlocks and other buggy interactions if you mess around with it.

## What is this?

This mod uses a modified version [doomgeneric](https://github.com/ozkl/doomgeneric) to create a native library that outputs the screen's contents to a destination buffer instead of a display.

The mod calls the exported functions from the native library to retrieve the game's next frame and displays it. It also passes the game's controller inputs directly to DOOM. You'll see the game inside a desktop PC that Link spawns in-game.

![Gaming](/img/clock_tower.gif)

Platforms other than Windows aren't supported at the moment but it shouldn't be too hard to add if somebody's willing to add them.

## How to play

Install the mod using the [latest release](https://github.com/DarioSamo/MMRecompDoomMod/releases/latest).

Before running the mod, you must have a a `DOOM1.wad` file next to the dynamic library (`doomgeneric.dll` or others) file in the mods.

Once you're in-game, you can just press L at any time and Link will spawn a desk and start playing DOOM.

## How to build

Instructions on how to build each component of the mod can be found in each of the project's folders. You'll have to build both components separately and copy them together to the mods folder.

## Special thanks

- [Wiseguy](https://github.com/Mr-Wiseguy) for providing general code assistance and material tweaking.
- [Reonu](https://github.com/Reonu) for providing the desk model and the animations for Link.
