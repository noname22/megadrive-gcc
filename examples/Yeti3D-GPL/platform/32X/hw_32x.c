/*
 * Licensed under the BSD license
 *
 * debug_32x.c - Debug screen functions.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * Altered for 32X by Chilly Willy
 */

#include "32x.h"
#include "sound.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int X = 0, Y = 0;
static int MX = 40, MY = 25;
static int init = 0;
static unsigned short fgc = 0, bgc = 0;
static unsigned char fgs = 0, bgs = 0;

static unsigned short currentFB = 0;

void Hw32xSetFGColor(int s, int r, int g, int b)
{
    volatile unsigned short *palette = &MARS_CRAM;
    fgs = s;
    fgc = COLOR(r, g, b);
    palette[fgs] = fgc;
}

void Hw32xSetBGColor(int s, int r, int g, int b)
{
    volatile unsigned short *palette = &MARS_CRAM;
    bgs = s;
    bgc = COLOR(r, g, b);
    palette[bgs] = bgc;
}

void Hw32xInit(int vmode, int lineskip)
{
    volatile unsigned short *frameBuffer16 = &MARS_FRAMEBUFFER;
    int i;

    // Wait for the SH2 to gain access to the VDP
    while ((MARS_SYS_INTMSK & MARS_SH2_ACCESS_VDP) == 0) ;

    if (vmode == MARS_VDP_MODE_256)
    {
        // Set 8-bit paletted mode, 224 lines
        MARS_VDP_DISPMODE = MARS_224_LINES | MARS_VDP_MODE_256;

        // init both framebuffers

        // Flip the framebuffer selection bit and wait for it to take effect
        MARS_VDP_FBCTL = currentFB ^ 1;
        while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
        currentFB ^= 1;
        // rewrite line table
        for (i=0; i<224/(lineskip+1); i++)
        {
            int j = lineskip + 1;
            while (j)
            {
                frameBuffer16[i*(lineskip+1) + (lineskip + 1 - j)] = i*160 + 0x100; /* word offset of line */
                j--;
            }
        }
        // clear screen
        for (i=0x100; i<0x10000; i++)
            frameBuffer16[i] = 0;

        // Flip the framebuffer selection bit and wait for it to take effect
        MARS_VDP_FBCTL = currentFB ^ 1;
        while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
        currentFB ^= 1;
        // rewrite line table
        for (i=0; i<224/(lineskip+1); i++)
        {
            int j = lineskip + 1;
            while (j)
            {
                frameBuffer16[i*(lineskip+1) + (lineskip + 1 - j)] = i*160 + 0x100; /* word offset of line */
                j--;
            }
        }
        // clear screen
        for (i=0x100; i<0x10000; i++)
            frameBuffer16[i] = 0;

        MX = 40;
        MY = 28/(lineskip+1);
    }
    else if (vmode == MARS_VDP_MODE_32K)
    {
        // Set 16-bit direct mode, 224 lines
        MARS_VDP_DISPMODE = MARS_224_LINES | MARS_VDP_MODE_32K;

        // init both framebuffers

        // Flip the framebuffer selection bit and wait for it to take effect
        MARS_VDP_FBCTL = currentFB ^ 1;
        while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
        currentFB ^= 1;
        // rewrite line table
        for (i=0; i<224/(lineskip+1); i++)
        {
            if (lineskip)
            {
                int j = lineskip + 1;
                while (j)
                {
                    frameBuffer16[i*(lineskip+1) + (lineskip + 1 - j)] = i*320 + 0x100; /* word offset of line */
                    j--;
                }
            }
            else
            {
                if (i<200)
                    frameBuffer16[i] = i*320 + 0x100; /* word offset of line */
                else
                    frameBuffer16[i] = 200*320 + 0x100; /* word offset of line */
            }
        }
        // clear screen
        for (i=0x100; i<0x10000; i++)
            frameBuffer16[i] = 0;

        // Flip the framebuffer selection bit and wait for it to take effect
        MARS_VDP_FBCTL = currentFB ^ 1;
        while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
        currentFB ^= 1;
        // rewrite line table
        for (i=0; i<224/(lineskip+1); i++)
        {
            if (lineskip)
            {
                int j = lineskip + 1;
                while (j)
                {
                    frameBuffer16[i*(lineskip+1) + (lineskip + 1 - j)] = i*320 + 0x100; /* word offset of line */
                    j--;
                }
            }
            else
            {
                if (i<200)
                    frameBuffer16[i] = i*320 + 0x100; /* word offset of line */
                else
                    frameBuffer16[i] = 200*320 + 0x100; /* word offset of line */
            }
        }
        // clear screen
        for (i=0x100; i<0x10000; i++)
            frameBuffer16[i] = 0;

        MX = 40;
        MY = 25/(lineskip+1);
    }

    Hw32xSetFGColor(255,31,31,31);
    Hw32xSetBGColor(0,0,0,0);
    X = Y = 0;
    init = vmode;
}

int Hw32xScreenGetX()
{
    return X;
}

int Hw32xScreenGetY()
{
    return Y;
}

