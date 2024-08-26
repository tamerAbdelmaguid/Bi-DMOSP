//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <io.h>
#include <stdio.h>
#include <errno.h>
#include "FormTuningMOGWO.h"
#include "TuningInstances.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmTuningMOGWO *frmTuningMOGWO;
//---------------------------------------------------------------------------
__fastcall TfrmTuningMOGWO::TfrmTuningMOGWO(TComponent* Owner)
	: TForm(Owner)
{
   bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::FormShow(TObject *)
{
  grdLoadedInstances->ColCount = 4;
  grdLoadedInstances->RowCount = N_INSTANCES + 1;
  grdLoadedInstances->Cells[0][0] = "No.";
  grdLoadedInstances->Cells[1][0] = "Instance name";
  grdLoadedInstances->Cells[2][0] = "Level";
  grdLoadedInstances->Cells[3][0] = "Status";

  memoTCs->Clear();
  memoGD->Clear();
  memoIGD->Clear();
  memoDHVpr->Clear();
  memoTime->Clear();
  lblTCReadResult->Caption = "";

  memoTCs->Enabled = false;
  btnReadRuns->Enabled = false;
  btnRun->Enabled = false;
  btnTerminate->Enabled = false;

  // Allocate memory for instances
  Instance = new DMOSP_Problem*[N_INSTANCES];
  for(int i=0; i<N_INSTANCES; i++) Instance[i] = NULL;

  TCs = NULL;
  mogwoAlg = NULL;

  bActive = true;

  pbRun->Position = 0;
  pbConstruction->Position = 0;
  pbIterations->Position = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::btnLoadProblemsClick(TObject *)
{
  static String SolvedSetDir = ".\\SolvedSet\\";
  String sFileName;
  for(int i=0; i<N_INSTANCES; i++){
	sFileName = SolvedSetDir + InstName[i] + ".dmosp";
	DMOSP_FIO_RESULT Result;
	Instance[i] = LoadDMOSP(sFileName.c_str(), Result);
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
	  return;
	}
	Instance[i]->i = SolvedProbNo[i];

	grdLoadedInstances->Cells[0][i+1] = IntToStr(i+1);
	grdLoadedInstances->Cells[1][i+1] = InstName[i];
	grdLoadedInstances->Cells[2][i+1] = IntToStr(InstLevel[i]);
	grdLoadedInstances->Cells[3][i+1] = "Loaded";
  }

  btnLoadProblems->Enabled = false;
  memoTCs->Enabled = true;
  btnReadRuns->Enabled = true;
  memoTCs->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::FormClose(TObject *, TCloseAction &)
{
  for(int i=0; i<N_INSTANCES; i++){
	if(Instance[i]) delete Instance[i];
  }
  delete [] Instance;

  if(TCs) delete [] TCs;
  if(mogwoAlg) delete mogwoAlg;      mogwoAlg = NULL;

  bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::btnReadRunsClick(TObject *)
{
  nTCs = memoTCs->Lines->Count;
  if(nTCs == 0) return;
  TCs = new DMOSP_TreatmentCombination[nTCs];

  char buf[160];
  unsigned int strpos, bufpos;
  char *str;
  String msg;
  bool bError = false;
  for(int ln=0; ln<nTCs; ln++){
	str = memoTCs->Lines->Strings[ln].t_str();
	strpos = 0;

	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].popsize = atoi(buf);
	if(TCs[ln].popsize == 0){
	   msg = "Error reading population size";
	   bError = true;
	   break;
	}

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].nGrid = atoi(buf);
	if(TCs[ln].nGrid == 0){
	   msg = "Error reading number of grids";
	   bError = true;
	   break;
	}

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].alpha = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].beta = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].gamma = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].p_SA = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	if(strcmp(buf, "Shift") == 0) TCs[ln].MOp = DMOSP_MOGWO::SHIFT_MUTATION;
	else if(strcmp(buf, "Swap") == 0) TCs[ln].MOp = DMOSP_MOGWO::SWAP_MUTATION;
	else{
	   msg = "Error reading mutation operator";
	   bError = true;
	   break;
	}

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t' && str[strpos] != '\0'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].PSize = atoi(buf);
	if(TCs[ln].PSize == 0 && !(TCs[ln].PSize == 2 || TCs[ln].PSize == 4 ||
							   TCs[ln].PSize == 8 || TCs[ln].PSize == 11 ||
							   TCs[ln].PSize == 14 || TCs[ln].PSize == 17)){
	   msg = "Error reading size of Pareto front";
	   bError = true;
	   break;
	}
  }

  if(!bError){
	msg = IntToStr(nTCs) + " treatment combinations are successfully read.";

	pbRun->Max = nTCs;
	pbRun->Position = 0;
	pbConstruction->Position = 0;
	pbIterations->Position = 0;

	memoTCs->Enabled = false;
	btnReadRuns->Enabled = false;
	btnRun->Enabled = true;
	btnTerminate->Enabled = true;
  }
  lblTCReadResult->Caption = msg;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::btnRunClick(TObject *)
{
  char strbuf[128];

  FILE *ResultsFile = fopen("MOGWO-Results.txt", "a+");;

  btnRun->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;

  for(int r=0; r<nTCs; r++){
	pbIterations->Position = 0;
	pbConstruction->Position = 0;

	DMOSP_Problem *Problem = NULL;
	switch(TCs[r].PSize){
	   case 2: // DMOSP-5-10-1-0.7-1-2-1-2-1-7
			   Problem = Instance[0];
			   break;
	   case 4: // DMOSP-5-10-1-0.7-1-2-1-2-1-6
			   Problem = Instance[1];
			   break;
	   case 8: // DMOSP-5-10-1-0.7-1-2-1-2-1-24
			   Problem = Instance[2];
			   break;
	   case 11: // DMOSP-5-10-1-0.7-1-2-1-2-1-17
			   Problem = Instance[3];
			   break;
	   case 14: // DMOSP-5-10-1-0.7-1-2-1-2-1-4
			   Problem = Instance[4];
			   break;
	   case 17: // DMOSP-5-10-1-0.7-1-2-1-2-1-15
			   Problem = Instance[5];
			   break;
	}
	if(Problem == NULL) break;

	mogwoAlg = new DMOSP_MOGWO(Problem, TCs[r].popsize, 300, -1, 100,
							   TCs[r].alpha, TCs[r].nGrid, TCs[r].beta,
							   TCs[r].gamma, TCs[r].MOp, 1000, 100, 0.95,
							   TCs[r].p_SA);
	mogwoAlg->Run();
	if(mogwoAlg->bInterrupt){
	  delete mogwoAlg;
	  break;
	}

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", mogwoAlg->GD);
	memoGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", mogwoAlg->IGD);
	memoIGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", mogwoAlg->TGD);
	memoTGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", mogwoAlg->HV0pr - mogwoAlg->HVpr);
	memoDHVpr->Lines->Strings[r] = strbuf;

	double ctime = mogwoAlg->GetTime();
	ctime /= 1000.0;
	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", ctime);
	memoTime->Lines->Strings[r] = strbuf;

	pbRun->Position = r+1;

	sprintf(strbuf, "%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\n",
					mogwoAlg->GD, mogwoAlg->IGD, mogwoAlg->TGD,
					mogwoAlg->HV0pr - mogwoAlg->HVpr, ctime);
	fprintf(ResultsFile, strbuf);
	fflush(ResultsFile);

	delete mogwoAlg;
  }
  mogwoAlg = NULL;

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  fclose(ResultsFile);
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOGWO::btnTerminateClick(TObject *)
{
  if(mogwoAlg) mogwoAlg->bInterrupt = true;
  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
