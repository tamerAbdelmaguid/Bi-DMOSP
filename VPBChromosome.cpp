//---------------------------------------------------------------------------


#pragma hdrstop

#include "VPBChromosome.h"

//--------------------------------------------------- Sub-string data structure

DMOSP_VPBSubString::DMOSP_VPBSubString(int s, int e, int n)
{
  // It must be s < e
  start = s;
  end = e;
  if(n <= 0){
	nSeparators = 0;
	SeparatorPos = NULL;
  }
  else{
	nSeparators = n;
	SeparatorPos = new int[nSeparators];
  }
}

DMOSP_VPBSubString::DMOSP_VPBSubString()
{
  nSeparators = 0;
  start = end = -1;
  SeparatorPos = NULL;
}

DMOSP_VPBSubString::~DMOSP_VPBSubString()
{
  if(SeparatorPos) delete [] SeparatorPos;
}

void DMOSP_VPBSubString::Copy(DMOSP_VPBSubString *substr)
{
  start = substr->start;
  end = substr->end;
  if(substr->nSeparators != 0){
	if(nSeparators != substr->nSeparators){
	  delete [] SeparatorPos;
	  nSeparators = substr->nSeparators;
	  SeparatorPos = new int[nSeparators];
	}
	memcpy((void *)SeparatorPos, (const void *)substr->SeparatorPos, nSeparators * sizeof(int));
  }
  else{
	if(nSeparators != 0){
	  nSeparators = 0;
	  delete [] SeparatorPos;
	  SeparatorPos = NULL;
	}
  }
}

void DMOSP_VPBSubString::Reset()
{
  if(nSeparators != 0){
	memset((void *)SeparatorPos, -1, nSeparators * sizeof(int));
  }
}

void DMOSP_VPBSubString::SetRange(int s, int e)
{
  start = s;
  end = e;
}

void DMOSP_VPBSubString::SetnSeparators(int ns)
{
  if(SeparatorPos) delete SeparatorPos;
  if(ns > 0){
	nSeparators = ns;
	SeparatorPos = new int[ns];
  }
  else{
	nSeparators = 0;
	SeparatorPos = NULL;
  }
}

bool DMOSP_VPBSubString::operator == (DMOSP_VPBSubString &ss)
{
  if(start != ss.start || end != ss.end || nSeparators != ss.nSeparators)
    return false;
  if(memcmp((const void *)SeparatorPos, (const void *)ss.SeparatorPos,sizeof(int)*nSeparators != 0))
	return false;
  return true;
}

//------------------------------------------------------- String data structure

DMOSP_VPBString::DMOSP_VPBString(int l, int ns)
{
  length = l;
  Gene = new DMOSP_Problem::SOperation *[length];
  nSubstrings = ns;
  Substring = new DMOSP_VPBSubString[nSubstrings];
}

DMOSP_VPBString::~DMOSP_VPBString()
{
  delete [] Substring;
  delete [] Gene;
}

bool DMOSP_VPBString::Copy(DMOSP_VPBString *str)
{
  if(length != str->length || nSubstrings != str->nSubstrings) return false;

  memcpy((void *)Gene, (const void *)str->Gene, length * sizeof(DMOSP_Problem::SOperation *));
  for(int s=0; s<nSubstrings; s++) Substring[s].Copy(&(str->Substring[s]));

  return true;
}

void DMOSP_VPBString::Reset()
{
  memset((void *)Gene, 0, length * sizeof(DMOSP_Problem::SOperation *));
  for(int s=0; s<nSubstrings; s++) Substring[s].Reset();
}

bool DMOSP_VPBString::operator == (DMOSP_VPBString &s)
{
  if(length != s.length) return false;
  if(memcmp((const void *)Gene, (const void *)s.Gene, length * sizeof(DMOSP_Problem::SOperation *)) != 0)
	return false;
  for(int ss=0; ss<nSubstrings; ss++) if(Substring[ss] != s.Substring[ss]) return false;
  return true;
}

//---- Decoder algorithm for the vectors of permutations-based (VPB) chromosome

DMOSP_VPBChromosome::Decoder::Decoder(DMOSP_VPBChromosome *chrom)
  : DMOSP_Algorithm(chrom->getProblem(), L"Decoder for the VPB chromosome"),
	Chromosome(chrom)
{
}

DMOSP_VPBChromosome::Decoder::~Decoder()
{
}

void DMOSP_VPBChromosome::Decoder::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();

  jbList = new SList[nJobs];
  mcList = new SList[nMCs];

  nxt = new int[nJobs];
  mcrt = new double[nMCs];
  jbrt = new double[nJobs];
}

