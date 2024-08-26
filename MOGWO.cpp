//---------------------------------------------------------------------------


#pragma hdrstop

#include <math.h>
#include "MOGWO.h"
#include "random.h"
#include "FormMOGWOCalcProgress.h"
#include "FormTuningMOGWO.h"


//---------------------------------------------- The DMOSP_Wolf class functions

DMOSP_Wolf::DMOSP_Wolf(DMOSP_Problem *P)
{
  Chromosome = new DMOSP_RKChromosome(P);
  Cmax = MWFT = MAXFLOAT;
  GridIndex = GridSubIndex[0] = GridSubIndex[1] = -1;
  bDominated = false;
}

DMOSP_Wolf::DMOSP_Wolf(DMOSP_Wolf *wlf)
{
  Chromosome = new DMOSP_RKChromosome(wlf->Chromosome->getProblem());
  Copy(wlf);
}

DMOSP_Wolf::~DMOSP_Wolf()
{
  delete Chromosome;
}

bool DMOSP_Wolf::Copy(DMOSP_Wolf *wlf)
{
  if(Chromosome->getProblem() != wlf->Chromosome->getProblem()) return false;
  Chromosome->Copy(wlf->Chromosome);
  Cmax = wlf->Cmax;
  MWFT = wlf->MWFT;
  bDominated = wlf->bDominated;

  GridIndex = wlf->GridIndex;
  for(int i=0; i<=1; i++){
	GridSubIndex[i] = wlf->GridSubIndex[i];
  }
  return true;
}

//--------------------------------------------------- The MOGWO class functions

DMOSP_MOGWO::DMOSP_MOGWO(DMOSP_Problem *P, int nGWs, int nIter, double timelimit,
						 int szArch, double a, int ngrd, double b, double g,
						 DMOSP_MUTATION_OPERATOR mOpr, int nsaitr,
						 double saitemp, double sacoolrt, double saprop)
  : DMOSP_Metaheuristic(P, L"MOGWO for DMOSP", timelimit), nGreyWolves(nGWs),
	maxIterations(nIter), ArchiveSize(szArch), alpha(a), beta(b), gamma(g),
	nGrids(ngrd), mutOper(mOpr), maxSAIterations(nsaitr), initSATemp(saitemp),
	SACoolingRate(sacoolrt), SAApplProp(saprop)
{
  GD = IGD = -1;
}

DMOSP_MOGWO::~DMOSP_MOGWO()
{
  if(P.Count() > 0){
    SList::SNode *snd;
	for(snd = P.head(); snd; snd = snd->Next()){
	  DMOSP_Solution *sol = (DMOSP_Solution *)snd->Data();
	  delete sol;
	}
    P.Clear();
  }
}

void DMOSP_MOGWO::AllocateMemory()
{
  // If previous solutions were generated in a previous run, delete them
  if(P.Count() > 0){
    SList::SNode *snd;
	for(snd = P.head(); snd; snd = snd->Next()){
	  DMOSP_Solution *sol = (DMOSP_Solution *)snd->Data();
	  delete sol;
	}
	P.Clear();
  }

  DMOSP_Metaheuristic::AllocateMemory();

  // Allocate memory for GreyWolves
  int ii;
  Wolves = new DMOSP_Wolf *[nGreyWolves];
  for(ii=0; ii<nGreyWolves; ii++){
	Wolves[ii] = new DMOSP_Wolf(Problem);
  }

  bestSAWolf = new DMOSP_Wolf(Problem);

  // Allocate memory for Archive wolves
  Archive = new DMOSP_Wolf *[ArchiveSize];
  ExtraArchive = new DMOSP_Wolf *[ArchiveSize];
  rep2 = new DMOSP_Wolf *[ArchiveSize];
  rep3 = new DMOSP_Wolf *[ArchiveSize];
  for(ii=0; ii<ArchiveSize; ii++){
	Archive[ii] = new DMOSP_Wolf(Problem);
	ExtraArchive[ii] = new DMOSP_Wolf(Problem);
	rep2[ii] = new DMOSP_Wolf(Problem);
	rep3[ii] = new DMOSP_Wolf(Problem);
  }

  // Allocate memroy for Combined set
  CombinedSize = ((nGreyWolves >= ArchiveSize)? nGreyWolves : ArchiveSize)
				   + ArchiveSize;
  Combined = new DMOSP_Wolf *[CombinedSize];
  for(ii=0; ii<CombinedSize; ii++){
	Combined[ii] = new DMOSP_Wolf(Problem);
  }

  // Allocate memory for hypercube
  Hypercube = new DMOSP_Hypercube[2];     // for two objectives of the DMOSP
  for(ii=0; ii<2; ii++){
	Hypercube[ii].Lower = new double[nGrids];
	Hypercube[ii].Upper = new double[nGrids];
  }

  // Allocate memory for occupeid cells
  occ_cell_index = new int[nGrids*nGrids];
  occ_cell_member_count = new int[nGrids*nGrids];

  // Allocate memory for updating vectors
  nVar = 2*nOps;
  c_ = new double[nVar];
  D_ = new double[nVar];
  A_ = new double[nVar];
  X1_ = new double[nVar];
  X2_ = new double[nVar];
  X3_ = new double[nVar];
}

