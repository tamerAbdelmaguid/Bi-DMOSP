//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMOSSCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "FormCalcProgress"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TfrmMOSSCalcProgress *frmMOSSCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmMOSSCalcProgress::TfrmMOSSCalcProgress(TComponent* Owner)
	: TfrmCalcProgress(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSSCalcProgress::FormShow(TObject *)
{
  grdInitialSolutions->Cells[3][0] = "MWFT_i";
  grdInitialSolutions->Cells[4][0] = "MWFT_f";

  grdInitialSolutions->Cells[0][0] = "Sol#";
  grdInitialSolutions->Cells[1][0] = "Cmax_i";
  grdInitialSolutions->Cells[2][0] = "Cmax_f";

  sgSCIterations->Cells[0][0] = "Iter#";
  sgSCIterations->Cells[1][0] = "Cmax_L";
  sgSCIterations->Cells[2][0] = "Cmax_F";
  sgSCIterations->Cells[3][0] = "Cmax_Ci";
  sgSCIterations->Cells[4][0] = "Cmax_Cf";

  sgSCIterations->Cells[3][0] = "MWFT_L";
  sgSCIterations->Cells[4][0] = "MWFT_F";
  sgSCIterations->Cells[5][0] = "Cmax_Ci";
  sgSCIterations->Cells[6][0] = "Cmax_Cf";
  sgSCIterations->Cells[7][0] = "MWFT_Ci";
  sgSCIterations->Cells[8][0] = "MWFT_Cf";

  ClearForm();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::ClearForm()
{
  lblCmaxLB->Caption = "";
  lblbestCmax->Caption = "";

  lblTSInitialCmax->Caption = "";
  lblTSCurrentCmax->Caption = "";
  lblTSBestCmax->Caption = "";
  lblTSIter->Caption = "";
  lblTSImpIter->Caption = "";
  pbTabuSearch->Position = 0;

  lblMWFTLB->Caption = "";
  lblbestMWFT->Caption = "";

  lblTSInitialMWFT->Caption = "";
  lblTSCurrentMWFT->Caption = "";
  lblTSBestMWFT->Caption = "";

  for(int i=1; i < grdInitialSolutions->RowCount; i++){
	grdInitialSolutions->Cells[0][i] = "";
	grdInitialSolutions->Cells[1][i] = "";
	grdInitialSolutions->Cells[2][i] = "";
	grdInitialSolutions->Cells[3][i] = "";
	grdInitialSolutions->Cells[4][i] = "";
  }

  pbSSPRIterations->Position = 0;
  pbConstruction->Position = 0;

  lblSCIter->Caption = "";
  pbSCProgress->Position = 0;

  for(int i=1; i < sgSCIterations->RowCount; i++){
	sgSCIterations->Cells[0][i] = "";
	sgSCIterations->Cells[1][i] = "";
	sgSCIterations->Cells[2][i] = "";
	sgSCIterations->Cells[3][i] = "";
	sgSCIterations->Cells[4][i] = "";
	sgSCIterations->Cells[5][i] = "";
	sgSCIterations->Cells[6][i] = "";
	sgSCIterations->Cells[7][i] = "";
	sgSCIterations->Cells[8][i] = "";
  }

  lsCmaxLB->Clear();
  lsMWFTLB->Clear();
  psParetoFront->Clear();
  psReferenceSet->Clear();
  psOptimalParetoFront->Clear();
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::DrawLBLines(double CmaxLB, double MWFTLB)
{
  CLB = CmaxLB;
  FLB = MWFTLB;

  lsCmaxLB->Clear();
  lsMWFTLB->Clear();

  lsCmaxLB->AddXY(CmaxLB, MWFTLB, "", clRed);
  lsCmaxLB->AddXY(CmaxLB, MWFTLB*1.8, "", clRed);

  lsMWFTLB->AddXY(CmaxLB, MWFTLB, "", clRed);
  lsMWFTLB->AddXY(CmaxLB*1.5, MWFTLB, "", clRed);

  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::DrawParetoFront(SList &P)
{
  psParetoFront->Clear();
  for(SList::SNode *slnd = P.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)slnd->Data();
	psParetoFront->AddXY(sol->CmaxValue(), sol->MWFTValue(),"", clBlue);
  }
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::DrawOnlyParetoFront(SList &P)
{
  psReferenceSet->Clear();
  psParetoFront->Clear();
  for(SList::SNode *slnd = P.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)slnd->Data();
	psParetoFront->AddXY(sol->CmaxValue(), sol->MWFTValue(),"", clBlue);
  }

  lsCmaxLB->Clear();
  lsMWFTLB->Clear();
  lsCmaxLB->AddXY(CLB, FLB*1.8, "", clWhite);
  lsCmaxLB->AddXY(CLB, FLB*1.8, "", clWhite);
  lsMWFTLB->AddXY(CLB*1.5, FLB, "", clWhite);
  lsMWFTLB->AddXY(CLB*1.5, FLB, "", clWhite);

  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::DrawOptimalParetoFront(int i)
{
  if(i < 0 || i > 30) return;
  psOptimalParetoFront->Clear();
  for(int s = 0; s < SolvedDMOSPs[i].nSol; s++){
	psOptimalParetoFront->AddXY(SolvedDMOSPs[i].Cmax[s], SolvedDMOSPs[i].MWFT[s],"", clYellow);
  }
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOSSCalcProgress::DrawReferenceSet(SList &R)
{
  psReferenceSet->Clear();
  for(SList::SNode *slnd = R.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)slnd->Data();
	psReferenceSet->AddXY(sol->CmaxValue(), sol->MWFTValue(),"", clRed);
  }
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSSCalcProgress::ExportWMF1Click(TObject *)
{
  AnsiString s = MainForm->CurrentChild->Caption;
  int i = s.AnsiPos(".mosp")-1;
  if(i != -1) s = s.SubString(1, i) + ".wmf";
  else s = s + ".wmf";
  dlgSaveWMF->FileName = s;
  if(dlgSaveWMF->Execute()){
	s = dlgSaveWMF->FileName;
	ParetoChart->SaveToMetafileEnh(s);
  }
}
//---------------------------------------------------------------------------