void DMOSP_VPBChromosome::Decoder::Initialize()
{
  // This procedure interprets the given strings into a schedule.
  // Note: the given VPB chromosome may result in an infeasible solution

  // The decoding procedure converts the given strings into vectors of
  // processing sequences on jobs and machines, then it interprets these vectors
  // into a schedule after checking the feasibility conditions.
  //

  // First: convert the chromosomes into lists (vectors) of operations for
  // the jobs and the machines
  //
  int i, j;
  for(j=0; j<Chromosome->jbString->nSubstrings; j++){
	DMOSP_VPBSubString& jSubString = Chromosome->jbString->Substring[j];
	for(i = jSubString.start; i <= jSubString.end; i++){
	  DMOSP_Problem::SOperation *op = Chromosome->jbString->Gene[i];
	  jbList[j].Add((void *)op);
	}
  }
  int w, m;
  for(w=0, m=0; w<Chromosome->wsString->nSubstrings; w++){
	DMOSP_VPBSubString& wSubString = Chromosome->wsString->Substring[w];
	i = wSubString.start;
	int remMachines = wSubString.nSeparators + 1;
	for(int s=0; s<wSubString.nSeparators; s++){
	  while(i < wSubString.SeparatorPos[s] && i <= wSubString.end){
		DMOSP_Problem::SOperation *op = Chromosome->wsString->Gene[i];
		mcList[m].Add((void *)op);
		i++;
	  }
	  remMachines--;
	  m++;
	}
	// Add remaining operations in the sub-string to the last machine
	if(i <= wSubString.end){
	  while(i <= wSubString.end){
		DMOSP_Problem::SOperation *op = Chromosome->wsString->Gene[i];
		mcList[m].Add((void *)op);
		i++;
	  }
	  remMachines--;
	  m++;
	}
	while(remMachines > 0){
	  remMachines--;
	  m++;
	}
  }

  // Second: Convert the generated vectors into a schedule and check
  // feasibility during the construction procedure
  //
  SList::SNode *wcnd, *mcnd, *jbnd, *opnd;

  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
	opnd = jbList[j].head();
	if(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  nxt[j] = op->i;   // index of the next schedulable operation based on OOs
	}
	else nxt[j] = -1;
  }

  bool bContinue;
  do{
	bContinue = false;
	for(m=0; m<nMCs; m++){
	  if(mcList[m].Count() != 0){
		DMOSP_Problem::SOperation *op =
		        (DMOSP_Problem::SOperation *)(mcList[m].head()->Data());
		j = op->Job->i;  // job index
		i = op->i; // operation index
		if(nxt[j] == i){
		  DMOSP_Solution::SchTask *task = tsk[i];
		  task->SelectedMC = M[m];
		  task->StartTime = max(jbrt[j], mcrt[m]);
		  task->EndTime = task->StartTime + pt[j][m];
		  task->bScheduled = true;
		  jbrt[j] = task->EndTime;
		  mcrt[m] = task->EndTime;

		  jbList[j].Remove(jbList[j].head());
		  opnd = jbList[j].head();
		  if(opnd){
			DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
			nxt[j] = op->i;   // index of the next schedulable operation based on OOs
		  }
		  else nxt[j] = -1;

		  mcList[m].Remove(mcList[m].head());
		  bContinue = true;
		  break;
		}
	  }
	}
  }while(bContinue);

  bool bComplete = true;          // check for infeasible processing sequence
  for(m=0; m<nMCs; m++){
	if(mcList[m].Count() != 0){
	  bComplete = false;
	  break;
	}
  }
  if(!bComplete) SolStatus = INFEASIBLE;
  else SolStatus = FEASIBLE;
}

bool DMOSP_VPBChromosome::Decoder::Stop()
{
  return true;
}

void DMOSP_VPBChromosome::Decoder::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_VPBChromosome::Decoder::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  for(int j=0; j<nJobs; j++) jbList[j].Clear();
  for(int m=0; m<nMCs; m++) mcList[m].Clear();

  delete [] jbList;
  delete [] mcList;

  delete [] nxt;
  delete [] mcrt;
  delete [] jbrt;
}

void DMOSP_VPBChromosome::Decoder::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//--------------------------------------------------- VPB chromosome operations

