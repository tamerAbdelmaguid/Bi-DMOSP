//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMOGWOCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "FormCalcProgress"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TfrmMOGWOCalcProgress *frmMOGWOCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmMOGWOCalcProgress::TfrmMOGWOCalcProgress(TComponent* Owner)
	: TfrmCalcProgress(Owner)
{
}
//---------------------------------------------------------------------------
void TfrmMOGWOCalcProgress::ClearForm()
{
  lblCmaxLB->Caption = "";
  lblbestCmax->Caption = "";

  lblSAInitialCmax->Caption = "";
  lblSACurrentCmax->Caption = "";
  lblSABestCmax->Caption = "";
  lblSAIter->Caption = "";
  lblSATemperature->Caption = "";
  pbSA->Position = 0;

  for(int i=1; i < grdInitialSolutions->RowCount; i++){
	grdInitialSolutions->Cells[0][i] = "";
	grdInitialSolutions->Cells[1][i] = "";
	grdInitialSolutions->Cells[2][i] = "";
  }

  pbConstruction->Position = 0;

  lblGeneration->Caption = "";
  pbGeneration->Position = 0;
  for(int i=1; i < sgGenerations->RowCount; i++){
	sgGenerations->Cells[0][i] = "";
	sgGenerations->Cells[1][i] = "";
	sgGenerations->Cells[2][i] = "";
	sgGenerations->Cells[3][i] = "";
	sgGenerations->Cells[4][i] = "";
  }

  lblMWFTLB->Caption = "";
  lblbestMWFT->Caption = "";

  lblSAInitialMWFT->Caption = "";
  lblSACurrentMWFT->Caption = "";
  lblSABestMWFT->Caption = "";

  for(int i=1; i < grdInitialSolutions->RowCount; i++){
	grdInitialSolutions->Cells[3][i] = "";
	grdInitialSolutions->Cells[4][i] = "";
  }
  for(int i=1; i < sgGenerations->RowCount; i++){
	sgGenerations->Cells[5][i] = "";
	sgGenerations->Cells[6][i] = "";
	sgGenerations->Cells[7][i] = "";
	sgGenerations->Cells[8][i] = "";
  }

  lsCmaxLB->Clear();
  lsMWFTLB->Clear();
  psParetoFront->Clear();
  psReferenceSet->Clear();
  psOptimalParetoFront->Clear();
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOGWOCalcProgress::DrawLBLines(double CmaxLB, double MWFTLB)
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
void TfrmMOGWOCalcProgress::DrawFronts()
{
  psParetoFront->Clear();
  psReferenceSet->Clear();
  DMOSP_MOGWO *mogwo = (DMOSP_MOGWO *)Alg;

  DMOSP_Wolf **Wolves = mogwo->getWolves();
  DMOSP_Wolf **Archive = mogwo->getArchive();

  for(int i=0; i<mogwo->getnArchive(); i++){
	DMOSP_Wolf *wlf = Archive[i];
	psParetoFront->AddXY(wlf->Cmax, wlf->MWFT);
  }
  for(int i=0; i<mogwo->getnWolves(); i++){
	DMOSP_Wolf *wlf = Wolves[i];
	psReferenceSet->AddXY(wlf->Cmax, wlf->MWFT);
  }

  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void TfrmMOGWOCalcProgress::DrawNonDominatedFront()
{
  psParetoFront->Clear();
  psReferenceSet->Clear();
  DMOSP_MOGWO *mogwo = (DMOSP_MOGWO *)Alg;

  DMOSP_Wolf **Archive = mogwo->getArchive();

  for(int i=0; i<mogwo->getnArchive(); i++){
	DMOSP_Wolf *wlf = Archive[i];
	psParetoFront->AddXY(wlf->Cmax, wlf->MWFT);
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
void TfrmMOGWOCalcProgress::DrawOptimalParetoFront(int i)
{
  if(i < 0 || i > 30) return;
  psOptimalParetoFront->Clear();
  for(int s = 0; s < SolvedDMOSPs[i].nSol; s++){
	psOptimalParetoFront->AddXY(SolvedDMOSPs[i].Cmax[s], SolvedDMOSPs[i].MWFT[s],"", clYellow);
  }
  ParetoChart->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOGWOCalcProgress::FormShow(TObject *)
{
  grdInitialSolutions->Cells[0][0] = "Sol#";
  grdInitialSolutions->Cells[1][0] = "Cmax_i";
  grdInitialSolutions->Cells[2][0] = "Cmax_f";
  grdInitialSolutions->Cells[3][0] = "MWFT_i";
  grdInitialSolutions->Cells[4][0] = "MWFT_f";

  sgGenerations->Cells[0][0] = "Itr#";
  sgGenerations->Cells[1][0] = "best Cmax";
  sgGenerations->Cells[2][0] = "best MWFT";
  sgGenerations->Cells[3][0] = "n Archive";
  //sgGenerations->Cells[4][0] = "var.MWFT";
  //sgGenerations->Cells[5][0] = "n fronts";
  //sgGenerations->Cells[6][0] = "delta fronts";

  ClearForm();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOGWOCalcProgress::ExportWMF1Click(TObject *)
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

