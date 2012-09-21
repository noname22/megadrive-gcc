object FormPreview: TFormPreview
  Left = 795
  Top = 451
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Yeti3D Preview Window'
  ClientHeight = 172
  ClientWidth = 221
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Visible = True
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnKeyUp = FormKeyUp
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object Timer: TTimer
    OnTimer = TimerTimer
    Left = 8
    Top = 8
  end
end
