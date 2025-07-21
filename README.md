# Xye readme

## A few notes from the author for context

Xye was both my first C++ project and also the last (for now) project I've released publicly. That was in 2005. It was actually a pleasant surprise over the years to see that the number of people that were happy to play this game well exceeds my initial estimate of 2.

As the years have passed and signs of there being some non-zero interest in t he game persisted, it has become increasingly clear that there is some need to preserve this game. I hope we can gradually modernize some aspects of the way the game and its code is distributed. And also avoid obsolescence somehow. The first step is to move it to a github repository.

Note however, that things like updating its gameplay such as adding new objects are going to be out of the question, at least for this official repository. This is a 20+ years old game and it's staying as such.

# Xye

Xye is a puzzle game in which the objective is to help a character that looks like a green circle to get all the gems in the room. This is, of course, not as easy as it sounds, Xye must solve all sorts of puzzles while at the same time avoiding all sorts of traps and beasts.

Xye is similar to other puzzle games like sokoban or boulderdash, yet it also includes some arcade elements.

Xye is a derivative of a classic windows game called Kye, which is the base of the gameplay experience and visual elements. Xye is able to play level files that were made for Kye and Sokoban. It is also able to play custom .xye files, a richer level format that allows the new objects and features, you can make these levels by hand or by using the built-in editor that comes with the game.

Thanks to the SDL library, Xye is cross platform software, and there are versions available for Linux and windows, there are also ports for FreeBSD and it should be possible to compile it in other platforms as well.

The project is currently on beta state.

## The Game

Xye started off as a clone of the 1990s game called Kye. To learn more about the original game that inspired Xye, you may read the page dedicated to Kye.

From that starting point, it has added many new gameplay elements and concepts. There is a variety of puzzles. Although the tutorial levels are relatively simple. The last elements of the official bundle will probably be quite a challenge.

The main objective is to get all the gems in the level. Some levels have a secundary objective - to get all the stars before getting the last gem.

## Introduction

Introduction
Xye is a game that begins by asking you to select a level file. Some level files are provided by default. The tutorials are meant to explain the actions of the various objects in the game. The levels.xye file contains the official levels. Usually the default Xye distribution also comes with plenty of other bonus levels.

The objective in each level is simple and derived from Kye: To obtain all the gems/diamonds/stones in the level. It is often not as easy as it sounds since there are many different objects in the game with which the game's character (Xye) has to interact with:

- Blocks that can be pushed and sometimes pulled. Blocks may have different shapes and sizes.
- Blocks with predefined movement.
- Magnets that can be used to pull blocks.
- Doors that open or close once you move certain blocks to certain locations.
- Monsters that will chase you.
- And many more.

There are many different interactions between xye, the objects and the monsters. In order to learn them all, you will need to play and experiment with the game in many situations. The tutorials help introduce the most basic ones but do not cover them all.

The game is a puzzle / arcade hybrid. Although Xye's official levels tend to put more emphasis on puzzles.

### Sand box

The game is not meant to be solved linearly. Its main objective is to provide fun for the players that try to solve it. When playing the game, you may always jump to the next level or the second next level, or the last level if you wish to.

Another feature is the undo option. At any point in the game you can undo your last movement and then the movement before it. This helps you backtrack while solving very thought puzzles and also stops you from having to go all over the level for a single key mistake.

The game also saves your progress inside a level automatically. So if you quit the game and then open the game again, your game (and the undo history) will be restored to the last time you played.

### The star gem

A recent feature of the game is the star gem. When development of the game just started plenty of levels that were very difficult to solve. To the point that even the first tutorial made a lot of people get stuck. I eventually decide to move towards easier level designs. But solving a very complicated level can be a good challenge and very motivating once you manage to have the sudden realization. We needed a way to introduce tough levels without making them a condition to pass the level.

The star gem works as an extra challenge that is not required to solve a level. If you see a star in a level, I recommend you to first try solving the level ignoring the star. Once you solve the level and if you want a challenge, try getting the star gem AND also getting the other gems (Just getting the star is not the only prerequisite for completing the special challenge). In many levels, the star requires you to solve a much harder puzzle than the basic one that is required for just the gems. In other cases, it requires you to solve the puzzle with an extra condition that makes it harder. In few levels that are closer to the arcade side of things, you may get stars for finishing the level under a tougher time limit or with more monsters.

The star gem in tutorial 5 is harder to get than the other gems.

### The editor

A very fun feature from Kye was the level editor. To mantain the spirit, Xye includes with a level editor that allows you to easily make level files. Instructions about how to use the editor may be found in the levels section.

If you make some levels, you are encouraged to share them. You may send them to the lead developer using the contact button. Experimenting with the editor is also a very nice way to find the many iteractions between objects.

## Some differences between Kye and Xye

Some information for Kye fans: Although one of the objectives of Xye is to support the old, classic Kye levels, it also modifies aspects of Kye both intentionally and unintentionally. Certain aspects of the game cannot be reproduced without severe reverse engineering and some others are the result of adding features:

