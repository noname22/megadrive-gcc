/*
Copyright (C) 2003 - Derek John Evans 

This file is part of Yeti3D Portable Engine

Yeti3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 2003 - Derek J. Evans <derek@theteahouse.com.au>
Prepared for public release: 10/24/2003 - Derek J. Evans <derek@theteahouse.com.au>
*/

/*
** Name: Yeti3D
** Desc: Portable GameBoy Advanced 3D Engine
** Auth: Derek J. Evans
**
** Copyright (C) 2003 Derek J. Evans. All Rights Reserved.
*/

The Yeti 3D Engine - 1:35 AM 10/24/2003
=======================================

How to compile
==============

You will need to download and install GCC for the GBA. I currently use 
DevKit Advanced: http://devkitadv.sourceforge.net/index.html

Once you have GCC installed, run the make.bat script to create a new build.

I have GCC installed in the folder "C:\devkitadv\", so if you install somewhere
else then you will need to change the make.bat script a little.

Contacts
========

If you would like a copy of the latest code or to ask what my future plans are,
email myself at:

derek@theteahouse.com.au

At some stage I'd like to optimize the texture mapper for the GBA, but that will
be after im 100% sure the C++ is as fast as it can be.

Contact me if you would like to contribute to the project. The latest version can be
downloaded from:

http://www.theteahouse.com.au/gba/index.html

Engine Technical Notes
======================

These are design notes for the Yeti engine. Other engines use very different algorithms.
Im still refining the engine to improve quality while maintaining a acceptable rendering
speed.

- The current viewport is 120x80 pixels, 15bit.
- All textures are 64x64 8bit. 
- Textures are converted to 15bit via a pre-calculated lighting LUT.
- Polygons can be any convex shape. Only squares are currently used.
- Each vertex is described as X, Y, Z, U, V and brightness.
- The renderer uses 24:8 fixed point maths.
- Polygons are clipped in 3D space using 45 degree planes. Distance to plane
  calculations therefore use only additions and subtractions.
- Polygon edges are clipped using one divide and 6 multiplies.
- 4 clipping planes are used. No front plane is required. No back plane is used.
- No per-span clipping is used. Fixed point errors are hidden offscreen.
- Ray-casting is used to build a visablity list and valid polygon rendering order.
- Models are merged into the VIS without sorting.
- No Z-buffers are used. Rendering is back-to-front (painters algorithm).
- The is an acceptable level of overdraw. Complete polygons are culled. Polygon edges
  are drawn faster than using a per span clipper. 
- Lighting is pre-calculated on startup. Lighting can be moved at runtime.
- Lighting is expanded per vertex and interpolated along polygon edges.
- A reciprocal table is used to eliminate all divides from the DDA texture mapper.
- The affine texture loop is unrolled and renders blocks of 32 pixels.

Kinda Regards
Derek Evans
derek@theteahouse.com.au

1:35 AM 10/24/2003