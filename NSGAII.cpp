//---------------------------------------------------------------------------


#pragma hdrstop

#include "NSGAII.h"
#include "random.h"
#include "FormNSGA2CalcProgress.h"
#include "FormEvolutionaryOperators.h"
#include <math.h>
#include "FormTuningNSGA2.h"

MOGA_Random myRandom;

//------------------------------------------------------------ DMOSP_Individual

DMOSP_Individual::DMOSP_Individual(DMOSP_Problem *P)
{
  Chromosome = new DMOSP_OBChromosome(P);
  Cmax = MWFT = -1.0;
  fitCmax = fitMWFT = 0.0;
  rank = 0;
  crowdingDistance = 0.0;
}

DMOSP_Individual::DMOSP_Individual(DMOSP_Individual *ind)
{
  Chromosome = new DMOSP_OBChromosome(ind->Chromosome->getProblem());
  Copy(ind);
}

DMOSP_Individual::~DMOSP_Individual()
{
  delete Chromosome;
}

bool DMOSP_Individual::Copy(DMOSP_Individual *ind)
{
  if(Chromosome->getProblem() != ind->Chromosome->getProblem()) return false;
  Chromosome->Copy(ind->Chromosome);

  Cmax = ind->Cmax;
  MWFT = ind->MWFT;
  fitCmax = ind->fitCmax;
  fitMWFT = ind->fitMWFT;
  rank = ind->rank;
  crowdingDistance = ind->crowdingDistance;
  return true;
}

//---------------------------------------------------------------- DMOSP_NSGAII

DMOSP_NSGAII::DMOSP_NSGAII(DMOSP_Problem *P, int ps, int maxgen, double timelimit,
				 double xPrb, DMOSP_NSGAII::DMOSP_XOVER_OPERATOR xOpr,
				 double mPrb, DMOSP_NSGAII::DMOSP_MUTATION_OPERATOR mOpr,
				 double rlsMutProb, int nsaitr, double saitemp, double sacoolrt,
				 double saprop)
   : DMOSP_Metaheuristic(P, L"NSGA-II for DMOSP", timelimit), popSize(ps), maxGenerations(maxgen),
	 xOverProbability(xPrb), xOverOper(xOpr), mutProbability(mPrb), mutOper(mOpr),
	 RLBmutProbability(rlsMutProb), maxSAIterations(nsaitr), initSATemp(saitemp),
	 SACoolingRate(sacoolrt), SAApplProp(saprop)
{
  GD = IGD = -1;
}

DMOSP_NSGAII::~DMOSP_NSGAII()
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

void DMOSP_NSGAII::AllocateMemory()
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

  int ii, combinedPopSize = 2 * popSize;

  // Allocate memory
  guys = new DMOSP_Individual *[popSize];
  newGuys = new DMOSP_Individual *[popSize];
  for(ii=0; ii<popSize; ii++){
	guys[ii] = new DMOSP_Individual(Problem);
	newGuys[ii] = new DMOSP_Individual(Problem);
  }
  mpool = new int[popSize];

  combinedGuys = new DMOSP_Individual*[combinedPopSize];
  numIndsFront = new int[combinedPopSize];
  paretoFront = new int*[combinedPopSize];
  for(ii = 0; ii < combinedPopSize; ii++) {
	combinedGuys[ii] = new DMOSP_Individual(Problem);
	paretoFront[ii] = new int[combinedPopSize];
  }

  bestobj = new double[2];
  worstobj = new double[2];
  avgobj = new double[2];
  maxfit = new double[2];
  minfit = new double[2];
  avgfit = new double[2];
  varfit = new double[2];

  xOverLocus = new int[nOps];
  xOverActions = new DMOSP_xOverOpActions[nOps];

  tmpGene = new DMOSP_OBGene();

  nCandidateWSs = 0;
  int maxnMCs = 0;
  for(SList::SNode *wsnd = Problem->WCs.head(); wsnd; wsnd = wsnd->Next()){
	DMOSP_Problem::SWC *ws = (DMOSP_Problem::SWC *)wsnd->Data();
	int nMCs = ws->MCs.Count();
	if(nMCs > 1) nCandidateWSs++;
	if(nMCs > maxnMCs) maxnMCs = nMCs;
  }
  iCandidateWSs = new int[nCandidateWSs];
  nCandidateWSs = 0;  // to be used as a temporary index
  for(SList::SNode *wsnd = Problem->WCs.head(); wsnd; wsnd = wsnd->Next()){
	DMOSP_Problem::SWC *ws = (DMOSP_Problem::SWC *)wsnd->Data();
	int nMCs = ws->MCs.Count();
	if(nMCs > 1){
	  iCandidateWSs[nCandidateWSs] = ws->i;
	  nCandidateWSs++;
	}
  }
  totalMCLoad = new double[maxnMCs];
  iLoadedMC = new int[maxnMCs];
  nOpsLoaded = new int[maxnMCs];

  bestSAInd = new DMOSP_Individual(Problem);
}

