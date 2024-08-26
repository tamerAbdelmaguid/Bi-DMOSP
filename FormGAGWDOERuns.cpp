//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormGAGWDOERuns.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmGAGWDOERuns *frmGAGWDOERuns;
//---------------------------------------------------------------------------
__fastcall TfrmGAGWDOERuns::TfrmGAGWDOERuns(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmGAGWDOERuns::FormShow(TObject *)
{
  memoTCs->Clear();
  lblTCReadResult->Caption = "";

  memoTCs->Enabled = true;
  btnReadRuns->Enabled = true;

  btnRun->Enabled = false;
  btnTerminate->Enabled = false;
  btnClose->Enabled = true;

  pbRun->Position = 0;

  TCs = NULL;
  nsga2Alg = NULL;
  mogwoAlg = NULL;
  Instance = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmGAGWDOERuns::FormClose(TObject *, TCloseAction &)

{
  if(TCs) delete [] TCs;             TCs = NULL;
  if(Instance) delete Instance;      Instance = NULL;
  if(nsga2Alg) delete nsga2Alg;      nsga2Alg = NULL;
  if(mogwoAlg) delete mogwoAlg;      mogwoAlg = NULL;

  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;

  this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmGAGWDOERuns::btnReadRunsClick(TObject *)
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
	TCs[ln].W = atoi(buf);
	if(TCs[ln].W == 0){
	   msg = "Error reading number of workstations";
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
	TCs[ln].J = atoi(buf);
	if(TCs[ln].J == 0){
	   msg = "Error reading number of jobs to number of workstations ratio";
	   bError = true;
	   break;
	}
	TCs[ln].J *= TCs[ln].W;

	while(str[strpos] == '\t' || str[strpos] == ' ') strpos++;
	bufpos = 0;
	while(str[strpos] != '\t'){
	  buf[bufpos] = str[strpos];
	  bufpos++;
	  strpos++;
	}
	buf[bufpos] = 0;
	TCs[ln].LL = atof(buf);
	if(TCs[ln].LL != 0.5 && TCs[ln].LL != 0.9){
	   msg = "Error reading loading level value (it should be either 0.5 or 0.9)";
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
	TCs[ln].PLJ = atoi(buf);
	if(TCs[ln].PLJ == 0){
	   msg = "Error reading PLJ value";
	   bError = true;
	   break;
	}
	if(TCs[ln].PLJ == 30) TCs[ln].PLJ = 1;
	else if(TCs[ln].PLJ == 50) TCs[ln].PLJ = 2;
	else{
	   msg = "Unrecognized PLJ value";
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
	TCs[ln].PBM = atoi(buf);
	if(TCs[ln].PBM == 30) TCs[ln].PBM = 1;
	else if(TCs[ln].PBM == 50) TCs[ln].PBM = 2;
	else{
	   msg = "Unrecognized PLJ value";
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
	TCs[ln].ReplicateNo = atoi(buf);
	if(TCs[ln].ReplicateNo == 0){
	   msg = "Error reading replicate number";
	   bError = true;
	   break;
	}
  }

  if(!bError){
	msg = IntToStr(nTCs) + " treatment combinations are successfully read.";

	pbRun->Max = nTCs * 30 * 2;
	pbRun->Position = 0;

	memoTCs->Enabled = false;
	btnReadRuns->Enabled = false;
	btnRun->Enabled = true;
	btnTerminate->Enabled = false;
  }
  else{
	delete [] TCs;
	TCs = NULL;
  }

  lblTCReadResult->Caption = msg;
}
//---------------------------------------------------------------------------
void __fastcall TfrmGAGWDOERuns::btnRunClick(TObject *)
{
  char strbuf[128];

  FILE *ResultsFile = fopen(".\\LargeSet\\AvgResults.txt", "a+");
  fprintf(ResultsFile, "Instance name\tNSGA-II\tMOGWO\n");
  fflush(ResultsFile);

  btnRun->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;

  for(int r=0; r<nTCs; r++){
	String prbname;
	prbname = "DMOSP-L-" + IntToStr(TCs[r].W) + "-" + IntToStr(TCs[r].J) + "-" +
			  FloatToStr(TCs[r].LL) + "-" + ((TCs[r].PLJ == 1)? "30" : "50")
			  + "-" + ((TCs[r].PBM == 1)? "30" : "50")
			  + "-" + IntToStr(TCs[r].ReplicateNo);
	fwprintf(ResultsFile, prbname.c_str());

	// Create instance
	Instance = new DMOSP_Problem(prbname.c_str(), L"DMOSP problem",
								 TCs[r].W, TCs[r].J, 0, TCs[r].LL,
								 1, TCs[r].PLJ, 1, TCs[r].PBM, 1);
	lblInstName->Caption = prbname;

	// Save instance to file
	String FileName = ".\\LargeSet\\" + prbname + ".dmosp";
	DMOSP_FIO_RESULT Result;
	SaveDMOSP(Instance, FileName.c_str(), Result);
	if(Result != SUCCESS){
	  String s;
	  switch(Result){
		case FAIL_TO_OPEN:
		  s = "failed to save file";
		  break;
		case DATA_ERROR:
		  s = "Error in data";
		  break;
	  }
	  Application->MessageBox(s.c_str(), L"Error saving file", MB_OK);
	  delete Instance;
	  break;
	}

	// Create instance detailed results file
	sprintf(strbuf, ".\\LargeSet\\DMOSP-L-%i-%i-%0.1f-%s-%s-%i-results.txt",
					TCs[r].W, TCs[r].J, TCs[r].LL, ((TCs[r].PLJ == 1)? "30" : "50"),
					((TCs[r].PBM == 1)? "30" : "50"), TCs[r].ReplicateNo);
	FILE *InstanceResultsFile = fopen(strbuf, "a+");
	fprintf(InstanceResultsFile, "run #\tNSGA-II\tMOGWO\n");
	fflush(InstanceResultsFile);

	// Run both metaheuristics interchangeably 30 times each
	double CompTime = 3.0 * TCs[r].J;  // computational time in seconds
	double avgNSGAHV=0.0, avgMOGWOHV=0.0;
	bool bInterrupted = false;
	for(int run=0; run<30; run++){
	  lblRunNo->Caption = IntToStr(run+1);

	  double NSGAHV, MOGWOHV;

	  // run NSGA-II
	  lblAlg->Caption = "NSGA-II";
	  if(nsga2Alg) delete nsga2Alg;
	  nsga2Alg = new DMOSP_NSGAII(Instance, 200, 1000, CompTime * 1000.0,
								  0.8, DMOSP_NSGAII::SINGLE_POINT_OX,
								  0.5, DMOSP_NSGAII::SWAP_MUTATION,
								  0.1, 1000, 100, 0.95, 1.0);
	  nsga2Alg->Run();
	  if(nsga2Alg->bInterrupt){
		bInterrupted = true;
		break;
	  }
	  pbRun->Position = pbRun->Position + 1;
	  NSGAHV = nsga2Alg->HVpr;

	  // run MOGWO
	  lblAlg->Caption = "MOGWO";
	  if(mogwoAlg) delete mogwoAlg;

	  mogwoAlg = new DMOSP_MOGWO(Instance, 200, 1000, CompTime * 1000.0, 100,
								 0.1, 10, 4,
								 2, DMOSP_MOGWO::SWAP_MUTATION, 1000, 100, 0.95,
								 1.0);

	  mogwoAlg->Run();
	  if(mogwoAlg->bInterrupt){
		bInterrupted = true;
		break;
	  }
	  pbRun->Position = pbRun->Position + 1;
	  MOGWOHV = mogwoAlg->HVpr;

	  // save run results
	  fprintf(InstanceResultsFile, "%i\t%0.4f\t%0.4f\n", run+1, NSGAHV, MOGWOHV);
	  fflush(InstanceResultsFile);

	  // update total values
	  avgNSGAHV += NSGAHV;
	  avgMOGWOHV += MOGWOHV;
	}
	if(bInterrupted){
	  delete Instance;
	  break;
	}

	avgNSGAHV /= 30.0;
	avgMOGWOHV /= 30.0;

	fwprintf(ResultsFile, L"\t%0.4f\t%0.4f\n", avgNSGAHV, avgMOGWOHV);
	fflush(ResultsFile);

	fflush(InstanceResultsFile);
	fclose(InstanceResultsFile);

	// delete instance
	delete Instance;
  }
  Instance = NULL;
  if(nsga2Alg) delete nsga2Alg;   nsga2Alg = NULL;
  if(mogwoAlg) delete mogwoAlg; mogwoAlg = NULL;

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  fflush(ResultsFile);
  fclose(ResultsFile);
}
//---------------------------------------------------------------------------
void __fastcall TfrmGAGWDOERuns::btnTerminateClick(TObject *)
{
  if(nsga2Alg) nsga2Alg->bInterrupt = true;
  if(mogwoAlg) mogwoAlg->bInterrupt = true;

  btnRun->Enabled = true;
  btnClose->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
