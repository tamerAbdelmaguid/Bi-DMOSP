object frmTuningMOSS: TfrmTuningMOSS
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'DOE runs'
  ClientHeight = 686
  ClientWidth = 616
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
  object Label3: TLabel
    Left = 498
    Top = 517
    Width = 22
    Height = 13
    Caption = 'Time'
  end
  object Label1: TLabel
    Left = 171
    Top = 517
    Width = 18
    Height = 13
    Caption = 'IGD'
  end
  object Label2: TLabel
    Left = 43
    Top = 414
    Width = 24
    Height = 13
    Caption = 'Runs'
  end
  object Label9: TLabel
    Left = 43
    Top = 454
    Width = 106
    Height = 13
    Caption = 'Construction progress'
  end
  object Label15: TLabel
    Left = 43
    Top = 487
    Width = 109
    Height = 13
    Caption = 'Improvement progress'
  end
  object Label4: TLabel
    Left = 59
    Top = 517
    Width = 14
    Height = 13
    Caption = 'GD'
  end
  object btnTerminate: TSpeedButton
    Left = 258
    Top = 361
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
  object Label5: TLabel
    Left = 59
    Top = 160
    Width = 345
    Height = 13
    Caption = 
      '|R|      Th^X       TSize       TSmaxitr     noimp     bstmvs   ' +
      '  rndmvs       |P|'
  end
  object lblTCReadResult: TLabel
    Left = 43
    Top = 320
    Width = 486
    Height = 25
    AutoSize = False
  end
  object Label6: TLabel
    Left = 277
    Top = 517
    Width = 20
    Height = 13
    Caption = 'TGD'
  end
  object Label7: TLabel
    Left = 389
    Top = 517
    Width = 81
    Height = 13
    Caption = 'HV%(P)-HP%(D)'
  end
  object grdLoadedInstances: TStringGrid
    Left = 160
    Top = 20
    Width = 415
    Height = 105
    ColCount = 4
    RowCount = 7
    TabOrder = 0
    ColWidths = (
      54
      196
      58
      81)
    RowHeights = (
      24
      24
      24
      24
      24
      24
      24)
  end
  object btnLoadProblems: TButton
    Left = 24
    Top = 53
    Width = 105
    Height = 25
    Caption = 'Load Problems'
    TabOrder = 1
    OnClick = btnLoadProblemsClick
  end
  object memoTime: TMemo
    Left = 498
    Top = 536
    Width = 77
    Height = 134
    Lines.Strings = (
      'memoTime')
    TabOrder = 7
  end
  object memoIGD: TMemo
    Left = 171
    Top = 536
    Width = 77
    Height = 134
    Lines.Strings = (
      'memIGD')
    TabOrder = 6
  end
  object pbRun: TProgressBar
    Left = 86
    Top = 405
    Width = 503
    Height = 25
    TabOrder = 2
  end
  object pbConstruction: TProgressBar
    Left = 176
    Top = 448
    Width = 413
    Height = 25
    TabOrder = 3
  end
  object pbIterations: TProgressBar
    Left = 176
    Top = 479
    Width = 413
    Height = 25
    TabOrder = 4
  end
  object memoGD: TMemo
    Left = 59
    Top = 536
    Width = 77
    Height = 134
    Lines.Strings = (
      'memoGD')
    TabOrder = 5
  end
  object btnRun: TBitBtn
    Left = 160
    Top = 361
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
    TabOrder = 8
    OnClick = btnRunClick
  end
  object btnClose: TBitBtn
    Left = 368
    Top = 361
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
    TabOrder = 9
  end
  object memoTCs: TMemo
    Left = 52
    Top = 179
    Width = 383
    Height = 118
    Lines.Strings = (
      'memoTCs')
    TabOrder = 10
  end
  object btnReadRuns: TButton
    Left = 472
    Top = 219
    Width = 75
    Height = 25
    Caption = 'Read Runs'
    TabOrder = 11
    OnClick = btnReadRunsClick
  end
  object memoTGD: TMemo
    Left = 277
    Top = 536
    Width = 77
    Height = 134
    Lines.Strings = (
      'memTGD')
    TabOrder = 12
  end
  object memoDHVpr: TMemo
    Left = 389
    Top = 536
    Width = 77
    Height = 134
    Lines.Strings = (
      'memoDHVpr')
    TabOrder = 13
  end
end