void DMOSP_NSGAII::Initialize()
{
  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);
  bTerminate = false;

  // Calculate lower bounds
  CalculateCmaxLowerBound();
  CalculateMWFTLowerBound();

  if(frmNSGA2CalcProgress->bActive){
	frmNSGA2CalcProgress->grdInitialSolutions->RowCount = popSize + 1;
	frmNSGA2CalcProgress->pbConstruction->Min = 0;
	frmNSGA2CalcProgress->pbConstruction->Max = popSize;
	frmNSGA2CalcProgress->pbConstruction->Position = 0;
	frmNSGA2CalcProgress->pbGeneration->Min = 0;
	frmNSGA2CalcProgress->pbGeneration->Max = maxGenerations;
	frmNSGA2CalcProgress->pbGeneration->Position = 0;
	frmNSGA2CalcProgress->sgGenerations->RowCount =  maxGenerations+1;
	frmNSGA2CalcProgress->ClearForm();
	frmNSGA2CalcProgress->DrawLBLines(CmaxLB, MWFTLB);
  }
  else if(frmTuningNSGA2->bActive){
	frmTuningNSGA2->pbIterations->Min = 0;
	frmTuningNSGA2->pbIterations->Max = maxGenerations;
	frmTuningNSGA2->pbConstruction->Min = 0;
	frmTuningNSGA2->pbConstruction->Max = popSize;
  }

  // Construct initial population
  double bestCmax, bestMWFT;
  bestCmax = bestMWFT = MAXFLOAT;
  DSG_JSR jsr[] = {RANDj, MCTj, WSPT, WLWR};
  DSG_MSR msr[] = {RANDm, MCTm, MWL};
  int jsri, msri;
  for(int i=0; i<popSize; i++){
	jsri = rnd(0,3);
	msri = rnd(0,2);
	DSG(jsr[jsri], msr[msri], guys[i]->Cmax, guys[i]->MWFT);
	guys[i]->Chromosome->Encode(Solution);

	if(bestCmax > guys[i]->Cmax) bestCmax = guys[i]->Cmax;
	if(bestMWFT > guys[i]->MWFT) bestMWFT = guys[i]->MWFT;

	if(frmNSGA2CalcProgress->bActive){
	  frmNSGA2CalcProgress->pbConstruction->Position = i+1;
	  if(frmNSGA2CalcProgress->cbxConstruction->Checked){
		frmNSGA2CalcProgress->grdInitialSolutions->Cells[0][i+1] = IntToStr(i+1);
		frmNSGA2CalcProgress->grdInitialSolutions->Cells[1][i+1] = FloatToStr(guys[i]->Cmax);
		frmNSGA2CalcProgress->grdInitialSolutions->Cells[3][i+1] = FloatToStr(guys[i]->MWFT);
		frmNSGA2CalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
		frmNSGA2CalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	  }
	}
	else if(frmTuningNSGA2->bActive){
	  frmTuningNSGA2->pbConstruction->Position = i+1;
	}

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  if(frmNSGA2CalcProgress->bActive){
	frmNSGA2CalcProgress->lblCmaxLB->Caption = FloatToStr(CmaxLB);
	frmNSGA2CalcProgress->lblMWFTLB->Caption = FloatToStr(MWFTLB);
	frmNSGA2CalcProgress->DrawLBLines(CmaxLB, MWFTLB);
	frmNSGA2CalcProgress->DrawOptimalParetoFront(Problem->i);
  }

  // Apply simulated annealing local search
  for(int i = 0; i < popSize; i++){
	if(myRandom.flip(SAApplProp)){
	  InitializeSA(guys[i]);
	  RunSA(guys[i]);

	  if(bestCmax > guys[i]->Cmax) bestCmax = guys[i]->Cmax;
	  if(bestMWFT > guys[i]->MWFT) bestMWFT = guys[i]->MWFT;

	  Application->ProcessMessages();
	  if(frmNSGA2CalcProgress->bActive){
		if(frmNSGA2CalcProgress->cbxConstruction->Checked){
		  frmNSGA2CalcProgress->grdInitialSolutions->Cells[2][i+1] = FloatToStr(guys[i]->Cmax);
		  frmNSGA2CalcProgress->grdInitialSolutions->Cells[4][i+1] = FloatToStr(guys[i]->MWFT);
		  frmNSGA2CalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
		  frmNSGA2CalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
		}
	  }
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
	else{
	  if(frmNSGA2CalcProgress->bActive && frmNSGA2CalcProgress->cbxConstruction->Checked){
		frmNSGA2CalcProgress->grdInitialSolutions->Cells[2][i+1] = L"--";
		frmNSGA2CalcProgress->grdInitialSolutions->Cells[4][i+1] = L"--";
	  }

	  if(frmNSGA2CalcProgress->bActive && frmNSGA2CalcProgress->cbxSA->Checked){
		frmNSGA2CalcProgress->lblSAInitialCmax->Caption = "";
		frmNSGA2CalcProgress->lblSACurrentCmax->Caption = "";
		frmNSGA2CalcProgress->lblSABestCmax->Caption = "";
		frmNSGA2CalcProgress->lblSAInitialMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSACurrentMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSABestMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSAIter->Caption = "";
		frmNSGA2CalcProgress->lblSATemperature->Caption = "";
		frmNSGA2CalcProgress->pbSA->Position = 0;
	  }
	}
  }
  if(bTerminate) return;

  // Compute objectives and fitnesses statistics for the constructed guys
  computeObjStatistics(GUYS);
  mapObjectiveToFitness(GUYS);
  computeFitnessStatistics(GUYS);

  // Initialize the number of fronts and the changes in the number of fronts
  numFronts = 0;
  numFrontChange = 0;

  genID = 0;
  noOfGlobalEvals = (long)(popSize);
  noOfLocalEvals = 0;

  doNonDominatedSort(GUYS);
  oldNoOfFronts = numFronts;
  computeCrowdingDistance(GUYS);

  if(frmNSGA2CalcProgress->bActive){
	frmNSGA2CalcProgress->DrawFronts();
	frmNSGA2CalcProgress->lblCmaxLB->Caption = FloatToStr(CmaxLB);
	frmNSGA2CalcProgress->lblMWFTLB->Caption = FloatToStr(MWFTLB);
	frmNSGA2CalcProgress->DrawOptimalParetoFront(Problem->i);
	frmNSGA2CalcProgress->lblbestCmax->Caption = FloatToStr(bestobj[0]);
	frmNSGA2CalcProgress->lblbestMWFT->Caption = FloatToStr(bestobj[1]);
  }
}

void DMOSP_NSGAII::Improve()
{
  int ii;

  // 1. Apply tournament selection operator
  TournamentSelection();

  // ---> Now the set of newGuys are copied from the current population (Guys)
  // 2. Apply selected crossover operator on every pair of consecutive newGuys
  for(ii = 0; ii < popSize; ii += 2){
	if(myRandom.flip(xOverProbability)){
	  switch(xOverOper){
		case SINGLE_POINT_OX:
				  SinglePointOX(newGuys[ii], newGuys[ii+1]);
				  break;
		case TWO_POINT_OX:
				  TwoPointOX(newGuys[ii], newGuys[ii+1]);
				  break;
		case UNIFORM_OX:
				  UniformOX(newGuys[ii], newGuys[ii+1]);
				  break;
		case GENPOSX:
				  GPX(newGuys[ii], newGuys[ii+1]);
				  break;
		case SIMBLOCK2OX:
				  SB2OX(newGuys[ii], newGuys[ii+1]);
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

  // 3. Apply selected mutation operator on every child solution and then apply
  //    the randomized load balancing operator
  for(ii = 0; ii < popSize; ii++){
	if(myRandom.flip(mutProbability)){
	  switch(mutOper){
		case SHIFT_MUTATION: ShiftMutation(newGuys[ii]);
							 break;
		case SWAP_MUTATION:  SwapMutation(newGuys[ii]);
							 break;
	  }
	}
	if(myRandom.flip(RLBmutProbability)){
	  RLSMutation(newGuys[ii]);

	  Application->ProcessMessages();
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
  }
  if(bTerminate) return;

  noOfGlobalEvals += (long)(popSize * 2);

  // Apply simulated annealing local search
  for(ii = 0; ii < popSize; ii++){
	if(myRandom.flip(SAApplProp)){
	  InitializeSA(newGuys[ii]);
	  RunSA(newGuys[ii]);
	  Application->ProcessMessages();
	  if(bInterrupt){
		bTerminate = true;
		break;
	  }
	}
	else{
	  if(frmNSGA2CalcProgress->bActive && frmNSGA2CalcProgress->cbxSA->Checked){
		frmNSGA2CalcProgress->lblSAInitialCmax->Caption = "";
		frmNSGA2CalcProgress->lblSACurrentCmax->Caption = "";
		frmNSGA2CalcProgress->lblSABestCmax->Caption = "";
		frmNSGA2CalcProgress->lblSAInitialMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSACurrentMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSABestMWFT->Caption = "";
		frmNSGA2CalcProgress->lblSAIter->Caption = "";
		frmNSGA2CalcProgress->lblSATemperature->Caption = "";
		frmNSGA2CalcProgress->pbSA->Position = 0;
	  }
	}
  }
  if(bTerminate) return;

  // Compute objectives and fitnesses statistics for the constructed guys
  computeObjStatistics(NEWGUYS);
  mapObjectiveToFitness(NEWGUYS);

  // Combine the current population and new population into one population of size 2n
  for(ii = 0; ii < popSize; ii++) {
	  *(combinedGuys[ii]) = *(guys[ii]);
  }
  for(ii = 0; ii < popSize; ii++)
      *(combinedGuys[ii+popSize]) = *(newGuys[ii]);

  computeFitnessStatistics(COMBINEDGUYS);
  doNonDominatedSort(COMBINEDGUYS);
  computeCrowdingDistance(COMBINEDGUYS);

  double *crowdDist;
  int np, fid;
  int numNextGen, indID, *index, *sortedIndex;
  int numEvals;

  //Start with indiviudals with rank 0. While the number of individuals in
  // the next generation plus the number of individuals in the current front
  // is less than the population size then copy all the individuals in the
  // current front and increase the front counter.
  numNextGen = fid = 0;
  while(numNextGen + numIndsFront[fid] <= popSize) {
	for(ii = 0; ii < numIndsFront[fid]; ii++) {
	  indID = paretoFront[fid][ii];
	  *(guys[numNextGen+ii]) = *(combinedGuys[indID]);
	}
	numNextGen += numIndsFront[fid++];
  }

  //If the number of individuals in the next generation plus the number of
  // individuals in the current front is greater than the population size then
  // copy the crowding distances of all the individuals in the current front
  // to an array CrowdDist and also store the index of individuals in Index.
  if(numNextGen < popSize) {
	crowdDist = new double[numIndsFront[fid]];
	index = new int[numIndsFront[fid]];
	sortedIndex = new int[numIndsFront[fid]];

	for(ii = 0; ii < numIndsFront[fid]; ii++) {
	  indID = paretoFront[fid][ii];
	  crowdDist[ii] = getCrowdingDistance(indID);
	  index[ii] = indID;
	  sortedIndex[ii] = ii;
	}

	//Sort the individuals in Index according to ascending order of Crowding Distance
	 regQSort(crowdDist, sortedIndex, 0, numIndsFront[fid]);

	// Copy the guys with higher crowding distance into the new population
	for(ii = numNextGen; ii < popSize; ii++) {
	  indID = index[sortedIndex[numIndsFront[fid] + numNextGen - ii - 1]];
		*(guys[ii]) = *(combinedGuys[indID]);
	}

	delete []index;
	delete []crowdDist;
	delete []sortedIndex;
  }

  computeFitnessStatistics(GUYS);
  doNonDominatedSort(GUYS);
  numFrontChange = abs(oldNoOfFronts - numFronts);
  oldNoOfFronts = numFronts;
  computeCrowdingDistance(GUYS);

  if(frmNSGA2CalcProgress->bActive){
	frmNSGA2CalcProgress->DrawFronts();
	frmNSGA2CalcProgress->lblbestCmax->Caption = FloatToStr(bestobj[0]);
	frmNSGA2CalcProgress->lblbestMWFT->Caption = FloatToStr(bestobj[1]);
	frmNSGA2CalcProgress->pbGeneration->Position = genID;
	String s = IntToStr(genID);
	frmNSGA2CalcProgress->lblGeneration->Caption = s;
	if(frmNSGA2CalcProgress->cbxGenerations->Checked){
	  frmNSGA2CalcProgress->sgGenerations->Cells[0][genID] = s;
	  frmNSGA2CalcProgress->sgGenerations->Cells[1][genID] = FloatToStr(avgobj[0]);
	  frmNSGA2CalcProgress->sgGenerations->Cells[2][genID] = FloatToStr(avgobj[1]);
	  frmNSGA2CalcProgress->sgGenerations->Cells[3][genID] = FloatToStr(varfit[0]);;
	  frmNSGA2CalcProgress->sgGenerations->Cells[4][genID] = FloatToStr(varfit[1]);;
	  frmNSGA2CalcProgress->sgGenerations->Cells[5][genID] = IntToStr(numFronts);
	  frmNSGA2CalcProgress->sgGenerations->Cells[6][genID] = IntToStr(numFrontChange);
	}
  }
  Application->ProcessMessages();
  if(bInterrupt) bTerminate = true;
}

bool DMOSP_NSGAII::Stop()
{
  bool bStop = false;

  if(bTerminate){
	bStop = true;
  }

  if(!bStop){
	genID++;
	if(frmNSGA2CalcProgress->bActive){
	  frmNSGA2CalcProgress->pbGeneration->Position = genID;
	}
	else if(frmTuningNSGA2->bActive){
	  frmTuningNSGA2->pbIterations->Position = genID;
	}

	if(genID > maxGenerations){
	  bStop = true;
	}
  }

  if(bStop){
	return true;
  }
  return false;
}

void DMOSP_NSGAII::Finalize()
{
  DMOSP_Metaheuristic::Finalize();
  SolStatus = FEASIBLE;
  StoreParetoFrontSolutions();
  ComputePerformanceMetrics();   // Compute GD and IGD for the set of solved instances

  if(frmNSGA2CalcProgress->bActive){
	frmNSGA2CalcProgress->DrawFronts();
	frmNSGA2CalcProgress->btnTerminate->Enabled = false;
	frmNSGA2CalcProgress->btnStart->Enabled = true;
	frmNSGA2CalcProgress->btnClose->Enabled = true;
	frmNSGA2CalcProgress->DrawNonDominatedFront();
  }
}

void DMOSP_NSGAII::FreeMemory()
{
  DMOSP_Metaheuristic::FreeMemory();

  int ii, combinedPopSize = 2 * popSize;

  // free allocated memory for the current population
  for(ii=0; ii<popSize; ii++){
	delete guys[ii];
	delete newGuys[ii];
  }
  delete [] guys;
  delete [] newGuys;
  delete [] mpool;

  for(ii = 0; ii < combinedPopSize; ii++) {
	delete combinedGuys[ii];
	delete paretoFront[ii];
  }
  delete [] combinedGuys;

  delete [] numIndsFront;
  delete [] paretoFront;
  delete [] bestobj;
  delete [] worstobj;
  delete [] avgobj;
  delete [] minfit;
  delete [] maxfit;
  delete [] avgfit;
  delete [] varfit;

  delete [] xOverLocus;
  delete [] xOverActions;

  delete tmpGene;

  delete [] iCandidateWSs;
  delete [] totalMCLoad;
  delete [] iLoadedMC;
  delete [] nOpsLoaded;

  delete bestSAInd;
}

void DMOSP_NSGAII::ComputeObjectiveValue()
{
  // This is just a weighted summation of the two considered objectives
  ObjectiveValue = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  ObjectiveValue += Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);
  ObjectiveValue /= 2.0;
}

void DMOSP_NSGAII::ComputePerformanceMetrics()
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

  int *nIndsFront = getNoOfIndsFront();
  int **ParetoFront = getParetoFront();

  int ps, fs;
  int nP = nIndsFront[0];
  int nF = SolvedDMOSPs[prb_i].nSol;
  int nConsidered = 0;

  double **ED = new double *[nF];     // Eucludean distance matrix
  for(fs=0; fs<nF; fs++) ED[fs] = new double[nP];

  GD = IGD = HV0 = 0.0;
  for(ps=0; ps<nP; ps++){
	DMOSP_Individual *guy = getGuy(ParetoFront[0][ps]);

	// If another guy with the same Cmax and MWFT was encountered,
	//   do not consider this guy
	bool bAdd = true;
	for(int pg=ps-1; pg>=0; pg--){
	  DMOSP_Individual *prevguy = getGuy(ParetoFront[0][pg]);
	  if(prevguy->Cmax == guy->Cmax && prevguy->MWFT == guy->MWFT){
		bAdd = false;
		break;
	  }
	}
	if(bAdd){
	  for(fs=0; fs<nF; fs++){
		double d;
		d = pow(guy->Cmax - SolvedDMOSPs[prb_i].Cmax[fs], 2.0)
			+ pow(guy->MWFT - SolvedDMOSPs[prb_i].MWFT[fs], 2.0);
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

void DMOSP_NSGAII::StoreParetoFrontSolutions()
{
  SList::SNode *snd;
  if(P.Count() > 0){
	for(snd = P.head(); snd; snd = snd->Next()){
	  DMOSP_Solution *sol = (DMOSP_Solution *)snd->Data();
	  delete sol;
	}
  }

  int *nIndsFront = getNoOfIndsFront();
  int **ParetoFront = getParetoFront();

  int nP = nIndsFront[0];
  for(int ps=0; ps < nP; ps++){
	DMOSP_Individual *guy = getGuy(ParetoFront[0][ps]);
	DMOSP_OBChromosome *Chosen = guy->Chromosome;
	DMOSP_Solution *sol = new DMOSP_Solution(Problem);
	DecodeOBChromosome(Chosen, sol, guy->Cmax, guy->MWFT);

	bool bAdd = true;
	for(int pg=ps-1; pg>=0; pg--){
	  DMOSP_Individual *prevguy = getGuy(ParetoFront[0][pg]);
	  if(prevguy->Cmax == guy->Cmax && prevguy->MWFT == guy->MWFT){
		bAdd = false;
		break;
	  }
	}
	if(bAdd) P.Add(sol);
  }
}

DMOSP_OBChromosome *DMOSP_NSGAII::ChooseIndividual(int i)
{
  if(i >= popSize || i < 0) return NULL;
  DMOSP_OBChromosome *Chosen = guys[i]->Chromosome;
  DecodeOBChromosome(Chosen, Solution, guys[i]->Cmax, guys[i]->MWFT);
  return Chosen;
}

DMOSP_OBChromosome *DMOSP_NSGAII::ChooseParetoIndividual(int i)
{
  int *nIndsFront = getNoOfIndsFront();
  int **ParetoFront = getParetoFront();
  if(i >= nIndsFront[0] || i < 0) return NULL;
  DMOSP_Individual *guy = getGuy(ParetoFront[0][i]);
  DMOSP_OBChromosome *Chosen = guy->Chromosome;
  DecodeOBChromosome(Chosen, Solution, guys[i]->Cmax, guys[i]->MWFT);
  return Chosen;
}

/*============================================================
** Function Name: DMOSP_NSGAII::computeObjStatistics(int whichGuys)
** The flag whichGuys refer to either the current population (for the
** initial generation) or the combined population.
** Function Task: this method generates statistics according to
** each objective function values. It finds out the average objective
** value for the population and the best and worst objective
** values for each objective function. This depends on whether the
** type of optimization in the problem of each objective function
** is minimization or maximization.
** Output:None
** Functions called: Individual::getObjective(int jj)
**========================================================*/
void DMOSP_NSGAII::computeObjStatistics(int whichGuys)
{
  int ii, jj, pSize;
  double oneOverPopulationSize;
  DMOSP_Individual **theGuys;

  if(whichGuys == GUYS) {
	pSize = popSize;
	theGuys = guys;
  }
  else if(whichGuys == NEWGUYS) {
	pSize = popSize;
	theGuys = newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	pSize = 2 * popSize;
	theGuys = combinedGuys;
  }

  oneOverPopulationSize = 1.0/(1.0*pSize);

  for(jj = 0; jj < 2; jj++) {
	bestobj[jj] = worstobj[jj] = theGuys[0]->getObjective(jj);
	avgobj[jj] = oneOverPopulationSize*(theGuys[0]->getObjective(jj));
	for(ii = 1; ii < pSize; ii++) {
	  avgobj[jj] += oneOverPopulationSize*(theGuys[ii]->getObjective(jj));
	  if(theGuys[ii]->getObjective(jj) < bestobj[jj])
		   bestobj[jj] = theGuys[ii]->getObjective(jj);
	  if(theGuys[ii]->getObjective(jj) > worstobj[jj])
		   worstobj[jj] = theGuys[ii]->getObjective(jj);
	}
  }
}

/***
Map each objective to corresponding fitness value depending on minimization or maximization.
The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
If the jth objective value is a maximization, then the fitness is set equal to the objective value. On the other hand, if the problem is a minimization one, then set the fitness equal to the negative of the objective value.
*/
void DMOSP_NSGAII::mapObjectiveToFitness(int whichGuys)
{
  int ii,jj, pSize;
  DMOSP_Individual **theGuys;

  if(whichGuys == GUYS) {
	pSize = popSize;
	theGuys = guys;
  }
  else if(whichGuys == NEWGUYS) {
	pSize = popSize;
	theGuys = newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	pSize = 2 * popSize;
	theGuys = combinedGuys;
  }

  // Since both objectives are minimization,
  //     the fitness is set as the negative the objective
  for (jj=0; jj<2; jj++) {
	for (ii=0; ii< pSize; ii++)
	  theGuys[ii]->setFitness(jj, -theGuys[ii]->getObjective(jj));
  }
}

/*============================================================
** Function Name: DMOSP_NSGAII::computeFitnessStatistics(int whichGuys)
** The flag whichGuys refer to either the current population
** (for the initial generation) or the combined population.
** Function Task: this method generates statistics according to
** every fitness function values. It finds out the average fitness
** value for the population and the maximum and minimum fitness
** values as well as the fitness variance for each fitness function.
** Output:None
** Functions called: Individual::getFitness(int jj)
**========================================================*/
void DMOSP_NSGAII::computeFitnessStatistics(int whichGuys)
{
  int ii, jj, pSize;
  double oneOverPopulationSize;
  DMOSP_Individual **theGuys;

  if(whichGuys == GUYS) {
	pSize = popSize;
	theGuys = guys;
  }
  else if(whichGuys == NEWGUYS) {
	pSize = popSize;
	theGuys = newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	pSize = 2 * popSize;
	theGuys = combinedGuys;
  }

  oneOverPopulationSize = 1.0/(1.0*pSize);

  for(jj = 0; jj < 2; jj++) {
	maxfit[jj] = minfit[jj] = theGuys[0]->getFitness(jj);
	avgfit[jj] = oneOverPopulationSize*(theGuys[0]->getFitness(jj));
	varfit[jj] = oneOverPopulationSize*(theGuys[0]->getFitness(jj))*(theGuys[0]->getFitness(jj));

	for(ii = 1; ii < pSize; ii++) {
	  avgfit[jj] += oneOverPopulationSize*(theGuys[ii]->getFitness(jj));
	  varfit[jj] += oneOverPopulationSize*(theGuys[ii]->getFitness(jj))*(theGuys[ii]->getFitness(jj));
	  if(theGuys[ii]->getFitness(jj) > maxfit[jj]) maxfit[jj] = theGuys[ii]->getFitness(jj);
	  if(theGuys[ii]->getFitness(jj) < minfit[jj]) minfit[jj] = theGuys[ii]->getFitness(jj);
	}
	varfit[jj] -= avgfit[jj]*avgfit[jj];
  }
}


/***
Peforms a nondominated sort of individuals to create a set of pareto fronts.
Details:
  *  The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
  *  domCount[i]: (domination count) Number of the individuals that dominate individual i
  *  numIndDom[i]: Number of individuals that individual i dominates
  *  indDomByMe[i]: Array of indices of individuals that individual i dominates
  *  paretoFront[i]: Array of indices of individuals that are members of pareto front i.
  *  numIndsFront[i]: Number of individuals in the ith front.
  *  For every unique pair of individuals (i,j)
     --Check if i dominates j, If yes then add j to indDomByMe[i], increment numIndDom[i] by 1 and increment domCount[j] by 1.
     --On the other hand if j dominates i, then add i to indDomByMe[j], increment numIndDom[j] by 1 and increment domCount[i] by 1.
  * For every individual i
     --If domCount[i] equals 0, then add individual i to paretoFront[0], increment numIndsFront[0], and set the rank of individual i to 0
  * Set frontID = 0. While number of individuals in the pareto Front is greater than zero, do
     --For every individual i in the pareto front frontID
         *  For every individual j that individual i dominates
	    --Decrement the domination count of j by i
            --If the domination count of j is equal to zero, then add j to the paretoFron[frontID+1], increment numIndsFront[frontID+1], and set the rank of individual j to frontID+1.
     --Increment the frontID by one
 * Set total number of pareto fronts to frontID.
*/
void DMOSP_NSGAII::doNonDominatedSort(int whichGuys)
{
  int ii, jj, pSize, fid, idomj, nif;
  int *domCount, **indDomByMe, *numIndDom;
  DMOSP_Individual **theGuys;

  if(whichGuys == GUYS) {
	pSize = popSize;
	theGuys = guys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	pSize = 2 * popSize;
	theGuys = combinedGuys;
  }

  domCount = new int[pSize];
  numIndDom = new int[pSize];
  indDomByMe = new int*[pSize];
  for(ii = 0; ii < pSize; ii++) {
	indDomByMe[ii] = new int[pSize];
	domCount[ii] = numIndDom[ii] = 0;
  }
  for(ii = 0; ii < pSize-1; ii++) {
	for(jj = ii+1; jj < pSize; jj++) {
	  idomj = dominates(theGuys[ii],theGuys[jj]);
	  if(idomj == IDOMJ) {
		indDomByMe[ii][numIndDom[ii]] = jj;
		numIndDom[ii] += 1;
		domCount[jj] += 1;
	  }
	  else if(idomj == JDOMI) {
		indDomByMe[jj][numIndDom[jj]] = ii;
		numIndDom[jj] += 1;
		domCount[ii] += 1;
	  }
	}
  }

  nif = 0;     // number of individuals in the front
  for(ii = 0; ii < pSize; ii++) {
	if(domCount[ii] == 0) {
	  paretoFront[0][nif++] = ii;
	  theGuys[ii]->setRank(0);
	}
  }
  numIndsFront[0] = nif;

  fid = 0;
  while(numIndsFront[fid] != 0) {
	nif = 0;
	for(ii = 0; ii < numIndsFront[fid]; ii++) {
	  for(jj = 0; jj < numIndDom[paretoFront[fid][ii]]; jj++) {
		domCount[indDomByMe[paretoFront[fid][ii]][jj]] -= 1;
		if(domCount[indDomByMe[paretoFront[fid][ii]][jj]] == 0) {
		  paretoFront[fid+1][nif++] = indDomByMe[paretoFront[fid][ii]][jj];
		  theGuys[indDomByMe[paretoFront[fid][ii]][jj]]->setRank(fid+1);
		}
	  }
	}
	fid += 1;
	numIndsFront[fid] = nif;
  }
  numFronts = fid;

  // I don't really know why this part of the code is added here, but I am
  // leaving it as is .........................................................
  if(whichGuys == GUYS)
	for(ii = 0; ii < pSize; ii++) guys[ii] = theGuys[ii];
  else if(whichGuys == COMBINEDGUYS)
	for(ii = 0; ii < pSize; ii++) combinedGuys[ii] = theGuys[ii];
  //...........................................................................

  for(ii = 0; ii < pSize; ii++)
	delete indDomByMe[ii];
  delete []domCount;
  delete []numIndDom;
  delete []indDomByMe;
}

/***
Computes the crowding distance of each individual.
Details:
  * The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
  * For every front i
    -- For every objective j
	   * Sort the individuals in front i in ascending order of objective j
       * Set the crowding distance of the first and the last individual to infinity
       * For every individual k in the front i except the first and the last individuals
         --Normalize the fitness j by dividing the fitness j of individual k-1  and individual k+1 by maximum jth fitness.
	 --Add absolute value of (Normalized jth fitness of individual k+1 - Normalized jth fitness of individual k-1) to the crowding distance of kth individual.
*/
void DMOSP_NSGAII::computeCrowdingDistance(int whichGuys)
{
  int ii, jj, kk, firstInd, lastInd;
  int indId1, indId2, indId, pSize;
  int *sortListByObj;
  double normFit1, normFit2, *crowdDist;
  DMOSP_Individual **theGuys;

  if(whichGuys == GUYS) {
	pSize = popSize;
	theGuys = guys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	pSize = 2 * popSize;
	theGuys = combinedGuys;
  }

  crowdDist = new double[pSize];
  for(ii = 0; ii < pSize; ii++) crowdDist[ii] = 0.0;

  for(ii = 0; ii < numFronts; ii++) {
	sortListByObj = new int[numIndsFront[ii]];
	for(jj = 0; jj < 2; jj++) {
	  for(kk = 0; kk < numIndsFront[ii]; kk++)
		  sortListByObj[kk] = paretoFront[ii][kk];
	  quickSort(theGuys, sortListByObj, 0, numIndsFront[ii],jj);
	  firstInd = sortListByObj[0];
	  lastInd = sortListByObj[numIndsFront[ii]-1];
	  crowdDist[firstInd] = crowdDist[lastInd] = INFTY;
	  for(kk = 1; kk < numIndsFront[ii]-1; kk++) {
		indId = sortListByObj[kk];
		indId1 = sortListByObj[kk+1];
		indId2 = sortListByObj[kk-1];
		normFit1 = theGuys[indId1]->getFitness(jj)/(1.0+maxfit[jj]);
		normFit2 = theGuys[indId2]->getFitness(jj)/(1.0+maxfit[jj]);
	    crowdDist[indId] += fabs(normFit1 - normFit2);
      }
    }
    delete [] sortListByObj;
  }

  if(whichGuys == GUYS)
	for(ii = 0; ii < pSize; ii++)
	  guys[ii]->setCrowdingDistance(crowdDist[ii]);
  else if (whichGuys == COMBINEDGUYS)
	for(ii = 0; ii < pSize; ii++)
	  combinedGuys[ii]->setCrowdingDistance(crowdDist[ii]);
  delete [] crowdDist;
}

/// Quick sort routine used for crowding distance computation. Sorts the
// individuals in a given pareto front in ascending order of objId the
// objective value.
void DMOSP_NSGAII::quickSort(DMOSP_Individual **theGuys, int *output,
							 int left, int right, int objId)
{
  int ii, xx;
  double target;

  if(right > left) {
	target = theGuys[output[right-1]]->getFitness(objId);
	ii = left-1;
	xx = right-1;
	for(;;) {
	  while(theGuys[output[++ii]]->getFitness(objId) < target)
		 if(ii >= right-1) break;
	  if(ii >= xx) break;
	  while(theGuys[output[--xx]]->getFitness(objId) > target)
	     if(xx <= 0) break;
      if(ii >= xx) break;
      swap(output[ii],output[xx]);
    }
    swap(output[ii],output[right-1]);
    quickSort(theGuys, output, left, ii, objId);
	quickSort(theGuys, output, ii+1, right, objId);
  }
}

///Swap used by the sort function
void DMOSP_NSGAII::swap(int& ii, int& jj)
{
  int temp;
  temp = jj; jj = ii; ii = temp;
}

///Quick sort function to sort individuals in a pareto front according to the crowding distance.
void DMOSP_NSGAII::regQSort(double *crowdDist, int *output, int left, int right)
{
  int ii, xx;
  double target;
  if(right > left) {
	target = crowdDist[output[right-1]];
	ii = left-1;
	xx = right-1;
	for(;;) {
	  while(crowdDist[output[++ii]] < target)
	if(ii >= right-1) break;
	  if(ii >= xx) break;
	  while(crowdDist[output[--xx]] > target)
	if(xx <= 0) break;
	  if(ii >= xx) break;
	  swap(output[ii],output[xx]);
	}
	swap(output[ii],output[right-1]);
	regQSort(crowdDist, output, left, ii);
	regQSort(crowdDist, output, ii+1, right);
  }
}

/**
Domination determinator. Determines if NsgaIndvidual 1 dominates NsgaIndvidual 2.
If the constraint handling method is either penalty method or if there are no constraints, then
  -- If every fitness value of individual 1 is greater than or equal to the fitness value of individual 2 then return 1.
  -- If every fitness value of individual 2 is greater than or equal to the fitness value of individual 2 then return -1.
  -- For all other cases return 0.
*/
int DMOSP_NSGAII::dominates (const DMOSP_Individual *guy1, const DMOSP_Individual *guy2)
{
  int ii, idomj = 0, jdomi = 0;

  for(ii = 0; ii < 2; ii++) {
	if(guy1->getFitness(ii) > guy2->getFitness(ii)) idomj = 1;
	else if(guy1->getFitness(ii) < guy2->getFitness(ii)) jdomi = 1;
  }

  if((idomj == 1) && (jdomi == 1)) return INONDOMJ;
  else if((idomj == 1)&&(jdomi == 0)) return IDOMJ;
  else if((idomj == 0)&&(jdomi == 1)) return JDOMI;
  else return INONDOMJ;
}

/***
Crowding comparison operator: Determines if Indvidual 1 is better than Individual 2.
If the rank of individual 1 is less than that of individual 2, then return 1.
If the rank of individual 1 and 2 are same and the crowding distance of individual 1
is greater than that of individual 2 then return 1. For all other cases return 0.
*/
bool DMOSP_NSGAII::crowdingComp (const DMOSP_Individual *guy1, const DMOSP_Individual *guy2)
{
  if((guy1->rank < guy2->rank)||
	 ((guy1->rank == guy2->rank)&&
	  (guy1->crowdingDistance > guy2->crowdingDistance))) return 1;
  else return 0;
}

/***
Returns 1 if individual 1 is better than individual 2 depending on
Details:  If the GA type is NSGA then, return 1 if crowdingComp returns 1,
	      otherwise return 0.
 */
int DMOSP_NSGAII::betterIndividual(DMOSP_Individual *guy1, DMOSP_Individual *guy2)
{
  if(crowdingComp(guy1,guy2)) return 1;
  return 0;
}

/***
Tournament Selection Without Replacement: Randomly select S individuals without
replacement and copy the best individual to the mating pool. Repeat this process
till N individuals are selected.
Implementation:
   1. Shuffle the Individuals of the current population
   2. Select S individuals without replacement
   3. Copy the index of the best individual among those S individuals
	  to the mating pool
   4. Go to step 2 till all the individuals are chosen
   5. If the number of individuals in mating pool is less than N
	  then go to step 1.

Note: The new population is set equal to the selected parents at the end of
	  selection.
*/
void DMOSP_NSGAII::TournamentSelection(int tournamentSize)
{
  int ii, jj, kk, ll = 0;
  int p1, p2, *randomArray, *winner;

  randomArray = new int[popSize];
  winner = new int[popSize];

  for(ii = 0; ii < popSize; ii++) randomArray[ii] = ii;
  for(ii = 0; ii < tournamentSize; ii++) {
	myRandom.shuffleArray(randomArray, popSize);
	for(jj = 0; jj < popSize; jj += tournamentSize) {
	  p1 = randomArray[jj];
	  for(kk = 1; kk < tournamentSize; kk++) {
		p2 = randomArray[jj+kk];
		if(betterIndividual(guys[p1],guys[p2])) winner[ll] = p1;
		else winner[ll] = p2;
		p1 = winner[ll];
	  }
	  ++ll;
	}
  }
  for(ii = 0; ii < popSize; ii++)
	mpool[ii] = winner[ii];
  delete []randomArray;
  delete []winner;

  //Selection updates the index of selected individuals which is stored in mpool.
  // Use this index to copy the selected individuals to create new pop
  for(ii = 0; ii < popSize; ii++)
	 *(newGuys[ii]) = *(guys[mpool[ii]]);
}

// ------------------------- Crossover operators ----------------------------

void DMOSP_NSGAII::SinglePointOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2,
                                 bool bConstruct)
{
  // This function is not recommended for small instances when nOps < 5
  int i, l;

  nXOverPoints = 1;
  xOverLocus[0] = myRandom.boundedIntegerRandom(nOps/5,4*nOps/5);
  DMOSP_OBGene *P1Genes = parent1->Chromosome->getGenes();
  DMOSP_OBGene *P2Genes = parent2->Chromosome->getGenes();

  DMOSP_OBChromosome *C1Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBChromosome *C2Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBGene *C1Genes = C1Chrom->getGenes();
  DMOSP_OBGene *C2Genes = C2Chrom->getGenes();

  // restart crossover actions records for the operations
  for(i=0; i<nOps; i++){
	xOverActions[i].bOpInheritedToChild1 = false;
	xOverActions[i].bOpInheritedToChild2 = false;
  }

  // Inherit left genes from parent 1 to child 1 and from parent 2 to child 2
  DMOSP_Problem::SOperation *op;
  for(l=0; l<=xOverLocus[0]; l++){
	// Child 1 inherits genes from parent 1 up to the crossover locus
	op = P1Genes[l].Operation;
	C1Genes[l].Copy(&P1Genes[l]);
	xOverActions[op->i].bOpInheritedToChild1 = true;
	// Child 2 inherits genes from parent 2 up to the crossover locus
	op = P2Genes[l].Operation;
	C2Genes[l].Copy(&P2Genes[l]);
	xOverActions[op->i].bOpInheritedToChild2 = true;
  }

  // Now inehrit remaining genes from parent 2 to child 1 in their order
  // and genes from parent 1 to child 2 in their order
  int isub1, isub2;   // indexes of substituting genes from both parents
  isub1 = isub2 = 0;  // start at the first gene
  for(; l<nOps; l++){
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 == nOps) break; // This should not happen!
	C1Genes[l].Copy(&P2Genes[isub2]);
	xOverActions[op->i].bOpInheritedToChild1 = true; // not really needed

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 == nOps) break; // This should not happen!
	C2Genes[l].Copy(&P1Genes[isub1]);
	xOverActions[op->i].bOpInheritedToChild2 = true; // not really needed
  }

  parent1->Chromosome->Copy(C1Chrom);
  parent2->Chromosome->Copy(C2Chrom);
  if(bConstruct){
	DecodeOBChromosome(parent1->Chromosome, Solution, parent1->Cmax, parent1->MWFT);
	DecodeOBChromosome(parent2->Chromosome, Solution, parent2->Cmax, parent2->MWFT);
  }

  delete C1Chrom;
  delete C2Chrom;
}

void DMOSP_NSGAII::TwoPointOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2,
							  bool bConstruct)
{
  // This function is not recommended for small instances when nOps < 5
  int i, l;

  nXOverPoints = 2;
  xOverLocus[0] = myRandom.boundedIntegerRandom(1,nOps*4/5-1);
  xOverLocus[1] = myRandom.boundedIntegerRandom(xOverLocus[0]+1,nOps-1);

  DMOSP_OBGene *P1Genes = parent1->Chromosome->getGenes();
  DMOSP_OBGene *P2Genes = parent2->Chromosome->getGenes();

  DMOSP_OBChromosome *C1Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBChromosome *C2Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBGene *C1Genes = C1Chrom->getGenes();
  DMOSP_OBGene *C2Genes = C2Chrom->getGenes();

  // restart crossover actions records for the operations
  for(i=0; i<nOps; i++){
	xOverActions[i].bOpInheritedToChild1 = false;
	xOverActions[i].bOpInheritedToChild2 = false;
  }

  // Inherit genes in left and right sides from parent 1 to child 1 and
  //  from parent 2 to child 2
  DMOSP_Problem::SOperation *op;
  for(l=0; l<=xOverLocus[0]; l++){
	// Child 1 inherits genes from parent 1 up to the crossover locus
	op = P1Genes[l].Operation;
	C1Genes[l].Copy(&P1Genes[l]);
	xOverActions[op->i].bOpInheritedToChild1 = true;
	// Child 2 inherits genes from parent 2 up to the crossover locus
	op = P2Genes[l].Operation;
	C2Genes[l].Copy(&P2Genes[l]);
	xOverActions[op->i].bOpInheritedToChild2 = true;
  }
  for(l=xOverLocus[1]; l<nOps; l++){
	// Child 1 inherits genes from parent 1 up to the crossover locus
	op = P1Genes[l].Operation;
	C1Genes[l].Copy(&P1Genes[l]);
	xOverActions[op->i].bOpInheritedToChild1 = true;
	// Child 2 inherits genes from parent 2 up to the crossover locus
	op = P2Genes[l].Operation;
	C2Genes[l].Copy(&P2Genes[l]);
	xOverActions[op->i].bOpInheritedToChild2 = true;
  }

  // Now inehrit remaining genes in the middle from parent 2 to child 1 and
  // from parent 1 to child 2
  int isub1, isub2;   // indexes of substituting genes from both parents
  isub1 = isub2 = 0;  // start at the first gene
  for(l=xOverLocus[0]+1; l<xOverLocus[1]; l++){
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 == nOps) break; // This should not happen!
	C1Genes[l].Copy(&P2Genes[isub2]);
	xOverActions[op->i].bOpInheritedToChild1 = true; // not really needed

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 == nOps) break; // This should not happen!
	C2Genes[l].Copy(&P1Genes[isub1]);
	xOverActions[op->i].bOpInheritedToChild2 = true; // not really needed
  }

  parent1->Chromosome->Copy(C1Chrom);
  parent2->Chromosome->Copy(C2Chrom);
  if(bConstruct){
	DecodeOBChromosome(parent1->Chromosome, Solution, parent1->Cmax, parent1->MWFT);
	DecodeOBChromosome(parent2->Chromosome, Solution, parent2->Cmax, parent2->MWFT);
  }

  delete C1Chrom;
  delete C2Chrom;
}

void DMOSP_NSGAII::UniformOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2,
							 bool bConstruct)
{
  int i, l;

  nXOverPoints = 0;
  DMOSP_OBGene *P1Genes = parent1->Chromosome->getGenes();
  DMOSP_OBGene *P2Genes = parent2->Chromosome->getGenes();

  DMOSP_OBChromosome *C1Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBChromosome *C2Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBGene *C1Genes = C1Chrom->getGenes();
  DMOSP_OBGene *C2Genes = C2Chrom->getGenes();

  // restart crossover actions records for the operations
  for(i=0; i<nOps; i++){
	xOverActions[i].bOpInheritedToChild1 = false;
	xOverActions[i].bOpInheritedToChild2 = false;
  }

  // determine the crossover locations and inehrit genes from parent 1 to child 1
  // and from parent 2 to child 2 at these locations
  DMOSP_Problem::SOperation *op;
  for(l=0; l<nOps; l++){
	if(myRandom.flip(0.5)) continue;  // it is 50-50 chance to be inehrited
	xOverLocus[nXOverPoints] = l;
	nXOverPoints++;

	// Child 1 inherits genes from parent 1 at each crossover locus
	op = P1Genes[l].Operation;
	C1Genes[l].Copy(&P1Genes[l]);
	xOverActions[op->i].bOpInheritedToChild1 = true;
	// Child 2 inherits genes from parent 2 at each crossover locus
	op = P2Genes[l].Operation;
	C2Genes[l].Copy(&P2Genes[l]);
	xOverActions[op->i].bOpInheritedToChild2 = true;
  }

  int curxoverpnt = 0;
  int isub1, isub2;   // indexes of substituting genes from both parents
  isub1 = isub2 = 0;  // start at the first gene
  for(l=0; l<nOps; l++){
    if(curxoverpnt < nXOverPoints)
	  if(xOverLocus[curxoverpnt] == l){
		curxoverpnt++;
		continue;
	  }

	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 == nOps) break; // This should not happen!
	C1Genes[l].Copy(&P2Genes[isub2]);
	xOverActions[op->i].bOpInheritedToChild1 = true; // not really needed

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 == nOps) break; // This should not happen!
	C2Genes[l].Copy(&P1Genes[isub1]);
	xOverActions[op->i].bOpInheritedToChild2 = true; // not really needed
  }

  parent1->Chromosome->Copy(C1Chrom);
  parent2->Chromosome->Copy(C2Chrom);
  if(bConstruct){
	DecodeOBChromosome(parent1->Chromosome, Solution, parent1->Cmax, parent1->MWFT);
	DecodeOBChromosome(parent2->Chromosome, Solution, parent2->Cmax, parent2->MWFT);
  }

  delete C1Chrom;
  delete C2Chrom;
}