void DMOSP_MOGWO::Initialize()
{
  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);
  bTerminate = false;

  // Calculate lower bounds
  CalculateCmaxLowerBound();
  CalculateMWFTLowerBound();

  itr = 0;

  if(frmMOGWOCalcProgress->bActive){
	frmMOGWOCalcProgress->grdInitialSolutions->RowCount = nGreyWolves + 1;
	frmMOGWOCalcProgress->pbConstruction->Min = 0;
	frmMOGWOCalcProgress->pbConstruction->Max = nGreyWolves;
	frmMOGWOCalcProgress->pbConstruction->Position = 0;
	frmMOGWOCalcProgress->pbGeneration->Min = 0;
	frmMOGWOCalcProgress->pbGeneration->Max = maxIterations;
	frmMOGWOCalcProgress->pbGeneration->Position = 0;
	frmMOGWOCalcProgress->sgGenerations->RowCount =  maxIterations+1;
	frmMOGWOCalcProgress->ClearForm();
	frmMOGWOCalcProgress->DrawLBLines(CmaxLB, MWFTLB);
  }
  else if(frmTuningMOGWO->bActive){
	frmTuningMOGWO->pbIterations->Min = 0;
	frmTuningMOGWO->pbIterations->Max = maxIterations;
	frmTuningMOGWO->pbConstruction->Min = 0;
	frmTuningMOGWO->pbConstruction->Max = nGreyWolves;
  }

  // Initialize the grey wolf population using the DSG
  // Calculate the objective values for each search agent
  bestCmax = bestMWFT = MAXFLOAT;
  DSG_JSR jsr[] = {RANDj, MCTj, WSPT, WLWR};
  DSG_MSR msr[] = {RANDm, MCTm, MWL};
  int jsri, msri;
  for(int i=0; i < nGreyWolves; i++){
	jsri = rnd(0,3);
	msri = rnd(0,2);
	DSG(jsr[jsri], msr[msri], Wolves[i]->Cmax, Wolves[i]->MWFT);
	Wolves[i]->Chromosome->Encode(Solution);       // Initial position

	if(bestCmax > Wolves[i]->Cmax) bestCmax = Wolves[i]->Cmax;
	if(bestMWFT > Wolves[i]->MWFT) bestMWFT = Wolves[i]->MWFT;

	if(frmMOGWOCalcProgress->bActive){
	  frmMOGWOCalcProgress->pbConstruction->Position = i+1;
	  if(frmMOGWOCalcProgress->cbxConstruction->Checked){
		frmMOGWOCalcProgress->grdInitialSolutions->Cells[0][i+1] = IntToStr(i+1);
		frmMOGWOCalcProgress->grdInitialSolutions->Cells[1][i+1] = FloatToStr(Wolves[i]->Cmax);
		frmMOGWOCalcProgress->grdInitialSolutions->Cells[3][i+1] = FloatToStr(Wolves[i]->MWFT);
		frmMOGWOCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
		frmMOGWOCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	  }
	}
	else if(frmTuningMOGWO->bActive){
	  frmTuningMOGWO->pbConstruction->Position = i+1;
	}

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
  if(bTerminate) return;

  // Apply simulated annealing local search
  for(int i = 0; i < nGreyWolves; i++){
	if(flip(SAApplProp)){
	  InitializeSA(Wolves[i]);
	  RunSA(Wolves[i]);

	  if(bestCmax > Wolves[i]->Cmax) bestCmax = Wolves[i]->Cmax;
	  if(bestMWFT > Wolves[i]->MWFT) bestMWFT = Wolves[i]->MWFT;

	  Application->ProcessMessages();
	  if(frmMOGWOCalcProgress->bActive){
		if(frmMOGWOCalcProgress->cbxConstruction->Checked){
		  frmMOGWOCalcProgress->grdInitialSolutions->Cells[2][i+1] = FloatToStr(Wolves[i]->Cmax);
		  frmMOGWOCalcProgress->grdInitialSolutions->Cells[4][i+1] = FloatToStr(Wolves[i]->MWFT);
		  frmMOGWOCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
		  frmMOGWOCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
		}
	  }
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
	else{
	  if(frmMOGWOCalcProgress->bActive && frmMOGWOCalcProgress->cbxConstruction->Checked){
		frmMOGWOCalcProgress->grdInitialSolutions->Cells[2][i+1] = L"--";
		frmMOGWOCalcProgress->grdInitialSolutions->Cells[4][i+1] = L"--";
	  }
	  if(frmMOGWOCalcProgress->bActive && frmMOGWOCalcProgress->cbxSA->Checked){
		frmMOGWOCalcProgress->lblSAInitialCmax->Caption = "";
		frmMOGWOCalcProgress->lblSACurrentCmax->Caption = "";
		frmMOGWOCalcProgress->lblSABestCmax->Caption = "";
		frmMOGWOCalcProgress->lblSAInitialMWFT->Caption = "";
		frmMOGWOCalcProgress->lblSACurrentMWFT->Caption = "";
		frmMOGWOCalcProgress->lblSABestMWFT->Caption = "";
		frmMOGWOCalcProgress->lblSAIter->Caption = "";
		frmMOGWOCalcProgress->lblSATemperature->Caption = "";
		frmMOGWOCalcProgress->pbSA->Position = 0;
	  }
	}
  }
  if(bTerminate) return;

  // Find the non-dominated solutions and initialized the archive with them
  DetermineDomination(Wolves);
  GetNonDominatedWolves(Wolves);        // Update Archive

  if(nExtraArchiveWolves > 0){    // If the ArchiveSize is exceeded
	CombineArchives();     // Combine Archive with ExtraArchive in Combined
    CreateHypercubes(Combined);
	GetGridIndexes(Combined);
	DeleteFromRep();
  }

  CreateHypercubes(Archive);
  GetGridIndexes(Archive);

  if(frmMOGWOCalcProgress->bActive){
	frmMOGWOCalcProgress->DrawFronts();
	frmMOGWOCalcProgress->lblCmaxLB->Caption = FloatToStr(CmaxLB);
	frmMOGWOCalcProgress->lblMWFTLB->Caption = FloatToStr(MWFTLB);
	frmMOGWOCalcProgress->DrawOptimalParetoFront(Problem->i);
	frmMOGWOCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
	frmMOGWOCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
  }
}

void DMOSP_MOGWO::Improve()
{
  int i;
  double a = 2.0 - (double) (itr+1.0) * (2.0 / (double) maxIterations);

  for(i=0; i<nGreyWolves; i++){
	nrep2 = nrep3 = 0;

	// Choose the alpha, beta, and delta grey wolves from archived wolves
	// randomly as a start
	Alpha = SelectLeader(Archive, beta);
	Beta = SelectLeader(Archive, beta);
	Delta = SelectLeader(Archive, beta);

	// If the number of archived wolves is greater than one, then we can
	// exclude solutions that are similar to Alpha wolf and choose
	// a Beta wolf from among the rest
	if(nArchiveWolves > 1){
	  for(int newi=0; newi<nArchiveWolves; newi++){
		if(SumDeltaPosition(Alpha, Archive[newi]) > 0.001){
		  rep2[nrep2]->Copy(Archive[newi]);
		  nrep2++;
		}
	  }
	  Beta = SelectLeader(rep2, beta);
	}

	// If the number of archived wolves is greater than two, then we can
	// exclude solutions that are similar to Alpha and Beta wolves and choose
	// a Delta wolf from among the rest
	if(nArchiveWolves > 2){
	  for(int newi=0; newi<nrep2; newi++){
		if(SumDeltaPosition(Beta, rep2[newi]) > 0.001){
		  rep3[nrep2]->Copy(rep2[newi]);
		  nrep3++;
		}
	  }
	  Delta = SelectLeader(rep3, beta);
	}

	double *curWolf_genes = Wolves[i]->Chromosome->getGenes();
	double *Alpha_genes = Alpha->Chromosome->getGenes();
	double *Beta_genes = Beta->Chromosome->getGenes();
	double *Delta_genes = Delta->Chromosome->getGenes();

	int p;

	//-------------------------------------------- Alpha equations
	// Eq (3.4) in the paper
	for(p=0; p < nVar; p++){
	  c_[p] = 2*frandom();
	}
	// Eq (3.1) in the paper
	for(p=0; p < nVar; p++){
	  D_[p] = fabs(c_[p] * Alpha_genes[p] - curWolf_genes[p]);
	}
	// Eq (3.3) in the paper
	for(p=0; p < nVar; p++){
	  A_[p] = 2 * a * frandom() - a;
	}
	// Eq (3.8) in the paper
	for(p=0; p < nVar; p++){
	  X1_[p] = Alpha_genes[p] - A_[p] * D_[p];
	}

	//-------------------------------------------- Beta equations
	// Eq (3.4) in the paper
	for(p=0; p < nVar; p++){
	  c_[p] = 2*frandom();
	}
	// Eq (3.1) in the paper
	for(p=0; p < nVar; p++){
	  D_[p] = fabs(c_[p] * Beta_genes[p] - curWolf_genes[p]);
	}
	// Eq (3.3) in the paper
	for(p=0; p < nVar; p++){
	  A_[p] = 2 * a * frandom() - a;
	}
	// Eq (3.8) in the paper
	for(p=0; p < nVar; p++){
	  X2_[p] = Beta_genes[p] - A_[p] * D_[p];
	}

	//-------------------------------------------- Delta equations
	// Eq (3.4) in the paper
	for(p=0; p < nVar; p++){
	  c_[p] = 2*frandom();
	}
	// Eq (3.1) in the paper
	for(p=0; p < nVar; p++){
	  D_[p] = fabs(c_[p] * Delta_genes[p] - curWolf_genes[p]);
	}
	// Eq (3.3) in the paper
	for(p=0; p < nVar; p++){
	  A_[p] = 2 * a * frandom() - a;
	}
	// Eq (3.8) in the paper
	for(p=0; p < nVar; p++){
	  X3_[p] = Delta_genes[p] - A_[p] * D_[p];
	}

	// Now, update the values of the genes in the current wolf and check
	// upper and lower values
	for(p=0; p < nVar; p++){
	  curWolf_genes[p] = (X1_[p] + X2_[p] + X3_[p]) / 3.0;
	  if(curWolf_genes[p] < 0.0) curWolf_genes[p] = 0.0;
	  else if(curWolf_genes[p] > 1.0) curWolf_genes[p] = 1.0;
	}

	// Finally, interpret the chromosome and evaluate the objective values
	DecodeRKChromosome(Wolves[i]->Chromosome, Solution, Wolves[i]->Cmax,
					   Wolves[i]->MWFT);

    // Apply simulated annealing local search
	if(flip(SAApplProp)){
	  InitializeSA(Wolves[i]);
	  RunSA(Wolves[i]);

	  if(bestCmax > Wolves[i]->Cmax) bestCmax = Wolves[i]->Cmax;
	  if(bestMWFT > Wolves[i]->MWFT) bestMWFT = Wolves[i]->MWFT;

	  Application->ProcessMessages();
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
  if(bTerminate) return;

  DetermineDomination(Wolves);

  // Combine the two sets of non-dominated Wolves and the Archive
  nCombined = 0;
  for(i=0; i<nGreyWolves; i++){
	if(!Wolves[i]->bDominated){
	  Combined[nCombined]->Copy(Wolves[i]);
	  nCombined++;
	}
  }
  for(i=0; i<nArchiveWolves; i++){
	Combined[nCombined]->Copy(Archive[i]);
	nCombined++;
  }

  DetermineDomination(Combined);
  GetNonDominatedWolves(Combined);   // update Archive based on the combined set

  if(nExtraArchiveWolves > 0){    // If the ArchiveSize is exceeded
	CombineArchives();     // Combine Archive with ExtraArchive in Combined
    CreateHypercubes(Combined);
	GetGridIndexes(Combined);
	DeleteFromRep();
  }

  CreateHypercubes(Archive);
  GetGridIndexes(Archive);

  if(frmMOGWOCalcProgress->bActive){
	frmMOGWOCalcProgress->DrawFronts();
	frmMOGWOCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
	frmMOGWOCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	frmMOGWOCalcProgress->pbGeneration->Position = itr;
	String s = IntToStr(itr);
	frmMOGWOCalcProgress->lblGeneration->Caption = s;
	if(frmMOGWOCalcProgress->cbxGenerations->Checked){
	  frmMOGWOCalcProgress->sgGenerations->Cells[0][itr] = s;
	  frmMOGWOCalcProgress->sgGenerations->Cells[1][itr] = FloatToStr(bestCmax);
	  frmMOGWOCalcProgress->sgGenerations->Cells[2][itr] = FloatToStr(bestMWFT);
	  frmMOGWOCalcProgress->sgGenerations->Cells[3][itr] = IntToStr(nArchiveWolves);;
	}
  }
  Application->ProcessMessages();
  if(bInterrupt) bTerminate = true;
}

bool DMOSP_MOGWO::Stop()
{
  bool bStop = false;

  if(bTerminate){
	bStop = true;
  }

  if(!bStop){
	itr++;
	if(frmTuningMOGWO->bActive){
	  frmTuningMOGWO->pbIterations->Position = itr;
	}

	if(itr > maxIterations){
	  bStop = true;
	}
  }

  if(bStop){
	return true;
  }
  return false;
}

void DMOSP_MOGWO::Finalize()
{
  DMOSP_Metaheuristic::Finalize();
  SolStatus = FEASIBLE;
  StoreParetoFrontSolutions();
  ComputePerformanceMetrics();   // Compute GD and IGD for the set of solved instances

  if(frmMOGWOCalcProgress->bActive){
	frmMOGWOCalcProgress->DrawFronts();
	frmMOGWOCalcProgress->btnTerminate->Enabled = false;
	frmMOGWOCalcProgress->btnStart->Enabled = true;
	frmMOGWOCalcProgress->btnClose->Enabled = true;
	frmMOGWOCalcProgress->DrawNonDominatedFront();
  }
}

void DMOSP_MOGWO::FreeMemory()
{
  DMOSP_Metaheuristic::FreeMemory();

  // Delete wolves
  int ii;
  for(ii=0; ii<nGreyWolves; ii++){
	delete Wolves[ii];
  }
  delete [] Wolves;

  delete bestSAWolf;

  // Delete Archived wolves
  for(ii=0; ii<ArchiveSize; ii++){
	delete Archive[ii];
	delete ExtraArchive[ii];
	delete rep2[ii];
	delete rep3[ii];
  }
  delete [] Archive;
  delete [] ExtraArchive;
  delete [] rep2;
  delete [] rep3;

  // Delete Combined wolves
  for(ii=0; ii<CombinedSize; ii++){
	delete Combined[ii];
  }
  delete [] Combined;

  // Free memory for hypercubes
  for(ii=0; ii<2; ii++){
	delete [] Hypercube[ii].Lower;
	delete [] Hypercube[ii].Upper;
  }
  delete [] Hypercube;

  // Free memory for occumed cells information
  delete [] occ_cell_index;
  delete [] occ_cell_member_count;

  // Free allocated memory for vectors
  delete [] c_;
  delete [] D_;
  delete [] A_;
  delete [] X1_;
  delete [] X2_;
  delete [] X3_;
}

void DMOSP_MOGWO::ComputeObjectiveValue()
{
  // This is just a weighted summation of the two considered objectives
  ObjectiveValue = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  ObjectiveValue += Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);
  ObjectiveValue /= 2.0;
}

// ---------- Multi-objective Grey Wolf Operaizer main functions  --------

void DMOSP_MOGWO::DetermineDomination(DMOSP_Wolf **pop)
{
  int npop;
  if(pop == Wolves) npop = nGreyWolves;
  else if(pop == Combined) npop = nCombined;
  else return;

  for(int i=0; i<npop; i++){
	pop[i]->bDominated = false;
	for(int j=0; j<i; j++){
	  if(!pop[j]->bDominated){
		 if((pop[i]->Cmax >= pop[j]->Cmax && pop[i]->MWFT >= pop[j]->MWFT)){
		   pop[i]->bDominated = true;
		   break;
		 }
		 else if((pop[i]->Cmax <= pop[j]->Cmax && pop[i]->MWFT < pop[j]->MWFT)
			 || (pop[i]->Cmax < pop[j]->Cmax && pop[i]->MWFT <= pop[j]->MWFT)){
		   pop[j]->bDominated = true;
		 }
	  }
	}
  }
}

void DMOSP_MOGWO::GetNonDominatedWolves(DMOSP_Wolf **pop)
{
  int npop;
  if(pop == Wolves) npop = nGreyWolves;
  else if(pop == Combined) npop = nCombined;
  else return;

  nArchiveWolves = nExtraArchiveWolves = 0;
  for(int i=0; i<npop; i++){
	if(!(pop[i]->bDominated)){
	  if(nArchiveWolves < ArchiveSize){
		Archive[nArchiveWolves]->Copy(pop[i]);
		nArchiveWolves++;
	  }
	  else if(nExtraArchiveWolves < ArchiveSize){
		ExtraArchive[nExtraArchiveWolves]->Copy(pop[i]);
		nExtraArchiveWolves++;
	  }
	  else break;
	}
  }
}

void DMOSP_MOGWO::CreateHypercubes(DMOSP_Wolf **pop)
{
  int npop;
  if(pop == Archive) npop = nArchiveWolves;
  else if(pop == Combined) npop = nCombined;
  else return;

  for(int i=0; i<2; i++){   // for each objective function
	// Find the minimum and the maximum values
	double min_cj=MAXFLOAT, max_cj=0.0;
	for(int j=0; j<npop; j++){
	  double objective = pop[j]->Objective[i];
	  if(min_cj > objective) min_cj = objective;
	  if(max_cj < objective) max_cj = objective;
	}

	// Assign upper and lower values
	double dcj = alpha * (max_cj - min_cj);
	min_cj -= dcj;
	max_cj += dcj;
	double grdinc = (max_cj - min_cj) / (nGrids - 1);
	for(int g=0; g<nGrids; g++){
	  if(g==0) Hypercube[i].Lower[g] = -MAXFLOAT;
	  else Hypercube[i].Lower[g] = min_cj + (double) (g-1) * grdinc;
	  if(g==nGrids-1) Hypercube[i].Upper[g] = MAXFLOAT;
	  else Hypercube[i].Upper[g] = min_cj + (double) g * grdinc;
	}
  }
}

void DMOSP_MOGWO::GetGridIndexes(DMOSP_Wolf **pop)
{
  int npop;
  if(pop == Archive) npop = nArchiveWolves;
  else if(pop == Combined) npop = nCombined;
  else return;

  // Grid subindex for each wolf in the Archive is the index in the upper
  //   array of the hypercube directly below which the objective value exists
  for(int j=0; j<npop; j++){              // j -> Archived wolf
	for(int i=0; i<2; i++){               // i -> objective
	  for(int g=0; g<nGrids; g++){        // g -> grid index
		if(pop[j]->Objective[i] < Hypercube[i].Upper[g]){
		   pop[j]->GridSubIndex[i] = g;
		  break;
		}
	  }
	}
	pop[j]->GridIndex =   pop[j]->GridSubIndex[1] * (nGrids - 1)
						  + pop[j]->GridSubIndex[0];
  }
}

void DMOSP_MOGWO::GetOccupiedCells(DMOSP_Wolf **pop)
{
  n_occ_cells = 0;

  int size;
  if(pop == Archive) size = nArchiveWolves;
  else if(pop == rep2) size = nrep2;
  else if(pop == rep3) size = nrep3;
  else if(pop == Combined) size = nCombined;
  else return;

  int gi;
  for(int j=0; j<size; j++){
	gi = pop[j]->GridIndex;

    bool bFilledCell = false;
	for(int c=n_occ_cells-1; c>=0; c--){
	  if(occ_cell_index[c] == gi){
		bFilledCell = true;
		break;
	  }
	}
    if(bFilledCell) continue;

	occ_cell_index[n_occ_cells] = gi;
	occ_cell_member_count[n_occ_cells] = 1;
	for(int i=j+1; i<size; i++){
	  if(pop[i]->GridIndex == gi) occ_cell_member_count[n_occ_cells]++;
	}
	n_occ_cells++;
  }
}

DMOSP_Wolf *DMOSP_MOGWO::SelectLeader(DMOSP_Wolf **pop, double bta)
{
  int size;
  if(pop == Archive) size = nArchiveWolves;
  else if(pop == rep2) size = nrep2;
  else if(pop == rep3) size = nrep3;
  else return NULL;

  // First construct the occupied cells information
  GetOccupiedCells(pop);

  // Construct the roulette wheel selection probabilities
  int c;   // index of the occupied cell
  double *p = new double[n_occ_cells];
  double sump = 0.0;
  for(c=0; c<n_occ_cells; c++){
	p[c] = pow((double)occ_cell_member_count[c], -bta);
	sump += p[c];
  }
  for(c=0; c<n_occ_cells; c++){
	p[c] /= sump;
  }

  // Apply roulette wheel selector
  double prob = frandom();
  for(c=0; c<n_occ_cells; c++){
	if(prob <= p[c]) break;
  }
  if(c == n_occ_cells) c = n_occ_cells - 1;  // now c is the selected cell index
  delete [] p;

  // select a wolf from the cell members randomly
  int s = rnd(0, occ_cell_member_count[c]-1);
  int j;
  for(j=0; j<size; j++){
	if(pop[j]->GridIndex == occ_cell_index[c]){
	  if(s == 0) break;
	  s--;
	}
  }

  return pop[j];
}

void DMOSP_MOGWO::CombineArchives()
{
  int i;
  nCombined = 0;
  for(i=0; i<nArchiveWolves; i++){
	Combined[nCombined]->Copy(Archive[i]);
	nCombined++;
  }
  for(i=0; i<nExtraArchiveWolves; i++){
	Combined[nCombined]->Copy(ExtraArchive[i]);
	nCombined++;
  }
}

void DMOSP_MOGWO::DeleteFromRep()
{
  // The Combined set includes all wolves from both Archive and ExtraArchive
  // before calling this function. Of course, this function cannot be reached
  // unless the size of the Combined set is greater than the Archive size
  //
  while(nCombined > ArchiveSize){
	GetOccupiedCells(Combined);

	// Construct the roulette wheel selection probabilities
	int c;
	double *p = new double[n_occ_cells];
	double sump = 0.0;
	for(c=0; c<n_occ_cells; c++){
	  p[c] = pow((double)occ_cell_member_count[c], gamma);
	  sump += p[c];
	}
	for(c=0; c<n_occ_cells; c++){
	  p[c] /= sump;
	}

	// Apply roulette wheel selector
	double prob = frandom();
	for(c=0; c<n_occ_cells; c++){
	  if(prob <= p[c]) break;
	}
	if(c == n_occ_cells) c = n_occ_cells - 1;  //  ---> now c is the selected cell index
	delete [] p;

	// select a wolf from the cell members randomly
	int s = rnd(0, occ_cell_member_count[c]);
	int j;
	for(j=0; j<nCombined; j++){
	  if(Combined[j]->GridIndex == occ_cell_index[c]){
		if(s == 0) break;
		s--;
	  }
	}

	// Now remove the wolve at index j from the Combined list
	for(int i=j+1; i<nCombined; i++){
      Combined[i-1]->Copy(Combined[i]);
	}

	nCombined--;
  }

  // Finally, copy the wolves in the Combined set into Archive
  for(int i=0; i<ArchiveSize; i++){
    Archive[i]->Copy(Combined[i]);
  }
}

// ------------  Suplementary functions

double DMOSP_MOGWO::SumDeltaPosition(DMOSP_Wolf *w1, DMOSP_Wolf *w2)
{
  double ret = 0.0;

  double *p1 = w1->Chromosome->getGenes();
  double *p2 = w2->Chromosome->getGenes();
  int l = w1->Chromosome->getLength();

  for(int p=0; p<l; p++){
	ret += fabs(p1[p] - p2[p]);
  }

  return ret;
}

// -------------------------- Mutation operators ------------------------------

void DMOSP_MOGWO::SwapMutation(DMOSP_Wolf *original, bool bConstruct)
{
  int length = nOps * 2;
  mutPoint1 = rnd(1,length*4/5-1);
  mutPoint2 = rnd(mutPoint1+1,length-1);

  double *kGenes = original->Chromosome->getGenes();

  double tmpGene = kGenes[mutPoint1];
  kGenes[mutPoint1] = kGenes[mutPoint2];
  kGenes[mutPoint2] = tmpGene;

  if(bConstruct)
	DecodeRKChromosome(original->Chromosome, Solution, original->Cmax, original->MWFT);
}

void DMOSP_MOGWO::ShiftMutation(DMOSP_Wolf *original, bool bConstruct)
{
  int length = nOps * 2;
  mutPoint1 = rnd(1,length*4/5-1);
  mutPoint2 = rnd(mutPoint1+1,length-1);

  double *kGenes = original->Chromosome->getGenes();

  double tmpGene = kGenes[mutPoint2];
  for(int l=mutPoint2; l>mutPoint1; l--){
	kGenes[l] = kGenes[l-1];
  }
  kGenes[mutPoint1] = tmpGene;

  if(bConstruct)
	DecodeRKChromosome(original->Chromosome, Solution, original->Cmax, original->MWFT);
}

//--------------------- Simulated Annealing local search --------------------
void DMOSP_MOGWO::setSAParameters(int nsaitr, double saitemp, double sacoolrt)
{
  maxSAIterations = nsaitr;
  initSATemp = curSATemp = saitemp;
  SACoolingRate = sacoolrt;
}

void DMOSP_MOGWO::InitializeSA(DMOSP_Wolf *wlf)
{
  SAIter = 0;
  curSATemp = initSATemp;
  bestSAWolf->Copy(wlf);

  if(frmMOGWOCalcProgress->bActive && frmMOGWOCalcProgress->cbxSA->Checked){
	String s = FloatToStr(wlf->Cmax);
	frmMOGWOCalcProgress->lblSAInitialCmax->Caption = s;
	frmMOGWOCalcProgress->lblSACurrentCmax->Caption = s;
	frmMOGWOCalcProgress->lblSABestCmax->Caption = s;
	s = FloatToStr(wlf->MWFT);
	frmMOGWOCalcProgress->lblSAInitialMWFT->Caption = s;
	frmMOGWOCalcProgress->lblSACurrentMWFT->Caption = s;
	frmMOGWOCalcProgress->lblSABestMWFT->Caption = s;
	frmMOGWOCalcProgress->lblSAIter->Caption = "0";
	frmMOGWOCalcProgress->lblSATemperature->Caption = FloatToStr(initSATemp);
	frmMOGWOCalcProgress->pbSA->Position = 0;
	frmMOGWOCalcProgress->pbSA->Max = maxSAIterations;
  }
}

void DMOSP_MOGWO::RunSA(DMOSP_Wolf *wlf)
{
  InitializeSA(wlf);
  while(!StepSA(wlf)){
	if(frmMOGWOCalcProgress->bActive && frmMOGWOCalcProgress->cbxSA->Checked){
	  String s;
	  frmMOGWOCalcProgress->pbSA->Position = SAIter;
	  s = FloatToStr(SAIter);
	  frmMOGWOCalcProgress->lblSAIter->Caption = s;
	  s = FloatToStr(curSATemp);
	  frmMOGWOCalcProgress->lblSATemperature->Caption = s;
	  s = FloatToStr(wlf->Cmax);
	  frmMOGWOCalcProgress->lblSACurrentCmax->Caption = s;
	  s = FloatToStr(wlf->MWFT);
	  frmMOGWOCalcProgress->lblSACurrentMWFT->Caption = s;
	  s = FloatToStr(bestSAWolf->Cmax);
	  frmMOGWOCalcProgress->lblSABestCmax->Caption = s;
	  s = FloatToStr(bestSAWolf->MWFT);
	  frmMOGWOCalcProgress->lblSABestMWFT->Caption = s;
	}

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;;
	}
  }
}

bool DMOSP_MOGWO::StepSA(DMOSP_Wolf *wlf)
{
  DMOSP_Wolf *newWolf = new DMOSP_Wolf(wlf);

  switch(mutOper){
	 case SHIFT_MUTATION: ShiftMutation(newWolf);
						  break;
	 case SWAP_MUTATION:  SwapMutation(newWolf);
						  break;
  }

  if(newWolf->Cmax <= bestSAWolf->Cmax && newWolf->MWFT <= bestSAWolf->MWFT){
	bestSAWolf->Copy(newWolf);
  }

  if(newWolf->Cmax <= wlf->Cmax && newWolf->MWFT <= wlf->MWFT){
	wlf->Copy(newWolf);
  }
  else {
	// Measure the percentage deviations for both objectives
	double delta_Cmax = 100.0 * ((newWolf->Cmax - wlf->Cmax)/wlf->Cmax);
	double delta_MWFT = 100.0 * ((newWolf->MWFT - wlf->MWFT)/wlf->MWFT);
	double delta = delta_Cmax + delta_MWFT;
	if(delta <= 0.0){
	  if(flip(0.5)) wlf->Copy(newWolf);
	}
	else{
	  double prob = exp(-delta/curSATemp);
	  if(flip(prob)) wlf->Copy(newWolf);
	}
  }
  delete newWolf;

  curSATemp *= SACoolingRate;
  SAIter++;
  if(SAIter <= maxSAIterations) return false;

  wlf->Copy(bestSAWolf);
  return true;   // terminate!
}

//------------------ Storing non-dominated solutions in set P

void DMOSP_MOGWO::StoreParetoFrontSolutions()
{
  SList::SNode *snd;
  if(P.Count() > 0){
	for(snd = P.head(); snd; snd = snd->Next()){
	  DMOSP_Solution *sol = (DMOSP_Solution *)snd->Data();
	  delete sol;
	}
  }

  for(int ps=0; ps < nArchiveWolves; ps++){
	DMOSP_Wolf *wlf = Archive[ps];
	DMOSP_RKChromosome *Chosen = wlf->Chromosome;
	DMOSP_Solution *sol = new DMOSP_Solution(Problem);
	DecodeRKChromosome(Chosen, sol, wlf->Cmax, wlf->MWFT);

	bool bAdd = true;
	for(int pg=ps-1; pg>=0; pg--){
	  DMOSP_Wolf *prevwlf = Archive[pg];
	  if(prevwlf->Cmax == wlf->Cmax && prevwlf->MWFT == wlf->MWFT){
		bAdd = false;
		break;
	  }
	}
	if(bAdd) P.Add(sol);
  }
}

void DMOSP_MOGWO::ComputePerformanceMetrics()
{
  // Calculate Hypervolumd (HV)
  //  NOTE: the set P of non-dominated solutions should have been formed previously
  //
  P.QSort(BiObjSolComp);
  double CmaxLimit = 1.5 * CmaxLB;
  double MWFTLimit = 1.8 * MWFTLB;

  HV = 0.0;
  DMOSP_Solution *prvS;
  double dCmax, dMWFT;
  bool bFirstSol = true;
  for(SList::SNode *pnd = P.head(); pnd; pnd = pnd->Next()){
	DMOSP_Solution *S = (DMOSP_Solution *)pnd->Data();
	if(bFirstSol){  // first solution with the minimum Cmax and maximum MWFT
	  dCmax = CmaxLimit - S->Cmax;
	  dMWFT = MWFTLimit - S->MWFT;
	  if(dCmax < 0.0 || dMWFT < 0.0) continue;
	  HV += (dCmax * dMWFT);
	  prvS = S;
	  bFirstSol = false;
	}
	else {
	  dCmax = CmaxLimit - S->Cmax;
	  dMWFT = prvS->MWFT - S->MWFT;
	  if(dCmax < 0.0 || dMWFT < 0.0) break;
	  HV += (dCmax * dMWFT);
	  prvS = S;
	}
  }
  HVpr = HV / (0.5 * CmaxLB * 0.8 * MWFTLB) * 100.0;

  HV0 = -1;  // This indicates that the optimal Pareto front is not known

  int prb_i = Problem->i;
  if(prb_i < 0 || prb_i > 30) return; // GD & IGD are available only to known optimal Pareto fronts

  int ps, fs;
  int nP = nArchiveWolves;
  int nF = SolvedDMOSPs[prb_i].nSol;
  int nConsidered = 0;

  double **ED = new double *[nF];     // Eucludean distance matrix
  for(fs=0; fs<nF; fs++) ED[fs] = new double[nP];

  GD = IGD = HV0 = 0.0;
  for(ps=0; ps<nP; ps++){
	DMOSP_Wolf *wlf = Archive[ps];

	// If another guy with the same Cmax and MWFT was encountered,
	//   do not consider this guy
	bool bAdd = true;
	for(int pg=ps-1; pg>=0; pg--){
	  DMOSP_Wolf *prevwlf = Archive[pg];
	  if(prevwlf->Cmax == wlf->Cmax && prevwlf->MWFT == wlf->MWFT){
		bAdd = false;
		break;
	  }
	}
	if(bAdd){
	  for(fs=0; fs<nF; fs++){
		double d;
		d = pow(wlf->Cmax - SolvedDMOSPs[prb_i].Cmax[fs], 2.0)
			+ pow(wlf->MWFT - SolvedDMOSPs[prb_i].MWFT[fs], 2.0);
		ED[fs][nConsidered] = d;
	  }
	  nConsidered++;
	}
  }

  // to calculate GD
  for(ps=0; ps<nConsidered; ps++){
	double minED = MAXFLOAT;
	for(fs=0; fs<nF; fs++) if(ED[fs][ps] < minED) minED = ED[fs][ps];
	GD += minED;
  }
  GD = pow(GD, 0.5) / nConsidered;

  // to calculate IGD
  for(fs=0; fs<nF; fs++){
	double minED = MAXFLOAT;
	for(ps=0; ps<nConsidered; ps++) if(ED[fs][ps] < minED) minED = ED[fs][ps];
	IGD += minED;

	if(fs == 0){  // first solution with the maximum Cmax and minimum MWFT
	  HV0 += (  (CmaxLimit - SolvedDMOSPs[prb_i].Cmax[fs])
			  * (MWFTLimit - SolvedDMOSPs[prb_i].MWFT[fs]));
	}
	else {
	  HV0 += ((SolvedDMOSPs[prb_i].Cmax[fs-1] - SolvedDMOSPs[prb_i].Cmax[fs])
			  * (MWFTLimit - SolvedDMOSPs[prb_i].MWFT[fs]));
	}
  }
  IGD = pow(IGD, 0.5) / nF;
  TGD = GD + IGD;

  HV0pr = HV0 / (0.5 * CmaxLB * 0.8 * MWFTLB) * 100.0;

  for(fs=0; fs<nF; fs++) delete [] ED[fs];
  delete [] ED;
}


//---------------------------------------------------------------------------

#pragma package(smart_init)
