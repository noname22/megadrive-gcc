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

YETI3D EDITOR
=============

This is the first version of the Yeti3D Editor. Its very ruff, but usable. The
editing keys are the same as the CUBE world editor.

There are two editing modes. Selection and Wall Drawing. (Swap modes using the toolbar buttons).

Wall Drawing is used to get a ruff wall layout using the mouse. Left button draws a wall, 
right button deletes a wall.

Selection allows you to select a range of cells for editing using the keys below.

O       = Move Ceiling down
P       = Move Ceiling up
[       = Move Floor down
]       = Move Floor up

INSERT  = Floor texture id incease.
DELETE  = Floor texture id decrease.
HOME    = Wall texture id increase.
END     = Wall texture id decrease.
PAGE UP = Ceiling texture id increase.
PAGE DN = Ceiling texture id decrease.

L       = Toggle the light switch for the selected cells.
F       = Fill the selected cells with walls.
G       = Clear the selected cells.
E       = Insert entity id. Enter the entity id from 0..255. 0 = no entity. Entities show up as
          little green circles in cells.

ARROWS  = Camera movement.
SPACE   = Camera Jump.
CTRL    = Shoot.

Loading and saving are avaliable from the menu. You will need to export maps to C files so 
they can be reloaded. NOTE: You can not load exported C files. You must save maps as y3d files.

DESIGN TIPS
===========

Start a new map using the NEW command from the menu. Switch to WALL drawing mode using the toolbar 
button.

Sketch a ruff layout of your map plan. ie: Major & minor rooms/locations. Outside/Inside locations 
& hallways/passages linking the player to each. You dont need to use up the entire map, as empty
spaces can later be used for hidden rooms/extra bonus levels.

Give the player a sence of direction & add multiple paths so a player can skip areas that may be 
difficult to pass.

Add lighting. I tend to find lighing door ways the best idea since it allows the player to see
were to go without lighting the entire world. Placing lights 2 cells away from walls gives nicer
spots on the floor. Group lights together to increase the brightness of a light.

WARNINGS!

>>>> MAKE SURE YOU SAVE YOUR MAPS & BACKUP! 
>>>> FOR NOW, DONT EDIT THE EDGES OF THE MAP. LIGHT RAYS WILL SHOOT OUT INTO NOWHERE.

This is a very ruff copy of the editor, so play it safe. Organsize map names from the start. 
ie: E2M1.y3d E1M5.y3d

Mmm, thats about it. This version only has ~30 textures and they are Quake's, so they need to be 
changed. 

Regards
Derek Evans