void DMOSP_NSGAII::GPX(DMOSP_Individual *parent1, DMOSP_Individual *parent2,
                       bool bConstruct)
{
  // This function is not recommended for small instances when nOps < 5
  int i, l;

  nXOverPoints = 2;
  xOverLocus[0] = myRandom.boundedIntegerRandom(1,nOps*4/5-1);
  xOverLocus[1] = myRandom.boundedIntegerRandom(xOverLocus[0]+1,nOps-1);

  DMOSP_OBGene *P1Genes = parent1->Chromosome->getGenes();
  DMOSP_OBGene *P2Genes = parent2->Chromosome->getGenes();

  DMOSP_OBChromosome *C1Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBChromosome *C2Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBGene *C1Genes = C1Chrom->getGenes();
  DMOSP_OBGene *C2Genes = C2Chrom->getGenes();

  // restart crossover actions records for the operations
  for(i=0; i<nOps; i++){
	xOverActions[i].bOpInheritedToChild1 = false;
	xOverActions[i].bOpInheritedToChild2 = false;
  }

  // Inherit genes in the middle from parent 1 to child 1 and
  //  from parent 2 to child 2
  DMOSP_Problem::SOperation *op;
  for(l=xOverLocus[0]; l<=xOverLocus[1]; l++){
	// Child 1 inherits genes from parent 1 up to the crossover locus
	op = P1Genes[l].Operation;
	C1Genes[l].Copy(&P1Genes[l]);
	xOverActions[op->i].bOpInheritedToChild1 = true;
	// Child 2 inherits genes from parent 2 up to the crossover locus
	op = P2Genes[l].Operation;
	C2Genes[l].Copy(&P2Genes[l]);
	xOverActions[op->i].bOpInheritedToChild2 = true;
  }

  // Now inehrit remaining genes in the left and right from parent 2 to
  //  child 1 and  from parent 1 to child 2
  int isub1, isub2;   // indexes of substituting genes from both parents
  isub1 = isub2 = 0;  // start at the first gene
  for(l=0; l<xOverLocus[0]; l++){
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 == nOps) break; // This should not happen!
	C1Genes[l].Copy(&P2Genes[isub2]);
	xOverActions[op->i].bOpInheritedToChild1 = true; // not really needed

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 == nOps) break; // This should not happen!
	C2Genes[l].Copy(&P1Genes[isub1]);
	xOverActions[op->i].bOpInheritedToChild2 = true; // not really needed
  }
  for(l=xOverLocus[1]+1; l<nOps; l++){
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 == nOps) break; // This should not happen!
	C1Genes[l].Copy(&P2Genes[isub2]);
	xOverActions[op->i].bOpInheritedToChild1 = true; // not really needed

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 == nOps) break; // This should not happen!
	C2Genes[l].Copy(&P1Genes[isub1]);
	xOverActions[op->i].bOpInheritedToChild2 = true; // not really needed
  }

  parent1->Chromosome->Copy(C1Chrom);
  parent2->Chromosome->Copy(C2Chrom);
  if(bConstruct){
	DecodeOBChromosome(parent1->Chromosome, Solution, parent1->Cmax, parent1->MWFT);
	DecodeOBChromosome(parent2->Chromosome, Solution, parent2->Cmax, parent2->MWFT);
  }

  delete C1Chrom;
  delete C2Chrom;
}

