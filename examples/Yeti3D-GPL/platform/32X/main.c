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

32X port by Chilly Willy, 2010
*/

/*
** Name: Yeti3D
** Desc: 32X port
** Auth: Chilly Willy
**
** Copyright (C) 2010, Chilly Willy
**
** YY  YY EEEEEE TTTTTT IIIIII 33333  DDDDD
** YY  YY EE       TT     II       33 DD  DD
**  YYYY  EEEE     TT     II     333  DD  DD
**   YY   EE       TT     II       33 DD  DD
**   YY   EEEEEE   TT   IIIIII 33333  DDDDD
*/

#include "../../src/yeti.h"
#include "../../src/game.h"
#include "32x.h"
#include "hw_32x.h"
#include "module.h"
#include "sound.h"
#include "files.h"

//#define SLAVE_FLIP
#ifndef SLAVE_FLIP
#include "../../src/viewports.h"
#define SRC_PTR (0x24000200 + YETI_VIEWPORT_Y1*YETI_VIEWPORT_WIDTH*2 +YETI_VIEWPORT_X1*2)
#define SRC_WIDTH (YETI_VIEWPORT_X2 - YETI_VIEWPORT_X1)
#define SRC_HEIGHT (YETI_VIEWPORT_Y2 - YETI_VIEWPORT_Y1)
#endif

struct ModTag __attribute__((aligned(16))) mod;

IN_EWRAM yeti_t yeti;

u32 total_frames;
u32 total_polys;

u16 mod_lvl = 0;

int min(int a, int b)
{
  return MIN(a, b);
}

int max(int a, int b)
{
  return MAX(a, b);
}


/*
** Name: yeti_putc
** Desc: Puts a given character onto the screen.
*/
void yeti_putc(int x, int c)
{
  Hw32xScreenPutChar(x%40, x/40, c);
}

/*
** Name: yeti_puts
** Desc: Puts a given string onto the screen.
*/
void yeti_puts(int x, char* s)
{
//  while (*s) yeti_putc(x++, *s++);
  Hw32xScreenSetXY(x%40, x/40);
  Hw32xScreenPrintData(s, strlen(s));
}

void itos(int i, char* s)
{
//#define DIGITS(X) if(i>=X){c=0;do{i-=X;c++;}while(i >= X);*s++='0'+c;}
#define DIGITS(X) for(c=0;i>=X;i-=X,c++);*s++='0'+c;

  int c;
  //DIGITS(100000); DIGITS(10000); DIGITS(1000); DIGITS(100); DIGITS(10);
  DIGITS(10);
  *s++ = '0' + i;
  *s = 0;
}

void ltos(int i, char* s)
{
//#define DIGITS(X) if(i>=X){c=0;do{i-=X;c++;}while(i >= X);*s++='0'+c;}
#define DIGITS(X) for(c=0;i>=X;i-=X,c++);*s++='0'+c;

  int c;
  DIGITS(10000); DIGITS(1000); DIGITS(100); DIGITS(10);
  *s++ = '0' + i;
  *s = 0;
}

void mod_init(u16 lvl)
{
  ExitMOD(&mod);
  InitMOD((uint8_t*)modPtr[lvl], &mod);
  VolumeMOD(12);
  StartMOD(&mod, 1);
}

void mars_init(void)
{
  //Hw32xInit(MARS_VDP_MODE_32K, 0);
  Hw32xInit(MARS_VDP_MODE_32K, 1);
  total_frames = 0;
}

void mars_flip(void)
{
  static u16 pi = 0;
  u16 i;

  i = MARS_SYS_COMM8; // pad value (0 0 0 1 M X Y Z S A C B R L D U) or (0 0 0 0 0 0 0 0 S A C B R L D U)

  yeti.keyboard.a      = (i & SEGA_CTRL_A) ? 1 : 0;
  yeti.keyboard.b      = (i & SEGA_CTRL_B) ? 1 : 0;
  yeti.keyboard.c      = (i & SEGA_CTRL_C) ? 1 : 0;
  yeti.keyboard.start  = (i & SEGA_CTRL_START) ? 1 : 0;
  yeti.keyboard.right  = (i & SEGA_CTRL_RIGHT) ? 1 : 0;
  yeti.keyboard.left   = (i & SEGA_CTRL_LEFT) ? 1 : 0;
  yeti.keyboard.up     = (i & SEGA_CTRL_UP) ? 1 : 0;
  yeti.keyboard.down   = (i & SEGA_CTRL_DOWN) ? 1 : 0;
  yeti.keyboard.x      = (i & SEGA_CTRL_X) ? 1 : 0;
  yeti.keyboard.y      = (i & SEGA_CTRL_Y) ? 1 : 0;
  yeti.keyboard.z      = (i & SEGA_CTRL_Z) ? 1 : 0;
  yeti.keyboard.mode   = (i & SEGA_CTRL_MODE) ? 1 : 0;

  if (!yeti.keyboard.mode && yeti.keyboard._mode)
  {
    // MODE just released
    mod_lvl = (mod_lvl < (NUM_MODS - 1)) ? mod_lvl + 1 : 0;
    mod_init(mod_lvl);
  }

  pi = i;

#ifdef SLAVE_FLIP
  MARS_SYS_COMM4 = 1; // do screen stretch and flip
#else
#ifndef DBL_PIXEL
  // double the frame buffer horizontally
  ScreenStretch(SRC_PTR, SRC_WIDTH, SRC_HEIGHT, 1); // stretch with interpolation
  //ScreenStretch(SRC_PTR, SRC_WIDTH, SRC_HEIGHT, 0); // stretch without interpolation
#endif
  // Flip the frame buffers.
  Hw32xScreenFlip(0);
#endif
  total_frames++;
}

