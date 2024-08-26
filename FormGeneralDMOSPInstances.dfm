object frmGeneralDMOSPInstances: TfrmGeneralDMOSPInstances
  Left = 0
  Top = 0
  Caption = 'Generating general DMOSP instances'
  ClientHeight = 460
  ClientWidth = 434
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 187
    Top = 135
    Width = 41
    Height = 13
    Caption = '|J| / |W|'
  end
  object Label1: TLabel
    Left = 85
    Top = 104
    Width = 143
    Height = 13
    Caption = 'Number of workstations (|W|)'
  end
  object Label3: TLabel
    Left = 187
    Top = 165
    Width = 39
    Height = 13
    Caption = 'qw level'
  end
  object Label4: TLabel
    Left = 142
    Top = 197
    Width = 86
    Height = 13
    Caption = 'Loading Level (LL)'
  end
  object Label5: TLabel
    Left = 36
    Top = 228
    Width = 192
    Height = 13
    Caption = 'Processing time adjustment factor (PTF)'
  end
  object Label6: TLabel
    Left = 127
    Top = 387
    Width = 99
    Height = 13
    Caption = 'Number of replicates'
  end
  object Label7: TLabel
    Left = 86
    Top = 287
    Width = 140
    Height = 13
    Caption = 'Job release time factor (RTF)'
  end
  object Label8: TLabel
    Left = 57
    Top = 346
    Width = 169
    Height = 13
    Caption = 'Machine available time factor (ATF)'
  end
  object Label9: TLabel
    Left = 114
    Top = 258
    Width = 112
    Height = 13
    Caption = 'Percentage of late jobs'
  end
  object Label10: TLabel
    Left = 85
    Top = 315
    Width = 141
    Height = 13
    Caption = 'Percentage of busy machines'
  end
  object Label11: TLabel
    Left = 358
    Top = 106
    Width = 6
    Height = 13
    Caption = '*'
  end
  object Label12: TLabel
    Left = 358
    Top = 131
    Width = 6
    Height = 13
    Caption = '*'
  end
  object Label13: TLabel
    Left = 358
    Top = 197
    Width = 6
    Height = 13
    Caption = '*'
  end
  object Label14: TLabel
    Left = 358
    Top = 258
    Width = 6
    Height = 13
    Caption = '*'
  end
  object Label15: TLabel
    Left = 358
    Top = 315
    Width = 6
    Height = 13
    Caption = '*'
  end
  object btnClose: TBitBtn
    Left = 246
    Top = 421
    Width = 131
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
    ModalResult = 2
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object btnGenerate: TBitBtn
    Left = 67
    Top = 421
    Width = 137
    Height = 25
    Caption = 'Generate Instances'
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
    OnClick = btnGenerateClick
  end
  object cbxNoOfPCs: TComboBox
    Left = 245
    Top = 101
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 2
    Text = '6'
    Items.Strings = (
      '6'
      '12')
  end
  object cbxnJobsTonWCs: TComboBox
    Left = 245
    Top = 128
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 3
    Text = '2'
    Items.Strings = (
      '2'
      '4')
  end
  object cbxqwLevel: TComboBox
    Left = 245
    Top = 162
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 4
    Text = '1 to 2'
    Items.Strings = (
      '1 to 2'
      '3 to 5'
      '6 to 8')
  end
  object cbxLoadingLevel: TComboBox
    Left = 246
    Top = 194
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 5
    Text = '0.5'
    Items.Strings = (
      '0.5'
      '0.8')
  end
  object cbxPTF: TComboBox
    Left = 246
    Top = 225
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 6
    Text = '0.95 to 1.05'
    Items.Strings = (
      '0.95 to 1.05'
      '0.75 to 1.25')
  end
  object edtnReplicates: TEdit
    Left = 245
    Top = 384
    Width = 60
    Height = 21
    TabOrder = 7
    Text = '5'
  end
  object rgProblemType: TRadioGroup
    Left = 142
    Top = 12
    Width = 172
    Height = 69
    Caption = 'Select problem type'
    ItemIndex = 0
    Items.Strings = (
      'Small instance (for CPLEX)'
      'Regular instance')
    TabOrder = 8
    OnClick = rgProblemTypeClick
  end
  object cbxJRTF: TComboBox
    Left = 245
    Top = 284
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 9
    Text = '0.95 to 1.05'
    Items.Strings = (
      '0.95 to 1.05'
      '0.75 to 1.25')
  end
  object cbxMATF: TComboBox
    Left = 245
    Top = 343
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 10
    Text = '0.95 to 1.05'
    Items.Strings = (
      '0.95 to 1.05'
      '0.75 to 1.25')
  end
  object cbxPercentLateJobs: TComboBox
    Left = 245
    Top = 255
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 11
    Text = '10%'
    Items.Strings = (
      '10%'
      '30%'
      '50%')
  end
  object cbxPercentBusyMachines: TComboBox
    Left = 245
    Top = 312
    Width = 97
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 12
    Text = '10%'
    Items.Strings = (
      '10%'
      '30%'
      '50%')
  end
end
