inherited frmChromosomeEditor: TfrmChromosomeEditor
  Caption = 'Evolutionary Solution Representation Editor'
  ClientHeight = 751
  ClientWidth = 789
  ExplicitWidth = 805
  ExplicitHeight = 790
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Top = 289
    Width = 789
    Height = 462
    ExplicitTop = 289
    ExplicitWidth = 789
    ExplicitHeight = 462
  end
  inherited pnlGTAParameters: TPanel
    Width = 789
    Height = 289
    ExplicitWidth = 789
    ExplicitHeight = 289
    inherited Label5: TLabel
      Left = 34
      Top = 256
      Visible = False
      ExplicitLeft = 34
      ExplicitTop = 256
    end
    inherited lblTime: TLabel
      Left = 141
      Top = 256
      Visible = False
      ExplicitLeft = 141
      ExplicitTop = 256
    end
    object btnSave: TSpeedButton [2]
      Left = 283
      Top = 253
      Width = 87
      Height = 25
      Hint = 'Save the current solution (if feasible and complete) to file'
      Caption = 'Save'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333330070
        7700333333337777777733333333008088003333333377F73377333333330088
        88003333333377FFFF7733333333000000003FFFFFFF77777777000000000000
        000077777777777777770FFFFFFF0FFFFFF07F3333337F3333370FFFFFFF0FFF
        FFF07F3FF3FF7FFFFFF70F00F0080CCC9CC07F773773777777770FFFFFFFF039
        99337F3FFFF3F7F777F30F0000F0F09999937F7777373777777F0FFFFFFFF999
        99997F3FF3FFF77777770F00F000003999337F773777773777F30FFFF0FF0339
        99337F3FF7F3733777F30F08F0F0337999337F7737F73F7777330FFFF0039999
        93337FFFF7737777733300000033333333337777773333333333}
      NumGlyphs = 2
      OnClick = btnSaveClick
    end
    object btnLoad: TSpeedButton [3]
      Left = 180
      Top = 253
      Width = 77
      Height = 25
      Hint = 'Load a solution from file'
      Caption = 'Load'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
        555555FFFFFFFFFF55555000000000055555577777777775FFFF00B8B8B8B8B0
        0000775F5555555777770B0B8B8B8B8B0FF07F75F555555575F70FB0B8B8B8B8
        B0F07F575FFFFFFFF7F70BFB0000000000F07F557777777777570FBFBF0FFFFF
        FFF07F55557F5FFFFFF70BFBFB0F000000F07F55557F777777570FBFBF0FFFFF
        FFF075F5557F5FFFFFF750FBFB0F000000F0575FFF7F777777575700000FFFFF
        FFF05577777F5FF55FF75555550F00FF00005555557F775577775555550FFFFF
        0F055555557F55557F755555550FFFFF00555555557FFFFF7755555555000000
        0555555555777777755555555555555555555555555555555555}
      NumGlyphs = 2
      OnClick = btnLoadClick
    end
    inherited btnRun: TBitBtn
      Left = 405
      Top = 253
      Width = 91
      Caption = 'Interpret'
      OnClick = btnRunClick
      ExplicitLeft = 405
      ExplicitTop = 253
      ExplicitWidth = 91
    end
    inherited btnClose: TBitBtn
      Left = 519
      Top = 253
      Width = 89
      ExplicitLeft = 519
      ExplicitTop = 253
      ExplicitWidth = 89
    end
    object pnlConstructionAlg: TPanel
      Left = 16
      Top = 16
      Width = 753
      Height = 41
      TabOrder = 2
      object Label2: TLabel
        Left = 51
        Top = 13
        Width = 85
        Height = 13
        Caption = 'Job selection rule:'
      end
      object Label3: TLabel
        Left = 302
        Top = 13
        Width = 109
        Height = 13
        Caption = 'Machine selection rule:'
      end
      object btnRunDSG: TSpeedButton
        Left = 583
        Top = 9
        Width = 75
        Height = 25
        Hint = 
          'Run dense schedule generator to construct new solution using the' +
          ' selected job and machine selection rules'
        Caption = 'Run DSG'
        Glyph.Data = {
          76010000424D7601000000000000760000002800000020000000100000000100
          04000000000000010000130B0000130B00001000000000000000000000000000
          800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0033333333B333
          333B33FF33337F3333F73BB3777BB7777BB3377FFFF77FFFF77333B000000000
          0B3333777777777777333330FFFFFFFF07333337F33333337F333330FFFFFFFF
          07333337F3FF3FFF7F333330F00F000F07333337F77377737F333330FFFFFFFF
          07333FF7F3FFFF3F7FFFBBB0F0000F0F0BB37777F7777373777F3BB0FFFFFFFF
          0BBB3777F3FF3FFF77773330F00F000003333337F773777773333330FFFF0FF0
          33333337F3FF7F37F3333330F08F0F0B33333337F7737F77FF333330FFFF003B
          B3333337FFFF77377FF333B000000333BB33337777777F3377FF3BB3333BB333
          3BB33773333773333773B333333B3333333B7333333733333337}
        NumGlyphs = 2
        OnClick = btnRunDSGClick
      end
      object cbxJSR: TComboBox
        Left = 426
        Top = 10
        Width = 98
        Height = 21
        Style = csDropDownList
        TabOrder = 0
        Items.Strings = (
          'FCFS'
          'SPT'
          'LPT'
          'MCT'
          'MOR'
          'LOR'
          'MWR'
          'LWR'
          'RANDOM')
      end
      object cbxMSR: TComboBox
        Left = 155
        Top = 10
        Width = 98
        Height = 21
        Style = csDropDownList
        TabOrder = 1
        Items.Strings = (
          'MIN_LOAD'
          'MINCT'
          'RANDOM')
      end
    end
    object pnlChromosome: TPanel
      Left = 16
      Top = 63
      Width = 753
      Height = 182
      TabOrder = 3
      object Label1: TLabel
        Left = 13
        Top = 7
        Width = 310
        Height = 13
        Caption = 'Solution Represenation for Evolutionary Algorithms (Chromosome):'
      end
      object Label4: TLabel
        Left = 464
        Top = 7
        Width = 35
        Height = 13
        Caption = 'Cmax ='
      end
      object Label6: TLabel
        Left = 592
        Top = 7
        Width = 42
        Height = 13
        Caption = 'MWFT ='
      end
      object lblCmax: TLabel
        Left = 512
        Top = 8
        Width = 57
        Height = 14
        AutoSize = False
      end
      object lblMWFT: TLabel
        Left = 648
        Top = 8
        Width = 57
        Height = 12
        AutoSize = False
      end
      object pcChromosome: TPageControl
        Left = 13
        Top = 28
        Width = 716
        Height = 127
        ActivePage = tsVPBChromosome
        TabOrder = 0
        object tsOBChromosome: TTabSheet
          Caption = 'OB representation'
          object grdOBChromosome: TStringGrid
            Left = 0
            Top = 13
            Width = 708
            Height = 75
            ColCount = 15
            DefaultColWidth = 50
            DefaultRowHeight = 25
            FixedCols = 0
            RowCount = 2
            FixedRows = 0
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
            TabOrder = 0
          end
        end
        object tsRKChromosome: TTabSheet
          Caption = 'RK representation'
          ImageIndex = 1
          object grdRKChromosome: TStringGrid
            Left = 0
            Top = 12
            Width = 708
            Height = 75
            ColCount = 15
            DefaultColWidth = 50
            DefaultRowHeight = 25
            FixedCols = 0
            RowCount = 2
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
            TabOrder = 0
          end
        end
        object tsVPBChromosome: TTabSheet
          Caption = 'VPB representation'
          ImageIndex = 2
          object grdVPBChromosome: TStringGrid
            Left = 0
            Top = 0
            Width = 708
            Height = 99
            Align = alClient
            ColCount = 15
            DefaultColWidth = 44
            DefaultRowHeight = 25
            RowCount = 3
            FixedRows = 0
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
            TabOrder = 0
          end
        end
      end
    end
  end
end