void mars_wait_flip(void)
{
#ifdef SLAVE_FLIP
  while (MARS_SYS_COMM4) ; // wait for slave to finish flipping the display
#else
  Hw32xFlipWait();
#endif
}

void mars_do_fps(u16 rate)
{
  static unsigned int last_frames = 0;
  static unsigned int last_polys = 0;
  static int secs = 0;
  static int ifps = 0;
  static int ipps = 0;
  char fps[12];
  u32 curr_ticks = MARS_SYS_COMM12;

  if ((curr_ticks / rate) != secs)
  {
    ifps = (int)(total_frames - last_frames);
    last_frames = total_frames;
    ipps = (int)(total_polys - last_polys);
    last_polys = total_polys;
    secs = curr_ticks / rate;
  }

  itos(ifps, fps);
  yeti_puts(45, fps);
  ltos(ipps, fps);
  yeti_puts(70, fps);
}

void start_snd(entity_t* e, u16 sfx)
{
  voice_t smp;
  smp.wave = sfxPtr[sfx];
  smp.index = 0;
  smp.step = 1 << 13; // 11kHz -> 22kHz
  smp.loop = 0;
  smp.length = sfxSize[sfx] << 14;
  smp.left = 0x7FFF;
  smp.right = 0x7FFF;
  smp.flags = VF_ENABLED | VF_ONESHOT | VF_PLAYING;

  if (e->visual.sndch != 0)
    SVC_Update(e->visual.sndch - 1, &smp);
  else
    e->visual.sndch = (SVC_Start(&smp) + 1) & 255;
}

void stop_snd(entity_t* e)
{
  voice_t smp;
  memset(&smp, 0xFF, sizeof(voice_t));
  smp.flags = 0; // not playing
  if (e->visual.sndch != 0)
    SVC_Update(e->visual.sndch - 1, &smp);
  e->visual.sndch = 0;
}

u16 check_snd(entity_t* e)
{
  if (e->visual.sndch != 0)
    return SVC_Status(e->visual.sndch - 1);
  else
    return 0;
}

void update_snd(entity_t* e)
{
}


// the game timing is designed around the 32X running slow
// we shoot for 4 VBL ticks per loop (NTSC... 3 for PAL)
// when the drawing makes us slow, we do game ticks until we catch up

int main(void)
{
//  char s[10];
//  u16 rate = (MARS_VDP_DISPMODE & MARS_NTSC_FORMAT) ? 60 : 50;
  u32 speed = (MARS_VDP_DISPMODE & MARS_NTSC_FORMAT) ? 4 : 3;
  u32 ticks = MARS_SYS_COMM12;
  u8 pause = 0;

  mars_init();
  //yeti_init(&yeti, (framebuffer_t*)0x24000200, (framebuffer_t*)0x24000200, (texture_t*)((u32)textures | 0x20000000), (palette_t*)((u32)palette | 0x20000000), (lua_t*)((u32)lua | 0x20000000));
  yeti_init(&yeti, (framebuffer_t*)0x24000200, (framebuffer_t*)0x24000200, textures, palette, lua);
  game_init(&yeti);

  mod_init(0);

  mars_flip();
  while (1)
  {
    if (MARS_SYS_COMM8 & SEGA_CTRL_START)
    {
      // pause/resume
      while (MARS_SYS_COMM8 & SEGA_CTRL_START) ; // wait for START released
      pause ^= 1;
      PauseMOD(&mod, pause);
      if (!pause)
        ticks = MARS_SYS_COMM12;
    }
    if (pause)
    {
        Hw32xDelay(1);
        continue;
    }

    game_tick(&yeti);
    ticks += speed;

    mars_wait_flip();
    game_draw(&yeti);
#if 0
    yeti_puts(40 + 10, "Yeti3D Engine (32X)");
    yeti_puts(80 + 9, "(C) 2003 Derek/Torlus");
#ifdef SHOW_FPS
    yeti_puts(41, "FPS:");
    mars_do_fps(rate);
#endif
#endif
    mars_flip();

    while (ticks < MARS_SYS_COMM12)
    {
      game_tick(&yeti);
      ticks += speed;
    }
  }
  return 0;
}

