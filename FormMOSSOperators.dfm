inherited frmMOSSOperators: TfrmMOSSOperators
  Caption = 'Experimenting MOSS operators'
  ClientHeight = 774
  ClientWidth = 815
  ExplicitWidth = 831
  ExplicitHeight = 813
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Left = 97
    Top = 409
    Width = 718
    Height = 365
    ExplicitLeft = 97
    ExplicitTop = 409
    ExplicitWidth = 718
    ExplicitHeight = 365
  end
  inherited pnlGTAParameters: TPanel
    Width = 815
    Height = 307
    ExplicitWidth = 815
    ExplicitHeight = 307
    object Label1: TLabel [2]
      Left = 46
      Top = 11
      Width = 71
      Height = 13
      Caption = 'Population size'
    end
    object Label21: TLabel [3]
      Left = 608
      Top = 128
      Width = 38
      Height = 13
      Caption = 'Label21'
    end
    object Label23: TLabel [4]
      Left = 235
      Top = 11
      Width = 78
      Height = 13
      Caption = 'delta threshold ='
    end
    inherited btnRun: TBitBtn
      Left = 571
      Top = 6
      Caption = 'Initialize'
      OnClick = btnRunClick
      ExplicitLeft = 571
      ExplicitTop = 6
    end
    inherited btnClose: TBitBtn
      Left = 679
      Top = 6
      ExplicitLeft = 679
      ExplicitTop = 6
    end
    object cbxPopSize: TComboBox
      Left = 132
      Top = 8
      Width = 79
      Height = 21
      ItemIndex = 0
      TabOrder = 2
      Text = '10'
      Items.Strings = (
        '10'
        '20'
        '30')
    end
    object pcOperators: TPageControl
      Left = 1
      Top = 31
      Width = 813
      Height = 275
      ActivePage = TabSheet1
      Align = alBottom
      TabOrder = 3
      ExplicitLeft = 2
      ExplicitTop = 33
      object tsInitialization: TTabSheet
        Caption = 'Initialization'
        ImageIndex = 3
        object Label17: TLabel
          Left = 346
          Top = 12
          Width = 132
          Height = 13
          Caption = 'Objectives'#39' values statistics:'
        end
        object Label19: TLabel
          Left = 26
          Top = 12
          Width = 94
          Height = 13
          Caption = 'Generated solutions'
        end
        object Label20: TLabel
          Left = 604
          Top = 36
          Width = 84
          Height = 13
          Caption = 'Total generated ='
        end
        object lblnGenerated: TLabel
          Left = 709
          Top = 36
          Width = 66
          Height = 13
          AutoSize = False
          Caption = 'lblnGenerated'
        end
        object Label22: TLabel
          Left = 604
          Top = 64
          Width = 92
          Height = 13
          Caption = 'Number excluded ='
        end
        object lblnExcluded: TLabel
          Left = 709
          Top = 64
          Width = 60
          Height = 13
          AutoSize = False
          Caption = 'lblnExcluded'
        end
        object sgInitialPop: TStringGrid
          Left = 22
          Top = 31
          Width = 286
          Height = 190
          ColCount = 4
          DefaultColWidth = 60
          FixedCols = 0
          TabOrder = 0
          ColWidths = (
            47
            70
            71
            67)
        end
        object sgStats: TStringGrid
          Left = 346
          Top = 35
          Width = 223
          Height = 128
          ColCount = 3
          DefaultColWidth = 50
          TabOrder = 1
          ColWidths = (
            50
            75
            91)
        end
      end
      object tsFronts: TTabSheet
        Caption = 'Initial Fronts'
        ImageIndex = 4
        object ParetoChart: TChart
          Left = 0
          Top = 0
          Width = 805
          Height = 247
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
          BottomAxis.Title.Caption = 'MWFT'
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
          LeftAxis.Title.Caption = 'Cmax'
          LeftAxis.Title.Font.Height = -16
          LeftAxis.Title.Font.Style = [fsBold]
          LeftAxis.TitleSize = 10
          RightAxis.Automatic = False
          RightAxis.AutomaticMaximum = False
          RightAxis.AutomaticMinimum = False
          View3D = False
          Align = alClient
          Color = clWhite
          TabOrder = 0
          object srsFront3: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.BackColor = clRed
            Marks.Color = clRed
            Marks.Visible = False
            Title = 'Front3'
            ClickableLine = False
            Pointer.InflateMargins = True
            Pointer.Style = psTriangle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object srsRest: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            SeriesColor = clRed
            Title = 'Rest'
            ClickableLine = False
            Pointer.InflateMargins = True
            Pointer.Style = psDiagCross
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object srsFront2: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            Title = 'Front2'
            ClickableLine = False
            Pointer.Brush.Color = clLime
            Pointer.InflateMargins = True
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object srsFront1: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            Title = 'Front1'
            ClickableLine = False
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
          object srsFront0: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            Title = 'Front0'
            ClickableLine = False
            Pointer.Brush.Color = clRed
            Pointer.InflateMargins = True
            Pointer.Style = psDiamond
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object srsFront4: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            Title = 'Front4'
            ClickableLine = False
            Pointer.InflateMargins = True
            Pointer.Style = psDownTriangle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object srsFront5: TPointSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = False
            Title = 'Front5'
            ClickableLine = False
            Pointer.Brush.Color = 16777088
            Pointer.InflateMargins = True
            Pointer.Style = psLeftTriangle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
        end
      end
      object tbDistance: TTabSheet
        Caption = 'Distance'
        ImageIndex = 5
        object Label2: TLabel
          Left = 37
          Top = 16
          Width = 92
          Height = 13
          Caption = 'Selected solutioons'
        end
        object Label3: TLabel
          Left = 448
          Top = 16
          Width = 62
          Height = 13
          Caption = 'Delta(Tau) = '
        end
        object lblDeltaTau: TLabel
          Left = 516
          Top = 16
          Width = 57
          Height = 13
          AutoSize = False
          Color = clYellow
          ParentColor = False
        end
        object Label25: TLabel
          Left = 593
          Top = 16
          Width = 58
          Height = 13
          Caption = 'Delta(WC) ='
        end
        object lblDeltaWC: TLabel
          Left = 657
          Top = 16
          Width = 57
          Height = 13
          AutoSize = False
          Color = clYellow
          ParentColor = False
        end
        object cbxDistSol1: TComboBox
          Left = 147
          Top = 13
          Width = 70
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cbxDistSol1Change
        end
        object cbxDistSol2: TComboBox
          Left = 246
          Top = 13
          Width = 62
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbxDistSol2Change
        end
        object grdDistSols: TStringGrid
          Left = 21
          Top = 51
          Width = 752
          Height = 177
          ColCount = 15
          DefaultColWidth = 40
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 6
          FixedRows = 0
          TabOrder = 2
        end
        object btnDistCalc: TButton
          Left = 338
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Distance'
          TabOrder = 3
          OnClick = btnDistCalcClick
        end
      end
      object tsSolComb: TTabSheet
        Caption = 'Solution Combination'
        ImageIndex = 1
        object Label4: TLabel
          Left = 120
          Top = 15
          Width = 33
          Height = 13
          Caption = 'Leader'
        end
        object Label7: TLabel
          Left = 429
          Top = 15
          Width = 81
          Height = 13
          Caption = 'Crossover points:'
        end
        object Label6: TLabel
          Left = 233
          Top = 16
          Width = 39
          Height = 13
          Caption = 'Follower'
        end
        object Label8: TLabel
          Left = 19
          Top = 15
          Width = 35
          Height = 13
          Caption = 'ThetaX'
        end
        object grdParents: TStringGrid
          Left = 22
          Top = 52
          Width = 382
          Height = 181
          ColCount = 15
          DefaultColWidth = 22
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 6
          FixedRows = 0
          TabOrder = 0
        end
        object cbxParent1: TComboBox
          Left = 163
          Top = 12
          Width = 64
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbxParent1Change
        end
        object cbxParent2: TComboBox
          Left = 277
          Top = 12
          Width = 69
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cbxParent2Change
        end
        object grdChild: TStringGrid
          Left = 413
          Top = 104
          Width = 362
          Height = 101
          ColCount = 15
          DefaultColWidth = 22
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 3
          FixedRows = 0
          TabOrder = 3
        end
        object btnApplyXOver: TButton
          Left = 357
          Top = 10
          Width = 55
          Height = 25
          Caption = 'Apply'
          TabOrder = 4
          OnClick = btnApplyXOverClick
        end
        object btnAppendxOverChild: TButton
          Left = 566
          Top = 211
          Width = 89
          Height = 25
          Caption = 'Add to list'
          TabOrder = 5
          OnClick = btnAppendxOverChildClick
        end
        object grdxOverPos: TStringGrid
          Left = 530
          Top = 9
          Width = 216
          Height = 77
          ColCount = 3
          FixedCols = 0
          TabOrder = 6
        end
        object edtThetaX: TEdit
          Left = 61
          Top = 12
          Width = 43
          Height = 21
          TabOrder = 7
          Text = 'edtThetaX'
        end
      end
      object tsTS: TTabSheet
        Caption = 'Tabu Search'
        ImageIndex = 5
        object Label11: TLabel
          Left = 17
          Top = 16
          Width = 54
          Height = 13
          Caption = 'Starting Sol'
        end
        object Label12: TLabel
          Left = 148
          Top = 16
          Width = 52
          Height = 13
          Caption = '# iterations'
        end
        object Label13: TLabel
          Left = 265
          Top = 16
          Width = 46
          Height = 13
          Caption = 'Tabu size'
        end
        object Label16: TLabel
          Left = 550
          Top = 51
          Width = 26
          Height = 13
          Caption = 'Cmax'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblCmax: TLabel
          Left = 582
          Top = 49
          Width = 64
          Height = 16
          AutoSize = False
          Caption = 'lblCmax'
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
        object Label18: TLabel
          Left = 547
          Top = 69
          Width = 33
          Height = 13
          Caption = 'MWFT'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblMWFT: TLabel
          Left = 582
          Top = 67
          Width = 66
          Height = 16
          AutoSize = False
          Caption = 'lblMWFT'
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
        object Label9: TLabel
          Left = 24
          Top = 92
          Width = 76
          Height = 13
          Caption = 'Original Solution'
        end
        object Label10: TLabel
          Left = 412
          Top = 95
          Width = 86
          Height = 13
          Caption = 'Resultant Solution'
        end
        object Label14: TLabel
          Left = 659
          Top = 51
          Width = 49
          Height = 13
          Caption = 'best Cmax'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblBestCmax: TLabel
          Left = 714
          Top = 49
          Width = 64
          Height = 16
          AutoSize = False
          Caption = 'lblBestCmax'
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
        object Label24: TLabel
          Left = 651
          Top = 68
          Width = 56
          Height = 13
          Caption = 'best MWFT'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblBestMWFT: TLabel
          Left = 714
          Top = 68
          Width = 66
          Height = 16
          AutoSize = False
          Caption = 'lblBestMWFT'
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
        object Label15: TLabel
          Left = 413
          Top = 52
          Width = 62
          Height = 13
          Caption = '# of N1 && N2'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblNCmax: TLabel
          Left = 481
          Top = 50
          Width = 64
          Height = 16
          AutoSize = False
          Caption = 'N1+N2'
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
        object Label27: TLabel
          Left = 412
          Top = 70
          Width = 62
          Height = 13
          Caption = '# of N3 && N4'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblNMWFT: TLabel
          Left = 481
          Top = 68
          Width = 66
          Height = 16
          AutoSize = False
          Caption = 'N3+N4'
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
        object lblTSItr: TLabel
          Left = 125
          Top = 68
          Width = 33
          Height = 13
          Caption = 'lblTSItr'
        end
        object Label26: TLabel
          Left = 306
          Top = 51
          Width = 29
          Height = 13
          Caption = 'nCimp'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblnCimp: TLabel
          Left = 344
          Top = 50
          Width = 64
          Height = 16
          AutoSize = False
          Caption = 'nCimp'
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
        object Label29: TLabel
          Left = 307
          Top = 70
          Width = 28
          Height = 13
          Caption = 'nFimp'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object lblnFimp: TLabel
          Left = 344
          Top = 68
          Width = 66
          Height = 16
          AutoSize = False
          Caption = 'nFimp'
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
        object Label28: TLabel
          Left = 383
          Top = 16
          Width = 64
          Height = 13
          Caption = '# best moves'
        end
        object Label31: TLabel
          Left = 518
          Top = 16
          Width = 65
          Height = 13
          Caption = '# rand moves'
        end
        object pbTSIterations: TProgressBar
          Left = 21
          Top = 47
          Width = 269
          Height = 20
          TabOrder = 4
        end
        object cbxTSStartingSol: TComboBox
          Left = 81
          Top = 13
          Width = 52
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cbxTSStartingSolChange
        end
        object btnRunTS: TButton
          Left = 655
          Top = 11
          Width = 55
          Height = 25
          Caption = 'Run'
          TabOrder = 1
          OnClick = btnRunTSClick
        end
        object btnRestartTS: TButton
          Left = 725
          Top = 11
          Width = 53
          Height = 25
          Caption = 'Restart'
          TabOrder = 2
          OnClick = btnRestartTSClick
        end
        object edtTSIterations: TEdit
          Left = 206
          Top = 13
          Width = 47
          Height = 21
          TabOrder = 3
          Text = 'edtTSIterations'
        end
        object edtTabuSize: TEdit
          Left = 323
          Top = 13
          Width = 50
          Height = 21
          TabOrder = 5
          Text = 'edtTabuSize'
        end
        object grdTSOriginalSol: TStringGrid
          Left = 22
          Top = 111
          Width = 364
          Height = 101
          ColCount = 15
          DefaultColWidth = 22
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 3
          FixedRows = 0
          TabOrder = 6
        end
        object grdTSResultantSol: TStringGrid
          Left = 410
          Top = 112
          Width = 365
          Height = 101
          ColCount = 15
          DefaultColWidth = 22
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 3
          FixedRows = 0
          TabOrder = 7
        end
        object btnAppendTSResultantSol: TButton
          Left = 566
          Top = 219
          Width = 89
          Height = 25
          Caption = 'Add to list'
          TabOrder = 8
          OnClick = btnAppendTSResultantSolClick
        end
        object edtnBestMoves: TEdit
          Left = 456
          Top = 13
          Width = 47
          Height = 21
          TabOrder = 9
          Text = 'edtnBestMoves'
        end
        object edtnRandMoves: TEdit
          Left = 591
          Top = 13
          Width = 50
          Height = 21
          TabOrder = 10
          Text = 'edtnRandMoves'
        end
      end
      object TabSheet1: TTabSheet
        Caption = 'MWFT NS'
        ImageIndex = 5
        ExplicitLeft = 7
        object Label32: TLabel
          Left = 17
          Top = 16
          Width = 54
          Height = 13
          Caption = 'Starting Sol'
        end
        object Label33: TLabel
          Left = 340
          Top = 16
          Width = 46
          Height = 13
          Caption = 'Operatoin'
        end
        object cbxMWFTNSStartingSol: TComboBox
          Left = 81
          Top = 13
          Width = 52
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cbxMWFTNSStartingSolChange
        end
        object btnMWFTNSRun: TButton
          Left = 482
          Top = 11
          Width = 175
          Height = 25
          Caption = 'Enumerate Selected Moves'
          TabOrder = 1
          OnClick = btnMWFTNSRunClick
        end
        object cbxMWFTNSOperation: TComboBox
          Left = 399
          Top = 13
          Width = 52
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cbxMWFTNSOperationChange
        end
        object grdMWFTNSResults: TStringGrid
          Left = 168
          Top = 55
          Width = 440
          Height = 162
          ColCount = 6
          DefaultColWidth = 60
          FixedCols = 0
          RowCount = 2
          TabOrder = 3
          ColWidths = (
            45
            51
            62
            64
            95
            90)
        end
        object btnEnumrateAllMWFTNSMoves: TButton
          Left = 168
          Top = 11
          Width = 140
          Height = 25
          Caption = 'Enumerate All Moves'
          TabOrder = 4
          OnClick = btnEnumrateAllMWFTNSMovesClick
        end
      end
    end
    object cbxEnforceDominance: TCheckBox
      Left = 434
      Top = 10
      Width = 124
      Height = 17
      Caption = 'Enforce dominance'
      TabOrder = 4
    end
    object edtDeltaThreshold: TEdit
      Left = 325
      Top = 8
      Width = 89
      Height = 21
      TabOrder = 5
      Text = 'edtDeltaThreshold'
    end
  end
  object pnlPopulation: TPanel
    Left = 0
    Top = 409
    Width = 97
    Height = 365
    Align = alLeft
    TabOrder = 2
    object Label30: TLabel
      Left = 8
      Top = 8
      Width = 50
      Height = 13
      Caption = 'Population'
    end
    object lstPopulation: TListBox
      Left = 8
      Top = 27
      Width = 73
      Height = 326
      ItemHeight = 13
      TabOrder = 0
      OnClick = lstPopulationClick
    end
  end
  object pnlChromosome: TPanel
    Left = 0
    Top = 307
    Width = 815
    Height = 102
    Align = alTop
    TabOrder = 3
    object grdVPBChromosome: TStringGrid
      Left = 1
      Top = 1
      Width = 813
      Height = 100
      Align = alClient
      ColCount = 15
      DefaultColWidth = 40
      DefaultRowHeight = 25
      FixedCols = 0
      RowCount = 3
      FixedRows = 0
      TabOrder = 0
    end
  end
end
