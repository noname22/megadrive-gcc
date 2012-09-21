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

#include "UnitMain.h"
#include "UnitPreview.h"
#include <Math.hpp>
#include "ToolBox.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
  SetFileName("");
  PaintBox->ControlStyle = PaintBox->ControlStyle << csOpaque;
  SetZoom(11);

  Graphics::TBitmap* BM = new Graphics::TBitmap;
  __try
  {
    BM->Width = 64;
    BM->Height = 64;
    
    for (int i = 0; i < YETI_TEXTURE_MAX; i++)
    {
      for (int y = 0; y < 64; y++)
      {
        for (int x = 0; x < 64; x++)
        {
          int c = textures[i][y][x];
          BM->Canvas->Pixels[x][y] = (TColor) RGB(
            palette[c][0], palette[c][1], palette[c][2]);
        }
      }
      TListItem* LI = ListView->Items->Add();
      LI->Caption = String("#") + i;
      LI->ImageIndex = ILTextures->Add(BM, NULL);
    }
  }
  __finally
  {
    BM->Free();
  }
  ListView->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormPaint(TObject *Sender)
{
  OnPaint = NULL;
  FormPreview->SetFocus();
}

void TFormMain::SetZoom(int AZoom)
{
  Zoom = AZoom;
  ScrollBox->OnResize(ScrollBox);
}

TRect TFormMain::CellRect(int x, int y)
{
  return Bounds(x * Zoom, (YETI_MAP_HEIGHT - y - 1) * Zoom, Zoom, Zoom);
}