- Xye does not allow diagonal movement. The main reason for this is that in a grid-based game it is not very intuitive. Disabling diagonal movement also makes the player easier to constraint in levels and helps forbid unintentional solutions. This has the unwanted effect that some original Kye levels may be unsolvable. The one level from shareware Kye that cannot be solved without diagonal movement is ADOORABLE. Another notable example is probably the third level in Colin Phipps' mystical.kye. Notable because that level is included in the game...
- In the original Kye, using the mouse to move the character could stop time and was far easier than using the keyboard. Mouse movement in Xye cannot make the character move faster than the keyboard.
- Pressing keys does not boost the game clock speed, this is just a decorative change that does not affect gameplay too much. Instead, there is a fast forward button that can be used to speed the game up temporarily while you wait for an event.
- Whenever Kye got into a black hole, the black hole didn't show its busy animation. This has changed in Kye. It may be difficult to find a level in which this change is important.
- In fact, the behaviour of black holes in Kye was kind of random / unpredictable. In Xye, their behavior is far more predictable (they always enter the busy animation) and thus more suitable for puzzles.
- In Xye, pushing a round block towards another rounded object / wall may cause the block to slide and take a new position depending on the curve. This is similar to the round arrow blocks. This ability may make some Kye levels far easier to solve.
- The monsters from Kye 2.0 are a little tougher and can find paths towards Xye in more situations than the Kye monsters. The change is not very strong, but you will notice that in monster-centric Kye levels, you need more time to catch the monsters.
- On that matter, I am not sure if the monsters in Kye 2.0 had different personalities like the help file claimed. What is certain is that it is really true in Xye. Ie: Spikes and Gnashers may behave differently when put in the same situation).

## Green

No, it is not green just because the main character is a green cycle. The game is free, open source software. It is also cross platform.

It has been tested on various Linux distributions and windows. There are packages available for FreeBSD. However, we will only provide tar.gz source package and windows zip files. If there are no packages for your operating system, you may try compiling using the tar.gz package, or looking for a binary package for your OS. I try to list all recent packages for the most important OSes in the download page. If you have made or found a binary package for an OS not listed in that page or if you need help compiling for a new OS, feel free to contact vexorian@gmail.com.

## License information

Xye is free, open source software. The license used is a copy of the zlib/libpng license. See [LICENSE](./LICENSE) for more information.

## Game information

The website at [http://xye.sourceforge.net](http://xye.sourceforge.net), provides a very detailed description and screenshots.

### Win32 users

The windows version of xye is portable software, it means that you do not need to install it and can run it directly from the directory you get after extracting the zip file. Double click on Xye.exe (With a green ball icon) to play.

### Linux/*nix users

#### Compiling the game

You might be able to avoid this step if you can find a package of the game for your operating system distribution. There might be some in your repository (look for Xye). Or in the web.

In order to compile Xye you need:

- g++ 4 or greater.
- GNU Make.
- Autoconf / Automake (if you wish to use `autogen.sh`)
- The last items may all be installable by using a package called build-essentials.
- The SDL library. Most *nix systems have an easy to install package for this. Note that since you are compiling a game, you need the `-dev` (development) packages as well.
- In the same note, SDL_Image and SDL_ttf.

Extract the tar.gz file. Then open a terminal and go to the extracted folder. (i.e. type `cd ~/Downloads/xye-0.XX.Y`).

In the command line type: `./configure && make`

IF the last step was succesful, to install type: `make install`

In many systems there might exist a more convenient alternative to `make install`. For example, in debian and ubuntu-based systems, try typing `sudo checkinstall`.

You may also run the game without installing. Just type: &quot;./xye ./&quot; from this directory.

If `./configure` is not succesful, try typing `./autoconf.sh` before `./configure`.

After installing, the `xye` command should run the game.

### Selecting a theme

The first screen you shall see after running the game for the first time is the theme selector. Pick the theme you like the most. If you make the wrong choice, do not worry. You can always return to the theme selector by using the `[F3]` Theme button.

### The level browser

Here you can select a level file. Newcomers shall try the `tutorials.xye` file first.

### The gameplay

Xye is a green ball that can move one cell up, down, left or right in every move. You can use the keyboard cursors to move or you can also use the mouse cursor. I recommend a keyboard because some levels require some speed.

### The levels

Tutorial levels are easy to solve by simple experimentation. You can read hints (The light bulb button) or play solution videos (The film with an S button) if you wish.

The `levels.xye` file holds the official levels. At this moment of beta, the order of the levels may not be optimal. Although level 1 is definitely easier than level 15, level 10 is not necessarily harder than level 4. Either way, Xye is designed in a way that lets you skip any level if you want  to. Whenever you leave a level, the progress is saved and will be loaded automatically when you return to it.

There are plenty of other bonus levels, many made by people in the community using the level editor. Also, the web site might have some other levels that are not included in the game.

### More levels and the editor

Visit this address: [http://xye.sourceforge.net/levels.php](http://xye.sourceforge.net/levels.php) if you wish to have information about the level editor and more levels that you can download and play.

## Charity

Kye was a charityware, although Xye targets to be completely free software, it is fair to add these lines to the readme: if you really enjoy this game and want to give me a good reason to continue improving it. Instead of asking for donations I am gonna ask you to donate to any Children Charity. If you do it BECAUSE OF XYE, and feel like making the authors feel better, email me at vexorian@gmail.com to tell me you did it :) (do not include any private information/proof as that is risky, just tell me you donated to charity because you enjoyed Xye). I will admit that after 6 years of development this has not happened yet.

## Who is responsible for this mess?

The AUTHORS file contains information about all those who contributed to this open project. Admittedly, a single developer does most of the work. If you have suggestions or bug reports please send them to vexorian@gmail.com.