void Hw32xScreenSetXY(int x, int y)
{
    if( x<MX && x>=0 )
        X = x;
    if( y<MY && y>=0 )
        Y = y;
}

void Hw32xScreenClear()
{
    int i;
    int l = (init == MARS_VDP_MODE_256) ? 320*224/2 + 0x100 : 320*200 + 0x100;
    volatile unsigned short *frameBuffer16 = &MARS_FRAMEBUFFER;

    // clear screen
    for (i=0x100; i<l; i++)
        frameBuffer16[i] = 0;

    // Flip the framebuffer selection bit and wait for it to take effect
    MARS_VDP_FBCTL = currentFB ^ 1;
    while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
    currentFB ^= 1;

    // clear screen
    for (i=0x100; i<l; i++)
        frameBuffer16[i] = 0;

    Hw32xSetFGColor(255,31,31,31);
    Hw32xSetBGColor(0,0,0,0);
    X = Y = 0;
}

extern unsigned char msx[];

static void debug_put_char_16(int x, int y, unsigned char ch)
{
    volatile unsigned short *fb = &MARS_FRAMEBUFFER;
    int i,j;
    unsigned char *font;
    int vram, vram_ptr;

    if(!init)
    {
        return;
    }

    vram = 0x100 + x * 8;
    vram += (y * 8 * 320);

    font = &msx[ (int)ch * 8];

    for (i=0; i<8; i++, font++)
    {
        vram_ptr  = vram;
        for (j=0; j<8; j++)
        {
            if ((*font & (128 >> j)))
                fb[vram_ptr] = fgc;
            else
                fb[vram_ptr] = bgc;
            vram_ptr++;
        }
        vram += 320;
    }
}

static void debug_put_char_8(int x, int y, unsigned char ch)
{
    volatile unsigned char *fb = (volatile unsigned char *)&MARS_FRAMEBUFFER;
    int i,j;
    unsigned char *font;
    int vram, vram_ptr;

    if(!init)
    {
        return;
    }

    vram = 0x200 + x * 8;
    vram += (y * 8 * 320);

    font = &msx[ (int)ch * 8];

    for (i=0; i<8; i++, font++)
    {
        vram_ptr  = vram;
        for (j=0; j<8; j++)
        {
            if ((*font & (128 >> j)))
                fb[vram_ptr] = fgs;
            else
                fb[vram_ptr] = bgs;
            vram_ptr++;
        }
        vram += 320;
    }
}

void Hw32xScreenPutChar(int x, int y, unsigned char ch)
{
    if (init == MARS_VDP_MODE_256)
    {
        debug_put_char_8(x, y, ch);
    }
    else if (init == MARS_VDP_MODE_32K)
    {
        debug_put_char_16(x, y, ch);
    }
}

void Hw32xScreenClearLine(int Y)
{
    int i;

    for (i=0; i < MX; i++)
    {
        Hw32xScreenPutChar(i, Y, ' ');
    }
}

/* Print non-nul terminated strings */
int Hw32xScreenPrintData(const char *buff, int size)
{
    int i;
    char c;

    if(!init)
    {
        return 0;
    }

    for (i = 0; i<size; i++)
    {
        c = buff[i];
        switch (c)
        {
            case '\r':
                X = 0;
                break;
            case '\n':
                X = 0;
                Y++;
                if (Y >= MY)
                    Y = 0;
                Hw32xScreenClearLine(Y);
                break;
            case '\t':
                X = (X + 4) & ~3;
                if (X >= MX)
                {
                    X = 0;
                    Y++;
                    if (Y >= MY)
                        Y = 0;
                    Hw32xScreenClearLine(Y);
                }
                break;
            default:
                Hw32xScreenPutChar(X, Y, c);
                X++;
                if (X >= MX)
                {
                    X = 0;
                    Y++;
                    if (Y >= MY)
                        Y = 0;
                    Hw32xScreenClearLine(Y);
                }
        }
    }

    return i;
}

int Hw32xScreenPuts(const char *str)
{
    int ret;

    // Flip the framebuffer selection bit and wait for it to take effect
    MARS_VDP_FBCTL = currentFB ^ 1;
    while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
    currentFB ^= 1;

    ret = Hw32xScreenPrintData(str, strlen(str));

    // Flip the framebuffer selection bit and wait for it to take effect
    MARS_VDP_FBCTL = currentFB ^ 1;
    while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
    currentFB ^= 1;

    return ret;
}

void Hw32xScreenPrintf(const char *format, ...)
{
   va_list  opt;
   char     buff[256];

   va_start(opt, format);
   vsnprintf(buff, (size_t)sizeof(buff), format, opt);
   va_end(opt);
   Hw32xScreenPuts(buff);
}

void Hw32xDelay(int ticks)
{
    unsigned long ct = MARS_SYS_COMM12 + ticks;
    while (MARS_SYS_COMM12 < ct) ;
}

void Hw32xScreenFlip(int wait)
{
    // Flip the framebuffer selection bit
    MARS_VDP_FBCTL = currentFB ^ 1;
    if (wait)
    {
        while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
        currentFB ^= 1;
    }
}

