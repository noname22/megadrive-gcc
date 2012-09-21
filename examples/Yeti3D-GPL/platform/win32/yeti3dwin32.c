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
** Auth: Derek J. Evans <derek@theteahouse.com.au>
**
** Copyright (C) 2003 Derek J. Evans. All Rights Reserved.
**
** YY  YY EEEEEE TTTTTT IIIIII 33333  DDDDD
** YY  YY EE       TT     II       33 DD  DD
**  YYYY  EEEE     TT     II     333  DD  DD
**   YY   EE       TT     II       33 DD  DD
**   YY   EEEEEE   TT   IIIIII 33333  DDDDD
*/

#include "yeti3dwin32.h"

/*
** Name: keyboard_update
** Desc: Updates a keyboard structure with the latest keyboard states. These
**       are standard Quake keys.
** Note: Win32 only.
*/
void keyboard_update(keyboard_t* kb)
{
  kb->up     = GetAsyncKeyState(VK_UP) < 0;
  kb->down   = GetAsyncKeyState(VK_DOWN) < 0;
  kb->left   = GetAsyncKeyState(VK_LEFT) < 0;
  kb->right  = GetAsyncKeyState(VK_RIGHT) < 0;
  kb->a      = GetAsyncKeyState(VK_CONTROL) < 0;
  kb->b      = GetAsyncKeyState(' ') < 0;
  kb->l      = GetAsyncKeyState('A') < 0;
  kb->r      = GetAsyncKeyState('Z') < 0;
}

