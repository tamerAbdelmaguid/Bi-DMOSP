//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormNSGAII.h"
#include "OBChromosome.h"
#include "FormSol.h"
#include "FormNSGA2CalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmNSGAII *frmNSGAII;
//---------------------------------------------------------------------------
__fastcall TfrmNSGAII::TfrmNSGAII(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  nsga2Alg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmNSGAII::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);

  lblGD->Caption = "";
  lblIGD->Caption = "";
  lblTGD->Caption = "";
  lblHV->Caption = "";
  lblHV0->Caption = "";
  lblHVpr->Caption = "";
  lblHV0pr->Caption = "";
  lstPareto->Clear();

  // Allocate memory for the NSGA-II algorithm
  nsga2Alg = new DMOSP_NSGAII(Problem);

  // Assigning initial default values for the parameters for NSGA-II
  edtPopSize->Text = "200";
  edtMaxGenerations->Text = "500";
  edtMaxTimeLimit->Text = "60";

  rgMutation->ItemIndex = 1;
  rgCrossover->ItemIndex = 0;

  edtpCrossover->Text = "0.8";
  edtpMutation->Text = "0.5";
  edtpMutationRLB->Text = "0.1";

  edtSAprop->Text = "1.0";
  edtSAT0->Text = "100";
  edtSAMaxIter->Text = "1000";
  edtSACoolingRate->Text = "0.95";

  pgGAType->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmNSGAII::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(nsga2Alg) delete nsga2Alg;        nsga2Alg = NULL;
  TfrmAlgorithm::FormClose(Sender, Action);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmNSGAII::ReadSettings()
{
  String s;

  try{
	s = edtPopSize->Text;
	ps = StrToInt(s);
	s = edtMaxGenerations->Text;
	maxgen = StrToInt(s);
	s = edtMaxTimeLimit->Text;
	timelimit = StrToFloat(s);
	xOpr = (DMOSP_NSGAII::DMOSP_XOVER_OPERATOR)rgCrossover->ItemIndex;
	s = edtpCrossover->Text;
	xPrb = StrToFloat(s);
	mOpr = (DMOSP_NSGAII::DMOSP_MUTATION_OPERATOR)rgMutation->ItemIndex;
	s = edtpMutation->Text;
	mPrb = StrToFloat(s);
	s = edtpMutationRLB->Text;
	rlsMutProb = StrToFloat(s);
	s = edtSAprop->Text;
	SAProp = StrToFloat(s);
	s = edtSAT0->Text;
	SAT0 = StrToFloat(s);
	s = edtSAMaxIter->Text;
	SAMaxIter = StrToInt(s);
	s = edtSACoolingRate->Text;
    SACoolingRate = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return false;
	}

  wchar_t *ermsg = L"Error entered numbers";

  if(ps <= 10 || ps%2 != 0){
	Application->MessageBox(L"population size must be greater than 10 and even.", ermsg, MB_OK);
	return false;
  }
  if(maxgen <= 0){
	Application->MessageBox(L"Choose at least 1 generations.", ermsg, MB_OK);
	return false;
  }
  if(xPrb < 0 || xPrb > 1.0){
	Application->MessageBox(L"Inappropriate crossover probability!.", ermsg, MB_OK);
	return false;
  }
  if(mPrb < 0 || mPrb > 1.0){
	Application->MessageBox(L"Inappropriate mutation probability!.", ermsg, MB_OK);
	return false;
  }
  if(SAProp < 0 || SAProp > 1.0){
	Application->MessageBox(L"Inappropriate SA application rate!.", ermsg, MB_OK);
	return false;
  }
  return true;
}

//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII::lstParetoClick(TObject *)
{
  int i = lstPareto->ItemIndex;
  if(i == -1) return;

  if(nsga2Alg){
	DMOSP_Solution *sol = (DMOSP_Solution *)nsga2Alg->P.Node(i)->Data();
	frmSolution->SetCurrentSolution(sol);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmNSGAII::btnRunClick(TObject *)
{
  if(!ReadSettings()) return;  // Read NSGA-II settings and allocate memory
  lstPareto->Clear();

  if(nsga2Alg) delete nsga2Alg;
  nsga2Alg = new DMOSP_NSGAII(Problem, ps, maxgen, timelimit*1000.0,
							  xPrb, xOpr, mPrb, mOpr, rlsMutProb, SAMaxIter,
							  SAT0, SACoolingRate, SAProp);
  frmNSGA2CalcProgress->Show(nsga2Alg);

  char buf[64];
  int c = nsga2Alg->P.Count();
  if(c != 0){
	for(int i=0; i<c; i++) lstPareto->AddItem(IntToStr(i+1), NULL);
	lstPareto->ItemIndex = 0;
	lstParetoClick(NULL);

	lblTime->Caption = FloatToStr(nsga2Alg->GetTime());
	sprintf(buf, "%.04f", nsga2Alg->GD);
	lblGD->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->IGD);
	lblIGD->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->TGD);
	lblTGD->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->HV);
	lblHV->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->HV0);
	lblHV0->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->HVpr);
	lblHVpr->Caption = buf;
	sprintf(buf, "%.04f", nsga2Alg->HV0pr);
	lblHV0pr->Caption = buf;
  }
}
//---------------------------------------------------------------------------