void DMOSP_NSGAII::SB2OX(DMOSP_Individual *parent1, DMOSP_Individual *parent2,
                         bool bConstruct)
{
  // This function is not recommended for small instances when nOps < 5
  int i, l;

  nXOverPoints = 2;
  xOverLocus[0] = myRandom.boundedIntegerRandom(1,nOps*4/5-1);
  xOverLocus[1] = myRandom.boundedIntegerRandom(xOverLocus[0]+1,nOps-1);

  DMOSP_OBGene *P1Genes = parent1->Chromosome->getGenes();
  DMOSP_OBGene *P2Genes = parent2->Chromosome->getGenes();

  DMOSP_OBChromosome *C1Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBChromosome *C2Chrom = new DMOSP_OBChromosome(Problem);
  DMOSP_OBGene *C1Genes = C1Chrom->getGenes();
  DMOSP_OBGene *C2Genes = C2Chrom->getGenes();

  // restart crossover actions records for the operations
  for(i=0; i<nOps; i++){
	xOverActions[i].bOpInheritedToChild1 = false;
	xOverActions[i].bOpInheritedToChild2 = false;
    xOverActions[i].bFilledLocus = false;
  }

  // First, inherit similar blocks from parent 1 to child 1 and from parent 2 to
  // child 2
  DMOSP_Problem::SOperation *op;
  bool bBlockFound = false, bFirstOpInBlockWasFound = false;
  for(i=0; i<nOps; i++){
	if(P1Genes[i].Operation == P2Genes[i].Operation){
	  if(bBlockFound){
		if(bFirstOpInBlockWasFound){
		  op = P1Genes[i-1].Operation;
		  C1Genes[i-1].Copy(&P1Genes[i-1]);
		  xOverActions[op->i].bOpInheritedToChild1 = true;
		  C2Genes[i-1].Copy(&P2Genes[i-1]);
		  xOverActions[op->i].bOpInheritedToChild2 = true;
		  bFirstOpInBlockWasFound = false;
		  xOverActions[i-1].bFilledLocus = true;
		}
		op = P1Genes[i].Operation;
		C1Genes[i].Copy(&P1Genes[i]);
		xOverActions[op->i].bOpInheritedToChild1 = true;
		C2Genes[i].Copy(&P2Genes[i]);
		xOverActions[op->i].bOpInheritedToChild2 = true;
		xOverActions[i].bFilledLocus = true;
	  }
	  else{
        bFirstOpInBlockWasFound = true;
		bBlockFound = true;
	  }
	}
	else bBlockFound = bFirstOpInBlockWasFound = false;
  }

  // Inherit genes in the middle from parent 1 to child 1 and
  //  from parent 2 to child 2
  for(l=xOverLocus[0]; l<=xOverLocus[1]; l++){
	if(xOverActions[l].bFilledLocus) continue;
	// Child 1 inherits genes from parent 1 up to the crossover locus
	op = P1Genes[l].Operation;
	if(!xOverActions[op->i].bOpInheritedToChild1){
	  C1Genes[l].Copy(&P1Genes[l]);
	  xOverActions[op->i].bOpInheritedToChild1 = true;
	}
	// Child 2 inherits genes from parent 2 up to the crossover locus
	op = P2Genes[l].Operation;
	if(!xOverActions[op->i].bOpInheritedToChild2){
	  C2Genes[l].Copy(&P2Genes[l]);
	  xOverActions[op->i].bOpInheritedToChild2 = true;
	}
  }

  // Now inehrit remaining genes in the left and right from parent 2 to
  //  child 1 and  from parent 1 to child 2
  int isub1, isub2;   // indexes of substituting genes from both parents
  isub1 = isub2 = 0;  // start at the first gene
  for(l=0; l<xOverLocus[0]; l++){
	if(xOverActions[l].bFilledLocus) continue;
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 < nOps){
	  C1Genes[l].Copy(&P2Genes[isub2]);
	  xOverActions[op->i].bOpInheritedToChild1 = true;
	}

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 < nOps){
	  C2Genes[l].Copy(&P1Genes[isub1]);
	  xOverActions[op->i].bOpInheritedToChild2 = true;
	}
  }
  for(l=xOverLocus[1]+1; l<nOps; l++){
	if(xOverActions[l].bFilledLocus) continue;
	// Child 1 inherits from parent 2 the remaining operations in their order
	for(; isub2<nOps; isub2++){  // move to next gene in the given order
	  op = P2Genes[isub2].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild1) break;
	}
	if(isub2 < nOps){
	  C1Genes[l].Copy(&P2Genes[isub2]);
	  xOverActions[op->i].bOpInheritedToChild1 = true;
	}

	// Child 2 inherits from parent 1 the remaining operations in their order
	for(; isub1<nOps; isub1++){ // move to the next gene in the given order
	  op = P1Genes[isub1].Operation;
	  if(!xOverActions[op->i].bOpInheritedToChild2) break;
	}
	if(isub1 < nOps){
	  C2Genes[l].Copy(&P1Genes[isub1]);
	  xOverActions[op->i].bOpInheritedToChild2 = true;
	}
  }

  parent1->Chromosome->Copy(C1Chrom);
  parent2->Chromosome->Copy(C2Chrom);
  if(bConstruct){
	DecodeOBChromosome(parent1->Chromosome, Solution, parent1->Cmax, parent1->MWFT);
	DecodeOBChromosome(parent2->Chromosome, Solution, parent2->Cmax, parent2->MWFT);
  }

  delete C1Chrom;
  delete C2Chrom;
}

