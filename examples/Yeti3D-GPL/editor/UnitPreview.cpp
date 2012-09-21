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

#include <vcl.h>
#pragma hdrstop

#include "UnitPreview.h"
#include "UnitMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPreview *FormPreview;
//---------------------------------------------------------------------------
__fastcall TFormPreview::TFormPreview(TComponent* Owner): TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormCreate(TObject *Sender)
{
  Timer->Interval = YETI_VIEWPORT_INTERVAL;
  
  Bitmap = new Graphics::TBitmap;

  Bitmap->PixelFormat = pf16bit;
  Bitmap->Width  = YETI_VIEWPORT_WIDTH;
  Bitmap->Height = YETI_VIEWPORT_HEIGHT;

  ClientWidth = Bitmap->Width;
  ClientHeight = Bitmap->Height;
             
  SetBounds(
    Screen->DesktopWidth - Width - 16,
    Screen->DesktopHeight - Height - 16,
    Width, Height);

  yeti_init(&yeti, &framebuffer, &framebuffer, textures, palette, lua);
  game_init(&yeti);
  yeti_default_lighting(&yeti);

  FormMain->yeti = &yeti;
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::TimerTimer(TObject *Sender)
{
  memset(&framebuffer, 0, sizeof(framebuffer));
  game_loop(&yeti);
  viewport_to_video(
    (u16*) Bitmap->ScanLine[0],
    (int) Bitmap->ScanLine[1] - (int) Bitmap->ScanLine[0],
    &yeti.viewport,
    31 << 11, 31 << 6, 31 << 0);
  FormPaint(this);
  FormMain->PanelCamera->SetBounds(
    f2fl(yeti.camera->x) * FormMain->Zoom - 2,
    (YETI_MAP_HEIGHT - f2fl(yeti.camera->y) - 0) * FormMain->Zoom - 2,
    5, 5);

  FormMain->ScrollBox->ScrollInView(FormMain->PanelCamera);
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormPaint(TObject *Sender)
{
  Canvas->Draw(0, 0, Bitmap);
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  FormMain->OnKeyDown(FormMain, Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TFormPreview::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  FormMain->OnKeyUp(FormMain, Key, Shift);
}
//---------------------------------------------------------------------------

