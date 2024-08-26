inherited frmMOGWOCalcProgress: TfrmMOGWOCalcProgress
  Caption = 'MOGWO calculation progress'
  ClientHeight = 695
  ClientWidth = 1035
  OnShow = FormShow
  ExplicitTop = -197
  ExplicitWidth = 1041
  ExplicitHeight = 724
  PixelsPerInch = 96
  TextHeight = 13
  inherited btnTerminate: TSpeedButton
    Left = 53
    Top = 408
    ExplicitLeft = 53
    ExplicitTop = 408
  end
  inherited btnStart: TSpeedButton
    Left = 53
    Top = 360
    ExplicitLeft = 53
    ExplicitTop = 360
  end
  inherited btnClose: TSpeedButton
    Left = 53
    Top = 456
    ExplicitLeft = 53
    ExplicitTop = 456
  end
  object lblCmaxLB: TLabel
    Left = 131
    Top = 19
    Width = 76
    Height = 16
    AutoSize = False
    Caption = 'lblCmaxLB'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object Label5: TLabel
    Left = 53
    Top = 19
    Width = 59
    Height = 16
    Caption = 'Cmax LB'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object Label10: TLabel
    Left = 489
    Top = 19
    Width = 71
    Height = 16
    Caption = 'Best Cmax'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object lblbestCmax: TLabel
    Left = 569
    Top = 19
    Width = 86
    Height = 16
    AutoSize = False
    Caption = 'lblbestCmax'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object lblMWFTLB: TLabel
    Left = 334
    Top = 19
    Width = 76
    Height = 16
    AutoSize = False
    Caption = 'lblMWFTLB'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object Label17: TLabel
    Left = 256
    Top = 19
    Width = 64
    Height = 16
    Caption = 'MWFT LB'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object Label18: TLabel
    Left = 719
    Top = 19
    Width = 76
    Height = 16
    Caption = 'Best MWFT'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentFont = False
  end
  object lblbestMWFT: TLabel
    Left = 801
    Top = 19
    Width = 86
    Height = 16
    AutoSize = False
    Caption = 'lblbestMWFT'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'System'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
  end
  object pnlTabuSearch: TPanel
    Left = 767
    Top = 53
    Width = 247
    Height = 215
    BevelOuter = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 61
      Top = 47
      Width = 63
      Height = 13
      Caption = 'Current Cmax'
    end
    object Label2: TLabel
      Left = 51
      Top = 65
      Width = 75
      Height = 13
      Caption = 'Best local Cmax'
    end
    object Label3: TLabel
      Left = 76
      Top = 143
      Width = 48
      Height = 13
      Caption = 'Iteration #'
    end
    object lblSACurrentCmax: TLabel
      Left = 141
      Top = 47
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSACurrentCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object lblSABestCmax: TLabel
      Left = 141
      Top = 65
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSABestCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object lblSAIter: TLabel
      Left = 141
      Top = 143
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSAIter'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label4: TLabel
      Left = 71
      Top = 27
      Width = 53
      Height = 13
      Caption = 'Initial Cmax'
    end
    object lblSAInitialCmax: TLabel
      Left = 141
      Top = 28
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSAInitialCmax'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label7: TLabel
      Left = 28
      Top = 4
      Width = 198
      Height = 16
      Caption = 'Simulated Annealing iterations'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object Label16: TLabel
      Left = 56
      Top = 103
      Width = 70
      Height = 13
      Caption = 'Current MWFT'
    end
    object Label19: TLabel
      Left = 44
      Top = 121
      Width = 82
      Height = 13
      Caption = 'Best local MWFT'
    end
    object lblSACurrentMWFT: TLabel
      Left = 141
      Top = 103
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSACurrentMWFT'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object lblSABestMWFT: TLabel
      Left = 141
      Top = 121
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSABestMWFT'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label22: TLabel
      Left = 66
      Top = 86
      Width = 60
      Height = 13
      Caption = 'Initial MWFT'
    end
    object lblSAInitialMWFT: TLabel
      Left = 141
      Top = 84
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSAInitialMWFT'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object Label6: TLabel
      Left = 64
      Top = 162
      Width = 60
      Height = 13
      Caption = 'Temperature'
    end
    object lblSATemperature: TLabel
      Left = 141
      Top = 162
      Width = 76
      Height = 13
      AutoSize = False
      Caption = 'lblSAIter'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object pbSA: TProgressBar
      Left = 17
      Top = 183
      Width = 217
      Height = 25
      TabOrder = 0
    end
    object cbxSA: TCheckBox
      Left = 8
      Top = 4
      Width = 14
      Height = 17
      TabOrder = 1
    end
  end
  object pnlConstruction: TPanel
    Left = 8
    Top = 53
    Width = 281
    Height = 216
    TabOrder = 1
    object Label8: TLabel
      Left = 24
      Top = 77
      Width = 168
      Height = 13
      Caption = 'List of Initially constructed solutions:'
    end
    object Label9: TLabel
      Left = 17
      Top = 27
      Width = 102
      Height = 13
      Caption = 'Construction progress'
    end
    object Label11: TLabel
      Left = 64
      Top = 3
      Width = 125
      Height = 16
      Caption = 'Construction phase'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object grdInitialSolutions: TStringGrid
      Left = 16
      Top = 96
      Width = 249
      Height = 106
      FixedCols = 0
      TabOrder = 0
      ColWidths = (
        31
        42
        43
        54
        53)
    end
    object pbConstruction: TProgressBar
      Left = 17
      Top = 46
      Width = 216
      Height = 25
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 295
    Top = 54
    Width = 466
    Height = 215
    TabOrder = 2
    object Label12: TLabel
      Left = 7
      Top = 77
      Width = 151
      Height = 13
      Caption = 'Performance measures progress'
    end
    object Label13: TLabel
      Left = 167
      Top = 27
      Width = 48
      Height = 13
      Caption = 'Iteration #'
    end
    object Label14: TLabel
      Left = 161
      Top = 5
      Width = 140
      Height = 16
      Caption = 'Progress of Iterations'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'System'
      Font.Style = []
      ParentFont = False
    end
    object lblGeneration: TLabel
      Left = 235
      Top = 27
      Width = 86
      Height = 13
      AutoSize = False
      Caption = 'lblGeneration'
      Color = clWhite
      ParentColor = False
      Transparent = False
    end
    object sgGenerations: TStringGrid
      Left = 5
      Top = 96
      Width = 449
      Height = 105
      ColCount = 7
      FixedCols = 0
      TabOrder = 0
      ColWidths = (
        43
        62
        62
        64
        62
        66
        60)
    end
    object pbGeneration: TProgressBar
      Left = 72
      Top = 46
      Width = 329
      Height = 25
      TabOrder = 1
    end
    object cbxGenerations: TCheckBox
      Left = 7
      Top = 3
      Width = 25
      Height = 17
      TabOrder = 2
    end
  end
  object cbxConstruction: TCheckBox
    Left = 24
    Top = 57
    Width = 25
    Height = 17
    TabOrder = 3
  end
  object ParetoChart: TChart
    Left = 202
    Top = 294
    Width = 763
    Height = 393
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    Border.Visible = True
    Legend.ColorWidth = 25
    Legend.Font.Height = -13
    Legend.Font.Style = [fsBold]
    Legend.HorizMargin = 17
    Legend.Shadow.HorizSize = 0
    Legend.Shadow.VertSize = 0
    Legend.Symbol.Width = 25
    Legend.TextStyle = ltsPlain
    Legend.Title.Font.Height = -8
    Legend.TopPos = 6
    Legend.VertMargin = 31
    Legend.Visible = False
    Title.Text.Strings = (
      'Solution points')
    Title.Visible = False
    BottomAxis.LabelsFont.Height = -13
    BottomAxis.LabelsSeparation = 20
    BottomAxis.LabelsSize = 26
    BottomAxis.MinorTickCount = 5
    BottomAxis.MinorTickLength = 3
    BottomAxis.TickLength = 8
    BottomAxis.Title.Caption = 'Cmax'
    BottomAxis.Title.Font.Height = -16
    BottomAxis.Title.Font.Style = [fsBold]
    BottomAxis.TitleSize = 16
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = 0.439999999999999700
    DepthAxis.Minimum = -0.560000000000000300
    LeftAxis.ExactDateTime = False
    LeftAxis.LabelsFont.Height = -13
    LeftAxis.LabelsSeparation = 20
    LeftAxis.LabelsSize = 35
    LeftAxis.TickLength = 8
    LeftAxis.Title.Caption = 'MWFT'
    LeftAxis.Title.Font.Height = -16
    LeftAxis.Title.Font.Style = [fsBold]
    LeftAxis.TitleSize = 10
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    View3D = False
    Color = clWhite
    PopupMenu = mnuWMF
    TabOrder = 4
    object psParetoFront: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'Non-dominated solutions'
      ClickableLine = False
      Pointer.Brush.Color = 4227327
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object lsCmaxLB: TLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'Cmax LB'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object lsMWFTLB: TLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      SeriesColor = clRed
      Title = 'MWFT LB'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object psOptimalParetoFront: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.BackColor = clRed
      Marks.Color = clRed
      Marks.Visible = False
      Title = 'Pareto Front'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 5
      Pointer.InflateMargins = True
      Pointer.Style = psDiamond
      Pointer.VertSize = 5
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object psReferenceSet: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      SeriesColor = clRed
      Title = 'Reference set'
      ClickableLine = False
      Pointer.InflateMargins = True
      Pointer.Style = psDiagCross
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object dlgSaveWMF: TSaveDialog
    Filter = 'Windows metafile|*.wmf'
    Left = 104
    Top = 576
  end
  object mnuWMF: TPopupMenu
    Left = 32
    Top = 576
    object ExportWMF1: TMenuItem
      Caption = 'Export WMF...'
      OnClick = ExportWMF1Click
    end
  end
end
