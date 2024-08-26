object frmNSGAII_turning: TfrmNSGAII_turning
  Left = 0
  Top = 0
  Caption = 'NSGA-II application form'
  ClientHeight = 713
  ClientWidth = 757
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label12: TLabel
    Left = 431
    Top = 196
    Width = 64
    Height = 13
    Caption = 'Generation #'
  end
  object lblGen: TLabel
    Left = 502
    Top = 196
    Width = 82
    Height = 13
    AutoSize = False
  end
  object Label9: TLabel
    Left = 286
    Top = 196
    Width = 18
    Height = 13
    Caption = 'N ='
  end
  object lblN: TLabel
    Left = 317
    Top = 196
    Width = 82
    Height = 13
    AutoSize = False
  end
  object btnStop: TButton
    Left = 85
    Top = 215
    Width = 75
    Height = 25
    Caption = 'Stop'
    TabOrder = 0
    OnClick = btnStopClick
  end
  object Chart: TChart
    Left = 20
    Top = 256
    Width = 729
    Height = 449
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
    Title.Text.Strings = (
      'Solution points')
    Title.Visible = False
    BottomAxis.Automatic = False
    BottomAxis.AutomaticMaximum = False
    BottomAxis.AutomaticMinimum = False
    BottomAxis.LabelsFont.Height = -13
    BottomAxis.LabelsSeparation = 20
    BottomAxis.LabelsSize = 26
    BottomAxis.Maximum = 0.080000000000000000
    BottomAxis.MinorTickCount = 5
    BottomAxis.MinorTickLength = 3
    BottomAxis.TickLength = 8
    BottomAxis.Title.Caption = 'Maximum dimensional quality deviation (mm)'
    BottomAxis.Title.Font.Height = -16
    BottomAxis.Title.Font.Style = [fsBold]
    BottomAxis.TitleSize = 16
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = 0.439999999999999700
    DepthAxis.Minimum = -0.560000000000000300
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.ExactDateTime = False
    LeftAxis.LabelsFont.Height = -13
    LeftAxis.LabelsSeparation = 20
    LeftAxis.LabelsSize = 35
    LeftAxis.Maximum = 0.200000000000000000
    LeftAxis.Minimum = 0.090000000000000000
    LeftAxis.TickLength = 8
    LeftAxis.Title.Caption = 'Throughput (parts/min.)'
    LeftAxis.Title.Font.Height = -16
    LeftAxis.Title.Font.Style = [fsBold]
    LeftAxis.TitleSize = 10
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    View3D = False
    Color = clWhite
    PopupMenu = mnuSaveImage
    TabOrder = 1
    object Series1: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 1 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psCircle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series2: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 1 '
      ClickableLine = False
      Pointer.Brush.Color = clBlue
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psCircle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series3: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 2 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series4: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 2'
      ClickableLine = False
      Pointer.Brush.Color = clRed
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series5: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 3 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
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
    object Series6: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 3'
      ClickableLine = False
      Pointer.Brush.Color = clGreen
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
    object Series7: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 4 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psDiamond
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series8: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 4'
      ClickableLine = False
      Pointer.Brush.Color = 12615935
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psDiamond
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series9: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 5 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psDownTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series10: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 5 '
      ClickableLine = False
      Pointer.Brush.Color = 16711808
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psDownTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series11: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 6 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psLeftTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series12: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 6 '
      ClickableLine = False
      Pointer.Brush.Color = clAqua
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psLeftTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series13: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 7 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psRightTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series14: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 7'
      ClickableLine = False
      Pointer.Brush.Color = 8454143
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psRightTriangle
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series15: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 8 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psHexagon
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series16: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 8'
      ClickableLine = False
      Pointer.Brush.Color = 33023
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Style = psHexagon
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series17: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 9 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clBlue
      Pointer.Style = psDiagCross
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series18: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 9'
      ClickableLine = False
      Pointer.Brush.Color = 33023
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clBlue
      Pointer.Style = psStar
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series19: TPointSeries
      Active = False
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 10 - Dominated'
      ClickableLine = False
      Pointer.Brush.Color = clWhite
      Pointer.Brush.Style = bsClear
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clRed
      Pointer.Pen.EndStyle = esSquare
      Pointer.Style = psDiagCross
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series20: TPointSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      Title = 'N = 10'
      ClickableLine = False
      Pointer.Brush.Color = 33023
      Pointer.HorizSize = 3
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clRed
      Pointer.Style = psStar
      Pointer.VertSize = 3
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object btnRun: TButton
    Left = 85
    Top = 215
    Width = 75
    Height = 25
    Caption = 'Run'
    TabOrder = 2
    OnClick = btnRunClick
  end
  object ProgressBar: TProgressBar
    Left = 183
    Top = 215
    Width = 504
    Height = 25
    TabOrder = 3
  end
  object pgGAType: TPageControl
    Left = 8
    Top = 8
    Width = 741
    Height = 177
    ActivePage = TabSheet1
    TabOrder = 4
    object TabSheet1: TTabSheet
      Caption = 'GA type and niching'
      object Label5: TLabel
        Left = 448
        Top = 43
        Width = 66
        Height = 13
        Caption = 'Niching radius'
      end
      object Label6: TLabel
        Left = 448
        Top = 70
        Width = 65
        Height = 13
        Caption = 'Scaling factor'
      end
      object Label7: TLabel
        Left = 448
        Top = 97
        Width = 59
        Height = 13
        Caption = 'Window size'
      end
      object rgNiching: TRadioGroup
        Left = 185
        Top = 16
        Width = 202
        Height = 113
        Caption = 'Niching'
        ItemIndex = 0
        Items.Strings = (
          'Fitness sharing'
          'Deterministic crowding'
          'Restricted tournament selection')
        TabOrder = 2
      end
      object rgGAType: TRadioGroup
        Left = 48
        Top = 16
        Width = 73
        Height = 113
        Caption = 'GA Type '
        Items.Strings = (
          'SGA'
          'NSGA')
        TabOrder = 0
        OnClick = rgGATypeClick
      end
      object cbxNiching: TCheckBox
        Left = 193
        Top = 15
        Width = 61
        Height = 17
        Caption = 'Niching'
        TabOrder = 1
        OnClick = cbxNichingClick
      end
      object edtNichingRadius: TEdit
        Left = 523
        Top = 40
        Width = 121
        Height = 21
        TabOrder = 3
        Text = 'edtNichingRadius'
      end
      object edtScalingFactor: TEdit
        Left = 523
        Top = 67
        Width = 121
        Height = 21
        TabOrder = 4
        Text = 'edtScalingFactor'
      end
      object edtWindowSize: TEdit
        Left = 523
        Top = 94
        Width = 121
        Height = 21
        TabOrder = 5
        Text = 'edtWindowSize'
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Search, Mutation and Crossover'
      ImageIndex = 2
      object Label10: TLabel
        Left = 580
        Top = 104
        Width = 48
        Height = 13
        Caption = 'pMutation'
      end
      object Label11: TLabel
        Left = 573
        Top = 69
        Width = 55
        Height = 13
        Caption = 'pCrossover'
      end
      object rgCrossover: TRadioGroup
        Left = 379
        Top = 22
        Width = 176
        Height = 107
        Caption = 'Crossover'
        ItemIndex = 0
        Items.Strings = (
          'One-point crossover'
          'Two-point crossover'
          'Uniform crossover'
          'Simulated binary crossover')
        TabOrder = 0
      end
      object rgMutation: TRadioGroup
        Left = 219
        Top = 22
        Width = 145
        Height = 107
        Caption = 'Mutation '
        ItemIndex = 0
        Items.Strings = (
          'Selective mutation'
          'Genewise mutation'
          'Polynomial mutation')
        TabOrder = 1
      end
      object edtpMutation: TEdit
        Left = 634
        Top = 99
        Width = 87
        Height = 21
        TabOrder = 2
        Text = 'edtpMutation'
      end
      object edtpCrossover: TEdit
        Left = 634
        Top = 64
        Width = 87
        Height = 21
        TabOrder = 3
        Text = 'edtpCrossover'
      end
      object chkLocalSearch: TCheckBox
        Left = 590
        Top = 33
        Width = 116
        Height = 17
        Caption = 'Simplex local search'
        TabOrder = 4
      end
      object gbxGeneralParameters: TGroupBox
        Left = 4
        Top = 16
        Width = 201
        Height = 113
        Caption = 'General paramters'
        TabOrder = 5
        object Label2: TLabel
          Left = 49
          Top = 24
          Width = 71
          Height = 13
          Caption = 'population size'
        end
        object Label3: TLabel
          Left = 16
          Top = 51
          Width = 104
          Height = 13
          Caption = 'Maximum generations'
        end
        object Label4: TLabel
          Left = 29
          Top = 75
          Width = 91
          Height = 13
          Caption = 'Replace proportion'
        end
        object edtPopSize: TEdit
          Left = 126
          Top = 21
          Width = 51
          Height = 21
          TabOrder = 0
        end
        object edtMaxGenerations: TEdit
          Left = 126
          Top = 48
          Width = 51
          Height = 21
          TabOrder = 1
        end
        object edtReplaceProportion: TEdit
          Left = 126
          Top = 75
          Width = 51
          Height = 21
          TabOrder = 2
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Scaling and constraint handling'
      ImageIndex = 1
      object rgScaling: TRadioGroup
        Left = 163
        Top = 31
        Width = 148
        Height = 87
        Caption = 'Scaling '
        ItemIndex = 0
        Items.Strings = (
          'No scaling'
          'Linear ranking'
          'Sigma scaling')
        TabOrder = 0
      end
      object rgConstraintHandling: TRadioGroup
        Left = 395
        Top = 31
        Width = 168
        Height = 87
        Caption = 'Constraint handling'
        ItemIndex = 0
        Items.Strings = (
          'Tournament method'
          'Linear penalty method'
          'Quadratic penalty method')
        TabOrder = 1
        OnClick = rgConstraintHandlingClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Selection'
      ImageIndex = 3
      object Label8: TLabel
        Left = 383
        Top = 72
        Width = 79
        Height = 13
        Caption = 'Tournament size'
      end
      object rgSelection: TRadioGroup
        Left = 76
        Top = 20
        Width = 241
        Height = 113
        Caption = 'Selection '
        ItemIndex = 0
        Items.Strings = (
          'Tournament with replacement'
          'Tournament without replacement'
          'Truncation selection'
          'Stochastic Universal selection (SUS)'
          'Roulette-Wheel selection')
        TabOrder = 0
        OnClick = rgSelectionClick
      end
      object edtTournamentSize: TEdit
        Left = 468
        Top = 69
        Width = 105
        Height = 21
        TabOrder = 1
        Text = 'edtTournamentSize'
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Run parameters'
      ImageIndex = 4
      object Label1: TLabel
        Left = 371
        Top = 32
        Width = 84
        Height = 13
        Caption = 'Max. no. of parts'
      end
      object chkOut: TCheckBox
        Left = 410
        Top = 92
        Width = 149
        Height = 17
        Caption = 'Output results in a file'
        TabOrder = 0
      end
      object chkGraphChange: TCheckBox
        Left = 76
        Top = 36
        Width = 177
        Height = 17
        Caption = 'Change graph limits dynamically'
        TabOrder = 1
      end
      object chkOutGraphs: TCheckBox
        Left = 76
        Top = 97
        Width = 136
        Height = 17
        Caption = 'Output graphs to files'
        TabOrder = 2
        OnClick = chkOutGraphsClick
      end
      object edtGOutStep: TEdit
        Left = 202
        Top = 96
        Width = 87
        Height = 21
        TabOrder = 3
        Text = 'edtGOutStep'
      end
      object edtMaxN: TEdit
        Left = 461
        Top = 29
        Width = 105
        Height = 21
        TabOrder = 4
        Text = 'edtMaxN'
      end
    end
  end
  object mnuSaveImage: TPopupMenu
    Left = 16
    Top = 208
    object SaveimageasWMV1: TMenuItem
      Caption = 'Save image as WMV...'
      OnClick = SaveimageasWMV1Click
    end
    object Copyimagetoclipboard1: TMenuItem
      Caption = 'Copy image to clipboard'
      OnClick = Copyimagetoclipboard1Click
    end
  end
  object dlgSaveWMF: TSaveDialog
    Filter = 'WMF (*.wmf)|*.wmf'
    Left = 48
    Top = 208
  end
end