// -------------------------- Mutation operators ------------------------------

void DMOSP_NSGAII::SwapMutation(DMOSP_Individual *original, bool bConstruct)
{
  mutPoint1 = myRandom.boundedIntegerRandom(1,nOps*4/5-1);
  mutPoint2 = myRandom.boundedIntegerRandom(mutPoint1+1,nOps-1);

  DMOSP_OBGene *oGenes = original->Chromosome->getGenes();

  *tmpGene = oGenes[mutPoint1];
  oGenes[mutPoint1] = oGenes[mutPoint2];
  oGenes[mutPoint2] = *tmpGene;

  if(bConstruct)
	DecodeOBChromosome(original->Chromosome, Solution, original->Cmax, original->MWFT);
}

void DMOSP_NSGAII::ShiftMutation(DMOSP_Individual *original, bool bConstruct)
{
  mutPoint1 = myRandom.boundedIntegerRandom(1,nOps*4/5-1);
  mutPoint2 = myRandom.boundedIntegerRandom(mutPoint1+1,nOps-1);

  DMOSP_OBGene *oGenes = original->Chromosome->getGenes();

  *tmpGene = oGenes[mutPoint2];
  for(int l=mutPoint2; l>mutPoint1; l--){
	oGenes[l] = oGenes[l-1];
  }
  oGenes[mutPoint1] = *tmpGene;

  if(bConstruct)
	DecodeOBChromosome(original->Chromosome, Solution, original->Cmax, original->MWFT);
}

