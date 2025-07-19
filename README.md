# Xye readme

## A few notes from the author for context

Xye was both my first C++ project and also the last (for now) project I've released publicly. That was in 2005. It was actually a pleasant surprise over the years to see that the number of people that were happy to play this game well exceeds my initial estimate of 2.

As the years have passed and signs of there being some non-zero interest in t he game persisted, it has become increasingly clear that there is some need to preserve this game. I hope we can gradually modernize some aspects of the way the game and its code is distributed. And also avoid obsolescence somehow. The first step is to move it to a github repository.

Note however, that things like updating its gameplay such as adding new objects are going to be out of the question, at least for this official repository. This is a 20+ years old game and it's staying as such.

## License information

Xye is free, open source software. The license used is a copy of the zlib/libpng license. See [LICENSE](./LICENSE) for more information.

## Game information

The website at [http://xye.sourceforge.net](http://xye.sourceforge.net), provides a very detailed description and screenshots.

## Win32 users

The windows version of xye is portable software, it means that you do not need to install it and can run it directly from the directory you get after extracting the zip file. Double click on Xye.exe (With a green ball icon) to play.

## Linux/*nix users

### Compiling the game

You might be able to avoid this step if you can find a package of the game for your operating system distribution. There might be some in your repository (look for Xye). Or in the web.

In order to compile Xye you need:

- g++ 4 or greater.
- GNU Make.
- Autoconf / Automake (if you wish to use autogen.sh)
- The last items may all be installable by using a package called build-essentials.
- The SDL library (Most *nix systems have an easy to install package  for this. Note that since you are compiling a game, you need the -dev (development) packages as well.
- In the same note, SDL_Image and SDL_ttf.

Extract the tar.gz file. Then open a terminal and go to the extracted folder. (ie: type `cd ~/Downloads/xye-0.XX.Y` (Quotes for clarity).

In the command line type: `./configure && make`

IF the last step was succesful, to install type: make install

In many systems there might exist a more convenient alternative to make install. For example, in debian and ubuntu-based systems, try typing : sudo checkinstall

You may also run the game without installing. Just type: `./xye ./` from this directory.

If `./configure` is not succesful, try typing `./autoconf.sh` before `./configure` .

After installing, the xye command should run the game.

## Selecting a theme

The first screen you shall see after running the game for the first time is the theme selector. Pick the theme you like the most. If you make the wrong choice, do not worry. You can always return to the theme selector by using the [F3] Theme button.

## The level browser

Here you can select a level file. Newcomers shall try the tutorials.xye file first.

## The gameplay

Xye is a green ball that can move one cell up, down, left or right in every move. You can use the keyboard cursors to move or you can also use the mouse cursor. I recommend a keyboard because some levels require some speed.

## The levels

Tutorial levels are easy to solve by simple experimentation. You can read hints (The light bulb button) or play solution videos (The film with an S button) if you wish.

The levels.xye file holds the official levels. At this moment of beta, the order of the levels may not be optimal. Although level 1 is definitely easier than level 15, level 10 is not necessarily harder than level 4. Either way, Xye is designed in a way that lets you skip any level if you want  to. Whenever you leave a level, the progress is saved and will be loaded automatically when you return to it.

There are plenty of other bonus levels, many made by people in the community using the level editor. Also, the web site might have some other levels that are not included in the game.

## More levels and the editor

Visit this address: [http://xye.sourceforge.net/levels.php](http://xye.sourceforge.net/levels.php) if you wish to have information about the level editor and more levels that you can download and play.


# Charity

Kye was a charityware, although Xye targets to be completelly free software, it is fair to add these lines to the readme: if you really enjoy this game and want to give me a good reason to continue improving it. Instead of asking for donations I am gonna ask you to donate to any Children Charity. If you do it BECAUSE OF XYE, and feel like making the authors feel better, email me at vexorian@gmail.com to tell me you did it :) (do not include any private information/proof as that is risky, just tell me you donated to charity because you enjoyed Xye). I will admit that after 6 years of development this has not happened yet.
 
# Who is responsible for this mess?

The AUTHORS file contains information about all those who contributed to this open project. Admitedly, a single developer does most of the work. If you have suggestions or bug reports please send them to vexorian@gmail.com.