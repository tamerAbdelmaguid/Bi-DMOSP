//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <io.h>
#include <stdio.h>
#include <errno.h>
#include "FormTuningNSGA2.h"
#include "TuningInstances.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmTuningNSGA2 *frmTuningNSGA2;
//---------------------------------------------------------------------------
__fastcall TfrmTuningNSGA2::TfrmTuningNSGA2(TComponent* Owner)
	: TForm(Owner)
{
   bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningNSGA2::FormShow(TObject *)
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
  nsga2Alg = NULL;

  bActive = true;

  pbRun->Position = 0;
  pbConstruction->Position = 0;
  pbIterations->Position = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningNSGA2::btnLoadProblemsClick(TObject *)
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
void __fastcall TfrmTuningNSGA2::FormClose(TObject *, TCloseAction &)
{
  for(int i=0; i<N_INSTANCES; i++){
	if(Instance[i]) delete Instance[i];
  }
  delete [] Instance;

  if(TCs) delete [] TCs;
  if(nsga2Alg) delete nsga2Alg;      nsga2Alg = NULL;

  bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningNSGA2::btnReadRunsClick(TObject *)
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
	if(strcmp(buf, "OX1p") == 0) TCs[ln].XOp = DMOSP_NSGAII::SINGLE_POINT_OX;
	else if(strcmp(buf, "OX2p") == 0) TCs[ln].XOp = DMOSP_NSGAII::TWO_POINT_OX;
	else if(strcmp(buf, "OX-u") == 0) TCs[ln].XOp = DMOSP_NSGAII::UNIFORM_OX;
	else if(strcmp(buf, "GPX") == 0) TCs[ln].XOp = DMOSP_NSGAII::GENPOSX;
	else if(strcmp(buf, "SB2OX") == 0) TCs[ln].XOp = DMOSP_NSGAII::SIMBLOCK2OX;
	else{
	   msg = "Error reading crossover operator";
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
	if(strcmp(buf, "Shift") == 0) TCs[ln].MOp = DMOSP_NSGAII::SHIFT_MUTATION;
	else if(strcmp(buf, "Swap") == 0) TCs[ln].MOp = DMOSP_NSGAII::SWAP_MUTATION;
	else{
	   msg = "Error reading mutation operator";
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
	TCs[ln].prob_xover = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].prob_mut = atof(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].prob_RLS = atof(buf);

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
void __fastcall TfrmTuningNSGA2::btnRunClick(TObject *)
{
  char strbuf[128];

  FILE *ResultsFile = fopen("NSGA2-Results.txt", "a+");;

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

	nsga2Alg = new DMOSP_NSGAII(Problem, TCs[r].popsize, 300, -1,
								TCs[r].prob_xover, TCs[r].XOp,
								TCs[r].prob_mut, TCs[r].MOp,
								TCs[r].prob_RLS, 1000, 100, 0.95,
								TCs[r].p_SA);

	nsga2Alg->Run();
	if(nsga2Alg->bInterrupt){
	  delete nsga2Alg;
	  break;
	}

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", nsga2Alg->GD);
	memoGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", nsga2Alg->IGD);
	memoIGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", nsga2Alg->TGD);
	memoTGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", nsga2Alg->HV0pr - nsga2Alg->HVpr);
	memoDHVpr->Lines->Strings[r] = strbuf;

	double ctime = nsga2Alg->GetTime();
	ctime /= 1000.0;
	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", ctime);
	memoTime->Lines->Strings[r] = strbuf;

	pbRun->Position = r+1;

	sprintf(strbuf, "%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\n",
					nsga2Alg->GD, nsga2Alg->IGD, nsga2Alg->TGD,
					nsga2Alg->HV0pr - nsga2Alg->HVpr, ctime);
	fprintf(ResultsFile, strbuf);
	fflush(ResultsFile);

	delete nsga2Alg;
  }
  nsga2Alg = NULL;

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  fclose(ResultsFile);
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningNSGA2::btnTerminateClick(TObject *)
{
  if(nsga2Alg) nsga2Alg->bInterrupt = true;
  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