void DMOSP_NSGAII::RLSMutation(DMOSP_Individual *original, bool bConstruct)
{
  if(nCandidateWSs == 0) return;

  DMOSP_OBGene *oGenes = original->Chromosome->getGenes();

  // Choose a candidate workstation randomly
  int w =  myRandom.boundedIntegerRandom(0, nCandidateWSs);
  if(w > nCandidateWSs-1) w = nCandidateWSs - 1;
  w = iCandidateWSs[w];  // Now w is the index of the selected workstation
  
  // Calculate machine loads based on the given chromosome
  int nMCs = W[w]->MCs.Count();
  int m;
  SList::SNode *mcnd = W[w]->MCs.head();
  for(m=0; m<nMCs; m++){
	DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	totalMCLoad[m] = mc->ReadyTime;
	iLoadedMC[m] = mc->i;
	nOpsLoaded[m] = 0;
	mcnd = mcnd->Next();
  }

  for(int l=0; l<nOps; l++){
	if(oGenes[l].Machine->WC->i == w){
	  for(m=0; m<nMCs; m++){
		if(iLoadedMC[m] == oGenes[l].Machine->i) break;
	  }
	  if(m == nMCs) return;   // This should not happen!
	  totalMCLoad[m] += pt[oGenes[l].Operation->Job->i][iLoadedMC[m]];
	  nOpsLoaded[m]++;
	}
  }

  // find the machines with the highest and the lowest loads
  int iHighestMC = -1, iLowestMC = -1;
  double MaxLoad = 0.0, MinLoad = MAXFLOAT;
  for(m=0; m<nMCs; m++){
	if(nOpsLoaded[m] > 0 && (totalMCLoad[m] > MaxLoad || 
	     (totalMCLoad[m] == MaxLoad && myRandom.flip(0.5))) ){
	  iHighestMC = m;
	  MaxLoad = totalMCLoad[m];
	}
	if(totalMCLoad[m] < MinLoad || (totalMCLoad[m] == MinLoad && myRandom.flip(0.5))){
	  iLowestMC = m;
	  MinLoad = totalMCLoad[m];
	}
  }
  if(iHighestMC == -1 || iLowestMC == -1 || iHighestMC == iLowestMC) return;

  // Select an operation randomly from the machine with the highest load
  int selOp = myRandom.boundedIntegerRandom(0, nOpsLoaded[iHighestMC]);
  if(selOp > nOpsLoaded[iHighestMC] - 1) selOp = nOpsLoaded[iHighestMC] - 1;

  // Change its assigned machine to the lowest loaded machine
  for(int l=0; l<nOps; l++){
	if(iLoadedMC[iHighestMC] == oGenes[l].Machine->i){
	  if(selOp == 0){
		oGenes[l].Machine = M[iLoadedMC[iLowestMC]];
		break;
	  }
	  selOp--;
	}
  }

  mutPoint1 = iLoadedMC[iHighestMC];        // from MC
  mutPoint2 = iLoadedMC[iLowestMC];         // to MC

  if(bConstruct)
	DecodeOBChromosome(original->Chromosome, Solution, original->Cmax, original->MWFT);
}

