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

#ifndef UnitMainH
#define UnitMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "../src/game.h"
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
  TStatusBar *StatusBar;
  TToolBar *ToolBar;
  TPanel *PanelClient;
  TScrollBox *ScrollBox;
  TMainMenu *MainMenu;
  TMenuItem *MIFile;
  TMenuItem *MINew;
  TMenuItem *MIOpen;
  TMenuItem *N1;
  TMenuItem *MISave;
  TMenuItem *MISaveAs;
  TMenuItem *N2;
  TMenuItem *MIExit;
  TMenuItem *MIEdit;
  TMenuItem *MIHelp;
  TToolButton *TBNew;
  TToolButton *TBOpen;
  TToolButton *TBSave;
  TToolButton *ToolButton4;
  TToolButton *TBCut;
  TToolButton *TBCopy;
  TToolButton *TBPaste;
  TToolButton *TBDelete;
  TToolButton *ToolButton9;
  TToolButton *TBArrow;
  TToolButton *TBDraw;
  TPanel *PanelScene;
  TPaintBox *PaintBox;
  TMenuItem *MIAbout;
  TImageList *ImageList;
  TSaveDialog *ExportDialog;
  TMenuItem *N3;
  TMenuItem *ExportCFile;
  TSaveDialog *SaveDialog;
  TOpenDialog *OpenDialog;
  TPanel *PanelCamera;
  TMenuItem *MIUndo;
  TMenuItem *MIRedo;
  TMenuItem *N4;
  TMenuItem *MICut;
  TMenuItem *MICopy;
  TMenuItem *MIPaste;
  TMenuItem *MIDelete;
  TMenuItem *N5;
  TMenuItem *MISelectAll;
  TPageControl *PageControl;
  TTabSheet *TSTextures;
  TSplitter *Splitter;
  TListView *ListView;
  TImageList *ILTextures;
  TToolButton *ToolButton1;
  TToolButton *ToolButton2;
  TToolButton *ToolButton3;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall PaintBoxPaint(TObject *Sender);
  void __fastcall ScrollBoxResize(TObject *Sender);
  void __fastcall MIExitClick(TObject *Sender);
  void __fastcall PaintBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall PaintBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall PaintBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall ExportCFileClick(TObject *Sender);
  void __fastcall MISaveAsClick(TObject *Sender);
  void __fastcall MIOpenClick(TObject *Sender);
  void __fastcall MINewClick(TObject *Sender);
  void __fastcall TBNewClick(TObject *Sender);
  void __fastcall TBOpenClick(TObject *Sender);
  void __fastcall TBSaveClick(TObject *Sender);
  void __fastcall MISaveClick(TObject *Sender);
  void __fastcall MIAboutClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall ToolButton2Click(TObject *Sender);
  void __fastcall ToolButton3Click(TObject *Sender);
  void __fastcall ListViewChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
private:	// User declarations
public:		// User declarations
  __fastcall TFormMain(TComponent* Owner);

  bool tc;
  bool Modified;
  String FileName;
  rom_map_t rommap;
  yeti_t* yeti;
  bool LButton;
  bool RButton;
  int Zoom;
  TRect Selection;
  void SetZoom(int AZoom);
  TRect CellRect(int x, int y);
  void CellDraw(int x, int y);
  void SetFileName(String AFileName);
  void SaveToFile(String AFileName);
  void LoadFromFile(String AFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