void TFormMain::CellDraw(int x, int y)
{
  cell_t* cell = &yeti->cells[y][x];

  TRect r = RectNormalize(&Selection);

  if (x >= r.Left && y >= r.Top && x <= r.Right && y <= r.Bottom)
  {
    PaintBox->Canvas->Pen->Color = clWhite;
  }
  else
  {
    PaintBox->Canvas->Pen->Color = clBlack;
  }

  r = CellRect(x, y);

  if (CELL_IS_SOLID(cell))
  {
    PaintBox->Canvas->Brush->Color = clRed;
  }
  else
  {
    int s = ((cell->top - cell->bot) >> 4);

    PaintBox->Canvas->Brush->Color = (TColor) RGB(s, s, s);
  }
  PaintBox->Canvas->Rectangle(r);

  if (cell->swi & CELL_SWI_LIGHT)
  {
    PaintBox->Canvas->Pen->Color = clWhite;
    PaintBox->Canvas->Rectangle(RectInflate(&r, -4, -4));
  }
  if (cell->ent)
  {
    PaintBox->Canvas->Pen->Color = clLime;
    PaintBox->Canvas->Brush->Style = bsClear;
    PaintBox->Canvas->Ellipse(RectInflate(&r, -1, -1));
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxPaint(TObject *Sender)
{
  for (int Y = 0; Y < YETI_MAP_HEIGHT; Y++)
  {
    for (int X = 0; X < YETI_MAP_WIDTH; X++)
    {
      CellDraw(X, Y);  
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ScrollBoxResize(TObject *Sender)
{
  int LWidth  = YETI_MAP_WIDTH  * Zoom;
  int LHeight = YETI_MAP_HEIGHT * Zoom;

  PanelScene->SetBounds(
    Max((ScrollBox->ClientWidth  - LWidth) / 2, 0) - ScrollBox->HorzScrollBar->Position,
    Max((ScrollBox->ClientHeight - LHeight) / 2, 0) - ScrollBox->VertScrollBar->Position,
    LWidth, LHeight);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExitClick(TObject *Sender)
{
  Close();  
}

void cell_block(cell_t* cell, int issolid)
{
  if (issolid)
  {
    if (!CELL_IS_SOLID(cell))
    {
      cell->tos = cell->top;
      cell->bos = cell->bot;
      cell->top = cell->bot = 0;
    }
  }
  else
  {
    if (CELL_IS_SOLID(cell))
    {
      cell->top = cell->tos;
      cell->bot = cell->bos;      
    }
  }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  X = X / Zoom;
  Y = 63 - (Y / Zoom);

  if (TBDraw->Down)
  {
    if (LButton || RButton)
    {
      cell_block(&yeti->cells[Y][X], LButton);
      CellDraw(X, Y);
      yeti_default_lighting(yeti);
      Modified = True;
    }
  }
  if (TBArrow->Down && LButton)
  {
    Selection.Right = X;
    Selection.Bottom = Y;
    
    PaintBox->Repaint();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft ) LButton = True;
  if (Button == mbRight) RButton = True;

  if (TBArrow->Down)
  {
    Selection = Bounds(X / Zoom, 63 - (Y / Zoom), 0, 0);
  }
  
  PaintBoxMouseMove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft ) LButton = False;
  if (Button == mbRight) RButton = False;
}
//---------------------------------------------------------------------------

#define APPLY(A) \
  for (int y = R.top; y <= R.bottom; y++) \
  for (int x = R.left; x <= R.right; x++) \
  {cell_t* cell = &yeti->cells[y][x]; A;}

void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  TRect R = RectNormalize(&Selection);
  unsigned tx = MAX(ListView->ItemIndex,0);

  //ShowMessage(Key);
  switch (Key)
  {
    case 221: {APPLY(cell->bot += 64) Key = 1; break;}
    case 219: {APPLY(cell->bot -= 64) Key = 1; break;}
    //case  36: {APPLY(cell->mid += 64) Key = 1; break;}
    //case  35: {APPLY(cell->mid -= 64) Key = 1; break;}
    case 'P': {APPLY(cell->top += 64) Key = 1; break;}
    case 'O': {APPLY(cell->top -= 64) Key = 1; break;}
    case 'L': {APPLY(cell->swi ^= CELL_SWI_LIGHT) Key = 1; break;}

    case  45: {if(tc)tx=(tx-1)%YETI_TEXTURE_MAX;APPLY(cell->btx=tx);Key=1;break;}
    case  46: {if(tc)tx=(tx+1)%YETI_TEXTURE_MAX;APPLY(cell->btx=tx);Key=1;break;}
    case  36: {if(tc)tx=(tx-1)%YETI_TEXTURE_MAX;APPLY(cell->wtx=tx);Key=1;break;}
    case  35: {if(tc)tx=(tx+1)%YETI_TEXTURE_MAX;APPLY(cell->wtx=tx);Key=1;break;}
    case  33: {if(tc)tx=(tx-1)%YETI_TEXTURE_MAX;APPLY(cell->ttx=tx);Key=1;break;}
    case  34: {if(tc)tx=(tx+1)%YETI_TEXTURE_MAX;APPLY(cell->ttx=tx);Key=1;break;}

    case 'F': {APPLY(cell_block(cell, true )); Key = 1; break;}
    case 'G': {APPLY(cell_block(cell, false)); Key = 1; break;}
    case 'E':
    {
      String Value = yeti->cells[R.top][R.left].ent;
      if (InputQuery("", "Enter Entity ID (0..255):", Value))
      {
        int ent = Value.ToIntDef(0);
        APPLY(cell->ent = ent);
        Key = 1;
      }
      break;
    }
    case  VK_LEFT     : {yeti->keyboard.left  = true; Key = 0; break;}
    case  VK_UP       : {yeti->keyboard.up    = true; Key = 0; break;}
    case  VK_RIGHT    : {yeti->keyboard.right = true; Key = 0; break;}
    case  VK_DOWN     : {yeti->keyboard.down  = true; Key = 0; break;}
    case  VK_CONTROL  : {yeti->keyboard.a     = true; Key = 0; break;}
    case  ' '         : {yeti->keyboard.b     = true; Key = 0; break;}
    case  'A'         : {yeti->keyboard.l     = true; Key = 0; break;}
    case  'Z'         : {yeti->keyboard.r     = true; Key = 0; break;}

    default:
    {
      //ShowMessage(Key);
    }
  }
  
  if (Key == 1)
  {
    Key = 0;
    yeti_default_lighting(yeti);
    PaintBoxPaint(PaintBox);
    FormPreview->TimerTimer(FormPreview->Timer);
    Modified = true;
    ListView->ItemIndex = tx;
    ListView->Selected->MakeVisible(False);
    tc = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch(Key)
  {
    case  VK_LEFT   : {yeti->keyboard.left  = false; Key = 0; break;}
    case  VK_UP     : {yeti->keyboard.up    = false; Key = 0; break;}
    case  VK_RIGHT  : {yeti->keyboard.right = false; Key = 0; break;}
    case  VK_DOWN   : {yeti->keyboard.down  = false; Key = 0; break;}
    case  VK_CONTROL: {yeti->keyboard.a     = false; Key = 0; break;}
    case  ' '       : {yeti->keyboard.b     = false; Key = 0; break;}
    case  'A'       : {yeti->keyboard.l     = false; Key = 0; break;}
    case  'Z'       : {yeti->keyboard.r     = false; Key = 0; break;}
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ExportCFileClick(TObject *Sender)
{
  if (ExportDialog->Execute())
  {
    TStringList* SL = new TStringList;
    __try
    {
      ChangeFileExt(ExtractFileName(ExportDialog->FileName), "");

      SL->Add(Format("YETI_ROM rom_map_t %s = {", ARRAYOFCONST((
        ChangeFileExt(ExtractFileName(ExportDialog->FileName), "")))));

      SL->Add(Format("\"%s\",\"%s\",\"%s\",{", ARRAYOFCONST((
        "Test Map",
        "Derek J. Evans",
        "Welcome to Yeti3D"))));

      for (int y = 0; y < YETI_MAP_HEIGHT; y++)
      {
        SL->Add("{");
        for (int x = 0; x < YETI_MAP_WIDTH; x++)
        {
          cell_t* cell = &yeti->cells[y][x];

          SL->Add(Format("{%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d},", ARRAYOFCONST((
            cell->swi,
            cell->ent,
            cell->top,
            cell->mid,
            cell->bot,
            cell->lit,
            cell->wtx,
            cell->ttx,
            cell->mtx,
            cell->btx,
            0,  // Reserved 1            
            0   // Reserved 2
            ))));
        }
        String S =  SL->Strings[SL->Count - 1];
        SL->Strings[SL->Count - 1] = S.SubString(1, S.Length() - 1);
        SL->Add("},");
      }
      String S =  SL->Strings[SL->Count - 1];
      SL->Strings[SL->Count - 1] = S.SubString(1, S.Length() - 1);
      SL->Add("}};");
      
      SL->SaveToFile(ExportDialog->FileName);
    }
    __finally
    {
      SL->Free();
    }
  }  
}
//---------------------------------------------------------------------------

void TFormMain::SetFileName(String AFileName)
{
  FileName = AFileName;
  if (FileName.Length())
  {
    Caption = Application->Title + " - [" + FileName + "]";
  }
  else
  {
    Caption = Application->Title + " - [untitled]";
  }
  Modified = false;
  PaintBox->Refresh();
}

void TFormMain::LoadFromFile(String AFileName)
{
  TFileStream* FS = new TFileStream(AFileName, fmOpenRead);
  try
  {
    FS->ReadBuffer(&rommap, sizeof(rommap));
    yeti_load_map(yeti, &rommap);
  }
  __finally
  {
    FS->Free();
  }
  SetFileName(AFileName);
}

void TFormMain::SaveToFile(String AFileName)
{
  TFileStream* FS = new TFileStream(AFileName, fmCreate);
  try
  {
    yeti_save_map(yeti, &rommap);
    FS->WriteBuffer(&rommap, sizeof(rommap));
  }
  __finally
  {
    FS->Free();
  }
  SetFileName(AFileName);
}

//---------------------------------------------------------------------------

void __fastcall TFormMain::MISaveClick(TObject *Sender)
{
  if (FileExists(FileName) && !FileIsReadOnly(FileName))
  {
    SaveToFile(FileName);
  }
  else
  {
    MISaveAs->Click();
  }
}

void __fastcall TFormMain::MISaveAsClick(TObject *Sender)
{
  SaveDialog->FileName = FileName;
  if (SaveDialog->Execute()) SaveToFile(SaveDialog->FileName);
}

void __fastcall TFormMain::MIOpenClick(TObject *Sender)
{
  OpenDialog->FileName = FileName;
  if (OpenDialog->Execute())
  {
    bool CanClose;
    FormCloseQuery(this, CanClose);
    if (CanClose)
    {
      LoadFromFile(OpenDialog->FileName);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MINewClick(TObject *Sender)
{
  bool CanClose;
  FormCloseQuery(this, CanClose);
  
  if (CanClose)
  {
    yeti_init_map(yeti);
    yeti_default_lighting(yeti);
    SetFileName("");
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TBNewClick(TObject *Sender)
{
  MINew->Click();  
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TBOpenClick(TObject *Sender)
{
  MIOpen->Click();  
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TBSaveClick(TObject *Sender)
{
  MISave->Click();  
}

//---------------------------------------------------------------------------

void __fastcall TFormMain::MIAboutClick(TObject *Sender)
{
  ShowMessage(YETI_STR_CAPTION);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = True;
  
  if (Modified)
  {
    switch (Application->MessageBox(
      "Current project has been modified. Do you want to save now?",
      Application->Title.c_str(), MB_YESNOCANCEL))
    {
      case IDYES:
      {
        MISaveAs->Click(); // << This should be Save, but SaveAs is safer.
        CanClose = !Modified;
        break;
      }
      case IDNO:
      {
        CanClose = True;
        break;
      }
      case IDCANCEL:
      {
        CanClose = False;
        break;
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolButton2Click(TObject *Sender)
{
  SetZoom(MIN(Zoom + 1, 24));
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ToolButton3Click(TObject *Sender)
{
  SetZoom(MAX(Zoom - 1, 6));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewChange(TObject *Sender, TListItem *Item,
      TItemChange Change)
{
  tc = false;
}
//---------------------------------------------------------------------------

