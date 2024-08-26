//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMOGWO.h"
#include "FormSol.h"
#include "FormMOGWOCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmMOGWO *frmMOGWO;
//---------------------------------------------------------------------------
__fastcall TfrmMOGWO::TfrmMOGWO(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  mogwo = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOGWO::FormShow(TObject *Sender)
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
  mogwo = new DMOSP_MOGWO(Problem);

  // Assigning initial default values for the parameters for NSGA-II
  edtnGreyWolves->Text = "200";
  edtMaxIterations->Text = "500";
  edtMaxTimeLimit->Text = "60";

  edtnGrids->Text = "10";
  edtArchiveSize->Text = "100";
  edtAlpha->Text = "0.5";
  edtBeta->Text = "4";
  edtGamma->Text = "2";

  rgMutation->ItemIndex = 1;

  edtSAprop->Text = "1.0";
  edtSAT0->Text = "100";
  edtSAMaxIter->Text = "1000";
  edtSACoolingRate->Text = "0.95";

  pgMOGWOParameters->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOGWO::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(mogwo) delete mogwo;        mogwo = NULL;
  TfrmAlgorithm::FormClose(Sender, Action);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmMOGWO::ReadSettings()
{
  String s;

  try{
	s = edtnGreyWolves->Text;
	nGWs = StrToInt(s);
	s = edtMaxIterations->Text;
	nIter = StrToInt(s);
	s = edtMaxTimeLimit->Text;
	timelimit = StrToFloat(s);
	s = edtnGrids->Text;
	ngrd = StrToInt(s);
	s = edtArchiveSize->Text;
	szArch = StrToInt(s);
	s = edtAlpha->Text;
	a = StrToFloat(s);
	s = edtBeta->Text;
	b = StrToFloat(s);
	s = edtGamma->Text;
    g = StrToFloat(s);

	mOpr = (DMOSP_MOGWO::DMOSP_MUTATION_OPERATOR)rgMutation->ItemIndex;

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

  if(nGWs < 10){
	Application->MessageBox(L"Choose at least 10 grey wolves.", ermsg, MB_OK);
	return false;
  }
  if(nIter < 10){
	Application->MessageBox(L"Choose at least 10 iterations.", ermsg, MB_OK);
	return false;
  }
  if(SAProp < 0 || SAProp > 1.0){
	Application->MessageBox(L"Inappropriate SA application rate!.", ermsg, MB_OK);
	return false;
  }
  return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMOGWO::lstParetoClick(TObject *)
{
  int i = lstPareto->ItemIndex;
  if(i == -1) return;

  if(mogwo){
	DMOSP_Solution *sol = (DMOSP_Solution *)mogwo->P.Node(i)->Data();
	frmSolution->SetCurrentSolution(sol);
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOGWO::btnRunClick(TObject *)
{
  if(!ReadSettings()) return;  // Read NSGA-II settings and allocate memory
  lstPareto->Clear();

  if(mogwo) delete mogwo;
  mogwo = new DMOSP_MOGWO(Problem, nGWs, nIter, timelimit*1000.0,
						  szArch, a, ngrd, b, g, mOpr, SAMaxIter, SAT0,
				          SACoolingRate, SAProp);

  frmMOGWOCalcProgress->Show(mogwo);

  char buf[64];
  int c = mogwo->P.Count();
  if(c != 0){
	for(int i=0; i<c; i++) lstPareto->AddItem(IntToStr(i+1), NULL);
	lstPareto->ItemIndex = 0;
	lstParetoClick(NULL);

	lblTime->Caption = FloatToStr(mogwo->GetTime());
	sprintf(buf, "%.04f", mogwo->GD);
	lblGD->Caption = buf;
	sprintf(buf, "%.04f", mogwo->IGD);
	lblIGD->Caption = buf;
	sprintf(buf, "%.04f", mogwo->TGD);
	lblTGD->Caption = buf;
	sprintf(buf, "%.04f", mogwo->HV);
	lblHV->Caption = buf;
	sprintf(buf, "%.04f", mogwo->HV0);
	lblHV0->Caption = buf;
	sprintf(buf, "%.04f", mogwo->HVpr);
	lblHVpr->Caption = buf;
	sprintf(buf, "%.04f", mogwo->HV0pr);
	lblHV0pr->Caption = buf;
  }
}
//---------------------------------------------------------------------------