DMOSP_VPBChromosome::DMOSP_VPBChromosome(DMOSP_Problem *Prob)
{
  SList::SNode *nd;
  int i, s, e;

  Problem = Prob;
  int length = Problem->nOperations();

  // Define the start and end positions for the jobs' substrings
  jbString = new DMOSP_VPBString(length, Problem->Jobs.Count());
  nd = Problem->Jobs.head();
  i = s = 0;
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	e = s + jb->Operations.Count() - 1;
	jbString->Substring[i].start = s;
	jbString->Substring[i].end = e;
	s = e + 1;
	i++;
	nd = nd->Next();
  }

  // Define the start and end positions for the workstations's substrings
  wsString = new DMOSP_VPBString(length, Problem->WCs.Count());
  nd = Problem->WCs.head();
  i = s = 0;
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	e = s + Problem->nOperationsByWC(wc) - 1;
	wsString->Substring[i].start = s;
	wsString->Substring[i].end = e;
	wsString->Substring[i].SetnSeparators(wc->MCs.Count() - 1);
	s = e + 1;
	i++;
	nd = nd->Next();
  }

  bConstructed = false;

  decoder = NULL;
}

DMOSP_VPBChromosome::DMOSP_VPBChromosome(DMOSP_VPBChromosome *chrom)
{
  Problem = chrom->Problem;
  int length = Problem->nOperations();
  jbString = new DMOSP_VPBString(length, Problem->Jobs.Count());
  wsString = new DMOSP_VPBString(length, Problem->WCs.Count());
  bConstructed = false;
  Copy(chrom);

  decoder = NULL;
}

DMOSP_VPBChromosome::~DMOSP_VPBChromosome()
{
  delete jbString;
  delete wsString;
  if(decoder) delete decoder;
}

bool DMOSP_VPBChromosome::Copy(DMOSP_VPBChromosome *chrom)
{
  if(Problem != chrom->Problem) return false;

  jbString->Copy(chrom->jbString);
  wsString->Copy(chrom->wsString);

  bConstructed = chrom->bConstructed;
  return true;
}

bool DMOSP_VPBChromosome::Encode(DMOSP_Solution *Sol)
{
  // Convert a given solution into the VPB chromosome representation

  // if different problem, return false
  if(Sol->Problem != Problem) return false;

  int *jpos = new int[Problem->Jobs.Count()];
  int j = 0;
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	jpos[j] = jbString->Substring[j].start;
	jnd = jnd->Next();
	++j;
  }

  int *wpos = new int[Problem->WCs.Count()];
  int w = 0;
  SList::SNode *wnd = Problem->WCs.head();
  while(wnd){
	wpos[w] = wsString->Substring[w].start;
    wnd = wnd->Next();
	++w;
  }

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);
  DMOSP_Solution::SchTask **tskGene = new DMOSP_Solution::SchTask *[wsString->length];

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL){   // just make sure that it is scheduled
	  j = Problem->Jobs.IndexOf(tsk->Operation->Job);
	  jbString->Gene[jpos[j]] = tsk->Operation;
	  jpos[j]++;

	  w = Problem->WCs.IndexOf(tsk->SelectedMC->WC);
	  tskGene[wpos[w]] = tsk;
	  wpos[w]++;
	}
	tsknd = tsknd->Next();
  }

  // re-arrange genes in workstations substrings and identify machine separators
  // positions
  wnd = Problem->WCs.head();
  for(w=0; w < wsString->nSubstrings; w++){
    DMOSP_Problem::SWC *WS = (DMOSP_Problem::SWC *)wnd->Data();

	SList *MCPermutations = new SList[wsString->Substring[w].nSeparators + 1];
	for(int p = wsString->Substring[w].start; p <= wsString->Substring[w].end; p++){
	  int m = WS->MCs.IndexOf(tskGene[p]->SelectedMC);
	  MCPermutations[m].Add(tskGene[p]->Operation);
	}

	// Now define the separators' positions and assign the gene values
	wpos[w] = wsString->Substring[w].start;
	for(int s = 0; s <= wsString->Substring[w].nSeparators; s++){
	  SList::SNode *opnd = MCPermutations[s].head();
	  while(opnd){
		DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
		wsString->Gene[wpos[w]] = op;
		wpos[w]++;
		opnd = opnd->Next();
	  }
	  if(s != wsString->Substring[w].nSeparators)
		wsString->Substring[w].SeparatorPos[s] = wpos[w];
	}

	delete [] MCPermutations;
	wnd = wnd->Next();
  }

  delete [] jpos;
  delete [] wpos;
  delete [] tskGene;

  // If the coding is successful
  bConstructed = true;
  return true;
}

bool DMOSP_VPBChromosome::Decode(DMOSP_Solution *Sol)
{
  // Convert the current VPB chromosome representation to a schedule
  // in the form of list of tasks

  // if different problem, return false
  if(Sol->Problem != Problem) return false;
  if(!bConstructed) return false;

  if(!decoder) decoder = new Decoder(this);
  decoder->Run();

  // Note that the solution will always be feasible
  if(decoder->SolStatus == DMOSP_Algorithm::FEASIBLE){
	Sol->Copy(decoder->Solution);
	return true;
  }
  else {
	return false;
  }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