//--------------------- Simulated Annealing local search --------------------
void DMOSP_NSGAII::setSAParameters(int nsaitr, double saitemp, double sacoolrt)
{
  maxSAIterations = nsaitr;
  initSATemp = curSATemp = saitemp;
  SACoolingRate = sacoolrt;
}

void DMOSP_NSGAII::InitializeSA(DMOSP_Individual *ind)
{
  SAIter = 0;
  curSATemp = initSATemp;
  bestSAInd->Copy(ind);

  if(frmNSGA2CalcProgress->bActive && frmNSGA2CalcProgress->cbxSA->Checked){
	String s = FloatToStr(ind->Cmax);
	frmNSGA2CalcProgress->lblSAInitialCmax->Caption = s;
	frmNSGA2CalcProgress->lblSACurrentCmax->Caption = s;
	frmNSGA2CalcProgress->lblSABestCmax->Caption = s;
	s = FloatToStr(ind->MWFT);
	frmNSGA2CalcProgress->lblSAInitialMWFT->Caption = s;
	frmNSGA2CalcProgress->lblSACurrentMWFT->Caption = s;
	frmNSGA2CalcProgress->lblSABestMWFT->Caption = s;
	frmNSGA2CalcProgress->lblSAIter->Caption = "0";
	frmNSGA2CalcProgress->lblSATemperature->Caption = FloatToStr(initSATemp);
	frmNSGA2CalcProgress->pbSA->Position = 0;
	frmNSGA2CalcProgress->pbSA->Max = maxSAIterations;
  }
}

