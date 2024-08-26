//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSolvedSetMultiRuns.h"
#include "FileSystem.h"
#include "TuningInstances.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSolvedSetMultiRuns *frmSolvedSetMultiRuns;

char *sBaseName = "DMOSP-5-10-1-0.7-1-2-1-2-1-";
int iStart[] = {1, 4, 7, 10, 13, 16, 19, 22, 25, 28};
int iNum[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

//---------------------------------------------------------------------------
__fastcall TfrmSolvedSetMultiRuns::TfrmSolvedSetMultiRuns(TComponent* Owner)
	: TForm(Owner)
{
  nsga2Alg = NULL;
  mossAlg = NULL;
  mogwoAlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSolvedSetMultiRuns::lstGroupsClick(TObject *)
{
  char buf[32];

  int i = lstGroups->ItemIndex;
  if(i>=0){
	lstInstances->Clear();
	for(int p = 0; p < iNum[i]; p++){
	  sprintf(buf, "%s%i", sBaseName, iStart[i]+p);
	  lstInstances->Items->Add(buf);
	}
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmSolvedSetMultiRuns::FormShow(TObject *)
{
  lstGroups->Enabled = true;
  cbxNSGA2->Enabled = true;
  cbxNSGA2->Checked = true;
  cbxMOSS->Enabled = true;
  cbxMOSS->Checked = true;
  cbxMOGWO->Enabled = true;
  cbxMOGWO->Checked = false;
  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSolvedSetMultiRuns::btnRunClick(TObject *)
{
  if(lstInstances->Items->Count == 0){
	Application->MessageBoxA(L"Please select a group", L"No group selected", MB_OK);
	return;
  }
  if(!cbxNSGA2->Checked && !cbxMOSS->Checked && !cbxMOGWO->Checked){
	Application->MessageBoxA(L"Please select at least one metaheuristic",
							 L"No metaheuristic selected", MB_OK);
	return;
  }

  lstGroups->Enabled = false;
  cbxNSGA2->Enabled = false;
  cbxMOSS->Enabled = false;
  cbxMOGWO->Enabled = false;
  btnRun->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;

  int nInstances = lstInstances->Items->Count;
  int barLength = 0;
  if(cbxNSGA2->Checked) barLength++;
  if(cbxMOSS->Checked) barLength++;
  if(cbxMOGWO->Checked) barLength++;
  barLength *= (30 * nInstances);
  pbRun->Min = 0;
  pbRun->Max = barLength;
  pbRun->Position = 0;

  int g = lstGroups->ItemIndex;      // group #
  char strbuf[300];
  sprintf(strbuf, "MultiRunsResults-%i.txt", g+1);
  FILE *ResultsFile = fopen(strbuf, "a+");

  fprintf(ResultsFile, "Prob #\t|P|\tHV0\tHV0\%\tRun #");
  if(cbxNSGA2->Checked)
	 fprintf(ResultsFile, "\t|D|_GA\tGD_GA\tIGD_GA\tTGD_GA\tHV_GA\tHV\%_GA\tDHV\%_GA");
  if(cbxMOSS->Checked)
	 fprintf(ResultsFile, "\t|D|_SS\tGD_SS\tIGD_SS\tTGD_SS\tHV_SS\tHV\%_SS\tDHV\%_SS");
  if(cbxMOGWO->Checked)
	 fprintf(ResultsFile, "\t|D|_GW\tGD_GW\tIGD_GW\tTGD_GW\tHV_GW\tHV\%_GW\tDHV\%_GW");
  fprintf(ResultsFile, "\n");
  fflush(ResultsFile);

  static String SolvedSetDir = ".\\SolvedSet\\";
  String sFileName;
  bool bInterrupted = false;
  int i;   // instance index
  for(int i=0; i<nInstances; i++){
	// Load the test instance....
	String InstName = lstInstances->Items->operator [](i);
	lblInstName->Caption = InstName;

	sFileName = SolvedSetDir + InstName + ".dmosp";
	DMOSP_FIO_RESULT Result;
	Instance = LoadDMOSP(sFileName.c_str(), Result);
	String msg;
	if(Result != SUCCESS){
	  switch(Result){
		case FAIL_TO_OPEN:
		  msg = "failed to open file";
		  break;
		case UNKNOWN_FILE_TYPE:
		  msg = "unknown file type!";
		  break;
		case DATA_ERROR:
		  msg = "Error in data";
		  break;
	  }
	  Application->MessageBox(msg.c_str(), L"Error reading file", MB_OK);
	  break;
	}

	int probno = iStart[g]+i-1;
	Instance->i = probno;

	// Just to calculate *** HV0 ***
	double HV0, HV0pr;
	if(nsga2Alg) delete nsga2Alg;
	nsga2Alg = new DMOSP_NSGAII(Instance, 10, 10, -1,
								0.1, DMOSP_NSGAII::SINGLE_POINT_OX,
								0.1, DMOSP_NSGAII::SWAP_MUTATION,
								0.05, 10, 10, 0.95, 0.1);
	nsga2Alg->Run();
	HV0 = nsga2Alg->HV0;
	HV0pr = nsga2Alg->HV0pr;

	// Conduct 30 runs of both algorithms
	for(int r=1; r<=30; r++){
	  sprintf(strbuf, "%i\t%i\t%f\t%f\t", probno+1, SolvedDMOSPs[probno].nSol,
	                                      HV0, HV0pr);
	  fprintf(ResultsFile, strbuf);
	  fflush(ResultsFile);

	  lblRunNo->Caption = IntToStr(r);

	  sprintf(strbuf, "%i", r);
	  fprintf(ResultsFile, strbuf);
	  fflush(ResultsFile);

	  // Run NSGA-II if checked
	  if(cbxNSGA2->Checked){
		lblAlg->Caption = "NSGA-II";
		if(nsga2Alg) delete nsga2Alg;

		nsga2Alg = new DMOSP_NSGAII(Instance, 200, 1000, 60000.0,
									0.8, DMOSP_NSGAII::SINGLE_POINT_OX,
									0.5, DMOSP_NSGAII::SWAP_MUTATION,
									0.1, 1000, 100, 0.95, 1.0);
		nsga2Alg->Run();
		if(nsga2Alg->bInterrupt){
		  bInterrupted = true;
		  break;
		}

		sprintf(strbuf, "\t%i\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f",
		   nsga2Alg->P.Count(), nsga2Alg->GD, nsga2Alg->IGD, nsga2Alg->TGD,
		   nsga2Alg->HV, nsga2Alg->HVpr, nsga2Alg->HV0pr - nsga2Alg->HVpr);
		fprintf(ResultsFile, strbuf);
		fflush(ResultsFile);

		pbRun->Position = pbRun->Position + 1;
	  }

	  // Run MOSS if checked
	  if(cbxMOSS->Checked){
		lblAlg->Caption = "MOSS";
		if(mossAlg) delete mossAlg;

		mossAlg = new DMOSP_MOSS(Instance);

		mossAlg->Run();
		if(mossAlg->bInterrupt){
		  bInterrupted = true;
		  break;
		}

		sprintf(strbuf, "\t%i\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f",
		   mossAlg->getNDSize(), mossAlg->GD, mossAlg->IGD, mossAlg->TGD,
		   mossAlg->HV, mossAlg->HVpr, mossAlg->HV0pr - mossAlg->HVpr);
		fprintf(ResultsFile, strbuf);
		fflush(ResultsFile);

		pbRun->Position = pbRun->Position + 1;
	  }

	  // Run MOGWO if checked
	  if(cbxMOGWO->Checked){
		lblAlg->Caption = "MOGWO";
		if(mogwoAlg) delete mogwoAlg;

		mogwoAlg = new DMOSP_MOGWO(Instance, 200, 1000, 60000.0, 100,
								   0.1, 10, 4,
								   2, DMOSP_MOGWO::SWAP_MUTATION, 1000, 100, 0.95,
								   1.0);

		mogwoAlg->Run();
		if(mogwoAlg->bInterrupt){
		  bInterrupted = true;
		  break;
		}

		sprintf(strbuf, "\t%i\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f",
		   mogwoAlg->P.Count(), mogwoAlg->GD, mogwoAlg->IGD, mogwoAlg->TGD,
		   mogwoAlg->HV, mogwoAlg->HVpr, mogwoAlg->HV0pr - mogwoAlg->HVpr);
		fprintf(ResultsFile, strbuf);
		fflush(ResultsFile);

		pbRun->Position = pbRun->Position + 1;
	  }

	  fprintf(ResultsFile, "\n");
	}

	delete Instance;
	if(bInterrupted) break;
  }

  if(nsga2Alg) delete nsga2Alg;      nsga2Alg = NULL;
  if(mossAlg)  delete mossAlg;       mossAlg = NULL;
  if(mogwoAlg) delete mogwoAlg;      mogwoAlg = NULL;

  lstGroups->Enabled = true;
  cbxNSGA2->Enabled = true;
  cbxMOSS->Enabled = true;
  cbxMOGWO->Enabled = true;
  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;

  fflush(ResultsFile);
  fclose(ResultsFile);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolvedSetMultiRuns::btnTerminateClick(TObject *)
{
  if(nsga2Alg) nsga2Alg->bInterrupt = true;
  if(mossAlg) mossAlg->bInterrupt = true;
  if(mogwoAlg) mogwoAlg->bInterrupt = true;

  lstGroups->Enabled = true;
  cbxNSGA2->Enabled = true;
  cbxMOSS->Enabled = true;
  cbxMOGWO->Enabled = true;
  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSolvedSetMultiRuns::FormClose(TObject *, TCloseAction &)

{
  if(nsga2Alg) delete nsga2Alg;      nsga2Alg = NULL;
  if(mogwoAlg) delete mogwoAlg;      mogwoAlg = NULL;

  lstGroups->Enabled = true;
  cbxNSGA2->Enabled = true;
  cbxMOSS->Enabled = true;
  cbxMOGWO->Enabled = true;
  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;

  this->Close();
}
//---------------------------------------------------------------------------

