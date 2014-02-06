=======================
Megadrive GCC Toolchain
=======================

About
=====
Build scripts for a GCC 4.5.2 toolchain for Sega Megadrive / Genesis and 32X.

Usage
=====
  * Install depdendencies including gcc and texinfo (makeinfo). ( debian/ubuntu: apt-get install build-essential texinfo )
  * Run toolchain.sh which will download and build gcc 4.5.2 for sega megadrive / genesis and x32. It'll probably take a good 30 minutes to finish.
  * After that you can use source /opt/toolchains/gen/sourceme.sh to set up the environment and start compiling the examples. 

Authors
=======
This is all based on a forum post by Chilly Willy on SpritesMind.Net, check it out at
http://gendev.spritesmind.net/forum/viewtopic.php?p=12798
There is also a pre-built windows version of the toolchain in that thread.

githubized by noname@nurd.se
