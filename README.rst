=======================
Megadrive GCC Toolchain
=======================

About
=====
Build scripts for a GCC 8.2.0 toolchain for Sega Megadrive / Genesis and 32X.

Usage
=====
  * Install depdendencies including gcc and texinfo (makeinfo). ( debian/ubuntu: apt-get install build-essential texinfo )
  * Run toolchain.sh which will download and build gcc for sega megadrive / genesis and x32. It'll probably take a good 30 minutes to finish.
  * After that you can use source /opt/toolchains/gen/sourceme.sh to set up the environment and start compiling the examples. 

Authors
=======
This is all based on a forum post by Chilly Willy on SpritesMind.Net, check it out at

http://gendev.spritesmind.net/forum/viewtopic.php?p=12798

There is also a pre-built windows version of the toolchain in that thread.

githubized by noname@nurd.se

Updated mars.ld from https://github.com/mic-/lusid-dreams .

Updated md.ld from //github.com/kubilus1/gendev .

See also
========
This is just a plain toolchain with a few helper libs provided by Chilly Willy. If you want a more complete SDK take a look at SGDK.

Windows: https://github.com/Stephane-D/SGDK

Linux port: https://github.com/kubilus1/gendev
