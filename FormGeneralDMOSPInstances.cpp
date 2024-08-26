//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormGeneralDMOSPInstances.h"
#include "Problem.h"
#include "FormProblem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmGeneralDMOSPInstances *frmGeneralDMOSPInstances;
//---------------------------------------------------------------------------
__fastcall TfrmGeneralDMOSPInstances::TfrmGeneralDMOSPInstances(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralDMOSPInstances::FormShow(TObject *)
{
  edtnReplicates->Text = "5";
  UpdateProblemType();
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralDMOSPInstances::btnGenerateClick(TObject *)
{
  String s;
  String prbname;

  int nReplicates;
  s = edtnReplicates->Text;
  try {
	nReplicates = StrToInt(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate values for the number of replicates",
							  L"Error reading numbers", MB_OK);
	  return;
	}

  int nPCs, nJobs, qwLevel, PTFLevel;
  double LL;

  if(rgProblemType->ItemIndex == 0){
	nPCs = 5;
	nJobs = 10;
	LL = 0.7;
  }
  else if(rgProblemType->ItemIndex == 1){
	nPCs = (cbxNoOfPCs->ItemIndex == 0)? 10 : 20;
	switch(cbxnJobsTonWCs->ItemIndex){
	  case 0: nJobs = 2 * nPCs;  break;
	  default: nJobs = 4 * nPCs;  break;
	}
	switch(cbxLoadingLevel->ItemIndex){
	  case 0:  LL = 0.9;   break;
	  default:  LL = 0.5;
	}

  }
  qwLevel = 0; // from 1 to 5      //cbxqwLevel->ItemIndex + 1;
  PTFLevel = cbxPTF->ItemIndex + 1;

  int PLJLevel = cbxPercentLateJobs->ItemIndex + 1;
  int JRTFLevel = cbxJRTF->ItemIndex + 1;
  int PBMCLevel = cbxPercentBusyMachines->ItemIndex + 1;
  int MCATFLevel = cbxMATF->ItemIndex + 1;

  for(int r = 1; r <= nReplicates; r++){
	prbname = "DMOSP-L-" + IntToStr(nPCs) + "-" + IntToStr(nJobs) + "-" +
			  FloatToStr(LL) + "-" + ((PLJLevel == 1)? "30" : "50")
			  + "-" + ((PBMCLevel == 1)? "30" : "50")
			  + "-" + IntToStr(r);
	DMOSP_Problem *newProblem = new DMOSP_Problem(prbname.c_str(), L"DMOSP problem",
									 nPCs, nJobs, qwLevel, LL, PTFLevel, PLJLevel,
									 JRTFLevel, PBMCLevel, MCATFLevel);
	TfrmProblem *Child = new TfrmProblem(Application, newProblem, prbname);
	Child->Caption = prbname;
	Child->tbSolvedSetParetoFront->TabVisible = false;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralDMOSPInstances::UpdateProblemType()
{
  if(rgProblemType->ItemIndex == 0){
	cbxNoOfPCs->Items->Clear();
	cbxNoOfPCs->Items->Add("5");
	cbxnJobsTonWCs->Items->Clear();
	cbxnJobsTonWCs->Items->Add("2");
	cbxqwLevel->Items->Clear();
	cbxqwLevel->Items->Add("1 to 2");
	cbxqwLevel->Enabled = false;
	cbxLoadingLevel->Items->Clear();
	cbxLoadingLevel->Items->Add("0.7");
	cbxLoadingLevel->ItemIndex = 0;
	cbxPTF->Items->Clear();
	cbxPTF->Items->Add("0.8 to 1.2");
	cbxPTF->ItemIndex = 0;
	cbxPercentLateJobs->Items->Clear();
	cbxPercentLateJobs->Items->Add("50%");
	cbxPercentLateJobs->ItemIndex = 0;
	cbxJRTF->Items->Clear();
	cbxJRTF->Items->Add("0.2 to 2");
	cbxJRTF->ItemIndex = 0;
	cbxPercentBusyMachines->Items->Clear();
	cbxPercentBusyMachines->Items->Add("50%");
	cbxPercentBusyMachines->ItemIndex = 0;
	cbxMATF->Items->Clear();
	cbxMATF->Items->Add("0.2 to 2");
	cbxMATF->ItemIndex = 0;
  }
  else if(rgProblemType->ItemIndex == 1){
	cbxNoOfPCs->Items->Clear();
	cbxNoOfPCs->Items->Add("10");
	cbxNoOfPCs->Items->Add("20");
	cbxnJobsTonWCs->Items->Clear();
	cbxnJobsTonWCs->Items->Add("2");
	cbxnJobsTonWCs->Items->Add("4");
	cbxqwLevel->Enabled = true;
	cbxqwLevel->Items->Clear();
	//cbxqwLevel->Items->Add("1 to 2");
	//cbxqwLevel->Items->Add("1 to 3");
	cbxqwLevel->Items->Add("1 to 5");
	cbxqwLevel->Enabled = false;
	cbxLoadingLevel->Items->Clear();
	cbxLoadingLevel->Items->Add("0.9");
	cbxLoadingLevel->Items->Add("0.5");
	//cbxLoadingLevel->Items->Add("0.5");
	cbxLoadingLevel->ItemIndex = 0;
	cbxPTF->Items->Clear();
	cbxPTF->Items->Add("0.8 to 1.2");
	//cbxPTF->Items->Add("0.6 to 1.4");
	cbxPTF->ItemIndex = 0;
	cbxPercentLateJobs->Items->Clear();
	cbxPercentLateJobs->Items->Add("30%");
	cbxPercentLateJobs->Items->Add("50%");
	cbxPercentLateJobs->ItemIndex = 0;
	cbxJRTF->Items->Clear();
	cbxJRTF->Items->Add("0.2 to 2");
	//cbxJRTF->Items->Add("2.1 to 4");
	cbxJRTF->ItemIndex = 0;
	cbxPercentBusyMachines->Items->Clear();
	cbxPercentBusyMachines->Items->Add("30%");
	cbxPercentBusyMachines->Items->Add("50%");
	cbxPercentBusyMachines->ItemIndex = 0;
	cbxMATF->Items->Clear();
	cbxMATF->Items->Add("0.2 to 2");
	//cbxMATF->Items->Add("2.1 to 4");
	cbxMATF->ItemIndex = 0;
  }
  cbxNoOfPCs->ItemIndex = 0;
  cbxnJobsTonWCs->ItemIndex = 0;
  cbxqwLevel->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmGeneralDMOSPInstances::rgProblemTypeClick(TObject *)
{
  UpdateProblemType();
}
//---------------------------------------------------------------------------

