
License information:
====================

 Check the COPYING.txt file for the license, it is zlib/libpng based so it is highly flexible.


Game information
=================

 http://xye.sourceforge.net


Linux Users
===========

 Linux users should refer to the README file instead of this readme.txt file which is for win32 users.


The Source
==========

 Should use Code::blocks to open and GCC / mingw32 to compile. (project file "src\xye.cbp")

 Also header files <SDL/SDL_image.h> , <SDL/SDL.h> and its libraris.


Playing Xye
===========

 Run xye.exe to play the game.

The editor
==========

 The editor should be simple to use, the less intuitive feature is to rotate objects, in the place where you select what type of object to place, you can use the mouse to rotate the object by holding the mouse button, moving it to the wanted direction and releasing the button.
 
 Once you finish a level you might be interested to use the "save as" button to save it with another file name.


Your levels
===========

A folder with path: "[xye folder]\levels\mylevels" is going to be used to store the levels you make, any level in that location can also be edited later. You may share your level files in the xye google group or by sending them to vexorian@gmail.com



xyeconf.xml guide
=================

A file xyeconf.xml placed in the xye folder allows you to customize xye until we add a non-manual method to access these options.

xyeconf.xml is an XML file with a unique tag , options.

options has some possible attributes:
- levelfile : the location of the level file that you want to play (relative to [xye location]/levels/ )
- levelnumber : the number of level (optional, default is 1)
- skinfile : the skin file (relative to [xye location]/res/ ) use either "default.xml" or "classic.xml"
- red,green,blue : the RGB color for xye (optional , default is pure green)
- undo , if this attribute is set to yes, then the undo command is allowed when playing xye.


Game instructions
=================

Check out the GAMEINTRO.txt for instructions about how to play xye.


More
====

 It is possible to make new skins for the game, check out the default.xml and classic.xml in the res subfolder, you require an editor with the capability of making PNG graphics with transparency.


Charity
=======
  Kye was a charityware, although Xye targets to be completelly free software, it is fair to add these lines to the readme: if you really enjoy this game and want to give me a good reason to continue improving it. Instead of asking for donations I am gonna ask you to donate to any Children Charity.


 Credits and Special thanks
 ===========================
  This sort of information lies in the AUTHORS file


 Version Info
 ============
 Details about version updates lie in the NEWS file