void DMOSP_NSGAII::RunSA(DMOSP_Individual *ind)
{
  InitializeSA(ind);
  while(!StepSA(ind)){
	if(frmEvolOperators->bActive){
	  String s;
	  frmEvolOperators->pbSAIterations->Position = SAIter;
	  s = FloatToStr(ind->Cmax);
	  frmEvolOperators->lblCmax->Caption = s;
	  s = FloatToStr(ind->MWFT);
	  frmEvolOperators->lblMWFT->Caption = s;
	  s = FloatToStr(curSATemp);
	  frmEvolOperators->lblTemperature->Caption = s;
	  frmNSGA2CalcProgress->pbSA->Position = SAIter;
	}
	if(frmNSGA2CalcProgress->bActive && frmNSGA2CalcProgress->cbxSA->Checked){
	  String s;
	  frmNSGA2CalcProgress->pbSA->Position = SAIter;
	  s = FloatToStr(SAIter);
	  frmNSGA2CalcProgress->lblSAIter->Caption = s;
	  s = FloatToStr(curSATemp);
	  frmNSGA2CalcProgress->lblSATemperature->Caption = s;
	  s = FloatToStr(ind->Cmax);
	  frmNSGA2CalcProgress->lblSACurrentCmax->Caption = s;
	  s = FloatToStr(ind->MWFT);
	  frmNSGA2CalcProgress->lblSACurrentMWFT->Caption = s;
	  s = FloatToStr(bestSAInd->Cmax);
	  frmNSGA2CalcProgress->lblSABestCmax->Caption = s;
	  s = FloatToStr(bestSAInd->MWFT);
	  frmNSGA2CalcProgress->lblSABestMWFT->Caption = s;
	}

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;;
	}
  }
}

bool DMOSP_NSGAII::StepSA(DMOSP_Individual *ind)
{
  DMOSP_Individual *newInd = new DMOSP_Individual(ind);

  switch(mutOper){
	 case SHIFT_MUTATION: ShiftMutation(newInd);
						  break;
	 case SWAP_MUTATION:  SwapMutation(newInd);
						  break;
  }
  noOfGlobalEvals++;
  if(myRandom.flip(RLBmutProbability)){
	RLSMutation(newInd);
	noOfGlobalEvals++;
  }

  if(newInd->Cmax <= bestSAInd->Cmax && newInd->MWFT <= bestSAInd->MWFT){
	bestSAInd->Copy(newInd);
  }

  if(newInd->Cmax <= ind->Cmax && newInd->MWFT <= ind->MWFT){
	ind->Copy(newInd);
  }
  else {
	// Measure the percentage deviations for both objectives
	double delta_Cmax = 100.0 * ((newInd->Cmax - ind->Cmax)/ind->Cmax);
	double delta_MWFT = 100.0 * ((newInd->MWFT - ind->MWFT)/ind->MWFT);
	double delta = delta_Cmax + delta_MWFT;
	if(delta <= 0.0){
	  if(myRandom.flip(0.5)) ind->Copy(newInd);
	}
	else{
	  double prob = exp(-delta/curSATemp);
	  if(myRandom.flip(prob)) ind->Copy(newInd);
	}
  }
  delete newInd;

  curSATemp *= SACoolingRate;
  SAIter++;
  if(SAIter <= maxSAIterations) return false;

  ind->Copy(bestSAInd);
  return true;   // terminate!
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
