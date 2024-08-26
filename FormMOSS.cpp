//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMOSS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmMOSS *frmMOSS;
//---------------------------------------------------------------------------
__fastcall TfrmMOSS::TfrmMOSS(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  mossAlg = NULL;
  tmp_mossAlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSS::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);

  lblLB1->Caption = "";
  lblLB2->Caption = "";
  lblLB3->Caption = "";
  lblLB->Caption = "";

  edtRSize->Text = "15";
  edtDeltaThreshold->Text = "";

  lblStatus->Caption = "";

  edtTabuSize->Text = "10";
  edtnImpIterations->Text = "20";
  edtnTSIterations->Text = "40";

  edtTSnBestMoves->Text = "5";
  edtTSnRandomMoves->Text = "5";
  edtTheta_X->Text = "0.5";

  edtnSSPRIterations->Text = "300";
  lblGD->Caption = "";
  lblIGD->Caption = "";
  lblTGD->Caption = "";
  lblHV->Caption = "";
  lblHV0->Caption = "";
  lblHVpr->Caption = "";
  lblHV0pr->Caption = "";
  edtTimeLimit->Text = "60";
  lstPareto->Clear();

  // Suggest delta threshold value
  DMOSP_MOSS *tmp_moss = new DMOSP_MOSS(Problem);
  tmp_moss->AllocateMemory();
  int Dth = tmp_moss->SuggestDeltaThreshold();
  edtDeltaThreshold->Text = IntToStr(Dth);
  tmp_moss->FreeMemory();
  delete tmp_moss;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSS::btnLBClick(TObject *)
{
  tmp_mossAlg = new DMOSP_MOSS(Problem);
  tmp_mossAlg->AllocateMemory();
  double lb1, lb2, lb;
  lb = tmp_mossAlg->CalculateCmaxLowerBound(lb1, lb2);
  lblLB1->Caption = FloatToStr(lb1);
  lblLB2->Caption = FloatToStr(lb2);
  lblLB->Caption = FloatToStr(lb);
  lb = tmp_mossAlg->CalculateMWFTLowerBound();
  lblLB3->Caption = FloatToStr(lb);
  tmp_mossAlg->FreeMemory();
  delete tmp_mossAlg;
  tmp_mossAlg = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSS::btnRunClick(TObject *)
{
  String s;
  int psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr;
  double probX, tLimit;

  lblStatus->Caption = "";

  try{
	s = edtRSize->Text;
	psz = StrToInt(s);
	s = edtDeltaThreshold->Text;
	Dth = StrToInt(s);
	s = edtTabuSize->Text;
	tbsz = StrToInt(s);
	s = edtnTSIterations->Text;
	tsitr = StrToInt(s);
	s = edtnImpIterations->Text;
	tsimpitr = StrToInt(s);
	s = edtTSnBestMoves->Text;
	nbstmvs = StrToInt(s);
	s = edtTSnRandomMoves->Text;
	nrndmvs = StrToInt(s);
	s = edtTheta_X->Text;
    probX = StrToFloat(s);
	s = edtnSSPRIterations->Text;
	nitr = StrToInt(s);
	s = edtTimeLimit->Text;
	tLimit = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return;
	}

  wchar_t *ermsg = L"Error entered numbers";

  if(Dth <= 0){
	Application->MessageBox(L"Delta threshold value must be greater than zero.", ermsg, MB_OK);
	return;
  }
  if(psz <= 1){
	Application->MessageBox(L"Population size must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(tbsz <= 1){
	Application->MessageBox(L"Tabu size must be greater than 1", ermsg, MB_OK);
	return;
  }
  if(tsitr <= 1){
	Application->MessageBox(L"Tabu search iterations must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(tsimpitr <= 1){
	Application->MessageBox(L"Tabu search improvement iterations must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(nbstmvs <= 1){
	Application->MessageBox(L"Tabu search number of best moves must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(nrndmvs <= 1){
	Application->MessageBox(L"Tabu search number of random moves must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(probX < 0 || probX > 1.0){
	Application->MessageBox(L"Crossover probability should be in the range [0,1].", ermsg, MB_OK);
	return;
  }
  if(nitr < 0){
	Application->MessageBox(L"SSPR number of iterations must be greater than 0.", ermsg, MB_OK);
	return;
  }

  if(mossAlg)  delete mossAlg;
  mossAlg = new DMOSP_MOSS(Problem, psz, Dth, tbsz, tsitr, tsimpitr,
                             nbstmvs, nrndmvs, probX, nitr, tLimit*1000.0);
  frmMOSSCalcProgress->Show(mossAlg);

  lblStatus->Caption = "";

  char buf[64];
  int c = mossAlg->ND.Count();
  if(c>0){
	lstPareto->Clear();
	for(int i=0; i<c; i++) lstPareto->AddItem(IntToStr(i+1), NULL);
	lblTime->Caption = FloatToStr(mossAlg->GetTime());

	sprintf(buf, "%.04f", mossAlg->GD);
	lblGD->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->IGD);
	lblIGD->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->TGD);
	lblTGD->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->HV);
	lblHV->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->HV0);
	lblHV0->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->HVpr);
	lblHVpr->Caption = buf;
	sprintf(buf, "%.04f", mossAlg->HV0pr);
	lblHV0pr->Caption = buf;
	lstPareto->ItemIndex = 0;
	lstParetoClick(NULL);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSS::FormClose(TObject *, TCloseAction &)
{
  if(mossAlg) delete mossAlg;           mossAlg = NULL;
  if(tmp_mossAlg) delete tmp_mossAlg;   tmp_mossAlg = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSS::lstParetoClick(TObject *)
{
  if(lstPareto->ItemIndex == -1) return;

  if(mossAlg){
	mossAlg->ChooseNDSolution(lstPareto->ItemIndex);
	frmSolution->SetCurrentSolution(mossAlg->Solution);
  }
}
//---------------------------------------------------------------------------

