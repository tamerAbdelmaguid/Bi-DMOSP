inherited frmNSGAII: TfrmNSGAII
  Caption = 'Applying NSGA-II to bi-objective DMOSP'
  ClientHeight = 733
  ClientWidth = 866
  ExplicitWidth = 882
  ExplicitHeight = 772
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Left = 97
    Top = 257
    Width = 769
    Height = 476
    ExplicitLeft = 97
    ExplicitTop = 257
    ExplicitWidth = 769
    ExplicitHeight = 476
  end
  inherited pnlGTAParameters: TPanel
    Width = 866
    Height = 257
    ExplicitWidth = 866
    ExplicitHeight = 257
    inherited Label5: TLabel
      Left = 28
      Top = 197
      ExplicitLeft = 28
      ExplicitTop = 197
    end
    inherited lblTime: TLabel
      Left = 135
      Top = 197
      ExplicitLeft = 135
      ExplicitTop = 197
    end
    object lblGD: TLabel [2]
      Left = 230
      Top = 197
      Width = 26
      Height = 13
      Caption = 'lblGD'
    end
    object Label32: TLabel [3]
      Left = 199
      Top = 197
      Width = 25
      Height = 13
      Caption = 'GD ='
    end
    object lblIGD: TLabel [4]
      Left = 320
      Top = 197
      Width = 29
      Height = 13
      Caption = 'lblIGD'
    end
    object Label34: TLabel [5]
      Left = 286
      Top = 197
      Width = 28
      Height = 13
      Caption = 'IGD ='
    end
    object lblHV: TLabel [6]
      Left = 489
      Top = 197
      Width = 25
      Height = 13
      Caption = 'lblHV'
    end
    object Label35: TLabel [7]
      Left = 459
      Top = 197
      Width = 24
      Height = 13
      Caption = 'HV ='
    end
    object lblHV0: TLabel [8]
      Left = 575
      Top = 197
      Width = 31
      Height = 13
      Caption = 'lblHV0'
    end
    object Label13: TLabel [9]
      Left = 538
      Top = 197
      Width = 30
      Height = 13
      Caption = 'HV0 ='
    end
    object lblHVpr: TLabel [10]
      Left = 681
      Top = 197
      Width = 33
      Height = 13
      Caption = 'lblHV%'
    end
    object Label16: TLabel [11]
      Left = 643
      Top = 197
      Width = 32
      Height = 13
      Caption = 'HV% ='
    end
    object lblHV0pr: TLabel [12]
      Left = 791
      Top = 198
      Width = 39
      Height = 13
      Caption = 'lblHV0%'
    end
    object Label18: TLabel [13]
      Left = 746
      Top = 198
      Width = 38
      Height = 13
      Caption = 'HV0% ='
    end
    object lblTGD: TLabel [14]
      Left = 407
      Top = 197
      Width = 33
      Height = 13
      Caption = 'lblTGD'
    end
    object Label14: TLabel [15]
      Left = 370
      Top = 197
      Width = 32
      Height = 13
      Caption = 'TGD ='
    end
    inherited btnRun: TBitBtn
      Left = 284
      Top = 219
      OnClick = btnRunClick
      ExplicitLeft = 284
      ExplicitTop = 219
    end
    inherited btnClose: TBitBtn
      Left = 508
      Top = 221
      ExplicitLeft = 508
      ExplicitTop = 221
    end
    object pgGAType: TPageControl
      Left = 1
      Top = 1
      Width = 864
      Height = 184
      ActivePage = TabSheet3
      Align = alTop
      TabOrder = 2
      object TabSheet3: TTabSheet
        Caption = 'Search, Mutation and Crossover'
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Label10: TLabel
          Left = 645
          Top = 65
          Width = 47
          Height = 13
          Caption = 'pMutation'
        end
        object Label11: TLabel
          Left = 638
          Top = 35
          Width = 53
          Height = 13
          Caption = 'pCrossover'
        end
        object Label1: TLabel
          Left = 616
          Top = 95
          Width = 77
          Height = 13
          Caption = 'pMutation (RLB)'
        end
        object rgCrossover: TRadioGroup
          Left = 423
          Top = 17
          Width = 176
          Height = 128
          Caption = 'Crossover'
          ItemIndex = 0
          Items.Strings = (
            'Single-point OX (OX-1pt)'
            'Two-point OX (OX-2pt)'
            'Uniform OX (OX-u)'
            'Gen. position (GPX)'
            'SB2OX')
          TabOrder = 0
        end
        object rgMutation: TRadioGroup
          Left = 262
          Top = 16
          Width = 145
          Height = 129
          Caption = 'Mutation '
          ItemIndex = 0
          Items.Strings = (
            'Shift Mutation'
            'Swap Mutation')
          TabOrder = 1
        end
        object edtpMutation: TEdit
          Left = 698
          Top = 60
          Width = 87
          Height = 21
          TabOrder = 2
          Text = 'edtpMutation'
        end
        object edtpCrossover: TEdit
          Left = 699
          Top = 30
          Width = 87
          Height = 21
          TabOrder = 3
          Text = 'edtpCrossover'
        end
        object gbxGeneralParameters: TGroupBox
          Left = 19
          Top = 7
          Width = 219
          Height = 141
          Caption = 'General paramters'
          TabOrder = 4
          object Label2: TLabel
            Left = 45
            Top = 32
            Width = 70
            Height = 13
            Caption = 'population size'
          end
          object Label3: TLabel
            Left = 16
            Top = 66
            Width = 102
            Height = 13
            Caption = 'Maximum generations'
          end
          object Label6: TLabel
            Left = 22
            Top = 99
            Width = 93
            Height = 13
            Caption = 'Max. time limit (Sec)'
          end
          object edtPopSize: TEdit
            Left = 125
            Top = 29
            Width = 68
            Height = 21
            TabOrder = 0
          end
          object edtMaxGenerations: TEdit
            Left = 124
            Top = 63
            Width = 68
            Height = 21
            TabOrder = 1
          end
          object edtMaxTimeLimit: TEdit
            Left = 125
            Top = 96
            Width = 68
            Height = 21
            TabOrder = 2
          end
        end
        object edtpMutationRLB: TEdit
          Left = 699
          Top = 92
          Width = 87
          Height = 21
          TabOrder = 5
          Text = 'edtpMutationRLB'
        end
      end
      object tsSA: TTabSheet
        Caption = 'Simulated Annealing'
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Label4: TLabel
          Left = 80
          Top = 21
          Width = 140
          Height = 13
          Caption = 'Application proportion (pSA) ='
        end
        object Label7: TLabel
          Left = 106
          Top = 51
          Width = 114
          Height = 13
          Caption = 'Initial temperature (T0) ='
        end
        object Label8: TLabel
          Left = 59
          Top = 80
          Width = 161
          Height = 13
          Caption = 'Maximum iterations (itr_SA^max} ='
        end
        object Label9: TLabel
          Left = 120
          Top = 110
          Width = 100
          Height = 13
          Caption = 'Cooling rate (alpha) ='
        end
        object edtSAprop: TEdit
          Left = 232
          Top = 18
          Width = 121
          Height = 21
          TabOrder = 0
          Text = 'edtSAprop'
        end
        object edtSAT0: TEdit
          Left = 232
          Top = 48
          Width = 121
          Height = 21
          TabOrder = 1
          Text = 'edtSAT0'
        end
        object edtSAMaxIter: TEdit
          Left = 232
          Top = 77
          Width = 121
          Height = 21
          TabOrder = 2
          Text = 'edtSAMaxIter'
        end
        object edtSACoolingRate: TEdit
          Left = 232
          Top = 107
          Width = 121
          Height = 21
          TabOrder = 3
          Text = 'edtSACoolingRate'
        end
      end
    end
  end
  object pnlPareto: TPanel
    Left = 0
    Top = 257
    Width = 97
    Height = 476
    Align = alLeft
    TabOrder = 2
    object Label30: TLabel
      Left = 8
      Top = 8
      Width = 75
      Height = 13
      Caption = 'Pareto solutions'
    end
    object lstPareto: TListBox
      Left = 10
      Top = 26
      Width = 73
      Height = 438
      ItemHeight = 13
      TabOrder = 0
      OnClick = lstParetoClick
    end
  end
end
