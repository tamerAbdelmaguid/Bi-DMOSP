object frmSolvedSetMultiRuns: TfrmSolvedSetMultiRuns
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Multiple runs of developed metaheuristics for solved instances'
  ClientHeight = 334
  ClientWidth = 570
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 304
    Top = 210
    Width = 30
    Height = 13
    Caption = 'Run #'
  end
  object btnTerminate: TSpeedButton
    Left = 237
    Top = 289
    Width = 89
    Height = 25
    Caption = 'Terminate'
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000130B0000130B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333FFFFF3333333333999993333333333F77777FFF333333999999999
      33333337777FF377FF3333993370739993333377FF373F377FF3399993000339
      993337777F777F3377F3393999707333993337F77737333337FF993399933333
      399377F3777FF333377F993339903333399377F33737FF33377F993333707333
      399377F333377FF3377F993333101933399377F333777FFF377F993333000993
      399377FF3377737FF7733993330009993933373FF3777377F7F3399933000399
      99333773FF777F777733339993707339933333773FF7FFF77333333999999999
      3333333777333777333333333999993333333333377777333333}
    NumGlyphs = 2
    OnClick = btnTerminateClick
  end
  object lblRunNo: TLabel
    Left = 349
    Top = 210
    Width = 25
    Height = 13
    AutoSize = False
    Caption = '0'
    Color = clWhite
    ParentColor = False
  end
  object Label1: TLabel
    Left = 47
    Top = 22
    Width = 95
    Height = 13
    Caption = 'Groups of instances'
  end
  object Label3: TLabel
    Left = 205
    Top = 22
    Width = 69
    Height = 13
    Caption = 'Test instances'
  end
  object Label4: TLabel
    Left = 409
    Top = 210
    Width = 45
    Height = 13
    Caption = 'Algorithm'
  end
  object lblAlg: TLabel
    Left = 461
    Top = 210
    Width = 74
    Height = 13
    AutoSize = False
    Caption = '0'
    Color = clWhite
    ParentColor = False
  end
  object Label6: TLabel
    Left = 41
    Top = 210
    Width = 42
    Height = 13
    Caption = 'Instance'
  end
  object lblInstName: TLabel
    Left = 89
    Top = 210
    Width = 185
    Height = 13
    AutoSize = False
    Caption = '0'
    Color = clWhite
    ParentColor = False
  end
  object pbRun: TProgressBar
    Left = 29
    Top = 237
    Width = 503
    Height = 25
    TabOrder = 0
  end
  object btnRun: TBitBtn
    Left = 115
    Top = 289
    Width = 75
    Height = 25
    Caption = 'Execute'
    DoubleBuffered = True
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      0400000000000001000000000000000000001000000010000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555550FF0559
      1950555FF75F7557F7F757000FF055591903557775F75557F77570FFFF055559
      1933575FF57F5557F7FF0F00FF05555919337F775F7F5557F7F700550F055559
      193577557F7F55F7577F07550F0555999995755575755F7FFF7F5570F0755011
      11155557F755F777777555000755033305555577755F75F77F55555555503335
      0555555FF5F75F757F5555005503335505555577FF75F7557F55505050333555
      05555757F75F75557F5505000333555505557F777FF755557F55000000355557
      07557777777F55557F5555000005555707555577777FF5557F55553000075557
      0755557F7777FFF5755555335000005555555577577777555555}
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = btnRunClick
  end
  object btnClose: TBitBtn
    Left = 372
    Top = 289
    Width = 75
    Height = 25
    Caption = 'Close'
    DoubleBuffered = True
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00388888888877
      F7F787F8888888888333333F00004444400888FFF444448888888888F333FF8F
      000033334D5007FFF4333388888888883338888F0000333345D50FFFF4333333
      338F888F3338F33F000033334D5D0FFFF43333333388788F3338F33F00003333
      45D50FEFE4333333338F878F3338F33F000033334D5D0FFFF43333333388788F
      3338F33F0000333345D50FEFE4333333338F878F3338F33F000033334D5D0FFF
      F43333333388788F3338F33F0000333345D50FEFE4333333338F878F3338F33F
      000033334D5D0EFEF43333333388788F3338F33F0000333345D50FEFE4333333
      338F878F3338F33F000033334D5D0EFEF43333333388788F3338F33F00003333
      4444444444333333338F8F8FFFF8F33F00003333333333333333333333888888
      8888333F00003333330000003333333333333FFFFFF3333F00003333330AAAA0
      333333333333888888F3333F00003333330000003333333333338FFFF8F3333F
      0000}
    ModalResult = 11
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 2
  end
  object lstGroups: TListBox
    Left = 47
    Top = 41
    Width = 121
    Height = 136
    ItemHeight = 13
    Items.Strings = (
      'Group 1 (3 instances)'
      'Group 2 (3 instances)'
      'Group 3 (3 instances)'
      'Group 4 (3 instances)'
      'Group 5 (3 instances)'
      'Group 6 (3 instances)'
      'Group 7 (3 instances)'
      'Group 8 (3 instances)'
      'Group 9 (3 instances)'
      'Group 10 (3 instances)')
    TabOrder = 3
    OnClick = lstGroupsClick
  end
  object lstInstances: TListBox
    Left = 205
    Top = 41
    Width = 180
    Height = 136
    ItemHeight = 13
    TabOrder = 4
  end
  object cbxNSGA2: TCheckBox
    Left = 421
    Top = 72
    Width = 97
    Height = 17
    Caption = 'NSGA-II'
    TabOrder = 5
  end
  object cbxMOGWO: TCheckBox
    Left = 421
    Top = 136
    Width = 97
    Height = 17
    Caption = 'MOGWO'
    TabOrder = 6
  end
  object cbxMOSS: TCheckBox
    Left = 421
    Top = 104
    Width = 97
    Height = 17
    Caption = 'MOSS'
    TabOrder = 7
  end
end
