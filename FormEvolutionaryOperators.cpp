//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormEvolutionaryOperators.h"
#include "FormSol.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TfrmEvolOperators *frmEvolOperators;
//---------------------------------------------------------------------------
__fastcall TfrmEvolOperators::TfrmEvolOperators(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  nsga2 = NULL;
  bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEvolOperators::btnRunClick(TObject *)
{
  // Construct initial population and update the population list
  String s = cbxPopSize->Text;
  int ps = StrToInt(s);
  if(nsga2){
    nsga2->FreeMemory();
	delete nsga2;
  }
  nsga2 = new DMOSP_NSGAII(Problem, ps);
  nsga2->AllocateMemory();
  nsga2->Initialize();

  // Enable operators controls
  cbxPopSize->Enabled = false;
  btnRun->Enabled = false;
  pcEvolOperators->Enabled = true;
  pcEvolOperators->TabIndex = 0;
  lstPopulation->Enabled = true;

  // Fill in the list of solutions
  lstPopulation->Clear();
  cbxParent1->Clear();
  cbxParent2->Clear();
  cbxOriginalChromosome->Clear();
  cbxSAOriginalChromosome->Clear();
  for(int i=0; i<ps; i++){
	s = IntToStr(i);
	lstPopulation->Items->Add(s);
	cbxParent1->Items->Add(s);
	cbxParent2->Items->Add(s);
	cbxOriginalChromosome->Items->Add(s);
	cbxSAOriginalChromosome->Items->Add(s);
  }
  lstPopulation->ItemIndex = 0;
  lstPopulationClick(NULL);

  // Display generated population information and statistics
  sgInitialPop->RowCount = ps + 1;
  for(int i=1; i<=ps; i++){
	DMOSP_Individual *guy = nsga2->getGuy(i-1);
	sgInitialPop->Cells[0][i] = IntToStr(i-1);
	sgInitialPop->Cells[1][i] = FloatToStr(guy->getObjective(0));
	sgInitialPop->Cells[2][i] = FloatToStr(guy->getObjective(1));
	sgInitialPop->Cells[3][i] = FloatToStr(guy->getFitness(0));
	sgInitialPop->Cells[4][i] = FloatToStr(guy->getFitness(1));
	sgInitialPop->Cells[5][i] = IntToStr(guy->getRank());
	sgInitialPop->Cells[6][i] = FloatToStr(guy->getCrowdingDistance());
  }
  for(int i=0; i<2; i++){
	int c = 1+i;
	sgStats->Cells[c][1] = FloatToStr(nsga2->getMinObj(i));
	sgStats->Cells[c][2] = FloatToStr(nsga2->getMaxObj(i));
	sgStats->Cells[c][3] = FloatToStr(nsga2->getAvgObj(i));
	c = 3+i;
	sgStats->Cells[c][1] = FloatToStr(nsga2->getMinFit(i));
	sgStats->Cells[c][2] = FloatToStr(nsga2->getMaxFit(i));
	sgStats->Cells[c][3] = FloatToStr(nsga2->getAvgFit(i));
  }
  sgStats->Cells[3][4] = FloatToStr(nsga2->getFitVar(0));
  sgStats->Cells[4][4] = FloatToStr(nsga2->getFitVar(1));

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
  int *nIndsFront = nsga2->getNoOfIndsFront();
  int **ParetoFront = nsga2->getParetoFront();
  for(int f=0; f<nsga2->getNoOfFronts(); f++){
	psf = (f > 6)? 6 : f;
	for(int i=0; i<nIndsFront[f]; i++){
	  DMOSP_Individual *guy = nsga2->getGuy(ParetoFront[f][i]);
	  fronts[psf]->AddXY(guy->MWFT, guy->Cmax);
	}
  }
  ParetoChart->Refresh();
  delete [] fronts;

  sgMatingPool->ColCount = 1;
  sgMatingPool->Cells[0][0] = "";

  SAIter = 0;
  pbSAIterations->Position = 0;
  lblCmax->Caption = "";
  lblMWFT->Caption = "";
}
//---------------------------------------------------------------------------
void __fastcall TfrmEvolOperators::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);
  pcEvolOperators->Enabled = true;

  cbxPopSize->Enabled = true;
  btnRun->Enabled = true;
  pcEvolOperators->TabIndex = 0;
  lstPopulation->Enabled = false;

  lstPopulation->Clear();
  cbxParent1->Clear();
  cbxParent2->Clear();
  cbxOriginalChromosome->Clear();
  cbxSAOriginalChromosome->Clear();
  edtnoSAIterations->Text = "";
  edtInitTemp->Text = "100";
  edtnoSAIterations->Text = "1000";
  edtCoolingFactor->Text = "0.95";
  lblTemperature->Caption = "";
  pbSAIterations->Position = 0;

  grdOBChromosome->ColCount = Problem->nOperations();
  grdParents->ColCount = Problem->nOperations();
  grdChildren->ColCount = Problem->nOperations();
  grdOriginalChromosome->ColCount = Problem->nOperations();
  grdMutatedChromosome->ColCount = Problem->nOperations();
  sgSAOriginalChromosome->ColCount = Problem->nOperations();
  sgSAFinalChromosome->ColCount = Problem->nOperations();
  for(int i=0; i<Problem->nOperations(); i++){
	grdOBChromosome->Cells[i][0] = "";
	grdOBChromosome->Cells[i][1] = "";
	grdParents->Cells[i][0] = "";
	grdParents->Cells[i][1] = "";
	grdChildren->Cells[i][0] = "";
	grdChildren->Cells[i][1] = "";
	grdOriginalChromosome->Cells[i][0] = "";
	grdMutatedChromosome->Cells[i][0] = "";
	sgSAOriginalChromosome->Cells[i][0] = "";
	sgSAFinalChromosome->Cells[i][0] = "";
  }

  pcEvolOperators->TabIndex = 0;

  sgInitialPop->RowCount = 2;
  sgInitialPop->Cells[0][0] = "Ind. #";
  sgInitialPop->Cells[1][0] = "Cmax";
  sgInitialPop->Cells[2][0] = "MWFT";
  sgInitialPop->Cells[3][0] = "Fit[1]";
  sgInitialPop->Cells[4][0] = "Fit[2]";
  sgInitialPop->Cells[5][0] = "Rank";
  sgInitialPop->Cells[6][0] = "Crowd. dist.";
  for(int i=0; i<=6; i++) sgInitialPop->Cells[i][1] = "";

  sgStats->RowCount = 5;
  sgStats->ColCount = 5;
  sgStats->Cells[0][1] = "Min.";
  sgStats->Cells[0][2] = "Max.";
  sgStats->Cells[0][3] = "Avg.";
  sgStats->Cells[0][4] = "Var.";
  sgStats->Cells[1][0] = "Cmax";
  sgStats->Cells[2][0] = "MWFT";
  sgStats->Cells[3][0] = "Fit[1]";
  sgStats->Cells[4][0] = "Fit[2]";
  for(int c=1; c<=4; c++)
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

  sgMatingPool->ColCount = 1;
  sgMatingPool->Cells[0][0] = "";

  lblxOverPoints->Caption = "";
  cbxParent1->Enabled = true;
  cbxParent2->Enabled = true;
  lblMutationPoints->Caption = "";
  cbxOriginalChromosome->Enabled = true;
  cbxSAOriginalChromosome->Enabled = true;
  iParent1 = iParent2 = iOriginalChromosome = iSAOriginalChromosome = -1;

  btnApplyXOver->Enabled = true;
  btnApplyMutation->Enabled = true;

  pcEvolOperators->Enabled = false;

  edtnoSAIterations->Enabled = false;
  edtInitTemp->Enabled = false;
  edtCoolingFactor->Enabled = false;

  bActive = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEvolOperators::lstPopulationClick(TObject *)
{
  int i = lstPopulation->ItemIndex;
  if(i == -1) return;

  if(nsga2){
	DMOSP_OBChromosome *Chosen = nsga2->ChooseIndividual(i);
	frmSolution->SetCurrentSolution(nsga2->Solution);

	// Display the operations based chromosome
	DMOSP_OBGene *obgenes = Chosen->getGenes();
	for(i=0; i<Chosen->getLength(); i++){
	  grdOBChromosome->Cells[i][0] = obgenes[i].Operation->ID();
	  grdOBChromosome->Cells[i][1] = obgenes[i].Machine->ID();
	}
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEvolOperators::FormClose(TObject *Sender, TCloseAction &Action)

{
  if(nsga2){
	nsga2->FreeMemory();
	delete nsga2;
  }
  nsga2 = NULL;
  bActive = false;
  TfrmAlgorithm::FormClose(Sender, Action);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnSelectClick(TObject *)
{
  String s = edtTournamentSize->Text;
  int tsz = StrToInt(s);
  int n = nsga2->getPopSize();
  nsga2->TournamentSelection(tsz);
  sgMatingPool->ColCount = n;
  for(int i=0; i<n; i++){
	sgMatingPool->Cells[i][0] = IntToStr(nsga2->getMatingPool(i));
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::cbxParent1Change(TObject *)
{
  int i = cbxParent1->ItemIndex;
  if(i == -1 || !nsga2) return;

  iParent1 = i;
  DMOSP_OBChromosome *Chosen = nsga2->ChooseIndividual(i);
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	String s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdParents->Cells[i][0] = s;
  }
  cbxParent1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::cbxParent2Change(TObject *)
{
  int i = cbxParent2->ItemIndex;
  if(i == -1 || !nsga2) return;

  iParent2 = i;
  DMOSP_OBChromosome *Chosen = nsga2->ChooseIndividual(i);
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	String s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdParents->Cells[i][1] = s;
  }
  cbxParent2->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnApplyXOverClick(TObject *)
{
  if(iParent1 == -1 || iParent2 == -1){
	Application->MessageBoxA(L"Please choose two parents", L"Missing parent(s)", MB_OK);
	return;
  }
  if(cbxXOverOperator->ItemIndex == -1){
	Application->MessageBoxA(L"Please choose a crossover operator", L"Unselected operator", MB_OK);
	return;
  }

  DMOSP_Individual *parent1 = nsga2->getGuy(iParent1);
  DMOSP_Individual *parent2 = nsga2->getGuy(iParent2);
  switch(cbxXOverOperator->ItemIndex){
	case 0:  // Single-point OX
			 nsga2->SinglePointOX(parent1, parent2);
			 break;
	case 1:  // Two-point OX
			 nsga2->TwoPointOX(parent1, parent2);
			 break;
	case 2:  // Uniform OX
			 nsga2->UniformOX(parent1, parent2);
			 break;
	case 3:  // Generalized positoin crossover (GPX)
             nsga2->GPX(parent1, parent2);
			 break;
	case 4:  // Similar Block 2-Point Order Crossover
             nsga2->SB2OX(parent1, parent2);
             break;
  }

  // display resultant child solutions
  int i;
  String s;
  DMOSP_OBChromosome *Chosen = parent1->Chromosome;
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdChildren->Cells[i][0] = s;
  }
  Chosen = parent2->Chromosome;
  obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdChildren->Cells[i][1] = s;
  }

  // Crossover points
  s = "";
  for(i=0; i<nsga2->nXOverPoints; i++){
	int xpnt = nsga2->xOverLocus[i];
	s = s + IntToStr(xpnt);
	if(i<nsga2->nXOverPoints - 1) s = s + ", ";
  }
  lblxOverPoints->Caption = s;

  btnApplyXOver->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnRestartClick(TObject *)
{
  grdParents->ColCount = Problem->nOperations();
  grdChildren->ColCount = Problem->nOperations();
  for(int i=0; i<Problem->nOperations(); i++){
	grdParents->Cells[i][0] = "";
	grdParents->Cells[i][1] = "";
	grdChildren->Cells[i][0] = "";
	grdChildren->Cells[i][1] = "";
  }
  cbxParent1->Enabled = true;
  cbxParent1->ItemIndex = -1;
  cbxParent2->Enabled = true;
  cbxParent2->ItemIndex = -1;
  iParent1 = iParent2 = -1;
  btnApplyXOver->Enabled = true;
  lblxOverPoints->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnRestartMutationClick(TObject *)
{
  grdOriginalChromosome->ColCount = Problem->nOperations();
  grdMutatedChromosome->ColCount = Problem->nOperations();
  for(int i=0; i<Problem->nOperations(); i++){
	grdOriginalChromosome->Cells[i][0] = "";
	grdMutatedChromosome->Cells[i][0] = "";
  }
  cbxOriginalChromosome->Enabled = true;
  cbxOriginalChromosome->ItemIndex = -1;
  iOriginalChromosome = -1;
  btnApplyMutation->Enabled = true;
  lblMutationPoints->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::cbxOriginalChromosomeChange(TObject *)
{
  int i = cbxOriginalChromosome->ItemIndex;
  if(i == -1 || !nsga2) return;

  iOriginalChromosome = i;
  DMOSP_OBChromosome *Chosen = nsga2->ChooseIndividual(i);
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	String s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdOriginalChromosome->Cells[i][0] = s;
  }
  cbxOriginalChromosome->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnApplyMutationClick(TObject *)
{
  if(iOriginalChromosome == -1){
	Application->MessageBoxA(L"Please choose a chromosome", L"Missing chromosome", MB_OK);
	return;
  }
  if(cbxMutationOperator->ItemIndex == -1){
	Application->MessageBoxA(L"Please choose a mutation operator", L"Unselected operator", MB_OK);
	return;
  }

  DMOSP_Individual *OriginalIndividual = nsga2->getGuy(iOriginalChromosome);

  // apply selected mutation operator
  switch(cbxMutationOperator->ItemIndex){
	case 0: // Swap mutation
			nsga2->SwapMutation(OriginalIndividual);
			break;
	case 1: // Shift mutation
			nsga2->ShiftMutation(OriginalIndividual);
			break;
	case 2: // Randomized load shuffling mutation
            nsga2->RLSMutation(OriginalIndividual);
			break;
  }

  // display resultant chromosome
  int i;
  String s;
  DMOSP_OBChromosome *Chosen = OriginalIndividual->Chromosome;
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	grdMutatedChromosome->Cells[i][0] = s;
  }

  // Mutation points
  s = "";
  if(cbxMutationOperator->ItemIndex == 2){
	DMOSP_Problem::SMC *mc1 = nsga2->M[nsga2->mutPoint1];
	DMOSP_Problem::SMC *mc2 = nsga2->M[nsga2->mutPoint2];
    s = "From " + String(mc1->ID()) + " to " + String(mc2->ID());
  }
  else{
	int mupnt = nsga2->mutPoint1;
	s = s + IntToStr(mupnt) + ", ";
	mupnt = nsga2->mutPoint2;
	s = s + IntToStr(mupnt);
  }
  lblMutationPoints->Caption = s;

  btnApplyMutation->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::cbxSAOriginalChromosomeChange(TObject *)

{
  int i = cbxSAOriginalChromosome->ItemIndex;
  if(i == -1 || !nsga2) return;

  iSAOriginalChromosome = i;
  DMOSP_OBChromosome *Chosen = nsga2->ChooseIndividual(i);
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	String s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	sgSAOriginalChromosome->Cells[i][0] = s;
  }
  cbxSAOriginalChromosome->Enabled = false;

  edtnoSAIterations->Enabled = true;
  edtInitTemp->Enabled = true;
  edtCoolingFactor->Enabled = true;

  OriginalSAIndividual = nsga2->getGuy(iSAOriginalChromosome);
  String s;
  s = FloatToStr(OriginalSAIndividual->Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(OriginalSAIndividual->MWFT);
  lblMWFT->Caption = s;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnRestartSAClick(TObject *)
{
  sgSAOriginalChromosome->ColCount = Problem->nOperations();
  sgSAFinalChromosome->ColCount = Problem->nOperations();
  for(int i=0; i<Problem->nOperations(); i++){
	sgSAOriginalChromosome->Cells[i][0] = "";
	sgSAFinalChromosome->Cells[i][0] = "";
  }
  cbxSAOriginalChromosome->Enabled = true;
  cbxSAOriginalChromosome->ItemIndex = -1;
  iSAOriginalChromosome = -1;
  btnRunSA->Enabled = true;
  btnStepSA->Enabled = true;
  edtnoSAIterations->Enabled = false;
  edtInitTemp->Enabled = false;
  edtCoolingFactor->Enabled = false;

  SAIter = 0;
  pbSAIterations->Position = 0;
  lblCmax->Caption = "";
  lblMWFT->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnRunSAClick(TObject *)
{
  if(iSAOriginalChromosome == -1){
	Application->MessageBoxA(L"Please choose a chromosome", L"Missing chromosome", MB_OK);
	return;
  }
  if(!ReadSAParameters()) return;

  btnRunSA->Enabled = false;
  btnStepSA->Enabled = false;
  edtnoSAIterations->Enabled = false;
  edtInitTemp->Enabled = false;
  edtCoolingFactor->Enabled = false;

  pbSAIterations->Max = nSAIterations;
  pbSAIterations->Position = 0;

  // Apply simulated annealing local search
  nsga2->setSAParameters(nSAIterations, SAInitialTemp, SACoolingFactor);
  nsga2->RunSA(OriginalSAIndividual);

  // display resultant chromosome
  int i;
  String s;
  DMOSP_OBChromosome *Chosen = OriginalSAIndividual->Chromosome;
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	sgSAFinalChromosome->Cells[i][0] = s;
  }
  s = FloatToStr(OriginalSAIndividual->Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(OriginalSAIndividual->MWFT);
  lblMWFT->Caption = s;
  pbSAIterations->Position = nSAIterations;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEvolOperators::btnStepSAClick(TObject *)
{
  if(iSAOriginalChromosome == -1){
	Application->MessageBoxA(L"Please choose a chromosome", L"Missing chromosome", MB_OK);
	return;
  }

  if(SAIter == 0){
	if(!ReadSAParameters()) return;
	edtnoSAIterations->Enabled = false;
	edtInitTemp->Enabled = false;
	edtCoolingFactor->Enabled = false;
	btnRunSA->Enabled = false;

	pbSAIterations->Max = nSAIterations;
	pbSAIterations->Position = 0;

	nsga2->setSAParameters(nSAIterations, SAInitialTemp, SACoolingFactor);
    nsga2->InitializeSA(OriginalSAIndividual);
	lblTemperature->Caption = FloatToStr(SAInitialTemp);
  }

  // Apply a single iteration of simulated annealing local search
  nsga2->StepSA(OriginalSAIndividual);

  // display resultant chromosome
  int i;
  String s;
  DMOSP_OBChromosome *Chosen = OriginalSAIndividual->Chromosome;
  DMOSP_OBGene *obgenes = Chosen->getGenes();
  for(i=0; i<Chosen->getLength(); i++){
	s = "(";
	s = s + obgenes[i].Operation->ID() + ", ";
	s = s + obgenes[i].Machine->ID() + ")";
	sgSAFinalChromosome->Cells[i][0] = s;
  }
  s = FloatToStr(OriginalSAIndividual->Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(OriginalSAIndividual->MWFT);
  lblMWFT->Caption = s;

  if(SAIter == nSAIterations){
	btnRunSA->Enabled = false;
	btnStepSA->Enabled = false;
	return;
  }
  SAIter++;
  pbSAIterations->Position = SAIter;

  s = FloatToStr(nsga2->getCurSATemp());
  lblTemperature->Caption = s;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmEvolOperators::ReadSAParameters()
{
  String s;
  try{
	s = edtnoSAIterations->Text;
	nSAIterations = StrToInt(s);
	s = edtInitTemp->Text;
	SAInitialTemp = StrToFloat(s);
	s = edtCoolingFactor->Text;
	SACoolingFactor = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading SA parameters", MB_OK);
	  return false;
	}
  return true;
}

