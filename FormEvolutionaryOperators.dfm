inherited frmEvolOperators: TfrmEvolOperators
  Left = 165
  Top = 123
  Caption = 'Evolutionary Operators'
  ClientHeight = 774
  ClientWidth = 815
  Position = poScreenCenter
  ExplicitWidth = 831
  ExplicitHeight = 813
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Left = 97
    Top = 397
    Width = 718
    Height = 377
    ExplicitLeft = 97
    ExplicitTop = 397
    ExplicitWidth = 718
    ExplicitHeight = 377
  end
  inherited pnlGTAParameters: TPanel
    Width = 815
    Height = 323
    ExplicitWidth = 815
    ExplicitHeight = 323
    inherited Label5: TLabel
      Left = 657
      Top = 304
      Visible = False
      ExplicitLeft = 657
      ExplicitTop = 304
    end
    inherited lblTime: TLabel
      Left = 765
      Top = 304
      Visible = False
      ExplicitLeft = 765
      ExplicitTop = 304
    end
    object Label1: TLabel [2]
      Left = 188
      Top = 11
      Width = 71
      Height = 13
      Caption = 'Population size'
    end
    inherited btnRun: TBitBtn
      Left = 382
      Top = 6
      Width = 97
      Caption = 'Construct'
      OnClick = btnRunClick
      ExplicitLeft = 382
      ExplicitTop = 6
      ExplicitWidth = 97
    end
    inherited btnClose: TBitBtn
      Left = 533
      Top = 8
      Width = 97
      Kind = bkCustom
      ModalResult = 11
      ExplicitLeft = 533
      ExplicitTop = 8
      ExplicitWidth = 97
    end
    object cbxPopSize: TComboBox
      Left = 274
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
    object pcEvolOperators: TPageControl
      Left = 1
      Top = 50
      Width = 813
      Height = 272
      ActivePage = tsSA
      Align = alBottom
      TabOrder = 3
      object tsInitialization: TTabSheet
        Caption = 'Initialization'
        ImageIndex = 3
        object sgInitialPop: TStringGrid
          Left = 62
          Top = 16
          Width = 345
          Height = 209
          ColCount = 7
          DefaultColWidth = 60
          FixedCols = 0
          TabOrder = 0
          ColWidths = (
            38
            42
            42
            43
            43
            42
            64)
        end
        object sgStats: TStringGrid
          Left = 457
          Top = 16
          Width = 265
          Height = 137
          DefaultColWidth = 50
          TabOrder = 1
        end
      end
      object tsFronts: TTabSheet
        Caption = 'Fronts'
        ImageIndex = 4
        object ParetoChart: TChart
          Left = 0
          Top = 0
          Width = 805
          Height = 244
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
      object tsSelection: TTabSheet
        Caption = 'Selection'
        object Label2: TLabel
          Left = 24
          Top = 64
          Width = 102
          Height = 13
          Caption = 'Resultant mating pool'
        end
        object Label3: TLabel
          Left = 24
          Top = 29
          Width = 87
          Height = 13
          Caption = 'Tournament size ='
        end
        object btnSelect: TButton
          Left = 209
          Top = 24
          Width = 169
          Height = 25
          Caption = 'Apply Tournament Selection'
          TabOrder = 0
          OnClick = btnSelectClick
        end
        object sgMatingPool: TStringGrid
          Left = 24
          Top = 83
          Width = 745
          Height = 62
          ColCount = 15
          DefaultColWidth = 40
          DefaultRowHeight = 40
          FixedCols = 0
          RowCount = 1
          FixedRows = 0
          TabOrder = 1
        end
        object edtTournamentSize: TEdit
          Left = 125
          Top = 26
          Width = 52
          Height = 21
          TabOrder = 2
          Text = '2'
        end
      end
      object tsCrossover: TTabSheet
        Caption = 'Crossover'
        ImageIndex = 1
        object Label4: TLabel
          Left = 40
          Top = 16
          Width = 39
          Height = 13
          Caption = 'Parents:'
        end
        object Label6: TLabel
          Left = 28
          Top = 134
          Width = 89
          Height = 13
          Caption = 'Crossover operator'
        end
        object Label7: TLabel
          Left = 368
          Top = 136
          Width = 81
          Height = 13
          Caption = 'Crossover points:'
        end
        object lblxOverPoints: TLabel
          Left = 455
          Top = 136
          Width = 313
          Height = 13
          AutoSize = False
          Caption = 'lblxOverPoints'
          Color = clWhite
          ParentColor = False
        end
        object grdParents: TStringGrid
          Left = 30
          Top = 48
          Width = 724
          Height = 72
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 2
          FixedRows = 0
          TabOrder = 0
        end
        object cbxParent1: TComboBox
          Left = 108
          Top = 13
          Width = 101
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbxParent1Change
        end
        object cbxParent2: TComboBox
          Left = 247
          Top = 13
          Width = 101
          Height = 21
          Style = csDropDownList
          TabOrder = 2
          OnChange = cbxParent2Change
        end
        object grdChildren: TStringGrid
          Left = 30
          Top = 162
          Width = 724
          Height = 72
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 2
          FixedRows = 0
          TabOrder = 3
        end
        object cbxXOverOperator: TComboBox
          Left = 125
          Top = 130
          Width = 154
          Height = 21
          Style = csDropDownList
          TabOrder = 4
          OnChange = cbxParent1Change
          Items.Strings = (
            'Single-point OX (OX-1pt)'
            'Two-point OX (OX-2pt)'
            'Uniform OX (OX-u)'
            'Gen. position (GPX)'
            'SB2OX')
        end
        object btnApplyXOver: TButton
          Left = 299
          Top = 130
          Width = 55
          Height = 25
          Caption = 'Apply'
          TabOrder = 5
          OnClick = btnApplyXOverClick
        end
        object btnRestart: TButton
          Left = 679
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Restart'
          TabOrder = 6
          OnClick = btnRestartClick
        end
      end
      object tsMutation: TTabSheet
        Caption = 'Mutation'
        ImageIndex = 2
        object Label8: TLabel
          Left = 40
          Top = 16
          Width = 99
          Height = 13
          Caption = 'Original Chromosome'
        end
        object Label9: TLabel
          Left = 33
          Top = 134
          Width = 83
          Height = 13
          Caption = 'Mutation operator'
        end
        object Label10: TLabel
          Left = 368
          Top = 134
          Width = 134
          Height = 13
          Caption = 'Mutation points/workstation:'
        end
        object lblMutationPoints: TLabel
          Left = 512
          Top = 134
          Width = 176
          Height = 13
          AutoSize = False
          Caption = 'lblMutationPoints'
          Color = clWhite
          ParentColor = False
        end
        object grdOriginalChromosome: TStringGrid
          Left = 30
          Top = 55
          Width = 724
          Height = 47
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 1
          FixedRows = 0
          TabOrder = 0
        end
        object cbxOriginalChromosome: TComboBox
          Left = 155
          Top = 13
          Width = 101
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbxOriginalChromosomeChange
        end
        object grdMutatedChromosome: TStringGrid
          Left = 30
          Top = 176
          Width = 724
          Height = 48
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 1
          FixedRows = 0
          TabOrder = 2
        end
        object cbxMutationOperator: TComboBox
          Left = 128
          Top = 131
          Width = 154
          Height = 21
          Style = csDropDownList
          TabOrder = 3
          OnChange = cbxParent1Change
          Items.Strings = (
            'Swap Mutation'
            'Shift Mutation'
            'RLS Mutation')
        end
        object btnApplyMutation: TButton
          Left = 298
          Top = 129
          Width = 55
          Height = 25
          Caption = 'Apply'
          TabOrder = 4
          OnClick = btnApplyMutationClick
        end
        object btnRestartMutation: TButton
          Left = 678
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Restart'
          TabOrder = 5
          OnClick = btnRestartMutationClick
        end
      end
      object tsSA: TTabSheet
        Caption = 'Simulated Annealing'
        ImageIndex = 5
        object Label11: TLabel
          Left = 40
          Top = 16
          Width = 99
          Height = 13
          Caption = 'Original Chromosome'
        end
        object Label12: TLabel
          Left = 49
          Top = 108
          Width = 74
          Height = 13
          Caption = 'No. of iterations'
        end
        object Label13: TLabel
          Left = 230
          Top = 108
          Width = 86
          Height = 13
          Caption = 'Initial termperature'
        end
        object Label14: TLabel
          Left = 422
          Top = 108
          Width = 65
          Height = 13
          Caption = 'Cooling factor'
        end
        object Label15: TLabel
          Left = 373
          Top = 143
          Width = 60
          Height = 13
          Caption = 'Temperature'
        end
        object lblTemperature: TLabel
          Left = 439
          Top = 142
          Width = 65
          Height = 13
          AutoSize = False
          Caption = 'lblTemperature'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'System'
          Font.Style = []
          ParentFont = False
        end
        object Label16: TLabel
          Left = 525
          Top = 143
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
          Left = 560
          Top = 140
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
          Left = 630
          Top = 143
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
          Left = 679
          Top = 141
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
        object sgSAOriginalChromosome: TStringGrid
          Left = 30
          Top = 42
          Width = 724
          Height = 47
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 1
          FixedRows = 0
          TabOrder = 0
        end
        object cbxSAOriginalChromosome: TComboBox
          Left = 153
          Top = 13
          Width = 101
          Height = 21
          Style = csDropDownList
          TabOrder = 1
          OnChange = cbxSAOriginalChromosomeChange
        end
        object sgSAFinalChromosome: TStringGrid
          Left = 30
          Top = 176
          Width = 724
          Height = 48
          ColCount = 15
          DefaultColWidth = 60
          DefaultRowHeight = 25
          FixedCols = 0
          RowCount = 1
          FixedRows = 0
          TabOrder = 2
        end
        object btnRunSA: TButton
          Left = 678
          Top = 103
          Width = 55
          Height = 25
          Caption = 'Run'
          TabOrder = 3
          OnClick = btnRunSAClick
        end
        object btnRestartSA: TButton
          Left = 678
          Top = 11
          Width = 75
          Height = 25
          Caption = 'Restart'
          TabOrder = 4
          OnClick = btnRestartSAClick
        end
        object edtnoSAIterations: TEdit
          Left = 134
          Top = 105
          Width = 65
          Height = 21
          TabOrder = 5
          Text = 'edtnoSAIterations'
        end
        object pbSAIterations: TProgressBar
          Left = 30
          Top = 136
          Width = 332
          Height = 20
          TabOrder = 6
        end
        object edtInitTemp: TEdit
          Left = 325
          Top = 105
          Width = 73
          Height = 21
          TabOrder = 7
          Text = 'edtInitTemp'
        end
        object edtCoolingFactor: TEdit
          Left = 496
          Top = 105
          Width = 65
          Height = 21
          TabOrder = 8
          Text = 'edtCoolingFactor'
        end
        object btnStepSA: TButton
          Left = 596
          Top = 103
          Width = 60
          Height = 25
          Caption = 'Step'
          TabOrder = 9
          OnClick = btnStepSAClick
        end
      end
    end
  end
  object pnlPopulation: TPanel
    Left = 0
    Top = 397
    Width = 97
    Height = 377
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
      Height = 334
      ItemHeight = 13
      TabOrder = 0
      OnClick = lstPopulationClick
    end
  end
  object pnlChromosome: TPanel
    Left = 0
    Top = 323
    Width = 815
    Height = 74
    Align = alTop
    TabOrder = 3
    object grdOBChromosome: TStringGrid
      Left = 1
      Top = 1
      Width = 813
      Height = 72
      Align = alClient
      ColCount = 15
      DefaultColWidth = 50
      DefaultRowHeight = 25
      FixedCols = 0
      RowCount = 2
      FixedRows = 0
      TabOrder = 0
      ExplicitHeight = 48
    end
  end
end