void Hw32xFlipWait()
{
    while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
    currentFB ^= 1;
}

#include "../../src/viewports.h"

#define SRC_PTR (0x24000200 + YETI_VIEWPORT_Y1*YETI_VIEWPORT_WIDTH*2 +YETI_VIEWPORT_X1*2)
#define SRC_WIDTH (YETI_VIEWPORT_X2 - YETI_VIEWPORT_X1)
#define SRC_HEIGHT (YETI_VIEWPORT_Y2 - YETI_VIEWPORT_Y1)

void task_handler(void)
{
    if (MARS_SYS_COMM4 == 1)
    {
        // double the frame buffer horizontally
        ScreenStretch(SRC_PTR, SRC_WIDTH, SRC_HEIGHT, 1); // stretch with interpolation
        //ScreenStretch(SRC_PTR, SRC_WIDTH, SRC_HEIGHT, 0); // stretch without interpolation

        // Flip the frame buffers.
        Hw32xScreenFlip(1);
    }
#ifdef SLAVE_POLYS
    else if (MARS_SYS_COMM4 == 2)
    {
        int n;
        polygon_t p;

        draw_poly(p, n, yeti->textures[tid], yeti->viewport.back, yeti->lighting[0]);
    }
#endif
    MARS_SYS_COMM4 = 0; // done
}


void slave(void)
{
    uint16_t sample, ix;

    // init DMA
    SH2_DMA_SAR0 = 0;
    SH2_DMA_DAR0 = 0;
    SH2_DMA_TCR0 = 0;
    SH2_DMA_CHCR0 = 0;
    SH2_DMA_DRCR0 = 0;
    SH2_DMA_SAR1 = 0;
    SH2_DMA_DAR1 = 0x20004034; // storing a long here will set left and right
    SH2_DMA_TCR1 = 0;
    SH2_DMA_CHCR1 = 0;
    SH2_DMA_DRCR1 = 0;
    SH2_DMA_DMAOR = 1; // enable DMA

    // init the sound hardware
    MARS_PWM_MONO = 1;
    MARS_PWM_MONO = 1;
    MARS_PWM_MONO = 1;
    if (MARS_VDP_DISPMODE & MARS_NTSC_FORMAT)
        MARS_PWM_CYCLE = (((23011361 << 1)/SAMPLE_RATE + 1) >> 1) + 1; // for NTSC clock
    else
        MARS_PWM_CYCLE = (((22801467 << 1)/SAMPLE_RATE + 1) >> 1) + 1; // for PAL clock
    MARS_PWM_CTRL = 0x0185; // TM = 1, RTP, RMD = right, LMD = left

    sample = 1;
    /* ramp up to SAMPLE_CENTER to avoid click in audio (real 32X) */
    while (sample < SAMPLE_CENTER)
    {
        for (ix=0; ix<SAMPLE_RATE/SAMPLE_CENTER; ix++)
        {
            while (MARS_PWM_MONO & 0x8000) ; // wait while full
            MARS_PWM_MONO = sample;
        }
        sample++;
    }

    while (1)
    {
        if (MARS_SYS_COMM4)
            task_handler();

        if (MARS_SYS_COMM6 == 2)
            continue; // locked by MSH2

        if (MARS_SYS_COMM6 == 0)
        {
            MARS_SYS_COMM6 = 1; // sound subsystem running
            // fill first buffer
            fill_buffer((unsigned long)&snd_buffer);
        }

        // only do sound when sound subsytem initialized and unlocked
        while (MARS_SYS_COMM6 == 1)
        {
            // start DMA on first buffer and fill second
            SH2_DMA_SAR1 = ((unsigned long)&snd_buffer) | 0x20000000;
            SH2_DMA_TCR1 = NUM_SAMPLES; // number longs
            SH2_DMA_CHCR1 = 0x18E1; // dest fixed, src incr, size long, ext req, dack mem to dev, dack hi, dack edge, dreq rising edge, cycle-steal, dual addr, intr disabled, clear TE, dma enabled

            fill_buffer((unsigned long)&snd_buffer + MAX_NUM_SAMPLES * 4);

            // wait on DMA
            while (!(SH2_DMA_CHCR1 & 2)) // wait on TE
            {
                if (MARS_SYS_COMM4)
                    task_handler();
            }

            // start DMA on second buffer and fill first
            SH2_DMA_SAR1 = ((unsigned long)&snd_buffer + MAX_NUM_SAMPLES * 4) | 0x20000000;
            SH2_DMA_TCR1 = NUM_SAMPLES; // number longs
            SH2_DMA_CHCR1 = 0x18E1; // dest fixed, src incr, size long, ext req, dack mem to dev, dack hi, dack edge, dreq rising edge, cycle-steal, dual addr, intr disabled, clear TE, dma enabled

            fill_buffer((unsigned long)&snd_buffer);

            // wait on DMA
            while (!(SH2_DMA_CHCR1 & 2)) // wait on TE
            {
                if (MARS_SYS_COMM4)
                    task_handler();
            }
        }
    }
}
