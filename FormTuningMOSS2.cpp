//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormTuningMOSS2.h"
#include "TuningInstances.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmTuningMOSS2 *frmTuningMOSS2;
//---------------------------------------------------------------------------
__fastcall TfrmTuningMOSS2::TfrmTuningMOSS2(TComponent* Owner)
	: TForm(Owner)
{
   bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTuningMOSS2::FormShow(TObject *)
{
  grdLoadedInstances->ColCount = 4;
  grdLoadedInstances->RowCount = N_INSTANCES + 1;
  grdLoadedInstances->Cells[0][0] = "No.";
  grdLoadedInstances->Cells[1][0] = "Instance name";
  grdLoadedInstances->Cells[2][0] = "Level";
  grdLoadedInstances->Cells[3][0] = "Status";

  memoTCs->Clear();
  memoTGD->Clear();
  memoDHVpr->Clear();
  lblTCReadResult->Caption = "";

  memoTCs->Enabled = false;
  btnReadRuns->Enabled = false;
  btnRun->Enabled = false;
  btnTerminate->Enabled = false;

  // Allocate memory for instances
  Instance = new DMOSP_Problem*[N_INSTANCES];
  for(int i=0; i<N_INSTANCES; i++) Instance[i] = NULL;

  TCs = NULL;
  mossAlg = NULL;

  bActive = true;

  pbRun->Position = 0;
  pbConstruction->Position = 0;
  pbIterations->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmTuningMOSS2::btnLoadProblemsClick(TObject *)
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

void __fastcall TfrmTuningMOSS2::FormClose(TObject *, TCloseAction &)

{
  for(int i=0; i<N_INSTANCES; i++){
	if(Instance[i]) delete Instance[i];
  }
  delete [] Instance;

  if(TCs) delete [] TCs;
  if(mossAlg) delete mossAlg;      mossAlg = NULL;

  bActive = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmTuningMOSS2::btnReadRunsClick(TObject *)
{
  nTCs = memoTCs->Lines->Count;
  if(nTCs == 0) return;
  TCs = new DMOSP_TreatmentCombination[nTCs];

  char buf[120];
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
	TCs[ln].RefSetSize = atoi(buf);

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].n_TS_maxitr = atoi(buf);

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

void __fastcall TfrmTuningMOSS2::btnRunClick(TObject *)
{
  char strbuf[168];

  FILE *ResultsFile = fopen("MOSS-Results2.txt", "a+");;

  btnRun->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;

  double **TrackingResults = new double*[5];  // at 0, 30s, 60s, 90s, 120s
  for(int i=0; i<5; i++){
	TrackingResults[i] = new double[4]; // DHVpr, GD, IGD, TGD
	memset((void *)TrackingResults[i], 0, 4 * sizeof(double));
  }

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

	int Dth;
	DMOSP_MOSS *tmp_mossDbgAlg = new DMOSP_MOSS(Problem);
	tmp_mossDbgAlg->AllocateMemory();
	Dth = tmp_mossDbgAlg->SuggestDeltaThreshold();
	tmp_mossDbgAlg->FreeMemory();
	delete tmp_mossDbgAlg;

	mossAlg = new DMOSP_MOSS(Problem, TCs[r].RefSetSize, TCs[r].n_TS_maxitr,
							 Dth, TrackingResults);
	mossAlg->Run();
	if(mossAlg->bInterrupt){
	  delete mossAlg;
	  break;
	}

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\r\n",
					TrackingResults[0][3], TrackingResults[1][3],
					TrackingResults[2][3], TrackingResults[3][3],
					TrackingResults[4][3]);
	memoTGD->Lines->Strings[r] = strbuf;

	memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\r\n",
					TrackingResults[0][0], TrackingResults[1][0],
					TrackingResults[2][0], TrackingResults[3][0],
					TrackingResults[4][0]);
	memoDHVpr->Lines->Strings[r] = strbuf;

	pbRun->Position = r+1;

	for(int m=0; m<4; m++)
		for(int i=0; i<5; i++)
			fprintf(ResultsFile, "%0.4f\t", TrackingResults[i][m]);
	fprintf(ResultsFile, "\n");
	fflush(ResultsFile);

	delete mossAlg;
  }

  for(int i=0; i<4; i++)	delete [] TrackingResults[i];
  delete [] TrackingResults;

  mossAlg = NULL;

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  fclose(ResultsFile);
}
//---------------------------------------------------------------------------

void __fastcall TfrmTuningMOSS2::btnTerminateClick(TObject *)
{
  if(mossAlg) mossAlg->bInterrupt = true;
  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------

