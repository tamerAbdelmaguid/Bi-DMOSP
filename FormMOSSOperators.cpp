//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMOSSOperators.h"
#include "FormSol.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TfrmMOSSOperators *frmMOSSOperators;
//---------------------------------------------------------------------------
__fastcall TfrmMOSSOperators::TfrmMOSSOperators(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  moss = NULL;
  SC = NULL;
  TSSol = NULL;
  bActive = false;
  deltaMWFT = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSSOperators::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);
  pcOperators->Enabled = true;

  cbxPopSize->Enabled = true;
  cbxEnforceDominance->Enabled = true;
  btnRun->Enabled = true;
  pcOperators->TabIndex = 0;
  lstPopulation->Enabled = false;

  lstPopulation->Clear();
  cbxDistSol1->Clear();
  cbxDistSol2->Clear();
  cbxParent1->Clear();
  cbxParent2->Clear();
  cbxTSStartingSol->Clear();
  cbxMWFTNSStartingSol->Clear();
  edtTSIterations->Text = "50";
  edtTabuSize->Text = "20";
  edtnBestMoves->Text = "10";
  edtnRandMoves->Text = "10";
  lblTSItr->Caption = "";
  lblnCimp->Caption = "";
  lblnFimp->Caption = "";
  lblNCmax->Caption = "";
  lblNMWFT->Caption = "";
  lblCmax->Caption = "";
  lblMWFT->Caption = "";
  lblBestCmax->Caption = "";
  lblBestMWFT->Caption = "";
  pbTSIterations->Position = 0;

  grdVPBChromosome->ColCount = Problem->nOperations();
  grdDistSols->ColCount = Problem->nOperations();
  grdParents->ColCount = Problem->nOperations();
  grdChild->ColCount = Problem->nOperations();
  grdTSOriginalSol->ColCount = Problem->nOperations();
  grdTSResultantSol->ColCount = Problem->nOperations();
  for(int i=0; i<Problem->nOperations(); i++){
	grdVPBChromosome->Cells[i][0] = "";
	grdVPBChromosome->Cells[i][1] = "";
	grdVPBChromosome->Cells[i][2] = "";
	grdDistSols->Cells[i][0] = "";
	grdDistSols->Cells[i][1] = "";
	grdDistSols->Cells[i][2] = "";
	grdDistSols->Cells[i][3] = "";
	grdDistSols->Cells[i][4] = "";
	grdDistSols->Cells[i][5] = "";
	grdParents->Cells[i][0] = "";
	grdParents->Cells[i][1] = "";
	grdParents->Cells[i][2] = "";
	grdParents->Cells[i][3] = "";
	grdParents->Cells[i][4] = "";
	grdParents->Cells[i][5] = "";
	grdChild->Cells[i][0] = "";
	grdChild->Cells[i][1] = "";
	grdChild->Cells[i][2] = "";
	grdTSOriginalSol->Cells[i][0] = "";
	grdTSOriginalSol->Cells[i][1] = "";
	grdTSOriginalSol->Cells[i][2] = "";
	grdTSResultantSol->Cells[i][0] = "";
	grdTSResultantSol->Cells[i][1] = "";
	grdTSResultantSol->Cells[i][2] = "";
  }
  for(int i=1; i<grdxOverPos->RowCount; i++){
	for(int j=0; j<grdxOverPos->ColCount; j++){
	  grdxOverPos->Cells[j][i] = "";
	}
  }
  grdxOverPos->Cells[0][0] = "Type";
  grdxOverPos->Cells[1][0] = "Part";
  grdxOverPos->Cells[2][0] = "Position";

  pcOperators->TabIndex = 0;

  sgInitialPop->RowCount = 2;
  sgInitialPop->Cells[0][0] = "Ind. #";
  sgInitialPop->Cells[1][0] = "Cmax";
  sgInitialPop->Cells[2][0] = "MWFT";
  sgInitialPop->Cells[3][0] = "Rank";
  for(int i=0; i<=3; i++) sgInitialPop->Cells[i][1] = "";

  sgStats->RowCount = 5;
  sgStats->ColCount = 3;
  sgStats->Cells[0][1] = "Min.";
  sgStats->Cells[0][2] = "Max.";
  sgStats->Cells[0][3] = "Avg.";
  sgStats->Cells[0][4] = "Var.";
  sgStats->Cells[1][0] = "Cmax";
  sgStats->Cells[2][0] = "MWFT";
  for(int c=1; c<=2; c++)
	for(int r=1; r<=4; r++)
	  sgStats->Cells[c][r] = "";

  srsFront0->Clear();
  srsFront1->Clear();
  srsFront2->Clear();
  srsFront3->Clear();
  srsFront4->Clear();
  srsFront5->Clear();
  srsRest->Clear();
  ParetoChart->Refresh();

  cbxDistSol1->Enabled = true;
  cbxDistSol2->Enabled = true;
  cbxDistSol1->Clear();
  cbxDistSol2->Clear();
  lblDeltaTau->Caption = "";
  lblDeltaWC->Caption = "";

  cbxParent1->Enabled = true;
  cbxParent2->Enabled = true;
  cbxParent1->Clear();
  cbxParent2->Clear();
  edtThetaX->Text = "";
  cbxTSStartingSol->Enabled = true;
  cbxTSStartingSol->Clear();
  pbTSIterations->Position = 0;
  cbxMWFTNSStartingSol->Enabled = true;
  cbxMWFTNSStartingSol->Clear();

  btnApplyXOver->Enabled = true;
  btnAppendxOverChild->Enabled = false;

  pcOperators->Enabled = false;

  edtTSIterations->Enabled = false;
  edtTabuSize->Enabled = false;
  edtnBestMoves->Enabled = false;
  edtnRandMoves->Enabled = false;
  btnAppendTSResultantSol->Enabled = false;
  btnRestartTS->Enabled = false;

  bActive = true;
  cbxEnforceDominance->Checked = false;

  lblnGenerated->Caption = "";
  lblnExcluded->Caption = "";

  grdMWFTNSResults->ColCount = 6;
  grdMWFTNSResults->Cells[0][0] = "Op";
  grdMWFTNSResults->Cells[1][0] = "Fn";
  grdMWFTNSResults->Cells[2][0] = "Job/MC";
  grdMWFTNSResults->Cells[3][0] = "Position";
  grdMWFTNSResults->Cells[4][0] = "Exact DMWFT";
  grdMWFTNSResults->Cells[5][0] = "Approx. DMWFT";
  for(int c=0; c<grdMWFTNSResults->ColCount; c++)
	for(int r=1; r<grdMWFTNSResults->RowCount; r++)
	  grdMWFTNSResults->Cells[1][r] = "";

  // suggest delat threshold value
  if(moss){
	moss->FreeMemory();
	delete moss;
  }
  moss = new DMOSP_MOSS(Problem, 20);
  moss->AllocateMemory();
  int dth = moss->SuggestDeltaThreshold();
  edtDeltaThreshold->Text = IntToStr(dth);
  moss->FreeMemory();
  delete moss;
  moss = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(moss){
	moss->FreeMemory();
	delete moss;
  }
  moss = NULL;
  if(SC) delete SC;    SC = NULL;
  if(TSSol) delete TSSol;    TSSol = NULL;
  bActive = false;
  for(SList::SNode *nd = xPos.head(); nd; nd = nd->Next()){
	DMOSP_xOverElement *xoverel = (DMOSP_xOverElement *)nd->Data();
	delete xoverel;
  }
  xPos.Clear();
  if(deltaMWFT) delete [] deltaMWFT;   deltaMWFT = NULL;
  TfrmAlgorithm::FormClose(Sender, Action);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSSOperators::btnRunClick(TObject *)
{
  // Construct initial population and update the population list
  String s = cbxPopSize->Text;
  String sdth = edtDeltaThreshold->Text;
  int ps = StrToInt(s);
  int dth = StrToInt(sdth);
  if(moss){
	moss->FreeMemory();
	delete moss;
  }
  moss = new DMOSP_MOSS(Problem, ps, dth);
  moss->AllocateMemory();
  moss->Initialize();
  moss->ConvertDtoND();
  moss->RankDSolutions();

  // Enable operators controls
  cbxPopSize->Enabled = false;
  cbxEnforceDominance->Enabled = false;
  btnRun->Enabled = false;
  pcOperators->Enabled = true;
  pcOperators->TabIndex = 0;
  lstPopulation->Enabled = true;

  // Fill in the list of solutions
  ps = moss->getDSize();
  lstPopulation->Clear();
  cbxDistSol1->Clear();
  cbxDistSol2->Clear();
  lblDeltaTau->Caption = "";
  lblDeltaWC->Caption = "";
  cbxParent1->Clear();
  cbxParent2->Clear();
  cbxTSStartingSol->Clear();
  cbxMWFTNSStartingSol->Clear();
  for(int i=0; i<ps; i++){
	s = IntToStr(i);
	lstPopulation->Items->Add(s);
	cbxDistSol1->Items->Add(s);
	cbxDistSol2->Items->Add(s);
	cbxParent1->Items->Add(s);
	cbxParent2->Items->Add(s);
	cbxTSStartingSol->Items->Add(s);
	cbxMWFTNSStartingSol->Items->Add(s);
  }
  edtThetaX->Text = FloatToStr(moss->getTheta_X());

  btnRunTS->Enabled = false;
  bTSStarted = false;
  btnMWFTNSRun->Enabled = false;
  btnEnumrateAllMWFTNSMoves->Enabled = false;
  cbxMWFTNSOperation->Items->Clear();
  for(int i=0; i<moss->nOps; i++){
    cbxMWFTNSOperation->Items->Add(moss->O[i]->ID());
  }
  cbxMWFTNSOperation->Enabled = false;
  cbxMWFTNSOperation->ItemIndex = -1;

  lstPopulation->ItemIndex = 0;
  lstPopulationClick(NULL);

  // Display generated population information and statistics
  sgInitialPop->RowCount = ps + 1;
  moss->RankDSolutions();
  for(int i=1; i<=ps; i++){
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i-1);
	sgInitialPop->Cells[0][i] = IntToStr(i-1);
	sgInitialPop->Cells[1][i] = FloatToStr(sol->CmaxValue());
	sgInitialPop->Cells[2][i] = FloatToStr(sol->MWFTValue());
	sgInitialPop->Cells[3][i] = IntToStr(sol->getRank());
  }
  moss->ComputeObjectivesStatistics();
  for(int i=0; i<2; i++){
	sgStats->Cells[1][1] = FloatToStr(moss->minCmax);
	sgStats->Cells[1][2] = FloatToStr(moss->maxCmax);
	sgStats->Cells[1][3] = FloatToStr(moss->avgCmax);
	sgStats->Cells[2][1] = FloatToStr(moss->minMWFT);
	sgStats->Cells[2][2] = FloatToStr(moss->maxMWFT);
	sgStats->Cells[2][3] = FloatToStr(moss->avgMWFT);
  }
  sgStats->Cells[1][4] = FloatToStr(moss->varCmax);
  sgStats->Cells[2][4] = FloatToStr(moss->varMWFT);

  // Display fronts
  TPointSeries **fronts;
  fronts = new TPointSeries *[7];
  fronts[0] = srsFront0;
  fronts[1] = srsFront1;
  fronts[2] = srsFront2;
  fronts[3] = srsFront3;
  fronts[4] = srsFront4;
  fronts[5] = srsFront5;
  fronts[6] = srsRest;
  for(int f=0; f<7; f++) fronts[f]->Clear();

  int psf;
  int *nIndsFront = moss->getNoOfIndsFront();
  int **ParetoFront = moss->getParetoFront();
  for(int f=0; f<moss->getNoOfFronts(); f++){
	psf = (f > 6)? 6 : f;
	for(int i=0; i<nIndsFront[f]; i++){
	  DMOSP_MOSS_Solution *guy = moss->GetDSolution(ParetoFront[f][i]);
	  fronts[psf]->AddXY(guy->MWFTValue(), guy->CmaxValue());
	}
  }
  ParetoChart->Refresh();
  delete [] fronts;

  lblnGenerated->Caption = IntToStr(moss->nGenerated);
  lblnExcluded->Caption = IntToStr(moss->nExcluded);

  // Allocate memory for deltaMWFT
  if(deltaMWFT) delete [] deltaMWFT;
  deltaMWFT = new double[((moss->nWCs>moss->nJobs)? moss->nWCs : moss->nJobs)+1];
}
//---------------------------------------------------------------------------
void __fastcall TfrmMOSSOperators::lstPopulationClick(TObject *)
{
  int i = lstPopulation->ItemIndex;
  if(i == -1) return;

  if(moss){
	moss->ChooseNDSolution(lstPopulation->ItemIndex);
	frmSolution->SetCurrentSolution(moss->Solution);

	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(lstPopulation->ItemIndex);

	DMOSP_VPBString *jbString = sol->getJobsString();
	DMOSP_VPBString *wsString = sol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdVPBChromosome->Cells[i][0] = jbString->Gene[i]->ID();
	  grdVPBChromosome->Cells[i][1] = wsString->Gene[i]->ID();
	  grdVPBChromosome->Cells[i][2] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdVPBChromosome->Cells[c+1][2];
		tmp = tmp + "^";
		grdVPBChromosome->Cells[c][2] = tmp;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxDistSol1Change(TObject *)
{
  int i = cbxDistSol1->ItemIndex;
  if(i == -1) return;

  lblDeltaTau->Caption = "";
  lblDeltaWC->Caption = "";

  if(moss){
	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i);

	DMOSP_VPBString *jbString = sol->getJobsString();
	DMOSP_VPBString *wsString = sol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdDistSols->Cells[i][0] = jbString->Gene[i]->ID();
	  grdDistSols->Cells[i][1] = wsString->Gene[i]->ID();
	  grdDistSols->Cells[i][2] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdDistSols->Cells[c+1][2];
		tmp = tmp + "^";
		grdDistSols->Cells[c][2] = tmp;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxDistSol2Change(TObject *)
{
  int i = cbxDistSol2->ItemIndex;
  if(i == -1) return;

  lblDeltaTau->Caption = "";
  lblDeltaWC->Caption = "";

  if(moss){
	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i);

	DMOSP_VPBString *jbString = sol->getJobsString();
	DMOSP_VPBString *wsString = sol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdDistSols->Cells[i][3] = jbString->Gene[i]->ID();
	  grdDistSols->Cells[i][4] = wsString->Gene[i]->ID();
	  grdDistSols->Cells[i][5] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdDistSols->Cells[c+1][5];
		tmp = tmp + "^";
		grdDistSols->Cells[c][5] = tmp;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnDistCalcClick(TObject *)
{
  int i1 = cbxDistSol1->ItemIndex;
  int i2 = cbxDistSol2->ItemIndex;
  if(i1 == -1 || i2 == -1){
	  Application->MessageBox(L"You must select two solutions from the drop down lists",
						  L"Select two solutions first", MB_OK);
	  return;
  }
  DMOSP_MOSS_Solution *S1 = moss->GetDSolution(i1);
  DMOSP_MOSS_Solution *S2 = moss->GetDSolution(i2);
  int jD, wcD;
  moss->Distance(S1, S2, jD, wcD);
  lblDeltaTau->Caption = IntToStr(jD);
  lblDeltaWC->Caption = IntToStr(wcD);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxParent1Change(TObject *)
{
  int i = cbxParent1->ItemIndex;
  if(i == -1) return;

  for(int i=0; i<Problem->nOperations(); i++){
	grdChild->Cells[i][0] = "";
	grdChild->Cells[i][1] = "";
	grdChild->Cells[i][2] = "";
  }
  btnAppendxOverChild->Enabled = false;

  for(int i=1; i<grdxOverPos->RowCount; i++){
	for(int j=0; j<grdxOverPos->ColCount; j++){
	  grdxOverPos->Cells[j][i] = "";
	}
  }

  if(moss){
	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i);

	DMOSP_VPBString *jbString = sol->getJobsString();
	DMOSP_VPBString *wsString = sol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdParents->Cells[i][0] = jbString->Gene[i]->ID();
	  grdParents->Cells[i][1] = wsString->Gene[i]->ID();
	  grdParents->Cells[i][2] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdParents->Cells[c+1][2];
		tmp = tmp + "^";
		grdParents->Cells[c][2] = tmp;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxParent2Change(TObject *)
{
  int i = cbxParent2->ItemIndex;
  if(i == -1) return;

  for(int i=0; i<Problem->nOperations(); i++){
	grdChild->Cells[i][0] = "";
	grdChild->Cells[i][1] = "";
	grdChild->Cells[i][2] = "";
  }
  btnAppendxOverChild->Enabled = false;

  for(int i=1; i<grdxOverPos->RowCount; i++){
	for(int j=0; j<grdxOverPos->ColCount; j++){
	  grdxOverPos->Cells[j][i] = "";
	}
  }

  if(moss){
	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i);

	DMOSP_VPBString *jbString = sol->getJobsString();
	DMOSP_VPBString *wsString = sol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdParents->Cells[i][3] = jbString->Gene[i]->ID();
	  grdParents->Cells[i][4] = wsString->Gene[i]->ID();
	  grdParents->Cells[i][5] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdParents->Cells[c+1][5];
		tmp = tmp + "^";
		grdParents->Cells[c][5] = tmp;
	  }
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnApplyXOverClick(TObject *)
{
  int i1 = cbxParent1->ItemIndex;
  int i2 = cbxParent2->ItemIndex;
  if(i1 == -1 || i2 == -1){
	  Application->MessageBox(L"You must select two solutions from the drop down lists",
						  L"Select two solutions first", MB_OK);
	  return;
  }

  String s = edtThetaX->Text;
  double thetaX;
  try{
	thetaX = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading theta_X", MB_OK);
	  return;
	}
  if(thetaX > 1.0 || thetaX <= 0.0){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Incorrect value of theta_X as it should be greater"
						  "than 0.0 and less than 1.0", MB_OK);
	  return;
  }
  moss->setTheta_X(thetaX);

  DMOSP_MOSS_Solution *SL = moss->GetDSolution(i1);
  DMOSP_MOSS_Solution *SF = moss->GetDSolution(i2);

  // Clear the list of previously generated crossover positions
  for(SList::SNode *nd = xPos.head(); nd; nd = nd->Next()){
	DMOSP_xOverElement *xoverel = (DMOSP_xOverElement *)nd->Data();
	delete xoverel;
  }
  xPos.Clear();

  if(SC) delete SC;
  SC = moss->CombinedFrom(SL, SF);
  if(SC){
	DMOSP_VPBString *jbString = SC->getJobsString();
	DMOSP_VPBString *wsString = SC->getWorkstationsString();
	for(int i=0; i<jbString->length; i++){
	  grdChild->Cells[i][0] = jbString->Gene[i]->ID();
	  grdChild->Cells[i][1] = wsString->Gene[i]->ID();
	  grdChild->Cells[i][2] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdChild->Cells[c+1][2];
		tmp = tmp + "^";
		grdChild->Cells[c][2] = tmp;
	  }
	}
  }

  grdxOverPos->RowCount = xPos.Count() + 1;
  grdxOverPos->FixedRows = 1;
  int r = 0;
  for(SList::SNode *nd = xPos.head(); nd; nd = nd->Next()){
	DMOSP_xOverElement *xel = (DMOSP_xOverElement *) nd->Data();
	r++;
	grdxOverPos->Cells[0][r] = (xel->type == 1)? "Job" : "WC";
	grdxOverPos->Cells[1][r] = xel->ID;
	grdxOverPos->Cells[2][r] = IntToStr(xel->pos);
  }

  btnAppendxOverChild->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnAppendxOverChildClick(TObject *)
{
  if(!SC) return;

  int c = lstPopulation->Items->Count;
  lstPopulation->Items->Add(IntToStr(c));
  cbxDistSol1->Items->Add(IntToStr(c));
  cbxDistSol2->Items->Add(IntToStr(c));
  cbxParent1->Items->Add(IntToStr(c));
  cbxParent2->Items->Add(IntToStr(c));
  cbxTSStartingSol->Items->Add(IntToStr(c));
  cbxMWFTNSStartingSol->Items->Add(IntToStr(c));
  btnAppendxOverChild->Enabled = false;

  moss->AddDSol(SC);
  lstPopulation->ItemIndex = c;
  lstPopulationClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxTSStartingSolChange(TObject *)
{
  int i = cbxTSStartingSol->ItemIndex;
  if(i == -1) return;

  for(int i=0; i<Problem->nOperations(); i++){
	grdTSResultantSol->Cells[i][0] = "";
	grdTSResultantSol->Cells[i][1] = "";
	grdTSResultantSol->Cells[i][2] = "";
  }
  btnAppendTSResultantSol->Enabled = false;
  btnRestartTS->Enabled = false;
  lblnCimp->Caption = "";
  lblnFimp->Caption = "";
  lblNCmax->Caption = "";
  lblNMWFT->Caption = "";
  lblCmax->Caption = "";
  lblMWFT->Caption = "";
  lblBestCmax->Caption = "";
  lblBestMWFT->Caption = "";

  edtTSIterations->Enabled = true;
  edtTabuSize->Enabled = true;
  edtnBestMoves->Enabled = true;
  edtnRandMoves->Enabled = true;
  btnRunTS->Enabled = true;

  if(moss){
	// Display the vector of permutations based chromosome
	DMOSP_MOSS_Solution *sol = moss->GetDSolution(i);
	if(TSSol) delete TSSol;
	TSSol = new DMOSP_MOSS_Solution(*sol);

	DMOSP_VPBString *jbString = TSSol->getJobsString();
	DMOSP_VPBString *wsString = TSSol->getWorkstationsString();
	for(i=0; i<jbString->length; i++){
	  grdTSOriginalSol->Cells[i][0] = jbString->Gene[i]->ID();
	  grdTSOriginalSol->Cells[i][1] = wsString->Gene[i]->ID();
	  grdTSOriginalSol->Cells[i][2] = "";
	}
	for(int s=0; s<wsString->nSubstrings; s++){
	  DMOSP_VPBSubString &substr = wsString->Substring[s];
	  for(int p=0; p < substr.nSeparators; p++){
		int c = substr.SeparatorPos[p];
		String tmp = grdTSOriginalSol->Cells[c+1][2];
		tmp = tmp + "^";
		grdTSOriginalSol->Cells[c][2] = tmp;
	  }
	}
  }

  cbxTSStartingSol->Enabled = false;
  bTSStarted = false;
  nCmaxNs = nMWFTNs = 0;
}
//---------------------------------------------------------------------------

bool TfrmMOSSOperators::ReadTSParameters()
{
  String s1, s2, s3, s4;
  s1 = edtTSIterations->Text;
  s2 = edtTabuSize->Text;
  s3 = edtnBestMoves->Text;
  s4 = edtnRandMoves->Text;
  try{
	nTSIterations = StrToInt(s1);
	TabuSize = StrToInt(s2);
	nBestMoves = StrToInt(s3);
	nRandMoves = StrToInt(s4);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading Tabu Search Paramters", MB_OK);
	  return false;
	}
  if(nTSIterations <= 0 || TabuSize <= 0 || nBestMoves <= 0 || nRandMoves <= 0){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Incorrect value of tabu size or number of iterations"
						  " as they should be greater than 0.0", MB_OK);
	  return false;
  }

  moss->SetTabuSize(TabuSize);
  moss->SetTSnIterations(nTSIterations);
  moss->SetTSnBestMoves(nBestMoves);
  moss->SetTSnRandMoves(nRandMoves);
  pbTSIterations->Max = TabuSize;
  pbTSIterations->Position = 0;
  cbxTSStartingSol->Enabled = false;
  edtTSIterations->Enabled = false;
  edtTabuSize->Enabled = false;
  edtnBestMoves->Enabled = false;
  edtnRandMoves->Enabled = false;
  lblTSItr->Caption = "0";
  return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnRunTSClick(TObject *)
{
  if(!bTSStarted)
	if(!ReadTSParameters()) return;
  bTSStarted = true;

  moss->TabuSearch(TSSol);

  DMOSP_VPBString *jbString = TSSol->getJobsString();
  DMOSP_VPBString *wsString = TSSol->getWorkstationsString();
  for(int i=0; i<jbString->length; i++){
	grdTSResultantSol->Cells[i][0] = jbString->Gene[i]->ID();
	grdTSResultantSol->Cells[i][1] = wsString->Gene[i]->ID();
	grdTSResultantSol->Cells[i][2] = "";
  }
  for(int s=0; s<wsString->nSubstrings; s++){
	DMOSP_VPBSubString &substr = wsString->Substring[s];
	for(int p=0; p < substr.nSeparators; p++){
	  int c = substr.SeparatorPos[p];
	  String tmp = grdTSResultantSol->Cells[c+1][2];
	  tmp = tmp + "^";
	  grdTSResultantSol->Cells[c][2] = tmp;
	}
  }

  btnRunTS->Enabled = false;
  btnRestartTS->Enabled = true;
  btnAppendTSResultantSol->Enabled = true;
  bTSStarted = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnAppendTSResultantSolClick(TObject *)
{
  if(!TSSol) return;

  int c = lstPopulation->Items->Count;
  lstPopulation->Items->Add(IntToStr(c));
  cbxDistSol1->Items->Add(IntToStr(c));
  cbxDistSol2->Items->Add(IntToStr(c));
  cbxParent1->Items->Add(IntToStr(c));
  cbxParent2->Items->Add(IntToStr(c));
  cbxTSStartingSol->Items->Add(IntToStr(c));
  cbxMWFTNSStartingSol->Items->Add(IntToStr(c));
  btnAppendTSResultantSol->Enabled = false;

  moss->AddDSol(TSSol);
  lstPopulation->ItemIndex = c;
  lstPopulationClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnRestartTSClick(TObject *)
{
  cbxTSStartingSol->Enabled = true;
  cbxTSStartingSol->ItemIndex = -1;
  edtTSIterations->Enabled = true;
  edtTabuSize->Enabled = true;
  lblTSItr->Caption = "";
  btnRunTS->Enabled = true;
  btnRestartTS->Enabled = false;
  btnAppendTSResultantSol->Enabled = false;
  bTSStarted = false;
  pbTSIterations->Position = 0;
  btnRunTS->Enabled = false;
  edtTSIterations->Enabled = false;
  edtTabuSize->Enabled = false;

  for(int i=0; i<Problem->nOperations(); i++){
	grdTSOriginalSol->Cells[i][0] = "";
	grdTSOriginalSol->Cells[i][1] = "";
	grdTSOriginalSol->Cells[i][2] = "";
	grdTSResultantSol->Cells[i][0] = "";
	grdTSResultantSol->Cells[i][1] = "";
	grdTSResultantSol->Cells[i][2] = "";
  }
  lblnCimp->Caption = "";
  lblnFimp->Caption = "";
  lblNCmax->Caption = "";
  lblNMWFT->Caption = "";
  lblCmax->Caption = "";
  lblMWFT->Caption = "";
  lblBestCmax->Caption = "";
  lblBestMWFT->Caption = "";

  if(TSSol) delete TSSol;
  TSSol = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::IncrementN1N2()
{
  lblNCmax->Caption = IntToStr(++nCmaxNs);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::IncrementN3N4()
{
  lblNMWFT->Caption = IntToStr(++nMWFTNs);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::updateNimp(int nCimp, int nFimp)
{
  lblnCimp->Caption = IntToStr(nCimp);
  lblnFimp->Caption = IntToStr(nFimp);
}

void __fastcall TfrmMOSSOperators::cbxMWFTNSStartingSolChange(TObject *)
{
  cbxMWFTNSOperation->Enabled = true;
  btnMWFTNSRun->Enabled = true;
  btnEnumrateAllMWFTNSMoves->Enabled = true;
  grdMWFTNSResults->RowCount = 2;
  for(int c=0; c<grdMWFTNSResults->ColCount; c++)
	grdMWFTNSResults->Cells[c][1] = "";
  cbxMWFTNSOperation->ItemIndex = -1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnMWFTNSRunClick(TObject *)
{
  int start;
  int s = cbxMWFTNSStartingSol->ItemIndex;
  int i = cbxMWFTNSOperation->ItemIndex;
  if(i == -1){
	Application->MessageBox(L"Select an operation from the dropdown list",
						  L"Operation not selected", MB_OK);
	return;
  }

  DMOSP_MOSS_Solution *sol = moss->GetDSolution(s);
  int alpha, omega, minX;
  sol->N3ExactResults(i, alpha, omega, deltaMWFT, minX);
  grdMWFTNSResults->RowCount = omega - alpha + 2;
  for(int r = 0; r <= omega - alpha; r++){
	grdMWFTNSResults->Cells[0][r+1] = moss->O[i]->ID();
	grdMWFTNSResults->Cells[1][r+1] = "N3";
	grdMWFTNSResults->Cells[2][r+1] = moss->O[i]->Job->ID();
	grdMWFTNSResults->Cells[3][r+1] = IntToStr(r+alpha);
	grdMWFTNSResults->Cells[4][r+1] = FloatToStr(deltaMWFT[r+alpha]);
  }
  sol->N3Results(i, alpha, omega, deltaMWFT, minX);
  grdMWFTNSResults->RowCount = omega - alpha + 2;
  for(int r = 0; r <= omega - alpha; r++){
	grdMWFTNSResults->Cells[5][r+1] = FloatToStr(deltaMWFT[r+alpha]);
  }
  SList::SNode *mcnd = moss->O[i]->AltMCs.head();
  while(mcnd){
	DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
	DMOSP_Problem::SMC *mc = opmc->MC;
	sol->N4ExactResults(i, mc->i, alpha, omega, deltaMWFT, minX);
	start = grdMWFTNSResults->RowCount;
	grdMWFTNSResults->RowCount = start + omega - alpha + 1;
	for(int r = 0; r <= omega - alpha; r++){
	  grdMWFTNSResults->Cells[0][r+start] = moss->O[i]->ID();
	  grdMWFTNSResults->Cells[1][r+start] = "N4";
	  grdMWFTNSResults->Cells[2][r+start] = mc->ID();
	  grdMWFTNSResults->Cells[3][r+start] = IntToStr(r+alpha);
	  grdMWFTNSResults->Cells[4][r+start] = FloatToStr(deltaMWFT[r+alpha]);
	}
	sol->N4Results(i, mc->i, alpha, omega, deltaMWFT, minX);
	for(int r = 0; r <= omega - alpha; r++){
	  grdMWFTNSResults->Cells[5][r+start] = FloatToStr(deltaMWFT[r+alpha]);
	}
	mcnd = mcnd->Next();
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::cbxMWFTNSOperationChange(TObject *)
{
  grdMWFTNSResults->RowCount = 2;
  for(int c=0; c<grdMWFTNSResults->ColCount; c++)
	grdMWFTNSResults->Cells[c][1] = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMOSSOperators::btnEnumrateAllMWFTNSMovesClick(TObject *)

{
  int start;
  cbxMWFTNSOperation->ItemIndex = -1;
  int s = cbxMWFTNSStartingSol->ItemIndex;
  grdMWFTNSResults->RowCount = 1;

  DMOSP_MOSS_Solution *sol = moss->GetDSolution(s);
  for(int i=0; i<moss->nOps; i++){
	  int alpha, omega, minX;
	  sol->N3ExactResults(i, alpha, omega, deltaMWFT, minX);
	  start = grdMWFTNSResults->RowCount;
	  grdMWFTNSResults->RowCount = start + omega - alpha + 1;
	  for(int r = 0; r <= omega - alpha; r++){
		grdMWFTNSResults->Cells[0][r+start] = moss->O[i]->ID();
		grdMWFTNSResults->Cells[1][r+start] = "N3";
		grdMWFTNSResults->Cells[2][r+start] = moss->O[i]->Job->ID();
		grdMWFTNSResults->Cells[3][r+start] = IntToStr(r+alpha);
		grdMWFTNSResults->Cells[4][r+start] = FloatToStr(deltaMWFT[r+alpha]);
	  }
	  sol->N3Results(i, alpha, omega, deltaMWFT, minX);
	  for(int r = 0; r <= omega - alpha; r++){
		grdMWFTNSResults->Cells[5][r+start] = FloatToStr(deltaMWFT[r+alpha]);
	  }

	  SList::SNode *mcnd = moss->O[i]->AltMCs.head();
	  while(mcnd){
		DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		DMOSP_Problem::SMC *mc = opmc->MC;
		sol->N4ExactResults(i, mc->i, alpha, omega, deltaMWFT, minX);
		start = grdMWFTNSResults->RowCount;
		grdMWFTNSResults->RowCount = start + omega - alpha + 1;
		for(int r = 0; r <= omega - alpha; r++){
		  grdMWFTNSResults->Cells[0][r+start] = moss->O[i]->ID();
		  grdMWFTNSResults->Cells[1][r+start] = "N4";
		  grdMWFTNSResults->Cells[2][r+start] = mc->ID();
		  grdMWFTNSResults->Cells[3][r+start] = IntToStr(r+alpha);
		  grdMWFTNSResults->Cells[4][r+start] = FloatToStr(deltaMWFT[r+alpha]);
		}
		sol->N4Results(i, mc->i, alpha, omega, deltaMWFT, minX);
		for(int r = 0; r <= omega - alpha; r++){
		  grdMWFTNSResults->Cells[5][r+start] = FloatToStr(deltaMWFT[r+alpha]);
		}
		mcnd = mcnd->Next();
	  }
  }
  grdMWFTNSResults->FixedRows = 1;
}
//---------------------------------------------------------------------------

