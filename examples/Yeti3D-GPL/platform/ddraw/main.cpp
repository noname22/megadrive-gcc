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
#include <ddraw.h>
#include <mmsystem.h>

#include "../win32/yeti3dwin32.h"

char* lpszClassName = "Yeti3D";
char* lpszCaption = YETI_STR_TITLE;
char* lpszMessage =
  YETI_STR_TITLE " " YETI_STR_VERSION " (DirectDraw Version)\n"
  "Compilied using Watcom C/C++ v11 (Release Build)\n\n"
  YETI_STR_COPYRIGHT "\n"
  "Website: " YETI_STR_URL "\n";

framebuffer_t framebuffer;
yeti_t yeti;

WNDCLASS wc;
MSG msg;
DDPIXELFORMAT ddpf;
DDSURFACEDESC ddsd;
DDSCAPS ddscaps;
IDirectDrawSurface* lpDDSPrimary;
IDirectDraw* lpDD;
 
void d3d_flip(void)
{
  static unsigned MarkTime;

  if ((int)(MarkTime - timeGetTime()) < 0)
  {
    MarkTime = timeGetTime() + YETI_VIEWPORT_INTERVAL;   

    keyboard_update(&yeti.keyboard);
    game_loop(&yeti);

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if (lpDDSPrimary->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL) == DD_OK)
    {
      viewport_to_video(
        (u16*) ddsd.lpSurface,
        ddsd.lPitch,
        &yeti.viewport,
        ddpf.dwRBitMask,
        ddpf.dwGBitMask,
        ddpf.dwBBitMask);
                  
      lpDDSPrimary->Unlock(ddsd.lpSurface);
    }
  }
}

LRESULT CALLBACK WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_KEYDOWN:
    {
      switch (wParam)
      {
        case VK_ESCAPE:
        {
          DestroyWindow(hWnd);
          break;
        }
      }
      break;
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
    default:
    {
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
  }
  return 0;
}

WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MessageBox(0, lpszMessage, lpszCaption, MB_OK);
  lpszMessage = 0;
  
  wc.style          = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc    = WindowProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = sizeof(DWORD);
  wc.hInstance      = hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = NULL;
  wc.hbrBackground  = GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = lpszClassName;

  if (!RegisterClass(&wc)) lpszMessage = "Could not register class.";
  else
  {
    HWND hWnd = CreateWindow(
      lpszClassName,
      lpszCaption,
      WS_VISIBLE |WS_POPUP,
      0, 0, YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT,
      NULL,
      NULL,
      hInstance,
      NULL);

    if (!hWnd) lpszMessage = "Could not create window.";
    else
    {
      ShowWindow(hWnd, nCmdShow);
      UpdateWindow(hWnd);
      ShowCursor(false);

      if (DirectDrawCreate(NULL, &lpDD, NULL) != DD_OK)
      {
        lpszMessage = "Could not create direct draw interface.";
      }
      else
      {
        lpDD->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT);
        lpDD->SetDisplayMode(YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT, 16);

        ddsd.dwSize  = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        if (lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL) != DD_OK)
        {
          lpszMessage = "Could not create direct draw primary surface.";
        }
        else
        {
          ddpf.dwSize = sizeof(ddpf);
          lpDDSPrimary->GetPixelFormat(&ddpf);
          
          yeti_init(&yeti, &framebuffer, &framebuffer, textures, palette, lua);
          game_init(&yeti);

          while (msg.message != WM_QUIT)
          {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
              TranslateMessage(&msg);
              DispatchMessage(&msg);
            }
            else
            {
              d3d_flip();
            }
          }
          lpDDSPrimary->Release();
        }
        lpDD->Release();
      }
    }
    ShowCursor(true);
  }
  if (lpszMessage)
  {
    MessageBox(0, lpszMessage, lpszCaption, MB_OK);
  }
            
  return msg.wParam;
}









