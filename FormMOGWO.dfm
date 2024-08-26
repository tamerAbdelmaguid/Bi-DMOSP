inherited frmMOGWO: TfrmMOGWO
  Caption = 'Applying MOGOW to bi-objective DMOSP'
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
    ExplicitTop = 2
    ExplicitWidth = 866
    ExplicitHeight = 257
    inherited Label5: TLabel
      Left = 24
      Top = 198
      ExplicitLeft = 24
      ExplicitTop = 198
    end
    inherited lblTime: TLabel
      Left = 131
      Top = 198
      ExplicitLeft = 131
      ExplicitTop = 198
    end
    object lblGD: TLabel [2]
      Left = 235
      Top = 199
      Width = 26
      Height = 13
      Caption = 'lblGD'
    end
    object Label32: TLabel [3]
      Left = 204
      Top = 199
      Width = 25
      Height = 13
      Caption = 'GD ='
    end
    object lblIGD: TLabel [4]
      Left = 320
      Top = 199
      Width = 29
      Height = 13
      Caption = 'lblIGD'
    end
    object Label34: TLabel [5]
      Left = 286
      Top = 199
      Width = 28
      Height = 13
      Caption = 'IGD ='
    end
    object lblHV: TLabel [6]
      Left = 489
      Top = 198
      Width = 25
      Height = 13
      Caption = 'lblHV'
    end
    object Label35: TLabel [7]
      Left = 459
      Top = 198
      Width = 24
      Height = 13
      Caption = 'HV ='
    end
    object lblHV0: TLabel [8]
      Left = 582
      Top = 198
      Width = 31
      Height = 13
      Caption = 'lblHV0'
    end
    object Label10: TLabel [9]
      Left = 545
      Top = 198
      Width = 30
      Height = 13
      Caption = 'HV0 ='
    end
    object lblHVpr: TLabel [10]
      Left = 681
      Top = 198
      Width = 33
      Height = 13
      Caption = 'lblHV%'
    end
    object Label16: TLabel [11]
      Left = 643
      Top = 198
      Width = 32
      Height = 13
      Caption = 'HV% ='
    end
    object lblHV0pr: TLabel [12]
      Left = 791
      Top = 199
      Width = 39
      Height = 13
      Caption = 'lblHV0%'
    end
    object Label18: TLabel [13]
      Left = 746
      Top = 199
      Width = 38
      Height = 13
      Caption = 'HV0% ='
    end
    object lblTGD: TLabel [14]
      Left = 409
      Top = 198
      Width = 33
      Height = 13
      Caption = 'lblTGD'
    end
    object Label17: TLabel [15]
      Left = 370
      Top = 199
      Width = 32
      Height = 13
      Caption = 'TGD ='
    end
    inherited btnRun: TBitBtn
      Left = 273
      Top = 219
      OnClick = btnRunClick
      ExplicitLeft = 273
      ExplicitTop = 219
    end
    inherited btnClose: TBitBtn
      Left = 499
      Top = 219
      ExplicitLeft = 499
      ExplicitTop = 219
    end
    object pgMOGWOParameters: TPageControl
      Left = 1
      Top = 1
      Width = 864
      Height = 184
      ActivePage = TabSheet3
      Align = alTop
      TabOrder = 2
      object TabSheet3: TTabSheet
        Caption = 'Main Parameters'
        ImageIndex = 2
        object Label2: TLabel
          Left = 110
          Top = 32
          Width = 88
          Height = 13
          Caption = 'No. of grey wolves'
        end
        object Label3: TLabel
          Left = 109
          Top = 66
          Width = 89
          Height = 13
          Caption = 'Maximum iterations'
        end
        object Label6: TLabel
          Left = 105
          Top = 100
          Width = 93
          Height = 13
          Caption = 'Max. time limit (Sec)'
        end
        object Label1: TLabel
          Left = 339
          Top = 32
          Width = 54
          Height = 13
          Caption = 'No. of grids'
        end
        object Label12: TLabel
          Left = 336
          Top = 66
          Width = 57
          Height = 13
          Caption = 'Archive size'
        end
        object Label13: TLabel
          Left = 577
          Top = 32
          Width = 35
          Height = 13
          Caption = 'alpha ='
        end
        object Label14: TLabel
          Left = 582
          Top = 66
          Width = 30
          Height = 13
          Caption = 'beta ='
        end
        object Label15: TLabel
          Left = 569
          Top = 100
          Width = 43
          Height = 13
          Caption = 'gamma ='
        end
        object edtnGreyWolves: TEdit
          Left = 210
          Top = 29
          Width = 68
          Height = 21
          TabOrder = 0
        end
        object edtMaxIterations: TEdit
          Left = 211
          Top = 63
          Width = 68
          Height = 21
          TabOrder = 1
        end
        object edtMaxTimeLimit: TEdit
          Left = 210
          Top = 97
          Width = 68
          Height = 21
          TabOrder = 2
        end
        object edtnGrids: TEdit
          Left = 404
          Top = 29
          Width = 68
          Height = 21
          TabOrder = 3
        end
        object edtArchiveSize: TEdit
          Left = 404
          Top = 63
          Width = 68
          Height = 21
          TabOrder = 4
        end
        object edtAlpha: TEdit
          Left = 624
          Top = 29
          Width = 57
          Height = 21
          TabOrder = 5
        end
        object edtBeta: TEdit
          Left = 624
          Top = 63
          Width = 57
          Height = 21
          TabOrder = 6
        end
        object edtGamma: TEdit
          Left = 624
          Top = 97
          Width = 57
          Height = 21
          TabOrder = 7
        end
      end
      object tsSA: TTabSheet
        Caption = 'Simulated Annealing'
        ImageIndex = 1
        object Label4: TLabel
          Left = 145
          Top = 30
          Width = 140
          Height = 13
          Caption = 'Application proportion (pSA) ='
        end
        object Label7: TLabel
          Left = 171
          Top = 60
          Width = 114
          Height = 13
          Caption = 'Initial temperature (T0) ='
        end
        object Label8: TLabel
          Left = 124
          Top = 89
          Width = 161
          Height = 13
          Caption = 'Maximum iterations (itr_SA^max} ='
        end
        object Label9: TLabel
          Left = 185
          Top = 119
          Width = 100
          Height = 13
          Caption = 'Cooling rate (alpha) ='
        end
        object edtSAprop: TEdit
          Left = 297
          Top = 27
          Width = 121
          Height = 21
          TabOrder = 0
          Text = 'edtSAprop'
        end
        object edtSAT0: TEdit
          Left = 297
          Top = 57
          Width = 121
          Height = 21
          TabOrder = 1
          Text = 'edtSAT0'
        end
        object edtSAMaxIter: TEdit
          Left = 297
          Top = 86
          Width = 121
          Height = 21
          TabOrder = 2
          Text = 'edtSAMaxIter'
        end
        object edtSACoolingRate: TEdit
          Left = 297
          Top = 116
          Width = 121
          Height = 21
          TabOrder = 3
          Text = 'edtSACoolingRate'
        end
        object rgMutation: TRadioGroup
          Left = 525
          Top = 9
          Width = 177
          Height = 135
          Caption = 'Mutation '
          ItemIndex = 0
          Items.Strings = (
            'Shift Mutation'
            'Swap Mutation')
          TabOrder = 4
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
