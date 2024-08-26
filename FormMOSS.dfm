inherited frmMOSS: TfrmMOSS
  Caption = 'Multi-Objective Scatter Search'
  ClientHeight = 741
  ClientWidth = 874
  ExplicitWidth = 890
  ExplicitHeight = 780
  PixelsPerInch = 96
  TextHeight = 13
  inherited pnlSolution: TPanel
    Left = 97
    Top = 217
    Width = 777
    Height = 524
    ExplicitLeft = 97
    ExplicitTop = 217
    ExplicitWidth = 777
    ExplicitHeight = 524
  end
  inherited pnlGTAParameters: TPanel
    Width = 874
    Height = 217
    ExplicitTop = 2
    ExplicitWidth = 874
    ExplicitHeight = 217
    inherited Label5: TLabel
      Left = 26
      Top = 154
      ExplicitLeft = 26
      ExplicitTop = 154
    end
    inherited lblTime: TLabel
      Left = 133
      Top = 154
      ExplicitLeft = 133
      ExplicitTop = 154
    end
    object lblGD: TLabel [2]
      Left = 230
      Top = 154
      Width = 26
      Height = 13
      Caption = 'lblGD'
    end
    object Label32: TLabel [3]
      Left = 199
      Top = 154
      Width = 25
      Height = 13
      Caption = 'GD ='
    end
    object lblIGD: TLabel [4]
      Left = 317
      Top = 154
      Width = 29
      Height = 13
      Caption = 'lblIGD'
    end
    object Label34: TLabel [5]
      Left = 283
      Top = 154
      Width = 28
      Height = 13
      Caption = 'IGD ='
    end
    object lblHV: TLabel [6]
      Left = 502
      Top = 154
      Width = 25
      Height = 13
      Caption = 'lblHV'
    end
    object Label35: TLabel [7]
      Left = 472
      Top = 154
      Width = 24
      Height = 13
      Caption = 'HV ='
    end
    object lblHV0: TLabel [8]
      Left = 589
      Top = 154
      Width = 31
      Height = 13
      Caption = 'lblHV0'
    end
    object Label36: TLabel [9]
      Left = 550
      Top = 154
      Width = 30
      Height = 13
      Caption = 'HV0 ='
    end
    object lblHVpr: TLabel [10]
      Left = 689
      Top = 154
      Width = 33
      Height = 13
      Caption = 'lblHV%'
    end
    object Label33: TLabel [11]
      Left = 651
      Top = 154
      Width = 32
      Height = 13
      Caption = 'HV% ='
    end
    object lblHV0pr: TLabel [12]
      Left = 799
      Top = 154
      Width = 39
      Height = 13
      Caption = 'lblHV0%'
    end
    object Label37: TLabel [13]
      Left = 754
      Top = 154
      Width = 38
      Height = 13
      Caption = 'HV0% ='
    end
    object lblTGD: TLabel [14]
      Left = 412
      Top = 154
      Width = 33
      Height = 13
      Caption = 'lblTGD'
    end
    object Label4: TLabel [15]
      Left = 373
      Top = 154
      Width = 32
      Height = 13
      Caption = 'TGD ='
    end
    inherited btnRun: TBitBtn
      Left = 304
      Top = 186
      OnClick = btnRunClick
      ExplicitLeft = 304
      ExplicitTop = 186
    end
    inherited btnClose: TBitBtn
      Left = 536
      Top = 183
      ExplicitLeft = 536
      ExplicitTop = 183
    end
    object pgctrlSSPR: TPageControl
      Left = 1
      Top = 1
      Width = 872
      Height = 147
      ActivePage = tsSSPRRun
      Align = alTop
      TabOrder = 2
      object tsSSPRNbrTest: TTabSheet
        Caption = 'Testing N1, N2, B1 && B2'
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Label15: TLabel
          Left = 55
          Top = 73
          Width = 46
          Height = 13
          Caption = 'Operation'
        end
        object Label7: TLabel
          Left = 549
          Top = 73
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label6: TLabel
          Left = 462
          Top = 73
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label1: TLabel
          Left = 347
          Top = 72
          Width = 41
          Height = 13
          Caption = 'Machine'
        end
        object Label10: TLabel
          Left = 549
          Top = 30
          Width = 19
          Height = 13
          Caption = 'End'
        end
        object Label9: TLabel
          Left = 462
          Top = 32
          Width = 22
          Height = 13
          Caption = 'Start'
        end
        object Label2: TLabel
          Left = 347
          Top = 30
          Width = 17
          Height = 13
          Caption = 'Job'
        end
        object cbxOps: TComboBox
          Left = 111
          Top = 70
          Width = 61
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 0
        end
        object btnNSParameters: TBitBtn
          Left = 192
          Top = 68
          Width = 116
          Height = 25
          Caption = 'NS Parameters'
          DoubleBuffered = True
          Enabled = False
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
        end
        object edtMCx2: TEdit
          Left = 575
          Top = 70
          Width = 55
          Height = 21
          Enabled = False
          TabOrder = 2
          Text = 'edtMCx2'
        end
        object edtMCx1: TEdit
          Left = 490
          Top = 70
          Width = 53
          Height = 21
          Enabled = False
          TabOrder = 3
          Text = 'edtMCx1'
        end
        object cbxFromMC: TComboBox
          Left = 399
          Top = 69
          Width = 57
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 4
        end
        object edtJobx2: TEdit
          Left = 575
          Top = 27
          Width = 55
          Height = 21
          Enabled = False
          TabOrder = 5
          Text = 'edtJobx2'
        end
        object edtJobx1: TEdit
          Left = 490
          Top = 29
          Width = 53
          Height = 21
          Enabled = False
          TabOrder = 6
          Text = 'edtJobx1'
        end
        object cbxJobs: TComboBox
          Left = 399
          Top = 29
          Width = 57
          Height = 21
          Style = csDropDownList
          Enabled = False
          TabOrder = 7
        end
        object btnB1Parameters: TBitBtn
          Left = 645
          Top = 27
          Width = 116
          Height = 25
          Caption = 'B1 Parameters'
          DoubleBuffered = True
          Enabled = False
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
        end
        object btnB2Parameters: TBitBtn
          Left = 645
          Top = 67
          Width = 116
          Height = 25
          Caption = 'B2 Parameters'
          DoubleBuffered = True
          Enabled = False
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
          TabOrder = 9
        end
        object btnEnumerate: TBitBtn
          Left = 93
          Top = 25
          Width = 172
          Height = 25
          Caption = 'Enumerate N1 && N2 moves'
          DoubleBuffered = True
          Enabled = False
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
          TabOrder = 10
        end
      end
      object tsSSPRLB: TTabSheet
        Caption = 'Lower Bound'
        ImageIndex = 3
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object btnLB: TSpeedButton
          Left = 37
          Top = 41
          Width = 145
          Height = 33
          Caption = 'Evaluate Lower bounds'
          OnClick = btnLBClick
        end
        object Label8: TLabel
          Left = 302
          Top = 41
          Width = 31
          Height = 13
          Caption = 'LB1 = '
        end
        object Label11: TLabel
          Left = 302
          Top = 61
          Width = 31
          Height = 13
          Caption = 'LB2 = '
        end
        object lblLB1: TLabel
          Left = 354
          Top = 41
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB1'
        end
        object lblLB2: TLabel
          Left = 354
          Top = 60
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB2'
        end
        object Label12: TLabel
          Left = 302
          Top = 87
          Width = 25
          Height = 13
          Caption = 'LB = '
        end
        object lblLB: TLabel
          Left = 354
          Top = 87
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB'
        end
        object lblLB3: TLabel
          Left = 538
          Top = 41
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'lblLB3'
        end
        object Label14: TLabel
          Left = 507
          Top = 41
          Width = 25
          Height = 13
          Caption = 'LB = '
        end
        object Label28: TLabel
          Left = 269
          Top = 11
          Width = 119
          Height = 13
          Caption = 'Makespan lower bounds:'
        end
        object Label29: TLabel
          Left = 485
          Top = 11
          Width = 102
          Height = 13
          Caption = 'MWFT lower bounds:'
        end
      end
      object tsSSPRRun: TTabSheet
        Caption = 'MOSS Run'
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object Label13: TLabel
          Left = 89
          Top = 11
          Width = 92
          Height = 13
          Caption = 'Reference Set Size'
        end
        object Label16: TLabel
          Left = 28
          Top = 37
          Width = 153
          Height = 13
          Caption = 'Distance Acceptance Threshold'
        end
        object lblStatus: TLabel
          Left = 636
          Top = 77
          Width = 192
          Height = 19
          Alignment = taCenter
          AutoSize = False
          Caption = 'lblStatus'
        end
        object Label17: TLabel
          Left = 92
          Top = 63
          Width = 89
          Height = 13
          Caption = 'Maximum tabu size'
        end
        object Label18: TLabel
          Left = 25
          Top = 90
          Width = 156
          Height = 13
          Caption = 'Maximum number of TS iterations'
        end
        object Label19: TLabel
          Left = 338
          Top = 11
          Width = 126
          Height = 13
          Caption = 'Improvement iterations limit'
        end
        object Label24: TLabel
          Left = 326
          Top = 37
          Width = 138
          Height = 13
          Caption = 'Number of stored best moves'
        end
        object Label25: TLabel
          Left = 343
          Top = 63
          Width = 121
          Height = 13
          Caption = 'Number of random moves'
        end
        object Label26: TLabel
          Left = 599
          Top = 11
          Width = 128
          Height = 13
          Caption = 'Number of MOSS iterations'
        end
        object Label27: TLabel
          Left = 367
          Top = 90
          Width = 97
          Height = 13
          Caption = 'Crossover probability'
        end
        object Label31: TLabel
          Left = 587
          Top = 37
          Width = 138
          Height = 13
          Caption = 'Computational time limit (sec.)'
        end
        object edtRSize: TEdit
          Left = 196
          Top = 8
          Width = 73
          Height = 21
          TabOrder = 0
          Text = 'edtRSize'
        end
        object edtDeltaThreshold: TEdit
          Left = 196
          Top = 34
          Width = 73
          Height = 21
          TabOrder = 1
          Text = 'edtDeltaThreshold'
        end
        object edtTabuSize: TEdit
          Left = 196
          Top = 60
          Width = 73
          Height = 21
          TabOrder = 2
          Text = 'edtTabuSize'
        end
        object edtnTSIterations: TEdit
          Left = 196
          Top = 87
          Width = 73
          Height = 21
          TabOrder = 3
          Text = 'edtnTSIterations'
        end
        object edtnImpIterations: TEdit
          Left = 477
          Top = 8
          Width = 89
          Height = 21
          TabOrder = 4
          Text = 'edtnImpIterations'
        end
        object edtTSnBestMoves: TEdit
          Left = 477
          Top = 34
          Width = 89
          Height = 21
          TabOrder = 5
          Text = 'edtTSnBestMoves'
        end
        object edtTSnRandomMoves: TEdit
          Left = 477
          Top = 60
          Width = 89
          Height = 21
          TabOrder = 6
          Text = 'edtTSnRandomMoves'
        end
        object edtnSSPRIterations: TEdit
          Left = 738
          Top = 8
          Width = 89
          Height = 21
          TabOrder = 7
          Text = 'edtnSSPRIterations'
        end
        object edtTheta_X: TEdit
          Left = 477
          Top = 87
          Width = 89
          Height = 21
          TabOrder = 8
          Text = 'edtTheta_X'
        end
        object edtTimeLimit: TEdit
          Left = 738
          Top = 34
          Width = 89
          Height = 21
          TabOrder = 9
          Text = 'edtTimeLimit'
        end
      end
    end
  end
  object pnlPareto: TPanel
    Left = 0
    Top = 217
    Width = 97
    Height = 524
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
      Top = 27
      Width = 73
      Height = 486
      ItemHeight = 13
      TabOrder = 0
      OnClick = lstParetoClick
    end
  end
end
