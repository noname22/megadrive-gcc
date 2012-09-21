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

#include <windows.h>
#include <mmsystem.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

#include "../win32/yeti3dwin32.h"

framebuffer_t framebuffer;
yeti_t yeti;

GLfloat fogColor[4] = {0.0, 0.5, 0.0, 1};
unsigned texture_lists[255];
u8 texture[64][64][3];
float yeti_to_gl = 1.0 / (256 * 64);

void wglSetDisplayMode(int width, int height, int bpp)
{
  DEVMODE dm;

  dm.dmSize       = sizeof(dm);
  dm.dmPelsWidth  = width;
  dm.dmPelsHeight = height;
  dm.dmBitsPerPel = bpp;
  dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
  
  ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
}

void wglResetDisplaymode(void)
{
  ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
}

void draw_texture(yeti_t* yeti, polyclip_t src, int n, int tid)
{
  int i, x, y;

  if (texture_lists[tid])
  {
    glBindTexture(GL_TEXTURE_2D, texture_lists[tid]);
  }
  else
  {
    glGenTextures(1, &texture_lists[tid]);
    glBindTexture(GL_TEXTURE_2D, texture_lists[tid]);

    for (y = 0; y < 64; y++)
    {
      for (x = 0; x < 64; x++)
      {        
        i = textures[tid][y][x];
        texture[y][x][0] = palette[i][0];
        texture[y][x][1] = palette[i][1];
        texture[y][x][2] = palette[i][2];
      }      
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);      
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);        
  }
  
  glBegin(n == 4 ? GL_QUADS : GL_POLYGON);
  
  for (i = n; i--;)
  {
    vertex_t* v = src[i];
    float c = 1.0 * v->l / i2f(63);
      
    glColor3f(c, c, c);
    glTexCoord2f(v->u * yeti_to_gl, v->v * yeti_to_gl);
    glVertex3f(f2fl(v->x), f2fl(v->y), -f2fl(v->z));
  }
  
  glEnd();
}
 
void CALLBACK IdleFunc()
{
  static unsigned MarkTime;
  
  if ((int)(MarkTime - timeGetTime()) < 0)
  {
    MarkTime = timeGetTime() + YETI_VIEWPORT_INTERVAL;

    keyboard_update(&yeti.keyboard);
   
    glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);
         
    game_loop(&yeti);
    
    glFlush();
    auxSwapBuffers();
  }
}
  
void CALLBACK ReshapeFunc(GLsizei w, GLsizei h)
{
  if (!h) return;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(85.0, 1.0 * w / h, 0.1, 40.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  //glHint (GL_FOG_HINT, GL_FASTEST);
        
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_FOG);
  //glFogi(GL_FOG_MODE, GL_EXP);
  glFogi (GL_FOG_MODE, GL_LINEAR);
  glFogi(GL_FOG_START, 0);
  glFogi(GL_FOG_END, 20);
  glFogfv (GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.05);               

  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);        
}

void CALLBACK MainLoop(void)
{
//
}

void main(void)
{
  int width = 640;
  int height = 480;

  auxInitDisplayMode (AUX_DOUBLE | AUX_RGB);
  auxInitPosition(
    (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
    (GetSystemMetrics(SM_CYSCREEN) - height) / 2,
    width, height);
  auxInitWindow(YETI_STR_CAPTION);

  //ShowCursor(0);
  //SetWindowPos(auxGetHWND(), HWND_TOPMOST, -4, -23, 0, 0, SWP_NOSIZE);
  //wglSetDisplayMode(width, height, 16);

  palette_overbright(palette, palette, i2fdiv2(5));
    
  yeti_init(&yeti, &framebuffer, &framebuffer, textures, palette, lua);
  game_init(&yeti);
  
  auxReshapeFunc(ReshapeFunc);
  auxIdleFunc(IdleFunc);
  
  auxMainLoop(MainLoop);
  
  //wglResetDisplaymode();
}
//---------------------------------------------------------------------------

 
