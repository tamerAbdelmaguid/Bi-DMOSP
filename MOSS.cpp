//---------------------------------------------------------------------------


#pragma hdrstop

#include "MOSS.h"
#include <math.h>
#include "SList.h"
#include "Main.h"
#include "random.h"
#include "FormMOSSCalcProgress.h"
#include "FormMOSSOperators.h"
#include "FormTuningMOSS.h"
#include "FormTuningMOSS2.h"

#ifdef PRNT_MOSS_DBG
FILE *mossDbgFile;
#endif

int DMOSPMOSSSolCompCmax(void * Item1, void * Item2)
{
  DMOSP_MOSS_Solution *Sol1, *Sol2;
  Sol1 = (DMOSP_MOSS_Solution *)Item1;
  Sol2 = (DMOSP_MOSS_Solution *)Item2;
  double CmaxDiff = Sol1->CmaxValue() - Sol2->CmaxValue();
  int ret = (CmaxDiff < 0)? -1 : 1;
  return ret;
}

int DMOSPMOSSSolCompMWFT(void * Item1, void * Item2)
{
  DMOSP_MOSS_Solution *Sol1, *Sol2;
  Sol1 = (DMOSP_MOSS_Solution *)Item1;
  Sol2 = (DMOSP_MOSS_Solution *)Item2;
  double MWFTDiff = Sol1->MWFTValue() - Sol2->MWFTValue();
  int ret = (MWFTDiff < 0)? -1 : 1;
  return ret;
}

int DMOSPMOSSSolCompDomin(void * Item1, void * Item2)
{
  DMOSP_MOSS_Solution *Sol1, *Sol2;
  Sol1 = (DMOSP_MOSS_Solution *)Item1;
  Sol2 = (DMOSP_MOSS_Solution *)Item2;
  double CmaxDiff = Sol1->CmaxValue() - Sol2->CmaxValue();
  double MWFTDiff = Sol1->MWFTValue() - Sol2->MWFTValue();
  int ret;
  if((CmaxDiff < 0 && MWFTDiff <= 0) || (CmaxDiff <= 0 && MWFTDiff < 0))
	 ret = CmaxDiff + MWFTDiff;
  else if((CmaxDiff > 0 && MWFTDiff >= 0) || (CmaxDiff >= 0 && MWFTDiff > 0))
	 ret = CmaxDiff + MWFTDiff;
  else
	 ret = 0;
  return ret;
}

//---------------------------------------------------------------------------

DMOSP_MOSS_Solution::DMOSP_MOSS_Solution(DMOSP_MOSS *alg)
{
  AllocateMemory(alg);
}

DMOSP_MOSS_Solution::DMOSP_MOSS_Solution(DMOSP_MOSS_Solution &Sol)
{
  AllocateMemory(Sol.moss);
  *this = Sol;
}

void DMOSP_MOSS_Solution::AllocateMemory(DMOSP_MOSS *alg)
{
  moss = alg;
  int nJobs = moss->nJobs;
  int nWCs = moss->nWCs;
  int nMCs = moss->nMCs;
  int nOps = moss->nOps;

  // Allocate memory for VPB chromosome data
  SList::SNode *nd;
  int i, s, e;

  // Define the start and end positions for the jobs' substrings
  jbString = new DMOSP_VPBString(nOps, nJobs);
  nd = moss->GetProblem()->Jobs.head();
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
  wsString = new DMOSP_VPBString(nOps, nWCs);
  nd = moss->GetProblem()->WCs.head();
  i = s = 0;
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	e = s + moss->GetProblem()->nOperationsByWC(wc) - 1;
	wsString->Substring[i].start = s;
	wsString->Substring[i].end = e;
	wsString->Substring[i].SetnSeparators(wc->MCs.Count() - 1);
	s = e + 1;
	i++;
	nd = nd->Next();
  }

  // Allocate memory for variables used in converting tasks list into VPBChromosome strings
  jpos = new int[nJobs];
  wpos = new int[nWCs];

  // Allocate all required memory for storing vector and network data
  tau = new int *[nJobs];
  for(int j=0; j<nJobs; j++){
	tau[j] = new int [nWCs+1];
  }

  pi = new int *[nMCs+1];
  for(int m=0; m<nMCs; m++){
	pi[m] = new int [nJobs+1];
  }

  jb = new int [nOps];
  mc = new int [nOps];
  tau_pos = new int [nOps];
  pi_pos = new int [nOps];

  Nodes = new DMOSP_MOSS_Node[nOps];
  FirstjNode = new int[nJobs];
  LastjNode = new int[nJobs];
  FirstmNode = new int[nMCs];
  LastmNode = new int[nMCs];

  MCrt = new double[nMCs];    // machine's ready time
  mnxt = new int[nMCs];       // next node index for the machine
  Jnft = new double[nJobs];   // job's nearest finish time
  jnxt = new int[nJobs];      // next node index for the job

  jcur = new int[nJobs];
  mcur = new int[nMCs];

  etgmx_1 = new double [nWCs+1];
  etgmx_2 = new double [nJobs+1];

  Reset();
}

DMOSP_MOSS_Solution::~DMOSP_MOSS_Solution()
{
  FreeMemory();
}

void DMOSP_MOSS_Solution::FreeMemory()
{
  delete jbString;
  delete wsString;

  delete [] jpos;
  delete [] wpos;

  for(int j=0; j<moss->nJobs; j++){
	delete [] tau[j];
  }
  delete [] tau;

  for(int m=0; m<moss->nMCs; m++){
	delete [] pi[m];
  }
  delete [] pi;

  delete [] jb;
  delete [] mc;
  delete [] tau_pos;
  delete [] pi_pos;

  delete [] Nodes;
  delete [] FirstjNode;
  delete [] LastjNode;
  delete [] FirstmNode;
  delete [] LastmNode;

  delete [] MCrt;    // machine's ready time
  delete [] mnxt;    // next node index for the machine
  delete [] Jnft;    // job's nearest finish time
  delete [] jnxt;    // next node index for the job

  delete [] jcur;
  delete [] mcur;

  delete [] etgmx_1;
  delete [] etgmx_2;
}

void DMOSP_MOSS_Solution::Reset()
{
  int nJobs = moss->nJobs;
  int nWCs = moss->nWCs;
  int nMCs = moss->nMCs;
  int nOps = moss->nOps;

  // reset all values so that no solution exists
  jbString->Reset();
  wsString->Reset();
  bVPBConstructed = false;

  for(int j=0; j<nJobs; j++){
	memset((void *)tau[j], -1, sizeof(int) * (nWCs+1));
  }

  for(int m=0; m < nMCs; m++){
	memset((void *)pi[m], -1, sizeof(int) * (nJobs+1));
  }

  memset((void *)jb, -1, sizeof(int) * nOps);
  memset((void *)mc, -1, sizeof(int) * nOps);
  memset((void *)tau_pos, -1, sizeof(int) * nOps);
  memset((void *)pi_pos, -1, sizeof(int) * nOps);
  bVectorsConstructed = false;

  memset(FirstjNode, -1, nJobs*sizeof(int));
  memset(LastjNode, -1, nJobs*sizeof(int));
  memset(FirstmNode, -1, nMCs*sizeof(int));
  memset(LastmNode, -1, nMCs*sizeof(int));

  Cmax = -1;   // Just to notify that the network solution has not been
			   // constructed yet

  for(int i=0; i<nOps; i++) Nodes[i].Reset();
}

void DMOSP_MOSS_Solution::Copy(DMOSP_MOSS_Solution *S)
{
  if(S->moss != moss){
	FreeMemory();
	AllocateMemory(S->moss);
  }

  jbString->Copy(S->jbString);
  wsString->Copy(S->wsString);
  bVPBConstructed = S->bVPBConstructed;

  int nJobs = moss->nJobs;
  int nWCs = moss->nWCs;
  int nMCs = moss->nMCs;
  int nOps = moss->nOps;

  for(int j=0; j<nJobs; j++){
	memcpy((void *)tau[j], (const void *)S->tau[j], sizeof(int) * (nWCs+1));
  }

  for(int m=0; m < nMCs; m++){
	memcpy((void *)pi[m], (const void *)S->pi[m], sizeof(int) * (nJobs+1));
  }

  memcpy((void *)jb, (const void *)S->jb, sizeof(int) * nOps);
  memcpy((void *)mc, (const void *)S->mc, sizeof(int) * nOps);
  memcpy((void *)tau_pos, (const void *)S->tau_pos, sizeof(int) * nOps);
  memcpy((void *)pi_pos, (const void *)S->pi_pos, sizeof(int) * nOps);
  bVectorsConstructed = S->bVectorsConstructed;

  memcpy((void *)Nodes, (const void *)S->Nodes, nOps * sizeof(DMOSP_MOSS_Node));

  memcpy((void *)FirstjNode, (const void *)S->FirstjNode, nJobs*sizeof(int));
  memcpy((void *)LastjNode, (const void *)S->LastjNode, nJobs*sizeof(int));
  memcpy((void *)FirstmNode, (const void *)S->FirstmNode, nMCs*sizeof(int));
  memcpy((void *)LastmNode, (const void *)S->LastmNode, nMCs*sizeof(int));

  Cmax = S->Cmax;
  MWFT = S->MWFT;
}

bool operator == (DMOSP_MOSS_Solution &S1, DMOSP_MOSS_Solution &S2)
{
  // Compare the VPB chromosome strings
  if(S1.bVPBConstructed & S2.bVPBConstructed){
	if(*S1.jbString != *S2.jbString || *S1.wsString != *S2.wsString) return false;
  }
  else { // Compare the vectors of both solutions
	// First, compare job and machine assignments
	if(memcmp((const void *)S1.jb, (const void *)S2.jb, sizeof(int)*S1.moss->nOps)
		!= 0) return false;
	if(memcmp((const void *)S1.mc, (const void *)S2.mc, sizeof(int)*S1.moss->nOps)
		!= 0) return false;

	// Second, check taus and pis
	for(int j=0; j<S1.moss->nJobs; j++){
	  if(S1.tau[j][0] != S2.tau[j][0]) return false;
	  if(memcmp((const void *)S1.tau[j], (const void *)S2.tau[j], sizeof(int)*(S1.tau[j][0]+1))
		  != 0) return false;
	}
	for(int m=0; m<S1.moss->nMCs; m++){
	  if(S1.pi[m][0] != S2.pi[m][0]) return false;
	  if(memcmp((const void *)S1.pi[m], (const void *)S2.pi[m], sizeof(int)*(S1.pi[m][0]+1))
		  != 0) return false;
	}
  }
  return true;
}

void DMOSP_MOSS_Solution::InterpretTaskList()
{
  // This function takes the solution stored in the moss object as a list of
  // scheduled tasks and interprets it to its equivalent VBP chromosome
  // and vectors of taus and pis

  Reset();

  int j, w, m, o;

  // define starting positions for jobs' strings
  for(j=0; j<moss->nJobs; j++){
	jpos[j] = jbString->Substring[j].start;
	tau[j][0] = 0;  // start with empty taus
  }

  // start with empty pis
  for(m=0; m < moss->nMCs; m++) pi[m][0] = 0;

  SList &Tasks = moss->Solution->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  // Now construct the vectors of taus and pis and assign genes to jobs' strings
  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	j = tsk->Operation->Job->i;
	m = tsk->SelectedMC->i;
	o = tsk->Operation->i;

	tau_pos[o] = ++tau[j][0];
	pi_pos[o] = ++pi[m][0];

	tau[j][tau_pos[o]] = o;
	pi[m][pi_pos[o]] = o;

	jb[o] = j;
	mc[o] = m;

	jbString->Gene[jpos[j]] = tsk->Operation;
	jpos[j]++;
  }

  // assign genes in workstations substrings and identify machine separators
  // positions
  for(w=0; w < wsString->nSubstrings; w++){
	// define starting positions for workstations' strings
	wpos[w] = wsString->Substring[w].start;
	m = moss->Wa[w];
	for(int s = 0; s <= wsString->Substring[w].nSeparators; s++){
	  for(int o=1; o <= pi[m][0]; o++){
		DMOSP_Problem::SOperation *op = moss->O[pi[m][o]];
		wsString->Gene[wpos[w]] = op;
		wpos[w]++;
	  }
	  if(s != wsString->Substring[w].nSeparators)
		wsString->Substring[w].SeparatorPos[s] = wpos[w];
	  m++;
	}
  }

  bVPBConstructed = true;
  bVectorsConstructed = true;
}

void DMOSP_MOSS_Solution::GenerateVectors()
{
  if(!bVPBConstructed){
	bVectorsConstructed = false;
	return;
  }

  // Generate pi and tau vectors based on the stored VPB chromosome
  int i, j, w, m, s, o;

  // Start with the tau vectors
  for(j=0; j<moss->nJobs; j++){
	tau[j][0] = 0;  // start with empty taus

	DMOSP_VPBSubString &jstr = jbString->Substring[j];
	for(i = jstr.start; i <= jstr.end; i++){
	  DMOSP_Problem::SOperation *op = jbString->Gene[i];
	  o = op->i;
	  tau_pos[o] = ++tau[j][0];
	  tau[j][tau_pos[o]] = o;
	  jb[o] = j;
	}
  }

  // Second, construct the pi vectors
  for(w=0; w<moss->nWCs; w++){
	DMOSP_VPBSubString &wstr = wsString->Substring[w];
    i = wstr.start;
	m = moss->Wa[w];
	for(s=0; s<=wstr.nSeparators; s++){
	  pi[m][0] = 0;

	  if(s == wstr.nSeparators){ // a single-machine workstation or the last machine
		while(i <= wstr.end){
		  DMOSP_Problem::SOperation *op = wsString->Gene[i];
		  o = op->i;
		  pi_pos[o] = ++pi[m][0];
		  pi[m][pi_pos[o]] = o;
		  mc[o] = m;
		  i++;
		}
	  }
	  else{
		while(i < wstr.SeparatorPos[s]){
		  DMOSP_Problem::SOperation *op = wsString->Gene[i];
		  o = op->i;
		  pi_pos[o] = ++pi[m][0];
		  pi[m][pi_pos[o]] = o;
		  mc[o] = m;
		  i++;
		};
		++m;
		if(i > wstr.end){  // the remaining machines are not assigned operations
		  while(s < wstr.nSeparators){
			 pi[m][0] = 0;
			 ++m;
			 ++s;
		  }
          break;
		}
	  }
	}
  }

  bVectorsConstructed = true;
}

void DMOSP_MOSS_Solution::GenerateVPBStrings()
{
  if(!bVectorsConstructed){
	bVPBConstructed = false;
	return;
  }

  // Generate VPB chromosome strings based on the stored pi and tau vectors
  int i, j, w, m, s, o;

  // Start with the jbStrings using the tau vectors
  for(j=0; j<moss->nJobs; j++){
	DMOSP_VPBSubString &jstr = jbString->Substring[j];
	for(i = 1; i <= tau[j][0]; i++){
	  o = tau[j][i];
	  jbString->Gene[jstr.start+i-1] = moss->O[o];
	}
  }

  // Second, construct wsStrings using the pi vectors
  // assign genes in workstations substrings and identify machine separators
  // positions
  for(w=0; w < wsString->nSubstrings; w++){
    // define starting positions for workstations' strings
	wpos[w] = wsString->Substring[w].start;
	m = moss->Wa[w];
	for(int s = 0; s <= wsString->Substring[w].nSeparators; s++){
	  for(int o=1; o <= pi[m][0]; o++){
		DMOSP_Problem::SOperation *op = moss->O[pi[m][o]];
		wsString->Gene[wpos[w]] = op;
		wpos[w]++;
	  }
	  if(s != wsString->Substring[w].nSeparators)
		wsString->Substring[w].SeparatorPos[s] = wpos[w];
	  m++;
	}
  }

  bVPBConstructed = true;
}

void DMOSP_MOSS_Solution::GenerateNetwork()
{
  // interpret vectors into nodes and arcs only and assign node weights
  int i, k, j, m, l;

  int nJobs = moss->nJobs;
  int nMCs = moss->nMCs;

  for(j=0; j<nJobs; j++){
	l = tau[j][0];
	if(l > 0){
	  i = tau[j][1];
	  FirstjNode[j] = i;
	  Nodes[i].j_prv = -1;
	  for(k = 2; k <= l; k++){
		m = tau[j][k];
		Nodes[i].j_nxt = m;
		Nodes[m].j_prv = i;
		i = m;
	  }
	  Nodes[i].j_nxt = -1;
	  LastjNode[j] = i;
	}
	else {
	  FirstjNode[j] = -1;
	  LastjNode[j] = -1;
	}
  }

  for(m=0; m<nMCs; m++){
	l = pi[m][0];
	if(l > 0){
	  i = pi[m][1];
	  FirstmNode[m] = i;
	  Nodes[i].m_prv = -1;
	  Nodes[i].weight = moss->pt[jb[i]][m];
	  for(k = 2; k <= l; k++){
		j = pi[m][k];
		Nodes[i].m_nxt = j;
		Nodes[j].m_prv = i;
		Nodes[j].weight = moss->pt[jb[j]][m];
		i = j;
	  }
	  Nodes[i].m_nxt = -1;
	  LastmNode[m] = i;
	}
	else{
	  FirstmNode[m] = -1;
	  LastmNode[m] = -1;
	}
  }
}

bool DMOSP_MOSS_Solution::InterpretNetwork()
{
  int i, j, m;

  bool bFeasible = true;

  for(i=0; i<moss->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  int nUsedMCs = moss->nMCs;          // number of machines that have nodes
  int nIncludedJobs = moss->nJobs;    // number of jobs with nodes included

  for(m=0; m<moss->nMCs; m++){
	MCrt[m] = moss->M[m]->ReadyTime;
	mnxt[m] = FirstmNode[m];
	if(mnxt[m] == -1) nUsedMCs--;
  }

  for(j=0; j<moss->nJobs; j++){
	Jnft[j] = moss->J[j]->ReleaseTime;
	jnxt[j] = FirstjNode[j];
	if(jnxt[j] == -1) nIncludedJobs--;
  }

  // Determine the earliest start times and minimum completion times
  // for all operations, and determine Cmax and MWFT
  Cmax = MWFT = 0;
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<moss->nMCs; m++){
	  i = mnxt[m];
	  if(i==-1) continue;
	  j = jb[i];
	  if(jnxt[j] == i){
		Nodes[i].estart = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		MCrt[m] = Jnft[j] = Nodes[i].estart + Nodes[i].weight;
		if(Cmax < MCrt[m]) Cmax = MCrt[m];
		mnxt[m] = Nodes[i].m_nxt;
		jnxt[j] = Nodes[i].j_nxt;

		DMOSP_MOSS_Node &ndi = Nodes[i];
		if(ndi.m_prv != -1 && ndi.j_prv != -1) ndi.rank =
						   Nodes[ndi.m_prv].rank + Nodes[ndi.j_prv].rank +  1;
		else if(ndi.j_prv != -1) ndi.rank = Nodes[ndi.j_prv].rank + 1;
		else if(ndi.m_prv != -1) ndi.rank = Nodes[ndi.m_prv].rank + 1;
		else ndi.rank = 1;

		if(mnxt[m] == -1) nUsedMCs--;
		if(jnxt[j] == -1) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  if(!bFeasible) Cmax = MWFT = -1;
  else{
	// Based on Jnft values, calculate MWFT
	for(j=0; j<moss->nJobs; j++){
	  MWFT += ((Jnft[j] - moss->J[j]->ReleaseTime) * moss->J[j]->Weight);
	}
	MWFT /= (double) moss->nJobs;

	int rem = moss->nOps; // number of remaining operations which need to define start/end times
	// Now, we define the latest start and criticality conditions for nodes
	while(rem > 0){
	  // Search for an operation with the most earliest finish time
	  double maxEFT = -1;
	  int imaxEFT = -1;
	  for(i = 0; i < moss->nOps; i++){
		if(Nodes[i].estart == -1) continue;  // the operation is not in the network
		if(Nodes[i].lstart != -1) continue;  // latest start is already defined
		double eft = Nodes[i].estart + Nodes[i].weight;
		if(eft > maxEFT){
		  maxEFT = eft;
		  imaxEFT = i;
		}
		else if(eft == maxEFT)
		  if(Nodes[i].rank > Nodes[imaxEFT].rank){
			maxEFT = eft;
			imaxEFT = i;
		  }
	  }

	  double mlft, jlft, lft;
	  if(Nodes[imaxEFT].m_nxt == -1) mlft = Cmax;
	  else mlft = Nodes[Nodes[imaxEFT].m_nxt].lstart;
	  if(Nodes[imaxEFT].j_nxt == -1) jlft = Cmax;
	  else jlft = Nodes[Nodes[imaxEFT].j_nxt].lstart;
	  lft = ((mlft < jlft)? mlft : jlft);
	  Nodes[imaxEFT].lstart = lft - Nodes[imaxEFT].weight;
	  Nodes[imaxEFT].tail = Cmax - Nodes[imaxEFT].lstart;
	  Nodes[imaxEFT].slack = Nodes[imaxEFT].lstart - Nodes[imaxEFT].estart;
	  if(fabs(Nodes[imaxEFT].slack) < 0.000000001) Nodes[imaxEFT].bCritical = true;
	  else Nodes[imaxEFT].bCritical = false;
	  rem--;
	}
  }

  return bFeasible;
}

bool DMOSP_MOSS_Solution::ConvertNetToTasks()
{
  if(Cmax == -1){   // Network is not interpretted
	return false;
  }

  for(int i=0; i<moss->nOps; i++){
	DMOSP_Solution::SchTask *tsk = moss->tsk[i];
	tsk->SelectedMC = moss->M[mc[i]];
	tsk->StartTime = Nodes[i].estart;
	tsk->EndTime = Nodes[i].estart + Nodes[i].weight;
	tsk->bScheduled = true;
  }

  return true;
}

bool DMOSP_MOSS_Solution::InterpretDMOSPSolution(DMOSP_Solution *sol)
{
  if(sol->Problem != moss->GetProblem()) return false;

  Reset();

  int j, w, m;

  // define starting positions for jobs' strings
  for(j=0; j<moss->nJobs; j++){
	jpos[j] = jbString->Substring[j].start;
	tau[j][0] = 0;  // start with empty taus
  }

  // start with empty pis
  for(m=0; m < moss->nMCs; m++) pi[m][0] = 0;

  SList &Tasks = sol->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  // Now construct the vectors of taus and pis and assign genes to jobs' strings
  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	int j, m, o;
	j = tsk->Operation->Job->i;
	m = tsk->SelectedMC->i;
	o = tsk->Operation->i;

	tau_pos[o] = ++tau[j][0];
	pi_pos[o] = ++pi[m][0];

	tau[j][tau_pos[o]] = o;
	pi[m][pi_pos[o]] = o;

	jb[o] = j;
	mc[o] = m;

	jbString->Gene[jpos[j]] = tsk->Operation;
	jpos[j]++;
  }

  // assign genes in workstations substrings and identify machine separators
  // positions
  for(w=0; w < wsString->nSubstrings; w++){
    // define starting positions for workstations' strings
	wpos[w] = wsString->Substring[w].start;
	m = moss->Wa[w];
	for(int s = 0; s <= wsString->Substring[w].nSeparators; s++){
	  for(int o=1; o <= pi[m][0]; o++){
		DMOSP_Problem::SOperation *op = moss->O[pi[m][o]];
		wsString->Gene[wpos[w]] = op;
		wpos[w]++;
	  }
	  if(s != wsString->Substring[w].nSeparators)
		wsString->Substring[w].SeparatorPos[s] = wpos[w];
	  m++;
	}
  }

  bVPBConstructed = true;
  bVectorsConstructed = true;
  return true;
}

void DMOSP_MOSS_Solution::alpha_omega_N1(int v1, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  DMOSP_MOSS_Node *nd_v1 = &Nodes[v1];
  int jv1 = jb[v1];
  int l = tau[jv1][0];

  if(nd_v1->m_prv == -1) apr_alpha = 1;
  else{
	apr_alpha = tau_pos[v1];
	if(apr_alpha != 1){
	  DMOSP_MOSS_Node *mprv = &Nodes[nd_v1->m_prv];
	  DMOSP_MOSS_Node *cur = nd_v1;
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		apr_alpha--;
	  }
	}
  }

  if(nd_v1->m_nxt == -1) apr_omega = l;
  else{
	apr_omega = tau_pos[v1];
	if(apr_omega != l){
	  DMOSP_MOSS_Node *mnxt = &Nodes[nd_v1->m_nxt];
	  DMOSP_MOSS_Node *cur = nd_v1;
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		apr_omega++;
	  }
	}
  }
}

double DMOSP_MOSS_Solution::phi_N1(int v1)
{
  if(Cmax == -1){   // Network is not interpretted
	return -1;
  }

  double phi;

  DMOSP_MOSS_Node &v = Nodes[v1];
  phi =   ((v.m_prv == -1)? 0.0 : (Nodes[v.m_prv].estart + Nodes[v.m_prv].weight))
		+ v.weight + ((v.m_nxt == -1)? 0.0 : Nodes[v.m_nxt].tail);

  return phi;
}

void DMOSP_MOSS_Solution::Deltas_N1(int v1, int x1, double &hat_delta, double &check_delta)
{
  if(Cmax == -1){   // Network is not interpretted
	hat_delta = check_delta = -MAXFLOAT;
	return;
  }

  DMOSP_MOSS_Node &v = Nodes[v1];
  int x = tau_pos[v1];
  if(x1 == x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_MOSS_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
	  hat_delta += (jx1prv.estart + jx1prv.weight);
	}
	if(v.m_prv != -1){
	  DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0.0;
	if(x1 < tau[jb[v1]][0]){
	  DMOSP_MOSS_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
	  check_delta += jx1nxt.tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_MOSS_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
	  hat_delta += (jx1prv.estart + jx1prv.weight);
	}
	if(v.m_prv != -1){
	  DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0.0;
	if(x1 > 0){
	  // Get an accurate estimate of the nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  DMOSP_MOSS_Node *j_xnd = &Nodes[v.j_prv];
	  double gjv1nxt = (v.j_nxt != -1)? Nodes[v.j_nxt].tail : 0.0;
	  double gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0.0;
	  double g = j_xnd->weight + ((gjv1nxt > gmx1)? gjv1nxt : gmx1);
	  if(x1 < x - 1){
		for(int _x = x - 2; _x >= x1; _x--){
		  j_xnd = &Nodes[j_xnd->j_prv];
		  gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0.0;
		  g = j_xnd->weight + ((g > gmx1)? g : gmx1);
		}
	  }

	  check_delta += g;
	}
	if(v.m_nxt != -1){
	  DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
  else{
	hat_delta = 0.0;
	if(x1 <= tau[jb[v1]][0]){
	  // Get an accurate estimated value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  DMOSP_MOSS_Node &jx1nd = Nodes[tau[jb[v1]][x1]];
	  DMOSP_MOSS_Node *j_xnd = &Nodes[v.j_nxt];
	  double etjv1prv = (v.j_prv != -1)? Nodes[v.j_prv].estart + Nodes[v.j_prv].weight : 0.0;
	  double etmx1 = (j_xnd->m_prv != -1)? Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight : 0.0;
	  double e = (etjv1prv > etmx1)? etjv1prv : etmx1;
	  if(x1 > x + 1){
		for(int _x = x + 2; _x <= x1; _x++){
		  double prvwt = j_xnd->weight;
		  j_xnd = &Nodes[j_xnd->j_nxt];
		  etmx1 = (j_xnd->m_prv != -1)? (Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight) : 0.0;
		  e = (e + prvwt > etmx1)? (e + prvwt) : etmx1;
		}
	  }
	  hat_delta += (e + jx1nd.weight);
	}
	if(v.m_prv != -1){
	  DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
	  hat_delta -= (mprv.estart + mprv.weight);
	}

	check_delta = 0.0;
	if(x1 < tau[jb[v1]][0]){
	  DMOSP_MOSS_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
	  check_delta += jx1nxt.tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
	  check_delta -= mnxt.tail;
	}
  }
}

void DMOSP_MOSS_Solution::alpha_omega_N2(int v2, int mk2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  DMOSP_MOSS_Node *nd_v2 = &Nodes[v2];

  int l = pi[mk2][0];

  if(mk2 == mc[v2]){ // if same machine
	if(nd_v2->j_prv == -1) apr_alpha = 1;
	else{
	  apr_alpha = pi_pos[v2];
	  if(apr_alpha != 1){
		DMOSP_MOSS_Node *jprv = &Nodes[nd_v2->j_prv];
		DMOSP_MOSS_Node *cur = nd_v2;
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		}
	  }
	}

	if(nd_v2->j_nxt == -1) apr_omega = l;
	else{
	  apr_omega = pi_pos[v2];
	  if(apr_omega != l){
		DMOSP_MOSS_Node *jnxt = &Nodes[nd_v2->j_nxt];
		DMOSP_MOSS_Node *cur = nd_v2;
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		}
	  }
	}
  }
  else{
	if(nd_v2->j_prv == -1) apr_alpha = 1;
	else{
	  apr_alpha = l + 1;
	  DMOSP_MOSS_Node *jprv = &Nodes[nd_v2->j_prv];
	  DMOSP_MOSS_Node *cur;

	  if(LastmNode[mk2] != -1){
		cur = &Nodes[LastmNode[mk2]];
		do{
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);
	  }
	}

	if(nd_v2->j_nxt == -1) apr_omega = l + 1;
	else{
	  apr_omega = 1;
	  DMOSP_MOSS_Node *jnxt = &Nodes[nd_v2->j_nxt];
	  DMOSP_MOSS_Node *cur;

	  if(FirstmNode[mk2] != -1){
		cur = &Nodes[FirstmNode[mk2]];
		do{
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		  if(cur->m_nxt == -1) break;
		  cur = &Nodes[cur->m_nxt];
		}while(true);
	  }
	}
  }
}

double DMOSP_MOSS_Solution::phi_N2(int v2, int mk2)
{
  if(Cmax == -1){   // Network is not interpretted
	return -1;
  }

  double phi;

  DMOSP_MOSS_Node &v = Nodes[v2];
  phi =   ((v.j_prv == -1)? 0 : (Nodes[v.j_prv].estart + Nodes[v.j_prv].weight))
		+ ((v.j_nxt == -1)? 0 : Nodes[v.j_nxt].tail);

  double d = moss->pt[jb[v2]][mk2];
  phi += d;

  return phi;
}

void DMOSP_MOSS_Solution::Deltas_N2(int v2, int mk2, int x2, double &hat_delta, double &check_delta)
{
  if(Cmax == -1){   // Network is not interpretted
	hat_delta = check_delta = -MAXFLOAT;
	return;
  }

  DMOSP_MOSS_Node &v = Nodes[v2];

  if(mk2 != mc[v2]){ // if different machine
	hat_delta = 0.0;
	check_delta = 0.0;

	if(v.j_prv != -1) hat_delta -= (Nodes[v.j_prv].estart +  Nodes[v.j_prv].weight);

	if(x2 != 1){
	  DMOSP_MOSS_Node *ndx2prv = &Nodes[pi[mk2][x2-1]];
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(v.j_nxt != -1) check_delta -= Nodes[v.j_nxt].tail;

	int l = pi[mk2][0];
	if(x2 != l+1){
	  DMOSP_MOSS_Node *ndx2nxt = &Nodes[pi[mk2][x2]];
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	int x = pi_pos[v2];
	if(x2 == x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_MOSS_Node *mx2prv = &Nodes[pi[mk2][x2-1]];
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < pi[mk2][0]){
		DMOSP_MOSS_Node *mx2nxt = &Nodes[pi[mk2][x2+1]];
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_MOSS_Node *mx2prv = &Nodes[pi[mk2][x2-1]];
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 > 0){
		// Get an accurate estimate of the nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		DMOSP_MOSS_Node *m_xnd = &Nodes[v.m_prv];
		double gmv2nxt = (v.m_nxt != -1)? Nodes[v.m_nxt].tail : 0;
		double gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
		double g = m_xnd->weight + ((gmv2nxt > gjx2)? gmv2nxt : gjx2);
		if(x2 < x - 1){
		  for(int _x = x - 2; _x >= x2; _x--){
			m_xnd = &Nodes[m_xnd->m_prv];
			gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
			g = m_xnd->weight + ((g > gjx2)? g : gjx2);
		  }
		}

		check_delta += g;
	  }
	  if(v.j_nxt != -1){
		DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0.0;
	  if(x2 <= pi[mk2][0]){
		// Get an accurate estimated value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
        DMOSP_MOSS_Node *mx2nd = &Nodes[pi[mk2][x2]];
		DMOSP_MOSS_Node *m_xnd = &Nodes[v.m_nxt];
		double etmv2prv = (v.m_prv != -1)? Nodes[v.m_prv].estart + Nodes[v.m_prv].weight : 0;
		double etjx2 = (m_xnd->j_prv != -1)? Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight : 0;
		double e = (etmv2prv > etjx2)? etmv2prv : etjx2;
		if(x2 > x + 1){
		  for(int _x = x + 2; _x <= x2; _x++){
			int prvwt = m_xnd->weight;
			m_xnd = &Nodes[m_xnd->m_nxt];
			etjx2 = (m_xnd->j_prv != -1)? (Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight) : 0;
			e = (e + prvwt > etjx2)? (e + prvwt) : etjx2;
		  }
		}
		hat_delta += (e + mx2nd->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < pi[mk2][0]){
		DMOSP_MOSS_Node *mx2nxt = &Nodes[pi[mk2][x2+1]];
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void DMOSP_MOSS_Solution::alpha_omega_B1(int j, int x1, int x2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  int l = tau[j][0];

  apr_alpha = 1;
  apr_omega = l - (x2 - x1);

  for(int x=x1; x <= x2; x++){  // of course x1 <= x2
	DMOSP_MOSS_Node *v = &Nodes[tau[j][x]];
	int v_apr_alpha, v_apr_omega;

	if(v->m_prv == -1 || x1 == 1) v_apr_alpha = 1;
	else{
	  v_apr_alpha = x1;
	  DMOSP_MOSS_Node *mprv = &Nodes[v->m_prv];
	  DMOSP_MOSS_Node *cur = &Nodes[tau[j][x1]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		v_apr_alpha--;
	  }
	}

	if(v->m_nxt == -1 || x2 == l) v_apr_omega = l - (x2 - x1);
	else{
	  v_apr_omega = x1;
	  DMOSP_MOSS_Node *mnxt = &Nodes[v->m_nxt];
	  DMOSP_MOSS_Node *cur = &Nodes[tau[j][x2]];
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		v_apr_omega++;
	  }
	}

	if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
  }
}

void DMOSP_MOSS_Solution::alpha_omega_B2(int m1, int m2, int x1, int x2, int &apr_alpha, int &apr_omega)
{
  if(Cmax == -1){   // Network is not interpretted
	apr_alpha = apr_omega = -1;
	return;
  }

  if(m1 == m2){ // if same machine
	int l = pi[m1][0];

	apr_alpha = 1;
	apr_omega = l - (x2 - x1);

	for(int x = x1; x <= x2; x++){
	  DMOSP_MOSS_Node *v = &Nodes[pi[m1][x]];
	  int v_apr_alpha, v_apr_omega;

	  if(v->j_prv == -1 || x1 == 1) v_apr_alpha = 1;
	  else{
		v_apr_alpha = x1;
		DMOSP_MOSS_Node *jprv = &Nodes[v->j_prv];
		DMOSP_MOSS_Node *cur = &Nodes[pi[m1][x1]];
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  v_apr_alpha--;
		}
	  }

	  if(v->j_nxt == -1 || x2 == l) v_apr_omega = l - (x2 - x1);
	  else{
		DMOSP_MOSS_Node *jnxt = &Nodes[v->j_nxt];
		DMOSP_MOSS_Node *cur = &Nodes[pi[m1][x2]];
		v_apr_omega = x1;
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  v_apr_omega++;
		}
	  }

	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
  else{ // in case of different machines
	int l = pi[m2][0];

	apr_alpha = 1;
	apr_omega = l + 1;

	for(int x = x1; x <= x2; x++){
	  DMOSP_MOSS_Node *v = &Nodes[pi[m1][x]];
	  int v_apr_alpha, v_apr_omega;

	  if(v->j_prv == -1) v_apr_alpha = 1;
	  else{
		v_apr_alpha = l + 1;
		DMOSP_MOSS_Node *jprv = &Nodes[v->j_prv];
		DMOSP_MOSS_Node *cur;

		if(LastmNode[m2] != -1){
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(cur->estart + cur->weight < jprv->estart) break;
			v_apr_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);
		}
	  }

	  if(v->j_nxt == -1) v_apr_omega = l + 1;
	  else{
		v_apr_omega = 1;
		DMOSP_MOSS_Node *jnxt = &Nodes[v->j_nxt];
		DMOSP_MOSS_Node *cur;

		if(FirstmNode[m2] != -1){
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(jnxt->tail - jnxt->weight > cur->tail) break;
			v_apr_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);
		}
	  }

	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
}

void DMOSP_MOSS_Solution::N1Results(int v1, int &alpha, int &omega, double *&apr_Cmax, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N1 and evaluate the approximate Cmax
  // values at each insertion position between alpha and omega inclusive. The
  // approximate Cmax values are returned in Cmax array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of work centers of the given problem + 1. The values
  // of Cmax between alpha and omega are only assigned. Cmax is not a zero-based
  // array. The value of minX is the position that has the lowest value of
  // approximate Cmax other than the current position.
  //
  memset((void *)apr_Cmax, -1.0, sizeof(double) * moss->nWCs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX =  -1;
	return;
  }

  alpha_omega_N1(v1, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  double phi = phi_N1(v1);

  DMOSP_MOSS_Node &v = Nodes[v1];
  int x = tau_pos[v1];
  double minCmax = MAXFLOAT;

  minX = 0;  // In case that there is only one operation in the job,
			 // the value of minX will be zero since there is only one position
			 // in the job which is the current position.

  // Get an accurate estimate of the nodes tail value in network Gamma-1
  // based on the values provided in Gamma network....
  //
  DMOSP_MOSS_Node *j_xnd;
  if(v.j_prv != -1){
	j_xnd = &Nodes[v.j_prv];
	double gjv1nxt = (v.j_nxt != -1)? Nodes[v.j_nxt].tail : 0.0;
	double gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0.0;
	double g = j_xnd->weight + ((gjv1nxt > gmx1)? gjv1nxt : gmx1);
	etgmx_1[x-1] = g;
	for(int _x = x - 2; _x >= alpha; _x--){
	  j_xnd = &Nodes[j_xnd->j_prv];
	  gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0.0;
	  g = j_xnd->weight + ((g > gmx1)? g : gmx1);
	  etgmx_1[_x] = g;
	}
  }

  // Get an accurate estimated value of eta in the network Gamma-1
  // based on the values available in network Gamma...
  //
  if(v.j_nxt != -1){
	j_xnd = &Nodes[v.j_nxt];
	double etjv1prv = (v.j_prv != -1)? Nodes[v.j_prv].estart + Nodes[v.j_prv].weight : 0.0;
	double etmx1 = (j_xnd->m_prv != -1)? Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight : 0.0;
	double e = (etjv1prv > etmx1)? etjv1prv : etmx1;
	etgmx_1[x+1] = e;
	for(int _x = x + 2; _x <= omega; _x++){
	  double prvwt = j_xnd->weight;
	  j_xnd = &Nodes[j_xnd->j_nxt];
	  etmx1 = (j_xnd->m_prv != -1)? (Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight) : 0.0;
	  e = (e + prvwt > etmx1)? (e + prvwt) : etmx1;
	  etgmx_1[_x] = e;
	}
  }

  double hat_delta, check_delta;
  for(int x1=alpha; x1<=omega; x1++){
	if(x1 == x){
	  hat_delta = 0.0;
	  if(x1 != 1){
		DMOSP_MOSS_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
		hat_delta += (jx1prv.estart + jx1prv.weight);
	  }
	  if(v.m_prv != -1){
		DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = 0.0;
	  if(x1 < tau[jb[v1]][0]){
		DMOSP_MOSS_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
		check_delta += jx1nxt.tail;
	  }
	  if(v.m_nxt != -1){
		DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}
	else if(x1 < x){
	  hat_delta = 0.0;
	  if(x1 != 1){
		DMOSP_MOSS_Node &jx1prv = Nodes[tau[jb[v1]][x1-1]];
		hat_delta += (jx1prv.estart + jx1prv.weight);
	  }
	  if(v.m_prv != -1){
		DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = etgmx_1[x1];
	  if(v.m_nxt != -1){
		DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}
	else{
	  DMOSP_MOSS_Node &jx1nd = Nodes[tau[jb[v1]][x1]];
	  hat_delta = (etgmx_1[x1] + jx1nd.weight);

	  if(v.m_prv != -1){
		DMOSP_MOSS_Node &mprv = Nodes[v.m_prv];
		hat_delta -= (mprv.estart + mprv.weight);
	  }

	  check_delta = 0.0;
	  if(x1 < tau[jb[v1]][0]){
		DMOSP_MOSS_Node &jx1nxt = Nodes[tau[jb[v1]][x1+1]];
		check_delta += jx1nxt.tail;
	  }
	  if(v.m_nxt != -1){
		DMOSP_MOSS_Node &mnxt = Nodes[v.m_nxt];
		check_delta -= mnxt.tail;
	  }
	}

	double delta = ((hat_delta > 0.0)? hat_delta : 0.0) + ((check_delta > 0.0)? check_delta : 0.0);
	apr_Cmax[x1] = delta + phi;
	if(apr_Cmax[x1] < minCmax && x1 != x){
	  minCmax = apr_Cmax[x1];
	  minX = x1;
	}
  }
}

void DMOSP_MOSS_Solution::N2Results(int v2, int k2, int &alpha, int &omega, double *&apr_Cmax, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N2 and evaluate the approximate Cmax
  // values at each insertion position between alpha and omega inclusive. The
  // approximate Cmax values are returned in Cmax array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of jobs of the given problem + 1. The values
  // of Cmax between alpha and omega are only assigned. Cmax is not a zero-based
  // array.
  //
  memset((void *)apr_Cmax, -1, sizeof(double) * moss->nJobs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX = -1;
	return;
  }

  alpha_omega_N2(v2, k2, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  double phi = phi_N2(v2, k2);

  double hat_delta, check_delta;
  DMOSP_MOSS_Node &v = Nodes[v2];
  double minCmax = MAXFLOAT;

  if(k2 != mc[v2]){ // if different machine
	for(int x2=alpha; x2<=omega; x2++){
	  hat_delta = 0.0;
	  check_delta = 0.0;

	  if(v.j_prv != -1) hat_delta -= (Nodes[v.j_prv].estart +  Nodes[v.j_prv].weight);

	  if(x2 != 1){
		DMOSP_MOSS_Node *ndx2prv = &Nodes[pi[k2][x2-1]];
		hat_delta += (ndx2prv->estart + ndx2prv->weight);
	  }

	  if(v.j_nxt != -1) check_delta -= Nodes[v.j_nxt].tail;

	  int l = pi[k2][0];
	  if(x2 != l+1){
		DMOSP_MOSS_Node *ndx2nxt = &Nodes[pi[k2][x2]];
		check_delta += ndx2nxt->tail;
	  }

	  double delta = ((hat_delta > 0.0)? hat_delta : 0.0) + ((check_delta > 0.0)? check_delta : 0.0);
	  apr_Cmax[x2] = delta + phi;
	  if(apr_Cmax[x2] < minCmax){
		minCmax = apr_Cmax[x2];
		minX = x2;
	  }
	}
  }
  else{
	minX = 0;  // In case that there is only one operation in the machine,
			   // the value of minX will be zero since there is only one position
			   // in the machine which is the current position.

	int x = pi_pos[v2];

	// Get an accurate estimate of the nodes tail value in network Gamma-1
	// based on the values provided in Gamma network....
	//
	DMOSP_MOSS_Node *m_xnd;
	if(v.m_prv != -1){
	  m_xnd = &Nodes[v.m_prv];
	  double gmv2nxt = (v.m_nxt != -1)? Nodes[v.m_nxt].tail : 0;
	  double gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
	  double g = m_xnd->weight + ((gmv2nxt > gjx2)? gmv2nxt : gjx2);
	  etgmx_2[x-1] = g;
	  for(int _x = x - 2; _x >= alpha; _x--){
		m_xnd = &Nodes[m_xnd->m_prv];
		gjx2 = (m_xnd->j_nxt != -1)? Nodes[m_xnd->j_nxt].tail : 0;
		g = m_xnd->weight + ((g > gjx2)? g : gjx2);
		etgmx_2[_x] = g;
	  }
	}

	// Get an accurate estimated value of eta in the network Gamma-1
	// based on the values available in network Gamma...
	//
	if(v.m_nxt != -1){
	  m_xnd = &Nodes[v.m_nxt];
	  double etmv2prv = (v.m_prv != -1)? Nodes[v.m_prv].estart + Nodes[v.m_prv].weight : 0;
	  double etjx2 = (m_xnd->j_prv != -1)? Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight : 0;
	  double e = (etmv2prv > etjx2)? etmv2prv : etjx2;
	  etgmx_2[x+1] = e;
	  for(int _x = x + 2; _x <= omega; _x++){
		double prvwt = m_xnd->weight;
		m_xnd = &Nodes[m_xnd->m_nxt];
		etjx2 = (m_xnd->j_prv != -1)? (Nodes[m_xnd->j_prv].estart + Nodes[m_xnd->j_prv].weight) : 0;
		e = (e + prvwt > etjx2)? (e + prvwt) : etjx2;
		etgmx_2[_x] = e;
	  }
	}

	for(int x2=alpha; x2<=omega; x2++){
	  if(x2 == x){
		hat_delta = 0.0;
		if(x2 != 1){
		  DMOSP_MOSS_Node *mx2prv = &Nodes[pi[k2][x2-1]];
		  hat_delta += (mx2prv->estart + mx2prv->weight);
		}
		if(v.j_prv != -1){
		  DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = 0.0;
		if(x2 < pi[k2][0]){
		  DMOSP_MOSS_Node *mx2nxt = &Nodes[pi[k2][x2+1]];
		  check_delta += mx2nxt->tail;
		}
		if(v.j_nxt != -1){
		  DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }
	  else if(x2 < x){
		hat_delta = 0.0;
		if(x2 != 1){
		  DMOSP_MOSS_Node *mx2prv = &Nodes[pi[k2][x2-1]];
		  hat_delta += (mx2prv->estart + mx2prv->weight);
		}
		if(v.j_prv != -1){
		  DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = etgmx_2[x2];
		if(v.j_nxt != -1){
		  DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }
	  else{
		DMOSP_MOSS_Node *mx2nd = &Nodes[pi[k2][x2]];
		hat_delta = (etgmx_2[x2] + mx2nd->weight);
		if(v.j_prv != -1){
		  DMOSP_MOSS_Node *jprv = &Nodes[v.j_prv];
		  hat_delta -= (jprv->estart + jprv->weight);
		}

		check_delta = 0.0;
		if(x2 < pi[k2][0]){
		  DMOSP_MOSS_Node *mx2nxt = &Nodes[pi[k2][x2+1]];
		  check_delta += mx2nxt->tail;
		}
		if(v.j_nxt != -1){
		  DMOSP_MOSS_Node *jnxt = &Nodes[v.j_nxt];
		  check_delta -= jnxt->tail;
		}
	  }

	  double delta = ((hat_delta > 0.0)? hat_delta : 0.0) + ((check_delta > 0.0)? check_delta : 0.0);
	  apr_Cmax[x2] = delta + phi;
	  if(apr_Cmax[x2] < minCmax && x2 != x){
		minCmax = apr_Cmax[x2];
		minX = x2;
	  }
	}
  }
}

void DMOSP_MOSS_Solution::N3ExactResults(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N3 and evaluate the exact change in MWFT
  // values at each insertion position between alpha and omega inclusive. The
  // Changes in MWFT values are returned in deltaMWFT array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of work centers of the given problem + 1. The values
  // of deltaMWFT between alpha and omega are only assigned. deltaMWFT is not a zero-based
  // array. The value of minX is the position that has the lowest value of
  // deltaMWFT other than the current position.
  //
  memset((void *)deltaMWFT, -1.0, sizeof(double) * moss->nWCs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX =  -1;
	return;
  }

  alpha_omega_N1(v1, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  int x = tau_pos[v1];

  minX = 0;  // In case that there is only one operation in the job,
			 // the value of minX will be zero since there is only one position
			 // in the job which is the current position.

  DMOSP_MOSS_Solution *tmpSol = new DMOSP_MOSS_Solution(moss);
  DMOSP_MOSS::NSMove N3mv;
  N3mv.F = DMOSP_MOSS::N1;
  N3mv.v = v1;

  double minDMWFT = MAXFLOAT;
  for(int x1=alpha; x1<=omega; x1++){
	if(x1 == x) deltaMWFT[x1] = 0;
	else{
	  N3mv.x = x1;
	  tmpSol->Copy(this);
	  moss->ApplyN(tmpSol, N3mv);
	  deltaMWFT[x1] = tmpSol->MWFT - MWFT;
	  if(deltaMWFT[x1] < minDMWFT){
		minDMWFT = deltaMWFT[x1];
		minX = x1;
	  }
	}
  }
  delete tmpSol;
}

void DMOSP_MOSS_Solution::N4ExactResults(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N4 and evaluate the exact change in MWFT
  // values at each insertion position between alpha and omega inclusive. The
  // exact changes in MWFT values are returned in deltaMWFT array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of jobs of the given problem + 1. The values
  // of deltaMWFT between alpha and omega are only assigned. deltaMWFT is not a zero-based
  // array.
  //
  memset((void *)deltaMWFT, -1, sizeof(double) * moss->nJobs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX = -1;
	return;
  }

  alpha_omega_N2(v2, k2, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  double minDMWFT = MAXFLOAT;

  DMOSP_MOSS_Solution *tmpSol = new DMOSP_MOSS_Solution(moss);
  DMOSP_MOSS::NSMove N4mv;
  N4mv.F = DMOSP_MOSS::N2;
  N4mv.v = v2;
  N4mv.m2 = k2;

  if(k2 != mc[v2]){ // if different machine
	for(int x2=alpha; x2<=omega; x2++){
	  N4mv.x = x2;
	  tmpSol->Copy(this);
	  moss->ApplyN(tmpSol, N4mv);
	  deltaMWFT[x2] = tmpSol->MWFT - MWFT;
	  if(deltaMWFT[x2] < minDMWFT){
		minDMWFT = deltaMWFT[x2];
		minX = x2;
	  }
	}
  }
  else{
	minX = 0;  // In case that there is only one operation in the machine,
			   // the value of minX will be zero since there is only one position
			   // in the machine which is the current position.

	int x = pi_pos[v2];
	for(int x2=alpha; x2<=omega; x2++){
	  if(x2 == x) deltaMWFT[x2] = 0;
	  else{
		N4mv.x = x2;
		tmpSol->Copy(this);
		moss->ApplyN(tmpSol, N4mv);
		deltaMWFT[x2] = tmpSol->MWFT - MWFT;
		if(deltaMWFT[x2] < minDMWFT){
		  minDMWFT = deltaMWFT[x2];
		  minX = x2;
		}
	  }
	}
  }

  delete tmpSol;
}

void DMOSP_MOSS_Solution::N3Results(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N3 and evaluate the approximate change in MWFT
  // values at each insertion position between alpha and omega inclusive. The
  // Changes in MWFT values are returned in deltaMWFT array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of work centers of the given problem + 1. The values
  // of deltaMWFT between alpha and omega are only assigned. deltaMWFT is not a zero-based
  // array. The value of minX is the position that has the lowest value of
  // deltaMWFT other than the current position.
  //
  memset((void *)deltaMWFT, -1.0, sizeof(double) * moss->nWCs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX =  -1;
	return;
  }

  alpha_omega_N1(v1, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  int x = tau_pos[v1];

  minX = 0;  // In case that there is only one operation in the job,
			 // the value of minX will be zero since there is only one position
			 // in the job which is the current position.

  DMOSP_MOSS_Solution *tmpSol = new DMOSP_MOSS_Solution(moss);
  DMOSP_MOSS::NSMove N3mv;
  N3mv.F = DMOSP_MOSS::N1;
  N3mv.v = v1;

  double minDMWFT = MAXFLOAT;
  for(int x1=alpha; x1<=omega; x1++){
	if(x1 == x) deltaMWFT[x1] = 0;
	//else if(x1 < x) deltaMWFT[x1] = MAXFLOAT;  // Excluded exchange with direct predecessor
	else{                                           //   since it is equivalent to single
	  N3mv.x = x1;                                  //   step forward of the preceding operation
	  //-------------------------------------------
	  tmpSol->Copy(this);
	  double eMWFT = moss->MWFTIfApplyN(tmpSol, N3mv);
	  //-------------------------------------------
	  deltaMWFT[x1] = eMWFT - MWFT;
	  if(deltaMWFT[x1] < minDMWFT){
		minDMWFT = deltaMWFT[x1];
		minX = x1;
	  }
	}
  }
  delete tmpSol;
}

void DMOSP_MOSS_Solution::N4Results(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  // The role of this function is to find the approximate values of alpha and omega for
  // the neighborhood search function N4 and evaluate the approximate change in MWFT
  // values at each insertion position between alpha and omega inclusive. The
  // exact changes in MWFT values are returned in deltaMWFT array whose necessary
  // memory should be allocated before calling this function with the full size
  // which equals the number of jobs of the given problem + 1. The values
  // of deltaMWFT between alpha and omega are only assigned. deltaMWFT is not a zero-based
  // array.
  //
  memset((void *)deltaMWFT, -1, sizeof(double) * moss->nJobs);
  if(Cmax == -1){   // Network is not interpretted
	alpha = omega = minX = -1;
	return;
  }

  alpha_omega_N2(v2, k2, alpha, omega);
  if(alpha > omega){
	alpha = omega = minX =  -1;
	return;
  }

  double minDMWFT = MAXFLOAT;

  DMOSP_MOSS_Solution *tmpSol = new DMOSP_MOSS_Solution(moss);
  DMOSP_MOSS::NSMove N4mv;
  N4mv.F = DMOSP_MOSS::N2;
  N4mv.v = v2;
  N4mv.m2 = k2;

  if(k2 != mc[v2]){ // if different machine
	for(int x2=alpha; x2<=omega; x2++){
	  N4mv.x = x2;
	  //------------------------------------------------
	  tmpSol->Copy(this);
	  double eMWFT = moss->MWFTIfApplyN(tmpSol, N4mv);
	  //------------------------------------------------
	  deltaMWFT[x2] = eMWFT - MWFT;
	  if(deltaMWFT[x2] < minDMWFT){
		minDMWFT = deltaMWFT[x2];
		minX = x2;
	  }
	}
  }
  else{
	minX = 0;  // In case that there is only one operation in the machine,
			   // the value of minX will be zero since there is only one position
			   // in the machine which is the current position.

	int x = pi_pos[v2];
	for(int x2=alpha; x2<=omega; x2++){
	  if(x2 == x) deltaMWFT[x2] = 0;
	  //else if(x2 < x) deltaMWFT[x2] = MAXFLOAT;
	  else{
		N4mv.x = x2;
		//------------------------------------------------
		tmpSol->Copy(this);
		double eMWFT = moss->MWFTIfApplyN(tmpSol, N4mv);
		//------------------------------------------------
	    deltaMWFT[x2] = eMWFT - MWFT;
		if(deltaMWFT[x2] < minDMWFT){
		  minDMWFT = deltaMWFT[x2];
		  minX = x2;
		}
	  }
	}
  }

  delete tmpSol;
}
//-------- Debugging functions

void DMOSP_MOSS_Solution::echoVPBChromosome(FILE *outf, wchar_t *heading)
{
  wchar_t buf[64];

  fwprintf(outf, L"\nVPB chromosome:");
  fwprintf(outf, heading);

  fwprintf(outf, L"\nSj:  |");
  int p = 0;
  for(int j=0; j<jbString->nSubstrings; j++){
	while(p <= jbString->Substring[j].end){
	  swprintf(buf, L"%s%s", jbString->Gene[p]->ID(),
				 ((p < jbString->Substring[j].end)? L"-" : L"|"));
	  fwprintf(outf, buf);
	  ++p;
	}
  }

  fflush(outf);

  fwprintf(outf, L"\nSw:  |");
  p = 0;
  for(int w=0; w<wsString->nSubstrings; w++){
	int s = 0;
	while(p <= wsString->Substring[w].end){
	  swprintf(buf, L"%s%s", wsString->Gene[p]->ID(),
				 ((p < wsString->Substring[w].end)? L"-" : L"|"));
	  fwprintf(outf, buf);
	  if(wsString->Substring[w].nSeparators != 0){
		while(wsString->Substring[w].SeparatorPos[s] - 1 == p){
		  if(s == wsString->Substring[w].nSeparators) break;
		  fwprintf(outf, L"^");
		  ++s;
		}
	  }
	  ++p;
	}
  }
  fwprintf(outf, L"\n\n");
  fflush(outf);
}

void DMOSP_MOSS_Solution::echoVechors(FILE *outf, wchar_t *heading)
{
  int m, j;
  wchar_t buf[256];
  fwprintf(outf, L"\n\nVectors representation: %s\n\n", heading);
  fwprintf(outf, L"\ta) pi\'s:\n");
  for(m=0; m<moss->nMCs; m++){
	int l = pi[m][0];
	swprintf(buf, L"\t\t%s [%i]: ", moss->M[m]->ID(), l);
	fwprintf(outf, buf);
	if(l <= 0) fwprintf(outf, L"\n");
	for(int i=1; i<=l; i++){
	  DMOSP_Problem::SOperation *op = moss->O[pi[m][i]];
	  swprintf(buf, L" %s ", op->ID());
	  fwprintf(outf, buf);
	  if(i<l) swprintf(buf, L"->");
	  else swprintf(buf, L"\n");
	  fwprintf(outf, buf);
	}
  }
  fwprintf(outf, L"\n\tb) tau\'s:\n");
  for(j=0; j<moss->nJobs; j++){
	int l = tau[j][0];
	swprintf(buf, L"\t\t%s [%i]: ", moss->J[j]->ID(), l);
	fwprintf(outf, buf);
	if(l<=0) fwprintf(outf, L"\n");
	for(int i=1; i<=l; i++){
	  DMOSP_Problem::SOperation *op = moss->O[tau[j][i]];
	  swprintf(buf, L" %s ", op->ID());
	  fwprintf(outf, buf);
	  if(i<l) swprintf(buf, L"->");
	  else swprintf(buf, L"\n");
	  fwprintf(outf, buf);
	}
  }
  fwprintf(outf, L"\n\n");
  fflush(outf);
}

void DMOSP_MOSS_Solution::echoNetwork(FILE *outf, wchar_t *heading)
{
  wchar_t buf[256];
  fwprintf(outf, L"\n\nNetwork representation: %s\n\n", heading);
  fwprintf(outf, L"\ta) List of Nodes:\n\n");
  fwprintf(outf, L"\tNo.\tID\ttau_pos\tpi_pos\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\tweight\testart\tlstart\ttail\tslack\tRank\tCritical\n");
  for(int i=0; i<moss->nOps; i++){
	swprintf(buf, L"\t%i\t%s\t%i\t%i\t%i\t%i\t%s\t%i\t%i\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%i\t%s\n",
				 i, moss->O[i]->ID(), tau_pos[i], pi_pos[i], Nodes[i].j_prv, Nodes[i].j_nxt,
				 moss->M[mc[i]]->ID(), Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? L"*" : L" "));
	fwprintf(outf, buf);
  }
  fwprintf(outf, L"\n\n\tb) List of Jobs:\n\n");
  fwprintf(outf, L"\tID\tFirst\tLast\n");
  for(int j=0; j<moss->nJobs; j++){
	swprintf(buf, L"\t%s\t%i\t%i\n", moss->J[j]->ID(), FirstjNode[j], LastjNode[j]);
	fwprintf(outf, buf);
  }
  fwprintf(outf, L"\n\n\tc) List of Machines:\n\n");
  fwprintf(outf, L"\tID\tFirst\tLast\n");
  for(int m=0; m<moss->nMCs; m++){
	swprintf(buf, L"\t%s\t%i\t%i\n", moss->M[m]->ID(), FirstmNode[m], LastmNode[m]);
	fwprintf(outf, buf);
  }
  swprintf(buf, L"\n\t\tCmax = %0.2f\n\n", Cmax);
  fwprintf(outf, buf);
  fflush(outf);
}

// ----------------------------------------------------- DMOSP_MOSS Algorithm

DMOSP_MOSS::DMOSP_MOSS(DMOSP_Problem *P, int ps, int dth, int tsz, int ntsitr,
					 int ntsimpitr, int nbstmvs, int nrndmvs, double probX,
					 int nitr, double TL)
	: DMOSP_Metaheuristic(P, L"Multi-objective Scatter Search for DMOSP", TL)
{
  pSize = ps;
  deltaThreshold = dth;
  TabuSize = tsz;
  nTSiterations = ntsitr;
  nImpIterations = ntsimpitr;
  nBestMoves = nbstmvs;
  nRandMoves = nrndmvs;
  Theta_X = probX;
  nIterations = nitr;

  bestCmaxSol = NULL;
  bestMWFTSol = NULL;
  startingSol = NULL;
  bestLocalSol = NULL;

  GD = IGD = -1;
  dSize = 0;  // number of individuals in set D used for generating fronts

  TrackingResults = NULL;

		#ifdef PRNT_MOSS_DBG
		  mossDbgFile = fopen("mossDebug.txt", "a+");
		#endif
}

DMOSP_MOSS::DMOSP_MOSS(DMOSP_Problem *P, int ps, int ntsitr, int dth,
						  double **tr)
	: DMOSP_Metaheuristic(P, L"Multi-objective Scatter Search for DMOSP", 120000)
{                      // Time limit is fixed at 120 seconds  <-----
  pSize = ps;
  deltaThreshold = dth;
  TabuSize = 10;
  nTSiterations = ntsitr;
  nImpIterations = 20;
  nBestMoves = 5;
  nRandMoves = 5;
  Theta_X = 0.5;
  nIterations = 1000;

  bestCmaxSol = NULL;
  bestMWFTSol = NULL;
  startingSol = NULL;
  bestLocalSol = NULL;

  GD = IGD = -1;
  dSize = 0;  // number of individuals in set D used for generating fronts

  TrackingResults = tr;    // an array for tracking GD, IGD, TGD, DHV%
  iTrack = 0;
  TrackTime[0] = 0.0;
  for(int i=1; i<5; i++){
	TrackTime[i] = TrackTime[i-1] + 30000.0;
		// --> tracked values every 30 seconds
  }
}

DMOSP_MOSS::~DMOSP_MOSS()
{
  if(bestCmaxSol) delete bestCmaxSol;    bestCmaxSol = NULL;
  if(bestMWFTSol) delete bestMWFTSol;    bestMWFTSol = NULL;
  if(startingSol) delete startingSol;    startingSol = NULL;
  if(bestLocalSol) delete bestLocalSol;  bestLocalSol = NULL;

  // Clear the set of non-dominated DMOSP_solution's
  for(SList::SNode *slnd = ND.head(); slnd; slnd = slnd->Next()){
	DMOSP_Solution *s = (DMOSP_Solution *)slnd->Data();
	delete s;
  }
  ND.Clear();

  if(dSize > 0){  // delete previously allocated fronts if any
	delete [] numIndsFront;
	for(int ii = 0; ii < dSize; ii++) delete [] paretoFront[ii];
	delete [] paretoFront;
  }

		#ifdef PRNT_MOSS_DBG
		  fclose(mossDbgFile);
		#endif
}

void DMOSP_MOSS::AllocateMemory()
{
  DMOSP_Metaheuristic::AllocateMemory();

  // Allocated memory needed by the distance calculator
  int maxnMCs = 0;  // Maximum number of machines in a given work cetner
  for(int w=0; w<nWCs; w++)
	if(Wz[w] - Wa[w] + 1 > maxnMCs) maxnMCs = Wz[w] - Wa[w] + 1;
  mxsz = (nJobs+maxnMCs > nWCs)? (nJobs+maxnMCs) : nWCs;
  mxsz++;
  r = new DMOSP_SubPerumutation[mxsz];
  a = new char *[mxsz];
  for(int i=0; i<mxsz; i++) a[i] = new char[mxsz];
  Suma_i = new int[mxsz];
  Suma_j = new int[mxsz];

  // Allocate the memory needed for solution combination
  jbstrGeneSel = new bool[nOps];
  wsstrGeneSel = new bool[nOps];;

  // Allocate the memory needed by Tabu Search
  TabuMoves = new NSMove[TabuSize];
  //------------------------------------------------------------------

  bestMoves = new NSMove [nBestMoves];  // Used locally in the iterations to
										// choose the best neighborhood move

  apr_Cmax = new double[((nWCs>nJobs)? nWCs : nJobs)+1];  // Used for local N1 & N2 evaluations
  deltaMWFT = new double[((nWCs>nJobs)? nWCs : nJobs)+1];  // Used for local N3 & N4 evaluations

  if(deltaThreshold == -1) SuggestDeltaThreshold();
}

void DMOSP_MOSS::Initialize()
{
  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);

  // Clear the set of non-dominated DMOSP_solution's
  ClearND();

  // Construct two initial solutions to store the best found solutions throughout
  // iterations for both objectives
  double Cmax, MWFT;
  DMOSP_MOSS_Solution *Sol1, *Sol2;

  // We first construct two solutions used to maintain best found solutions for
  // both objectives throughout iterations
  bestCmaxSol = new DMOSP_MOSS_Solution(this);
  DSG(MCTj, MWL, bestCmax, MWFT);
  bestCmaxSol->InterpretTaskList();
  bestMWFTSol = new DMOSP_MOSS_Solution(this);
  DSG(WLWR, MCTm, Cmax, bestMWFT);
  bestMWFTSol->InterpretTaskList();

  bestCmaxSol->GenerateNetwork();
  bestCmaxSol->InterpretNetwork();
  bestMWFTSol->GenerateNetwork();
  bestMWFTSol->InterpretNetwork();

  UpdateNonDominatedSolutions(bestCmaxSol);
  UpdateNonDominatedSolutions(bestMWFTSol);

  startingSol = new DMOSP_MOSS_Solution(this);  // Stores initial solution for TS
  startingSol->Copy(bestCmaxSol);
  bestLocalSol = new DMOSP_MOSS_Solution(this);  // Stores best Cmax local solution for TS
  bestLocalSol->Copy(bestCmaxSol);

  if(frmMOSSCalcProgress->bActive){
	frmMOSSCalcProgress->pbSSPRIterations->Min = 0;
	frmMOSSCalcProgress->pbSSPRIterations->Max = nIterations;
	frmMOSSCalcProgress->pbConstruction->Min = 0;
	frmMOSSCalcProgress->pbConstruction->Max = pSize;
	frmMOSSCalcProgress->grdInitialSolutions->RowCount = pSize + 1;
	frmMOSSCalcProgress->pbSCProgress->Min = 0;
	frmMOSSCalcProgress->sgSCIterations->RowCount = nIterations * pSize + 1;
	frmMOSSCalcProgress->ClearForm();
  }
  /*else if(frmMOSSMultiRuns->bActive){
	frmMOSSMultiRuns->pbSSPRIterations->Min = 0;
	frmMOSSMultiRuns->pbSSPRIterations->Max = nIterations;
	frmMOSSMultiRuns->pbConstruction->Min = 0;
	frmMOSSMultiRuns->pbConstruction->Max = pSize;
  }*/
  else if(frmTuningMOSS->bActive){
	frmTuningMOSS->pbIterations->Min = 0;
	frmTuningMOSS->pbIterations->Max = nIterations;
	frmTuningMOSS->pbConstruction->Min = 0;
	frmTuningMOSS->pbConstruction->Max = pSize;
  }
  else if(frmTuningMOSS2->bActive){
	frmTuningMOSS2->pbIterations->Min = 0;
	frmTuningMOSS2->pbIterations->Max = MaxTimeLimit/1000;
	frmTuningMOSS2->pbConstruction->Min = 0;
	frmTuningMOSS2->pbConstruction->Max = pSize;
  }

  CalculateCmaxLowerBound();
  CalculateMWFTLowerBound();

  if(frmMOSSCalcProgress->bActive){
	frmMOSSCalcProgress->lblCmaxLB->Caption = FloatToStr(CmaxLB);
	frmMOSSCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
	frmMOSSCalcProgress->lblMWFTLB->Caption = FloatToStr(MWFTLB);
	frmMOSSCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	frmMOSSCalcProgress->DrawLBLines(CmaxLB, MWFTLB);
	frmMOSSCalcProgress->DrawParetoFront(D);
	frmMOSSCalcProgress->DrawOptimalParetoFront(Problem->i);
  }

  if(bestCmax == CmaxLB && bestMWFT == MWFTLB && *bestCmaxSol == *bestMWFTSol) bOptSolFound = true;
  else{
	bOptSolFound = false;
	ConstructInitialPopulation();
	UpdateNonDominatedSolutions(R);
	if(frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(D);
  }

  mossitr = CombinedSolCounter = 1;
  bTerminate = false;

  if(TrackingResults){
    ComputePerformanceMetrics();
	TrackingResults[0][0] = HV0pr - HVpr;
	TrackingResults[0][1] = TGD;
	TrackingResults[0][2] = GD;
	TrackingResults[0][3] = IGD;
	iTrack = 1;
  }
}

void DMOSP_MOSS::Improve()
{
  // Clear all solutions in set T
  for(SList::SNode *nd = T.head(); nd; nd = nd->Next()){
	DMOSP_MOSS_Solution *uS = (DMOSP_MOSS_Solution *)nd->Data();
	delete uS;
  }
  T.Clear();

  int nComb = R.Count() * 2;

  if(frmMOSSCalcProgress->bActive){
	frmMOSSCalcProgress->pbSCProgress->Max = nComb;
	frmMOSSCalcProgress->pbSCProgress->Position = 0;
  }

  bNonDominatedSetUpdated = false;
  while(nComb != 0){
	// Select two random solutions
	DMOSP_MOSS_Solution *SL = (DMOSP_MOSS_Solution *)R[rnd(0, R.Count()-1)];
	DMOSP_MOSS_Solution *SF;
	do{
	  SF = (DMOSP_MOSS_Solution *)R[rnd(0, R.Count()-1)];
	}while(SF == SL);  // Make sure that the two solutions are different

	if(frmMOSSCalcProgress->bActive){
	  frmMOSSCalcProgress->pbSCProgress->Position = frmMOSSCalcProgress->pbSCProgress->Position + 1;
	  if(frmMOSSCalcProgress->cbxSolutionCombination->Checked)
		frmMOSSCalcProgress->lblSCIter->Caption = IntToStr(frmMOSSCalcProgress->pbSCProgress->Position);
	}

	Application->ProcessMessages();

	if(frmMOSSCalcProgress->bActive){
	  if(frmMOSSCalcProgress->cbxSolutionCombination->Checked){
		frmMOSSCalcProgress->sgSCIterations->Cells[0][CombinedSolCounter] = IntToStr(CombinedSolCounter);
		frmMOSSCalcProgress->sgSCIterations->Cells[1][CombinedSolCounter] = FloatToStr(SL->Cmax);
		frmMOSSCalcProgress->sgSCIterations->Cells[2][CombinedSolCounter] = FloatToStr(SF->Cmax);
		frmMOSSCalcProgress->sgSCIterations->Cells[3][CombinedSolCounter] = FloatToStr(SL->MWFT);
		frmMOSSCalcProgress->sgSCIterations->Cells[4][CombinedSolCounter] = FloatToStr(SF->MWFT);
	  }
	}

	DMOSP_MOSS_Solution *SC = CombinedFrom(SL, SF);  // SC is a new solution
         // resulted from combining geners from the SF and SL

	// Show initial makespan value (before applying TS) in the list
	if(frmMOSSCalcProgress->bActive){
	  if(frmMOSSCalcProgress->cbxSolutionCombination->Checked){
		frmMOSSCalcProgress->sgSCIterations->Cells[5][CombinedSolCounter] = FloatToStr(SC->Cmax);
		frmMOSSCalcProgress->sgSCIterations->Cells[7][CombinedSolCounter] = FloatToStr(SC->MWFT);
	  }
	}
	Application->ProcessMessages();

	// Check computational time limit
	::ftime(&cur_time);
	ComputationalTime = (cur_time.time - start_time.time)*1000.0 +
								   (cur_time.millitm - start_time.millitm);
	if(ComputationalTime >= MaxTimeLimit){
	  bCompuTimeLimitReached = true;
	  bInterrupt = true;
	  break;
	}

	if(SC->Cmax != -1){    // SC->Cmax == -1 should not happen anyway!!!
	  TabuSearch(SC, false, false);
	  T.Add((void *)SC);
	}

	// Show final makespan value (after applying TS) in the list
	if(frmMOSSCalcProgress->bActive){
	  if(frmMOSSCalcProgress->cbxSolutionCombination->Checked){
		frmMOSSCalcProgress->sgSCIterations->Cells[6][CombinedSolCounter] = FloatToStr(SC->Cmax);
		frmMOSSCalcProgress->sgSCIterations->Cells[8][CombinedSolCounter] = FloatToStr(SC->MWFT);
	  }
	}

	Application->ProcessMessages();

	CombinedSolCounter++;

	nComb--;
  }

  // Update the set of Pareto solutions using solutions in set T
  //UpdateNonDominatedSolutions(T);
  //if(frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(P);

  // Examine updating R using solutions in T
  bRefSetUpdated = false;
  UpdateRefSet();

  // If R does not change, diversify
  if(!bRefSetUpdated && !bNonDominatedSetUpdated){
	DiversifyR();
	bool bUpdated = UpdateNonDominatedSolutions(R);
	if(bUpdated && frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(D);
  }

  if(frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawReferenceSet(R);
}

bool DMOSP_MOSS::Stop()
{
  bool bStop = false;

  if(bTerminate){
	bStop = true;
  }
  if(bOptSolFound){
	bStop = true;
  }

  if(TrackingResults && iTrack < 5){
	 if(ComputationalTime >= TrackTime[iTrack]){
		ComputePerformanceMetrics();
		TrackingResults[iTrack][0] = HV0pr - HVpr;
		TrackingResults[iTrack][1] = TGD;
		TrackingResults[iTrack][2] = GD;
		TrackingResults[iTrack][3] = IGD;
		iTrack++;
	 }
  }

  if(!bStop){
	mossitr++;
	if(frmMOSSCalcProgress->bActive){
	  frmMOSSCalcProgress->pbSSPRIterations->Position = mossitr;
	}
	/*else if(frmMOSSMultiRuns->bActive){
	  frmMOSSMultiRuns->pbSSPRIterations->Position = mossitr;
	}*/
	else if(frmTuningMOSS->bActive){
	  frmTuningMOSS->pbIterations->Position = mossitr;
	}
	else if(frmTuningMOSS2->bActive){
	  frmTuningMOSS2->pbIterations->Position = ComputationalTime / 1000;
	}

	if(mossitr > nIterations){
	  bStop = true;
	}
  }

  if(bStop){
	if(frmMOSSCalcProgress->bActive){
	  frmMOSSCalcProgress->btnStart->Enabled = true;
	  frmMOSSCalcProgress->btnClose->Enabled = true;
	  frmMOSSCalcProgress->btnTerminate->Enabled = false;
    }
	return true;
  }
  return false;
}

void DMOSP_MOSS::Finalize()
{
  if(bCompuTimeLimitReached){
	if(frmMOSSCalcProgress->bActive){
	  frmMOSSCalcProgress->btnStart->Enabled = true;
	  frmMOSSCalcProgress->btnClose->Enabled = true;
	  frmMOSSCalcProgress->btnTerminate->Enabled = false;
	}
  }

  DMOSP_Metaheuristic::Finalize();
  SolStatus = FEASIBLE;

  // Construct set ND based on solutions in D
  ConvertDtoND();
  ComputePerformanceMetrics();
  if(frmMOSSCalcProgress->bActive){
	frmMOSSCalcProgress->DrawOnlyParetoFront(D);
  }
  if(TrackingResults && iTrack < 5){
	 if(ComputationalTime >= TrackTime[iTrack]){
		TrackingResults[iTrack][0] = HV0pr - HVpr;
		TrackingResults[iTrack][1] = TGD;
		TrackingResults[iTrack][2] = GD;
		TrackingResults[iTrack][3] = IGD;
	 }
  }
}

void DMOSP_MOSS::FreeMemory()
{
  DMOSP_Metaheuristic::FreeMemory();

  // Free the memory used for calculating distances
  delete [] r;
  for(int i=0; i<mxsz; i++) delete [] a[i];
  delete [] a;
  delete [] Suma_i;
  delete [] Suma_j;
  //--------------------------------------------------------

  delete [] jbstrGeneSel;
  delete [] wsstrGeneSel;

  delete [] TabuMoves;
  delete [] bestMoves;

  // Clear all solutions in set R
  for(SList::SNode *nd = R.head(); nd != NULL; nd = nd->Next()){
	DMOSP_MOSS_Solution *S = (DMOSP_MOSS_Solution *)nd->Data();
	delete S;
  }
  R.Clear();

  // Clear all solutions in set T
  for(SList::SNode *nd = T.head(); nd; nd = nd->Next()){
	DMOSP_MOSS_Solution *uS = (DMOSP_MOSS_Solution *)nd->Data();
	delete uS;
  }
  T.Clear();

  // Now free moemory for the non-dominated solutions set (D)
  for(SList::SNode *slnd = D.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	delete s;
  }
  D.Clear();

  //---> Keep the memory allocated for Non-dominated set (ND) solutions
  //     to show them in the MOSS dialog box. They are delete later
  //     from memory when the class object is destructed.

  if(bestCmaxSol) delete bestCmaxSol;     bestCmaxSol = NULL;
  if(bestMWFTSol) delete bestMWFTSol;     bestMWFTSol = NULL;
  if(startingSol) delete startingSol;     startingSol = NULL;
  if(bestLocalSol) delete bestLocalSol;   bestLocalSol = NULL;

  delete [] apr_Cmax;
  delete [] deltaMWFT;
}

void DMOSP_MOSS::ComputeObjectiveValue()
{
  // This is just a weighted summation of the two considered objectives
  ObjectiveValue = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  ObjectiveValue += Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);
  ObjectiveValue /= 2.0;
}

void DMOSP_MOSS::ClearND()
{
  for(SList::SNode *slnd = ND.head(); slnd; slnd = slnd->Next()){
	DMOSP_Solution *s = (DMOSP_Solution *)slnd->Data();
	delete s;
  }
  ND.Clear();
}

void DMOSP_MOSS::RemoveDuplicatesInD()
{
  D.QSort(MOSSIndComp);
  SList::SNode *slnd = D.head();

  while(slnd){
    DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	SList::SNode *nxtnd = slnd->Next();
	if(!nxtnd) break;

	bool bDifferentSol = false;
	do{
	  DMOSP_MOSS_Solution *nxts = (DMOSP_MOSS_Solution *)nxtnd->Data();
	  if(nxts->Cmax == s->Cmax && nxts->MWFT == s->MWFT){
		delete nxts;
        SList::SNode *removednd = nxtnd;
		nxtnd = nxtnd->Next();
		D.Remove(removednd);
		if(!nxtnd) bDifferentSol = true;
	  }
	  else bDifferentSol = true;
	}while(!bDifferentSol);

	slnd = nxtnd;
  }
}

void DMOSP_MOSS::ConvertDtoND()
{
  ClearND();
  RemoveDuplicatesInD();
  for(SList::SNode *slnd = D.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	s->ConvertNetToTasks();
	DMOSP_Solution *dmosps = new DMOSP_Solution(Problem);
	dmosps->Copy(Solution);
	ND.Add((void *)dmosps);
  }
}

int DMOSP_MOSS::SuggestDeltaThreshold()
{
   // This function is intented to suggest a delta threshold value for solutions
   // acceptance in the referecne set.
   double dTh = 0;
   double A = (Problem->nOperations() < 50)? 3.0 : 5.0;

   // 1) For each workstation, count the number of jobs then divide that by 3
   // and then add the closest higher integer to the deltaTreshold value
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)pcnd->Data();
	 double Nj = 0;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   Nj++;
	 }
	 dTh += Nj/A;
	 pcnd = pcnd->Next();
   }

   // 2) For each job count the number of workstations that need to be visited
   // divide that by 3 and then add the closest higher integer to the
   // deltaThreshold value
   for(int j=0; j<nJobs; j++){
	 double nWC = Jz[j] - Jz[j] + 1;
	 dTh += nWC/A;
   }

   deltaThreshold = ceil(dTh);

   return deltaThreshold;
}

int DMOSP_MOSS::ConstructInitialPopulation()
{
  int nFails = 0;    // count the number of times the algorithm fails to add
					 // a constructed initial solution due to similarity

  nGenerated = nExcluded = 0;
  while(R.Count() < pSize){
	DMOSP_MOSS_Solution *newSol = ConstructSolution();
	nGenerated++;

	TabuSearch(newSol);

	if(bOptSolFound) return R.Count();

	bool bAdd = true;
	for(SList::SNode *nd = R.head(); nd != NULL; nd = nd->Next()){
	  int jD, mD;
	  DMOSP_MOSS_Solution *extantSol = (DMOSP_MOSS_Solution *)nd->Data();
	  bool bDist = isDistanceGreaterThan(extantSol, newSol, deltaThreshold);

	  if(!bDist){
		bAdd = false;
		break;
	  }
	}
	if(!bAdd){
	  delete newSol;
	  nFails++;
      nExcluded++;
	  if(nFails == pSize / 2){  // maximum nFails = pSize / 2
		nFails = 0;
		//deltaThreshold *= 4;  	deltaThreshold /= 5;
		deltaThreshold /= 2;
	  }
	  continue;
	}

	R.Add((void *)newSol);

	int nConstructed = R.Count();
	if(frmMOSSCalcProgress->bActive){
	  frmMOSSCalcProgress->pbConstruction->Position = nConstructed;
	  if(frmMOSSCalcProgress->cbxConstruction->Checked){
		frmMOSSCalcProgress->grdInitialSolutions->Cells[0][nConstructed] = IntToStr(nConstructed);
		frmMOSSCalcProgress->grdInitialSolutions->Cells[1][nConstructed] = FloatToStr(initialLocalCmax);
		frmMOSSCalcProgress->grdInitialSolutions->Cells[2][nConstructed] = FloatToStr(bestLocalCmax);
		frmMOSSCalcProgress->grdInitialSolutions->Cells[3][nConstructed] = FloatToStr(initialLocalMWFT);
		frmMOSSCalcProgress->grdInitialSolutions->Cells[4][nConstructed] = FloatToStr(bestLocalMWFT);
	  }
	}
	/*else if(frmMOSSMultiRuns->bActive){
	  frmMOSSMultiRuns->pbConstruction->Position = nConstructed;
	}*/
	else if(frmTuningMOSS->bActive){
	  frmTuningMOSS->pbConstruction->Position = nConstructed;
	}
	else if(frmTuningMOSS2->bActive){
	  frmTuningMOSS2->pbConstruction->Position = nConstructed;
	}
	Application->ProcessMessages();

	// Check computational time limit
	::ftime(&cur_time);
	ComputationalTime = (cur_time.time - start_time.time)*1000.0 +
								   (cur_time.millitm - start_time.millitm);
	if(ComputationalTime >= MaxTimeLimit){
	  bCompuTimeLimitReached = true;
	  bInterrupt = true;
	  break;
	}

	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
  if(frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawReferenceSet(R);
  return R.Count();
}

DMOSP_MOSS_Solution *DMOSP_MOSS::ConstructSolution()
{
  DSG_JSR jsr[] = {RANDj, MCTj, WSPT, WLWR};
  DSG_MSR msr[] = {RANDm, MCTm, MWL};
  int jsri, msri;

  DMOSP_MOSS_Solution *newSol = new DMOSP_MOSS_Solution(this);
  jsri = rnd(0,3);
  msri = rnd(0,2);
  DSG(jsr[jsri], msr[msri], newSol->Cmax, newSol->MWFT);
  newSol->InterpretTaskList();
  return newSol;
}

bool DMOSP_MOSS::UpdateNonDominatedSolutions(DMOSP_MOSS_Solution *Sol)
{
  if(frmMOSSOperators->bActive &&
	frmMOSSOperators->cbxEnforceDominance->Checked == false) return false;

  // Preior to using this function, all solutions in set D and the new checked
  // solution must have their Cmax and MWFT values assigned...
  for(SList::SNode *slnd = D.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	if((s->Cmax < Sol->Cmax && s->MWFT <= Sol->MWFT) ||
	   (s->Cmax <= Sol->Cmax && s->MWFT < Sol->MWFT)){
	  return false;
	}
	else if (s->Cmax == Sol->Cmax && s->MWFT == Sol->MWFT) {
			if(*s == *Sol){
			  return false;
			}
		 }
  }

  SList tobeRemoved;
  for(SList::SNode *slnd = D.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	if((s->Cmax > Sol->Cmax && s->MWFT >= Sol->MWFT) ||
	   (s->Cmax >= Sol->Cmax && s->MWFT > Sol->MWFT)){
		tobeRemoved.Add((void *)slnd);
	}
  }
  for(SList::SNode *nd = tobeRemoved.head(); nd; nd = nd->Next()){
	SList::SNode *slnd = (SList::SNode *)nd->Data();
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	delete s;
	D.Remove(slnd);
  }
  DMOSP_MOSS_Solution *newsol = new DMOSP_MOSS_Solution(*Sol);
  D.Add((void *)newsol);
  return true;
}

bool DMOSP_MOSS::UpdateNonDominatedSolutions(SList &Sols)
{
  if(frmMOSSOperators->bActive &&
    frmMOSSOperators->cbxEnforceDominance->Checked == false){  // to see all solutions in set R
	for(SList::SNode *slnd = Sols.head(); slnd; slnd = slnd->Next()){
	  DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	  DMOSP_MOSS_Solution *newsol = new DMOSP_MOSS_Solution(*s);
	  D.Add((void *)newsol);
	}
	return true;
  }

  bool bUpdated = false;
  for(SList::SNode *slnd = Sols.head(); slnd; slnd = slnd->Next()){
	DMOSP_MOSS_Solution *s = (DMOSP_MOSS_Solution *)slnd->Data();
	if(UpdateNonDominatedSolutions(s)) bUpdated = true;  // at least one solution updates Pareto front
  }
  if(bUpdated && frmMOSSCalcProgress->bActive){
	frmMOSSCalcProgress->DrawParetoFront(D);
  }
  return bUpdated;
}

void DMOSP_MOSS::UpdateRefSet()
{
  // Update the reference set based on the solutions generated in the
  // current iteration and stored in set T

  DMOSP_MOSS_Solution *TSol, *RSol;
  SList::SNode *Rnd, *Tnd;
  double TotalCmax, TotalMWFT;

  // Put all solutions in set T and then sort them according to teir dominance
  // Meanwhile, calculate the total makespan and MWFT values
  //
  TotalCmax = TotalMWFT = 0.0;
  for(Rnd = R.head(); Rnd; Rnd = Rnd->Next()){
	RSol = (DMOSP_MOSS_Solution *)Rnd->Data();
	T.Add((void *)RSol);
	TotalCmax += RSol->Cmax;
	TotalMWFT += RSol->MWFT;
  }
  R.Clear();
  T.QSort(DMOSPMOSSSolCompDomin, ASCENDING);

  // Now add solutions from set T to set R one-by-one and check solutions'
  // deversity while adding
  Tnd = T.head();
  while(Tnd){
	TSol = (DMOSP_MOSS_Solution *)Tnd->Data();

	// Compare that solution with existing solutions in set R to maintain
	// xolutions diversity
	bool bAdd = true;
	for(Rnd = R.head(); Rnd; Rnd = Rnd->Next()){
	  int jD, mD;
	  RSol = (DMOSP_MOSS_Solution *)Rnd->Data();
	  bool bDist = isDistanceGreaterThan(RSol, TSol, deltaThreshold);

	  if(!bDist){
		bAdd = false;
		Tnd = Tnd->Next();
		break;
	  }
	}
	if(bAdd){
	  R.Add((void *)TSol);
	  SList::SNode *curnd = Tnd;
	  Tnd = Tnd->Next();
	  T.Remove(curnd);
	  if(R.Count() == pSize) break;
	}
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  // If the size of set R is less than the RefSetSize, create new solutions
  // and add them to set R the same way we did in the initialization process
  //
  int nFails = 0;
  while(R.Count() < pSize){
	DMOSP_MOSS_Solution *newSol = ConstructSolution();
	TabuSearch(newSol);

	bool bAdd = true;
	for(SList::SNode *nd = R.head(); nd; nd = nd->Next()){
	  int jD, mD;
	  DMOSP_MOSS_Solution *extantSol = (DMOSP_MOSS_Solution *)nd->Data();
	  bool bDist = isDistanceGreaterThan(extantSol, newSol, deltaThreshold);

	  if(!bDist){
		bAdd = false;
		break;
	  }
	}
	if(!bAdd){
	  delete newSol;
	  nFails++;
	  nExcluded++;
	  if(nFails == pSize / 2){  // maximum nFails = pSize / 2
		nFails = 0;
		//deltaThreshold *= 4;  	deltaThreshold /= 5;
		deltaThreshold /= 2;
	  }
	  continue;
	}

	R.Add((void *)newSol);

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  // Now, calculate the changes in the toal makespan and the total MWFT of
  // the updated solutions in set R
  double TotalCmax0 = TotalCmax;
  double TotalMWFT0 = TotalMWFT;
  for(Rnd = R.head(); Rnd; Rnd = Rnd->Next()){
	RSol = (DMOSP_MOSS_Solution *)Rnd->Data();
	TotalCmax -= RSol->Cmax;
	TotalMWFT -= RSol->MWFT;
  }
  double deltaCmax = fabs(TotalCmax) / TotalCmax0;
  double deltaMWFT = fabs(TotalMWFT) / TotalMWFT0;
  bRefSetUpdated = (deltaCmax > 0.02 && deltaMWFT > 0.02)? true : false;
}

void DMOSP_MOSS::DiversifyR()
{
  int ms;
  int nNewSol = 0;
  SList::SNode *Rnd = R.tail();
  while(nNewSol < R.Count() / 4){
	DMOSP_MOSS_Solution *RSol = (DMOSP_MOSS_Solution *)Rnd->Data();

	DMOSP_MOSS_Solution *newSol = ConstructSolution();
	newSol->GenerateNetwork();
	if(newSol->InterpretNetwork()){
	  TabuSearch(newSol, true);
	  Rnd->SetData((void *)newSol);
	  delete RSol;
	}
	nNewSol++;

	Rnd = Rnd->Prev();
	if(!Rnd) break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  if(!Rnd) return;  // this should not happen anyway

  //Inject best found non-dominated solutions into the reference set
  nNewSol = 0;
  SList::SNode *Pnd = D.head();
  while(nNewSol < R.Count() / 4){
	DMOSP_MOSS_Solution *RSol = (DMOSP_MOSS_Solution *)Rnd->Data();
	DMOSP_MOSS_Solution *PSol = (DMOSP_MOSS_Solution *)Pnd->Data();

	DMOSP_MOSS_Solution *newSol = new DMOSP_MOSS_Solution(this);
	newSol->Copy(PSol);
	Rnd->SetData((void *)newSol);
	delete RSol;

	nNewSol++;

	Rnd = Rnd->Prev();
	if(!Rnd) break;
	Pnd = Pnd->Next();
	if(!Pnd) break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
}

void DMOSP_MOSS::InjectPinR()
{
  int ms;
  int nNewSol = 0;
  SList::SNode *Rnd = R.head();
  SList::SNode *Pnd = D.head();
  while(nNewSol < R.Count() / 4){
	DMOSP_MOSS_Solution *RSol = (DMOSP_MOSS_Solution *)Rnd->Data();
	DMOSP_MOSS_Solution *PSol = (DMOSP_MOSS_Solution *)Pnd->Data();

	DMOSP_MOSS_Solution *newSol = new DMOSP_MOSS_Solution(this);
	newSol->Copy(PSol);
	Rnd->SetData((void *)newSol);
	delete RSol;

	nNewSol++;

	Rnd = Rnd->Next();
	if(!Rnd) break;
	Pnd = Pnd->Next();
	if(!Pnd) break;

	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }
}

double DMOSP_MOSS::ApplyN(DMOSP_MOSS_Solution *Sol, NSMove mv)
{
  // This function modifies the solution vectors by conducting the received move
  // mv, then the solution vectors are interpretted to network representation
  // and all start and finish times are calculated.
  // Note: It is assumed that the sent move is feasible.

  if(mv.F == N1){
	int j = Sol->jb[mv.v];
	int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->Cmax;  // There is nothing to do!
	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++){
		tau[x] = tau[x+1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--){
		tau[x] = tau[x-1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
  }
  else{  // N2 or N4
	int m1 = Sol->mc[mv.v];
	if(mv.m2 == m1){
	  int *&pi = Sol->pi[mv.m2];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->Cmax; // There is nothing to do
	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++){
		  pi[x] = pi[x+1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  else{
        int x;
		for(x=oldx; x>mv.x; x--){
		  pi[x] = pi[x-1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];
	  for(int x=curx; x<pi1[0]; x++){
		pi1[x] = pi1[x+1];
		Sol->pi_pos[pi1[x]] = x;
	  }
	  pi1[0]--;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0]){
		pi2[++pi2[0]] = mv.v;
		Sol->pi_pos[mv.v] = mv.x;
	  }
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--){
		  pi2[x] = pi2[x-1];
		  Sol->pi_pos[pi2[x]] = x;
		}
		pi2[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  Sol->mc[mv.v] = mv.m2;
	}
  }

  Sol->GenerateNetwork();
  if(Sol->InterpretNetwork()){
	if(Sol->Cmax < bestCmax || (Sol->Cmax == bestCmax && Sol->MWFT < bestCmaxSol->MWFT)){
	  bestCmax = Sol->Cmax;
	  bestCmaxSol->Copy(Sol);
	  if(frmMOSSCalcProgress->bActive)
		frmMOSSCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
	}
	if(Sol->MWFT < bestMWFT || (Sol->MWFT == bestMWFT && Sol->Cmax < bestMWFTSol->Cmax)){
	  bestMWFT = Sol->MWFT;
	  bestMWFTSol->Copy(Sol);
	  if(frmMOSSCalcProgress->bActive)
		frmMOSSCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	}
	if(bestCmax == CmaxLB && bestMWFT == MWFTLB){
	  if(*bestCmaxSol == *bestMWFTSol) bOptSolFound = true;
	}
	return Sol->Cmax;
  }
  return -1.0;
}

double DMOSP_MOSS::MWFTIfApplyN(DMOSP_MOSS_Solution *Sol, NSMove mv)
{
  // This function provides a quick evaluation of the MWFT if a neighborhood
  // search function is applied. The passed solution's network is for the
  // original solution. Its network representation will be changed and therefore,
  // this solution will be distorted. It cannot be used anymore.
  DMOSP_MOSS_Node *&Nodes = Sol->Nodes;

  if(mv.F == N1){  // N1 or N3
	int j = Sol->jb[mv.v];
	int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->MWFT;  // There is nothing to do!
	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++) tau[x] = tau[x+1];
	  tau[x] = mv.v;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--) tau[x] = tau[x-1];
	  tau[x] = mv.v;
	}
	int i = tau[1];
	Sol->FirstjNode[j] = i;
	Nodes[i].j_prv = -1;
	for(int k = 2; k <= tau[0]; k++){
	  int m = tau[k];
	  Nodes[i].j_nxt = m;
	  Nodes[m].j_prv = i;
	  i = m;
	}
	Nodes[i].j_nxt = -1;
  }
  else{  // N2 or N4
	int m1 = Sol->mc[mv.v];
	if(mv.m2 == m1){
	  int *&pi = Sol->pi[m1];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->MWFT; // There is nothing to do
	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++) pi[x] = pi[x+1];
		pi[x] = mv.v;
	  }
	  else{
		int x;
		for(x=oldx; x>mv.x; x--) pi[x] = pi[x-1];
		pi[x] = mv.v;
	  }

	  int i = pi[1];
	  Sol->FirstmNode[m1] = i;
	  Nodes[i].m_prv = -1;
	  for(int k = 2; k <= pi[0]; k++){
		int j = pi[k];
		Nodes[i].m_nxt = j;
		Nodes[j].m_prv = i;
		i = j;
	  }
	  Nodes[i].m_nxt = -1;
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];
	  for(int x=curx; x<pi1[0]; x++) pi1[x] = pi1[x+1];
	  pi1[0]--;

	  int i = pi1[1];
	  Sol->FirstmNode[m1] = i;
	  Nodes[i].m_prv = -1;
	  for(int k = 2; k <= pi1[0]; k++){
		int j = pi1[k];
		Nodes[i].m_nxt = j;
		Nodes[j].m_prv = i;
		i = j;
	  }
	  Nodes[i].m_nxt = -1;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0])	pi2[++pi2[0]] = mv.v;
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--) pi2[x] = pi2[x-1];
		pi2[x] = mv.v;
	  }
	  Sol->mc[mv.v] = mv.m2;

	  i = pi2[1];
	  Sol->FirstmNode[mv.m2] = i;
	  Nodes[i].m_prv = -1;
	  for(int k = 2; k <= pi2[0]; k++){
		int j = pi2[k];
		Nodes[i].m_nxt = j;
		Nodes[j].m_prv = i;
		i = j;
	  }
	  Nodes[i].m_nxt = -1;
	  Nodes[mv.v].weight = pt[Oj[mv.v]][mv.m2];
	}
  }

  MWFTBasedOnNetwork(Sol);
  return Sol->MWFT;
}

void DMOSP_MOSS::MWFTBasedOnNetwork(DMOSP_MOSS_Solution *Sol)
{
  int i, j, m;

  DMOSP_MOSS_Node *&Nodes = Sol->Nodes;
  int *&mnxt = Sol->mnxt;
  int *&jnxt = Sol->jnxt;
  double *&MCrt = Sol->MCrt;
  double *&Jnft = Sol->Jnft;

  int nUsedMCs = nMCs;          // number of machines that have nodes
  int nIncludedJobs = nJobs;    // number of jobs with nodes included

  for(m=0; m<nMCs; m++){
	MCrt[m] = M[m]->ReadyTime;
	mnxt[m] = Sol->FirstmNode[m];
	if(mnxt[m] == -1) nUsedMCs--;
  }
  for(j=0; j<nJobs; j++){
	Jnft[j] = J[j]->ReleaseTime;
	jnxt[j] = Sol->FirstjNode[j];
	if(jnxt[j] == -1) nIncludedJobs--;
  }

  // Determine the earliest start times and minimum completion times
  // for all operations, and determine Cmax and MWFT
  bool bNoAction;
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bNoAction = true;
	for(m=0; m<nMCs; m++){
	  i = mnxt[m];
	  if(i==-1) continue;
	  j = Sol->jb[i];
	  if(jnxt[j] == i){
		Nodes[i].estart = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		MCrt[m] = Jnft[j] = Nodes[i].estart + Nodes[i].weight;
		mnxt[m] = Nodes[i].m_nxt;
		jnxt[j] = Nodes[i].j_nxt;

		if(mnxt[m] == -1) nUsedMCs--;
		if(jnxt[j] == -1) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction) break;
  }

  // Based on Jnft values, calculate MWFT
  Sol->MWFT = 0.0;
  for(j=0; j<nJobs; j++){
	Sol->MWFT += ((Jnft[j] - J[j]->ReleaseTime) * J[j]->Weight);
  }
  Sol->MWFT /= (double)nJobs;
}

double DMOSP_MOSS::MWFTIfApplyN_Complete(DMOSP_MOSS_Solution *Sol, NSMove mv)
{
  // This function provides an evaluation of the MWFT if a neighborhood
  // search function is applied. The passed solution vectors of processing
  // sequences are used to modify the passed Sol and then its network is
  // constructed. The passed solution is fully modified.

  if(mv.F == N1){
	int j = Sol->jb[mv.v];
	int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->MWFT;  // There is nothing to do!
	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++){
		tau[x] = tau[x+1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--){
		tau[x] = tau[x-1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
  }
  else{  // N2 or N4
	int m1 = Sol->mc[mv.v];
	if(mv.m2 == m1){
	  int *&pi = Sol->pi[mv.m2];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->MWFT; // There is nothing to do
	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++){
		  pi[x] = pi[x+1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  else{
		int x;
		for(x=oldx; x>mv.x; x--){
		  pi[x] = pi[x-1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];
	  for(int x=curx; x<pi1[0]; x++){
		pi1[x] = pi1[x+1];
		Sol->pi_pos[pi1[x]] = x;
	  }
	  pi1[0]--;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0]){
		pi2[++pi2[0]] = mv.v;
		Sol->pi_pos[mv.v] = mv.x;
	  }
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--){
		  pi2[x] = pi2[x-1];
		  Sol->pi_pos[pi2[x]] = x;
		}
		pi2[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  Sol->mc[mv.v] = mv.m2;
	}
  }

  Sol->GenerateNetwork();
  if(Sol->InterpretNetwork())return Sol->MWFT;
  return -1.0;
}

int DMOSP_MOSS::TauIntersectionSize(int j, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2)
{
  // This function returns the intersection set size for two solutions for the
  // perumtations of a given job
  if(S1->moss != S2->moss) return -1;

  int *&v1 = S1->tau[j];
  int *&v2 = S2->tau[j];
  int sz = v1[0];
  int z = 1;
  int x1 = 1;
  bool bDone = false;
  do{
	int i = v1[x1];
	int x2 = S2->tau_pos[i];
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > v1[0]){
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		z++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		z++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  return z;
}

int DMOSP_MOSS::TauDistance(int j, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2)
{
  // This function returns the distance for two solutions for the
  // perumtations of a given job
  if(S1->moss != S2->moss) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(DMOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  int *&v1 = S1->tau[j];
  int *&v2 = S2->tau[j];
  int sz = v1[0];

  nr = 0;
  int x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];
	int x2 = S2->tau_pos[i];
	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  if(nr == 0) return 0;  // the two permutations are identical

  // Construct the initial subpermutation relative positions matrix
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

  // Now iterate on the "a" matrix to calculate the distance
  int Distance = 0;
  do{
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}

	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_Sa == 0) break;

	Distance++;
	if(Distance > nr) break;  // The distance cannot exceed the number of subpermutations

	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  Suma_i[max_ij] = 0;

	  // update the positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 >= min_Xj && r[i].x2 < r[max_ij].x2){
		  a[i][max_ij] = 1;
		  Suma_i[i]++;
		  Suma_j[max_ij]++;
		  r[i].x2 += r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 >= min_Xj && r[j].x2 < r[max_ij].x2){
		  a[max_ij][j] = 0;
		  Suma_j[j]--;
		  r[j].x2 += r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = min_Xj;
	}
	else{  // Forward move
	  int max_Xi = -1, max_i;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
			max_i = i;
		  }
		}
	  }
	  Suma_j[max_ij] = 0;

	  // update hte positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 > r[max_ij].x2 && r[i].x2 <= max_Xi){
		  a[i][max_ij] = 0;
		  Suma_i[i]--;
		  r[i].x2 -= r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 > r[max_ij].x2 && r[j].x2 <= max_Xi){
		  a[max_ij][j] = 1;
		  Suma_i[max_ij]++;
		  Suma_j[j]++;
		  r[j].x2 -= r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = r[max_i].x2 + r[max_i].l;
	}
  }while(true);

  return Distance;
}

int DMOSP_MOSS::WCIntersectionSize(int w, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2)
{
  // This function returns the intersection set size for two solutions for the
  // permutations among the machines of a given work center
  if(S1->moss != S2->moss) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(DMOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  nr = 0;  // starting number of subpermutations is 0

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  // where rho_1 = pi^1_w,1 -> <d1> -> pi^1_w,2 -> <d2> ..... -> pi^1_w,q_w
  // and  rho_2 = pi^2_w,1 -> <d1> -> pi^2_w,2 -> <d2> ..... -> pi^2_w,q_w
  int nmcs = Wz[w] - Wa[w] + 1;    // number of machines in workcetner w
  int nops1 = 0, nops2 = 0;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	nops1 += S1->pi[m][0];
	nops2 += S2->pi[m][0];
  }
  if(nops1 != nops2) return -1; //  This should not happen anyway

  int sz = nops1 + nmcs - 1;
  int *v1 = new int[sz+1];
  int *v2 = new int[sz+1];
  v1[0] = v2[0] = sz;

  int x1=1, x2=1;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	int pisz1 = S1->pi[m][0];
	memcpy((void *)&v1[x1], &S1->pi[m][1], sizeof(int) * pisz1);
	if(m != Wz[w]){
	  x1 += pisz1;
	  v1[x1] = nOps + m;
	  x1++;
	}

	int pisz2 = S2->pi[m][0];
	memcpy((void *)&v2[x2], &S2->pi[m][1], sizeof(int) * pisz2);
	if(m != Wz[w]){
	  x2 += pisz2;
	  v2[x2] = nOps + m;
	  x2++;
	}
  }

  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];

	x2 = 0;
	if(i < nOps){
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		if(m2 != S2->mc[i]) x2 += (S2->pi[m2][0] + 1);
		else{
		  x2 += S2->pi_pos[i];
		  break;
		}
	  }
	}
	else{
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		x2 += (S2->pi[m2][0] + 1);
		if(m2 == i-nOps) break;
	  }
	}

	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  delete [] v1;
  delete [] v2;

  return nr;
}

int DMOSP_MOSS::WCDistance(int w, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2)
{
  // This function returns the distance between two solutions for the
  // permutations among the machines of a given work center
  if(S1->moss != S2->moss) return -1;

  for(int i=0; i<mxsz; i++) memset((void *)a[i], 0, sizeof(char) * mxsz);
  memset((void *)r, 0, sizeof(DMOSP_SubPerumutation) * mxsz);
  memset((void *)Suma_i, 0, sizeof(int) * mxsz);
  memset((void *)Suma_j, 0, sizeof(int) * mxsz);

  nr = 0;  // starting number of subpermutations is 0

  // First, make a list of all subpermutations of rho_1 and rho_2 in the order of rho_1
  // where rho_1 = pi^1_w,1 -> <d1> -> pi^1_w,2 -> <d2> ..... -> pi^1_w,q_w
  // and  rho_2 = pi^2_w,1 -> <d1> -> pi^2_w,2 -> <d2> ..... -> pi^2_w,q_w
  int nmcs = Wz[w] - Wa[w] + 1;    // number of machines in workcetner w
  int nops1 = 0, nops2 = 0;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	nops1 += S1->pi[m][0];
	nops2 += S2->pi[m][0];
  }
  if(nops1 != nops2){  //  This should not happen anyway
	return -1;
  }

  int sz = nops1 + nmcs - 1;
  int *v1 = new int[sz+1];
  int *v2 = new int[sz+1];
  v1[0] = v2[0] = sz;

  int x1=1, x2=1;
  for(int m=Wa[w]; m<=Wz[w]; m++){
	int pisz1 = S1->pi[m][0];
	memcpy((void *)&v1[x1], &S1->pi[m][1], sizeof(int) * pisz1);
	if(m != Wz[w]){
	  x1 += pisz1;
	  v1[x1] = nOps + m;
	  x1++;
	}

	int pisz2 = S2->pi[m][0];
	memcpy((void *)&v2[x2], &S2->pi[m][1], sizeof(int) * pisz2);
	if(m != Wz[w]){
	  x2 += pisz2;
	  v2[x2] = nOps + m;
	  x2++;
	}
  }

  x1 = 1;
  bool bDone = false;
  do{
	r[nr].x1 = x1;
	int i = v1[x1];

	x2 = 0;
	if(i < nOps){
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
		if(m2 != S2->mc[i]) x2 += (S2->pi[m2][0] + 1);
		else{
		  x2 += S2->pi_pos[i];
		  break;
		}
	  }
	}
	else{
	  for(int m2=Wa[w]; m2<=Wz[w]; m2++){
        x2 += (S2->pi[m2][0] + 1);
		if(m2 == i-nOps) break;
	  }
	}

	r[nr].x2 = x2;
	int d = 1;

	for(; d<=sz; d++){
	  if(x1+d > sz){
		r[nr].l = d;
		bDone = true;
		break;
	  }
	  if(x2+d > sz){
		r[nr].l = d;
		nr++;
		break;
	  }
	  if(v1[x1+d] != v2[x2+d]){
		r[nr].l = d;
		nr++;
		break;
	  }
	}
	x1 += d;
	Application->ProcessMessages();
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }while(!bDone);

  if(nr == 0){   // the two permutations are identical
	delete [] v1;
	delete [] v2;
	return 0;
  }

  // Construct the initial subpermutation relative positions matrix
  // This matrix is upper-right triangular
  for(int i=0; i<nr; i++){
	for(int j=i+1; j<=nr; j++){
	  if(r[j].x2 < r[i].x2){
		a[i][j] = 1;
		Suma_i[i]++;
		Suma_j[j]++;
	  }
	}
  }

  // Now iterate on the "a" matrix to calculate the distance
  int Distance = 0;
  do{
	int max_Sa = 0;
	int max_ij = -1;
	bool bInRow = true;

	for(int i=0; i<nr; i++){
	  if(Suma_i[i] > max_Sa){
		max_Sa = Suma_i[i];
		max_ij = i;
	  }
	}

	for(int j=1; j<=nr; j++){
	  if(Suma_j[j] > max_Sa){
		max_Sa = Suma_j[j];
		max_ij = j;
		bInRow = false;
	  }
	}
	if(max_Sa == 0) break;

	Distance++;
	if(Distance > nr) break;  // The distance cannot exceed the number of subpermutations

	if(bInRow){  // Backward move
	  int min_Xj = nr+1;
	  for(int j=max_ij + 1; j<=nr; j++){
		if(a[max_ij][j] != 0){
		  if(min_Xj > r[j].x2){
			min_Xj = r[j].x2;
		  }
		}
	  }
	  Suma_i[max_ij] = 0;

	  // update the positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 >= min_Xj && r[i].x2 < r[max_ij].x2){
		  a[i][max_ij] = 1;
		  Suma_i[i]++;
		  Suma_j[max_ij]++;
		  r[i].x2 += r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 >= min_Xj && r[j].x2 < r[max_ij].x2){
		  a[max_ij][j] = 0;
		  Suma_j[j]--;
		  r[j].x2 += r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = min_Xj;
	}
	else{  // Forward move
	  int max_Xi = -1, max_i;
	  for(int i=0; i<max_ij; i++){
		if(a[i][max_ij] != 0){
		  if(max_Xi < r[i].x2){
			max_Xi = r[i].x2;
			max_i = i;
		  }
		}
	  }
	  Suma_j[max_ij] = 0;

	  // update hte positions of the subpermutations in rho_2
	  for(int i=0; i<max_ij; i++){
		if(r[i].x2 > r[max_ij].x2 && r[i].x2 <= max_Xi){
		  a[i][max_ij] = 0;
		  Suma_i[i]--;
		  r[i].x2 -= r[max_ij].l;
		}
	  }
	  for(int j=max_ij + 1; j<=nr; j++){
		if(r[j].x2 > r[max_ij].x2 && r[j].x2 <= max_Xi){
		  a[max_ij][j] = 1;
		  Suma_i[max_ij]++;
		  Suma_j[j]++;
		  r[j].x2 -= r[max_ij].l;
		}
	  }
	  r[max_ij].x2 = r[max_i].x2 + r[max_i].l;
	}
  }while(true);

  delete [] v1;
  delete [] v2;

  return Distance;
}

int DMOSP_MOSS::Distance(DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2, int &jD, int &wcD)
{
  jD = wcD = 0;

  for(int j=0; j<nJobs; j++){
	jD += TauDistance(j, S1, S2);
  }
  for(int w=0; w<nWCs; w++){
	wcD += WCDistance(w, S1, S2);
  }

  return (jD + wcD);
}

bool DMOSP_MOSS::isDistanceGreaterThan(DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2, int DistLimit)
{
  int D = 0;

  for(int j=0; j<nJobs; j++){
	if(D > DistLimit) break;
	D += TauDistance(j, S1, S2);
  }

  for(int w=0; w<nWCs; w++){
	if(D > DistLimit) break;
	D += WCDistance(w, S1, S2);
  }

  if(D > DistLimit) return true;
  return false;
}

int DMOSP_MOSS::SuggestTabuSize()
{
   // This function is intented to suggest a maximum tabu size value
   double max_tsz = 0;

   // 1) For each workstation, count the number of jobs then divide that by 3
   // and then add the closest higher integer to the maximum tabu size
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)pcnd->Data();
	 float Nj = 0;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   Nj++;
	 }
	 max_tsz += Nj/3.0;
	 pcnd = pcnd->Next();
   }

   // 2) For each job count the number of workstations that need to be visited
   // then divide that by 3 and then add the closest higher integer to the
   // maximum tabu size value
   for(int j=0; j<nJobs; j++){
	 float nWC = Jz[j] - Jz[j] + 1;
	 max_tsz += nWC/3.0;
   }

   max_tsz = ceil(max_tsz);

   return int(max_tsz);
}

void DMOSP_MOSS::SetTabuSize(int ts)
{
  delete [] TabuMoves;
  TabuSize = ts;
  TabuMoves = new NSMove[TabuSize];
  ClearTabus();
}

void DMOSP_MOSS::SetTSnIterations(int nitr)
{
  nTSiterations = nitr;
}

void DMOSP_MOSS::SetTSnBestMoves(int nbst)
{
  nBestMoves = nbst;
  delete [] bestMoves;
  bestMoves = new NSMove [nBestMoves];
}

void DMOSP_MOSS::SetTSnRandMoves(int nrnd)
{
  nRandMoves = nrnd;
}

void DMOSP_MOSS::TabuSearch(DMOSP_MOSS_Solution *S, bool bAllRandom, bool bGenerateNet)
{
  // Apply Tabu Search on the given solution to improve it
  if(bGenerateNet){
	S->GenerateNetwork();
	if(!S->InterpretNetwork()) return;
  }

  ClearTabus();
  startingSol->Copy(S);
  bestLocalSol->Copy(S);

  bool bDupdated = UpdateNonDominatedSolutions(bestLocalSol);
  if(bDupdated && frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(D);
  initialLocalCmax = bestLocalCmax = S->Cmax;
  initialLocalMWFT = bestLocalMWFT = S->MWFT;

  if(frmMOSSCalcProgress->bActive && frmMOSSCalcProgress->cbxTabuSearch->Checked){
	frmMOSSCalcProgress->lblTSInitialCmax->Caption = FloatToStr(initialLocalCmax);
	frmMOSSCalcProgress->lblTSCurrentCmax->Caption = FloatToStr(initialLocalCmax);
	frmMOSSCalcProgress->lblTSBestCmax->Caption = FloatToStr(bestLocalCmax);
	frmMOSSCalcProgress->lblTSInitialMWFT->Caption = FloatToStr(initialLocalMWFT);
	frmMOSSCalcProgress->lblTSCurrentMWFT->Caption = FloatToStr(initialLocalMWFT);
	frmMOSSCalcProgress->lblTSBestMWFT->Caption = FloatToStr(bestLocalMWFT);
	frmMOSSCalcProgress->lblTSIter->Caption = "0";
	frmMOSSCalcProgress->lblTSImpIter->Caption = "0";
	frmMOSSCalcProgress->pbTabuSearch->Min = 0;
	frmMOSSCalcProgress->pbTabuSearch->Max = nTSiterations;
	frmMOSSCalcProgress->pbTabuSearch->Position = 0;
  }
  if(frmMOSSOperators->bActive && frmMOSSOperators->bTSStarted){
	frmMOSSOperators->lblCmax->Caption = FloatToStr(initialLocalCmax);
	frmMOSSOperators->lblMWFT->Caption = FloatToStr(initialLocalMWFT);
	frmMOSSOperators->lblBestCmax->Caption = FloatToStr(bestLocalCmax);
	frmMOSSOperators->lblBestMWFT->Caption = FloatToStr(bestLocalMWFT);
	frmMOSSOperators->pbTSIterations->Max = nTSiterations;
	frmMOSSOperators->pbTSIterations->Position = 0;
  }

  int itr;        // current iteration number
  int itr_noimp;  // Number of iterations since last found best solution
  int itr_rand;   // Number of random Cmax iterations

  itr = itr_noimp = itr_rand = 0;

  float nCimp = 1.0, nFimp = 1.0;
  while(itr < nTSiterations){
	if(flip(nFimp / (nCimp + nFimp))){
	  EnumerateCmaxNonTabuMoves(S);
	  if(frmMOSSOperators->bActive && frmMOSSOperators->bTSStarted){
		frmMOSSOperators->IncrementN1N2();
		frmMOSSOperators->updateNimp(nCimp, nFimp);
	  }
	}
	else{
	  EnumerateMWFTNonTabuMoves(S);
	  if(frmMOSSOperators->bActive && frmMOSSOperators->bTSStarted){
		frmMOSSOperators->IncrementN3N4();
		frmMOSSOperators->updateNimp(nCimp, nFimp);
	  }
	}

	if(nMoves == 0){  // This should not happen unless the tabu size is very large
	  ClearTabus();
	  continue;
	}

	int iselNSMove;       // index of selected move
	if(bAllRandom) iselNSMove = rnd(0, nMoves-1);
	else {
	  if(itr_noimp == nImpIterations){
		itr_rand = nRandMoves;
		itr_noimp = 0;
	  }
	  if(itr_rand != 0){
		iselNSMove = rnd(0, nMoves-1);
		itr_rand--;
	  }
	  else iselNSMove = 0;  // Select the best one for Cmax
	}

	if(ApplyNandUpdateTabuList(S, bestMoves[iselNSMove]) == -1){
	  // If the application of the NSMove results in an infeasible solution,
	  //    revert to the best local solution and continue the search.
	  S->Copy(startingSol);
	  continue;
	};

	float delta_C = (bestLocalCmax > S->Cmax)? 1.0 : ((bestLocalCmax == S->Cmax)? 0.0 : -1.0);
	float delta_F = (bestLocalMWFT > S->MWFT)? 1.0 : ((bestLocalMWFT == S->MWFT)? 0.0 : -1.0);

	if(  (delta_C > 0.0 && delta_F >= 0.0)      // If the new solution S dominates
	  || (delta_F > 0.0 && delta_C >= 0.0)){    // the best local solution
	  bestLocalSol->Copy(S);
	  bestLocalCmax = S->Cmax;
	  bestLocalMWFT = S->MWFT;
	  bestLocalSol->GenerateVPBStrings();  // <-- update chromosomes of best found solution
	  bDupdated = UpdateNonDominatedSolutions(bestLocalSol);
	  if(bDupdated) bNonDominatedSetUpdated = true;
	  if(bDupdated && frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(D);
	  itr_noimp = -1;
	}
	else if((delta_C > 0.0 && delta_F < 0.0)       // If S does not dominate
			|| (delta_C < 0.0 && delta_F > 0.0)){   // the best locat solution nor the opposite
	  bDupdated = UpdateNonDominatedSolutions(S);
	  if(bDupdated) bNonDominatedSetUpdated = true;
	  if(bDupdated && frmMOSSCalcProgress->bActive) frmMOSSCalcProgress->DrawParetoFront(D);
	  if(bDupdated){
		bestLocalSol->Copy(S);
		bestLocalCmax = S->Cmax;
		bestLocalMWFT = S->MWFT;
		bestLocalSol->GenerateVPBStrings();  // <-- update chromosomes of best found solution
		itr_noimp = -1;
	  }
	}

	nCimp += (delta_C > 0.0)? 1.0 : 0.0;
	nFimp += (delta_F > 0.0)? 1.0 : 0.0;

	itr_noimp++;
	itr++;

	// Check if the global best solutions can be updated or not based
	// on the newly found solution
	//
	if(bestLocalCmax < bestCmax || (bestLocalCmax == bestCmax && bestLocalMWFT < bestCmaxSol->MWFT)){
	  bestCmax = bestLocalCmax;
	  bestCmaxSol->Copy(bestLocalSol);
	  if(frmMOSSCalcProgress->bActive)
		frmMOSSCalcProgress->lblbestCmax->Caption = FloatToStr(bestCmax);
	}
	if(bestLocalMWFT < bestMWFT || (bestLocalMWFT == bestMWFT && bestLocalCmax < bestMWFTSol->Cmax)){
	  bestMWFT = bestLocalMWFT;
	  bestMWFTSol->Copy(bestLocalSol);
	  if(frmMOSSCalcProgress->bActive)
		frmMOSSCalcProgress->lblbestMWFT->Caption = FloatToStr(bestMWFT);
	}

	if(bestCmax == CmaxLB && bestMWFT == MWFTLB){
	  if(*bestCmaxSol == *bestMWFTSol){
		bOptSolFound = true;
		break;
	  }
	}

	Application->ProcessMessages();
	if(frmMOSSCalcProgress->bActive && frmMOSSCalcProgress->cbxTabuSearch->Checked){
	  frmMOSSCalcProgress->lblTSCurrentCmax->Caption = FloatToStr(S->Cmax);
	  frmMOSSCalcProgress->lblTSBestCmax->Caption = FloatToStr(bestLocalCmax);
	  frmMOSSCalcProgress->lblTSCurrentMWFT->Caption = FloatToStr(S->MWFT);
	  frmMOSSCalcProgress->lblTSBestMWFT->Caption = FloatToStr(bestLocalMWFT);
	  frmMOSSCalcProgress->lblTSIter->Caption = IntToStr(itr);
	  frmMOSSCalcProgress->lblTSImpIter->Caption = IntToStr(itr_noimp);
	  frmMOSSCalcProgress->pbTabuSearch->Position = itr;
	}
	if(frmMOSSOperators->bActive && frmMOSSOperators->bTSStarted){
	  frmMOSSOperators->lblCmax->Caption = FloatToStr(S->Cmax);
	  frmMOSSOperators->lblMWFT->Caption = FloatToStr(S->MWFT);
	  frmMOSSOperators->lblBestCmax->Caption = FloatToStr(bestLocalCmax);
	  frmMOSSOperators->lblBestMWFT->Caption = FloatToStr(bestLocalMWFT);
	  frmMOSSOperators->lblTSItr->Caption = IntToStr(itr);
	  frmMOSSOperators->pbTSIterations->Position = itr;
	}
	if(bInterrupt){
	  bTerminate = true;
	  break;
	}
  }

  S->Copy(bestLocalSol);
  if(frmMOSSOperators->bActive && frmMOSSOperators->bTSStarted){
    frmMOSSOperators->TSSol->Copy(bestLocalSol);
  }
}

void DMOSP_MOSS::AddTabu(NSFunction F, int v, int x, int m)
{
  if(TabuSize == 0) return;
  if(TabuPntr == TabuSize) TabuPntr = 0;

  TabuMoves[TabuPntr].F = F;
  TabuMoves[TabuPntr].v = v;
  TabuMoves[TabuPntr].x = x;
  TabuMoves[TabuPntr].m2 = (F==N1)? -1 : m;

  TabuPntr++;
  if(nTabus < TabuSize) nTabus++;
}

void DMOSP_MOSS::ClearTabus()
{
  memset((void *)TabuMoves, -1, sizeof(NSMove)*TabuSize);
  TabuPntr = nTabus = 0;
}

bool DMOSP_MOSS::isTabu(NSFunction F, int v, int x, int m)
{
  if(TabuSize == 0) return false;
  bool ret = false;
  for(int i=0; i<nTabus; i++){
	if(TabuMoves[i].F != F) continue;
	if(TabuMoves[i].v != v) continue;
	if(TabuMoves[i].x != x) continue;
	if(F == N1){
	  ret = true;
	  break;
	}
	if(TabuMoves[i].m2 != m) continue;
	ret = true;
	break;
  }
  return ret;
}

void DMOSP_MOSS::EnumerateCmaxNonTabuMoves(DMOSP_MOSS_Solution *Sol)
{
  if(Sol->Cmax == -1) return;    // Network must be interpretted

  // Enumerate all possible non-tabu moves and store the best ones in bestMoves.
  // Meanwhile, if debugging info is needed, output all possible moves
  // info and the best ones.
  //
  int alpha, omega, minX;

  nMoves = 0;
  for(int i=0; i<nOps; i++){
	DMOSP_MOSS_Node &nd = Sol->Nodes[i];
	if(!nd.bCritical) continue;

    Sol->N1Results(i, alpha, omega, apr_Cmax, minX);

	if(minX > 0){
	  // Check if this best move is tabu and if this is the case try to choose
	  // a different insertion postion if possible.
	  // If unable to find a non-tabu move within the given range, let minX = -1
	  // so that no new move will be added.
	  if(isTabu(N1, i, minX) && apr_Cmax[minX] >= 3*bestCmax/4){
		int xr, xl;    // right and left positions of best non-tabu Cmax
		double rCmax, lCmax;  // best right and left makspan values

		minX = -1;
		rCmax = lCmax = MAXFLOAT;
		for(xr = minX + 1; xr <= omega; xr++){
		  if(!isTabu(N1, i, xr)  || apr_Cmax[xr] < 3*bestCmax/4){
			rCmax = apr_Cmax[xr];
			break;
		  }
		}
		for(xl = minX - 1; xl >= alpha; xl--){
		  if(!isTabu(N1, i, xl) || apr_Cmax[xl] < 3*bestCmax/4){
			lCmax = apr_Cmax[xl];
			break;
		  }
		}
		if(rCmax != MAXFLOAT || lCmax != MAXFLOAT){
		  if(rCmax < lCmax) minX = xr;
		  else minX = xl;
		}
	  }
	}
	if(minX > 0){
	  if(nMoves == 0){
		bestMoves[0].F = DMOSP_MOSS::N1;
		bestMoves[0].v = i;
		bestMoves[0].x = minX;
		bestMoves[0].aprCmax = apr_Cmax[minX];
		nMoves++;
	  }
	  else{
		bool bInserted = false;
		int m = 0;
		while(m < nMoves){
		  if(bestMoves[m].aprCmax > apr_Cmax[minX]){
			for(int n = nBestMoves-1; n > m; n--){
			  memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(DMOSP_MOSS::NSMove));
			}
			bestMoves[m].F = DMOSP_MOSS::N1;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].aprCmax = apr_Cmax[minX];

			bInserted = true;
			break;
		  }
		  m++;
		}
		if(!bInserted && nMoves < nBestMoves){
		  m = nMoves++;
		  bestMoves[m].F = DMOSP_MOSS::N1;
		  bestMoves[m].v = i;
		  bestMoves[m].x = minX;
		  bestMoves[m].aprCmax = apr_Cmax[minX];
		}
	  }
	}

	SList::SNode *mcnd = O[i]->AltMCs.head();
	while(mcnd){
	  DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
	  DMOSP_Problem::SMC *mc = opmc->MC;

      Sol->N2Results(i, mc->i, alpha, omega, apr_Cmax, minX);

	  if(minX > 0){
		// Check if this best move is tabu and if this is the case try to choose
		// a different insertion postion if possible.
		// If unable to find a non-tabu move within the given range, let minX = -1
		// so that no new move will be added.
		if(isTabu(N2, i, minX, mc->i) && apr_Cmax[minX] >= 3.0*bestCmax/4.0){
		  int xr, xl;    // right and left positions of best non-tabu Cmax
		  double rCmax, lCmax;  // best right and left makspan values

		  minX = -1;
		  rCmax = lCmax = MAXFLOAT;
		  for(xr = minX + 1; xr <= omega; xr++){
			if(!isTabu(N2, i, xr, mc->i)  || apr_Cmax[xr] < 3*bestCmax/4){
			  rCmax = apr_Cmax[xr];
			  break;
			}
		  }
		  for(xl = minX - 1; xl >= alpha; xl--){
			if(!isTabu(N2, i, xl, mc->i) || apr_Cmax[xl] < 3*bestCmax/4){
			  lCmax = apr_Cmax[xl];
			  break;
			}
		  }
		  if(rCmax != MAXFLOAT || lCmax != MAXFLOAT){
			if(rCmax < lCmax) minX = xr;
			else minX = xl;
		  }
		}
	  }
	  if(minX > 0){
		if(nMoves == 0){
		  bestMoves[0].F = DMOSP_MOSS::N2;
		  bestMoves[0].v = i;
		  bestMoves[0].x = minX;
		  bestMoves[0].m2 = mc->i;
		  bestMoves[0].aprCmax = apr_Cmax[minX];
		  nMoves++;
		}
		else{
		  bool bInserted = false;
		  int m = 0;
		  while(m < nMoves){
			if(bestMoves[m].aprCmax > apr_Cmax[minX]){

			  for(int n = nBestMoves-1; n > m; n--){
				memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(DMOSP_MOSS::NSMove));
			  }
			  bestMoves[m].F = DMOSP_MOSS::N2;
			  bestMoves[m].v = i;
			  bestMoves[m].x = minX;
			  bestMoves[m].m2 = mc->i;
			  bestMoves[m].aprCmax = apr_Cmax[minX];

			  bInserted = true;
			  break;
			}
			m++;
		  }
		  if(!bInserted && nMoves < nBestMoves){
			m = nMoves++;
			bestMoves[m].F = DMOSP_MOSS::N2;
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].m2 = mc->i;
			bestMoves[m].aprCmax = apr_Cmax[minX];
		  }
		}
	  }
	  mcnd = mcnd->Next();
	}
  }
}

void DMOSP_MOSS::EnumerateMWFTNonTabuMoves(DMOSP_MOSS_Solution *Sol)
{
  if(Sol->Cmax == -1) return;    // Network must be interpretted

  // Enumerate all possible non-tabu moves and store the best ones in bestMoves.
  // Meanwhile, if debugging info is needed, output all possible moves
  // info and the best ones.
  //
  int alpha, omega, minX;

  nMoves = 0;
  for(int i=0; i<nOps; i++){
	DMOSP_MOSS_Node &nd = Sol->Nodes[i];

	// Check possible movements in tau
	//
	if(nd.bCritical || Sol->tau_pos[i] >= Sol->tau[Sol->jb[i]][0] / 2){
	  // This is done only to critical operations or operations that are
	  //   positioned in the second half of the corresponding tau
	  Sol->N3Results(i, alpha, omega, deltaMWFT, minX);
	  if(minX > 0){
		// Check if this best move is tabu and if this is the case try to choose
		// a different insertion postion if possible.
		// If unable to find a non-tabu move within the given range, let minX = -1
		// so that no new move will be added.
		if(isTabu(N1, i, minX)){
		  int xr, xl;    // right and left positions of best non-tabu Cmax
		  double rdMWFT, ldMWFT;  // best right and left MWFT values

		  minX = -1;
		  rdMWFT = ldMWFT = MAXFLOAT;
		  for(xr = minX + 1; xr <= omega; xr++){
			if(!isTabu(N1, i, xr)){
			  rdMWFT = deltaMWFT[xr];
			  break;
			}
		  }
		  for(xl = minX - 1; xl >= alpha; xl--){
			if(!isTabu(N1, i, xl)){
			  ldMWFT = deltaMWFT[xl];
			  break;
			}
		  }
		  if(rdMWFT != MAXFLOAT || ldMWFT != MAXFLOAT){
			if(rdMWFT < ldMWFT) minX = xr;
			else minX = xl;
		  }
		}
	  }
	  if(minX > 0){
		if(nMoves == 0){
		  bestMoves[0].F = DMOSP_MOSS::N1;   // Keep this N1 for tabu list consistency
		  bestMoves[0].v = i;
		  bestMoves[0].x = minX;
		  bestMoves[0].deltaMWFT = deltaMWFT[minX];
		  nMoves++;
		}
		else{
		  bool bInserted = false;
		  int m = 0;
		  while(m < nMoves){
			if(bestMoves[m].deltaMWFT > deltaMWFT[minX]){
			  for(int n = nBestMoves-1; n > m; n--){
				memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(DMOSP_MOSS::NSMove));
			  }
			  bestMoves[m].F = DMOSP_MOSS::N1;  // Keep this N1 for tabu list consistency
			  bestMoves[m].v = i;
			  bestMoves[m].x = minX;
			  bestMoves[m].deltaMWFT = deltaMWFT[minX];

			  bInserted = true;
			  break;
			}
			m++;
		  }
		  if(!bInserted && nMoves < nBestMoves){
			m = nMoves++;
			bestMoves[m].F = DMOSP_MOSS::N1;   // Keep this N1 for tabu list consistency
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].deltaMWFT = deltaMWFT[minX];
		  }
		}
	  }
	}

	// Check possible movements in PI
	//
	SList::SNode *mcnd = O[i]->AltMCs.head();
	while(mcnd){
	  DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
	  DMOSP_Problem::SMC *mc = opmc->MC;

	  Sol->N4Results(i, mc->i, alpha, omega, deltaMWFT, minX);

	  if(minX > 0){
		// Check if this best move is tabu and if this is the case try to choose
		// a different insertion postion if possible.
		// If unable to find a non-tabu move within the given range, let minX = -1
		// so that no new move will be added.
		if(isTabu(N2, i, minX, mc->i)){
		  int xr, xl;    // right and left positions of best non-tabu Cmax
		  double rdMWFT, ldMWFT;  // best right and left MWFT values

		  minX = -1;
		  rdMWFT = ldMWFT = MAXFLOAT;
		  for(xr = minX + 1; xr <= omega; xr++){
			if(!isTabu(N2, i, xr, mc->i)){
			  rdMWFT = deltaMWFT[xr];
			  break;
			}
		  }
		  for(xl = minX - 1; xl >= alpha; xl--){
			if(!isTabu(N2, i, xl, mc->i)){
			  ldMWFT = deltaMWFT[xl];
			  break;
			}
		  }
		  if(rdMWFT != MAXFLOAT || ldMWFT != MAXFLOAT ){
			if(rdMWFT < ldMWFT) minX = xr;
			else minX = xl;
		  }
		}
	  }
	  if(minX > 0){
		if(nMoves == 0){
		  bestMoves[0].F = DMOSP_MOSS::N2;  // Keep this N2 for tabu list consistency
		  bestMoves[0].v = i;
		  bestMoves[0].x = minX;
		  bestMoves[0].m2 = mc->i;
		  bestMoves[0].deltaMWFT = deltaMWFT[minX];
		  nMoves++;
		}
		else{
		  bool bInserted = false;
		  int m = 0;
		  while(m < nMoves){
			if(bestMoves[m].deltaMWFT > deltaMWFT[minX]){
			  for(int n = nBestMoves-1; n > m; n--){
				memcpy((void *)&bestMoves[n], (const void *)&bestMoves[n-1], sizeof(DMOSP_MOSS::NSMove));
			  }
			  bestMoves[m].F = DMOSP_MOSS::N2;   // Keep this N2 for tabu list consistency
			  bestMoves[m].v = i;
			  bestMoves[m].x = minX;
			  bestMoves[m].m2 = mc->i;
			  bestMoves[m].deltaMWFT = deltaMWFT[minX];

			  bInserted = true;
			  break;
			}
			m++;
		  }
		  if(!bInserted && nMoves < nBestMoves){
			m = nMoves++;
			bestMoves[m].F = DMOSP_MOSS::N2;   // Keep this N2 for tabu list consistency
			bestMoves[m].v = i;
			bestMoves[m].x = minX;
			bestMoves[m].m2 = mc->i;
			bestMoves[m].deltaMWFT = deltaMWFT[minX];
		  }
		}
	  }
	  mcnd = mcnd->Next();
	}
  }
}

int DMOSP_MOSS::ApplyNandUpdateTabuList(DMOSP_MOSS_Solution *Sol, NSMove mv)
{
  // This function modifies the solution vectors by conducting the received move
  // mv, then the solution vectors are interpretted to network representation
  // and all start and finish times are calculated.
  // Note: It is assumed that the received move will generate a feasible solution.

  NSMove Inverse;  // inverse move to the one being applied
  Inverse.F = mv.F;
  Inverse.v = mv.v;

  if(mv.F == N1){
	int j = Sol->jb[mv.v];
    int *&tau = Sol->tau[j];
	int oldx = Sol->tau_pos[mv.v];
	if(oldx == mv.x) return Sol->Cmax;  // There is nothing to do!

    Inverse.m2 = -1;
	Inverse.x = oldx;

	if(oldx < mv.x){
	  int x;
	  for(x=oldx; x<mv.x; x++){
		tau[x] = tau[x+1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
	else{
	  int x;
	  for(x=oldx; x>mv.x; x--){
		tau[x] = tau[x-1];
		Sol->tau_pos[tau[x]] = x;
	  }
	  tau[x] = mv.v;
	  Sol->tau_pos[mv.v] = x;
	}
  }
  else{
	int m1 = Sol->mc[mv.v];

	Inverse.m2 = m1;

	if(mv.m2 == m1){
	  int *&pi = Sol->pi[mv.m2];
	  int oldx = Sol->pi_pos[mv.v];
	  if(oldx == mv.x) return Sol->Cmax; // There is nothing to do

	  Inverse.x = oldx;

	  if(oldx < mv.x){
		int x;
		for(x=oldx; x<mv.x; x++){
		  pi[x] = pi[x+1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  else{
        int x;
		for(x=oldx; x>mv.x; x--){
		  pi[x] = pi[x-1];
		  Sol->pi_pos[pi[x]] = x;
		}
		pi[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	}
	else{
	  int *&pi1 = Sol->pi[m1];
	  int curx = Sol->pi_pos[mv.v];

      Inverse.x = curx;

	  for(int x=curx; x<pi1[0]; x++){
		pi1[x] = pi1[x+1];
		Sol->pi_pos[pi1[x]] = x;
	  }
	  pi1[0]--;

	  int *&pi2 = Sol->pi[mv.m2];
	  if(mv.x > pi2[0]){
		pi2[++pi2[0]] = mv.v;
		Sol->pi_pos[mv.v] = mv.x;
	  }
	  else{
		int x;
		for(x=++pi2[0]; x>mv.x; x--){
		  pi2[x] = pi2[x-1];
		  Sol->pi_pos[pi2[x]] = x;
		}
		pi2[x] = mv.v;
		Sol->pi_pos[mv.v] = x;
	  }
	  Sol->mc[mv.v] = mv.m2;
	}
  }

  Sol->GenerateNetwork();
  if(Sol->InterpretNetwork()){
	if(isTabu(Inverse.F, Inverse.v, Inverse.x, Inverse.m2))
	  // If the inverse move is currently tabu, add the applied move to the tabu list instead
	  AddTabu(mv.F, mv.v, mv.x, mv.m2);
	else
	  // Now add the inverse of the applied move to the tabu list
	  AddTabu(Inverse.F, Inverse.v, Inverse.x, Inverse.m2);

	return Sol->Cmax;
  }
  return -1;
}

//---------------------------------------------- Solution Combination function

DMOSP_MOSS_Solution *DMOSP_MOSS::CombinedFrom(DMOSP_MOSS_Solution *SL,
											 DMOSP_MOSS_Solution *SF)
{
		#ifdef PRNT_MOSS_DBG
		  SL->echoVPBChromosome(mossDbgFile, L" Leader parent solution");
		  SF->echoVPBChromosome(mossDbgFile, L" Follower parent solution");
		#endif

  DMOSP_MOSS_Solution *SC = new DMOSP_MOSS_Solution(this);

  // Repeate the solution combination process 3 times until a feasible solution
  // is found. If none can be found, the combined solution will be one of the
  // parent solutions selected randomly
  //
  bool bFeasibleFound = false;
  for(int trial=0; trial<5; trial++){
	// Combine solutions S1 and S2 to generate solution S3, which is returned
	// by this function.
	memset((void *)jbstrGeneSel, 0, nOps * sizeof(bool));
	memset((void *)wsstrGeneSel, 0, nOps * sizeof(bool));

	// Inherit genes from the job strings of the two parent solutions
	DMOSP_VPBString *jbstrL = SL->jbString;
	DMOSP_VPBString *jbstrF = SF->jbString;
	for(int s=0; s<jbstrL->nSubstrings; s++){
		DMOSP_VPBSubString &substrL = jbstrL->Substring[s];
		DMOSP_VPBSubString &substrF = jbstrF->Substring[s];
		if(flip(Theta_X)){
		  int p = rnd(substrL.start, substrL.end);
		  int g = substrL.start;
		  while(g < p){
			DMOSP_Problem::SOperation *op = jbstrL->Gene[g];
			SC->jbString->Gene[g] = op;
			jbstrGeneSel[op->i] = true;
			g++;
		  }
		  int fg = substrF.start;
		  while(g <= substrL.end && fg <= substrF.end){
			DMOSP_Problem::SOperation *op = jbstrF->Gene[fg];
			while(jbstrGeneSel[op->i]){
			  fg++;
			  op = jbstrF->Gene[fg];
			}
			SC->jbString->Gene[g] = op;
			jbstrGeneSel[op->i] = true;
			g++;
			fg++;
		  }
		  if(frmMOSSOperators->bActive){  // record job crossover
			DMOSP_xOverElement *xoverel = new DMOSP_xOverElement();
            xoverel->type = 1;
			wcscpy(xoverel->ID, J[s]->ID());
			xoverel->pos = p;
			frmMOSSOperators->xPos.Add((void *)xoverel);
		  }
		}
		else{ // copy the substring as is from the leader parent
		  int g = substrL.start;
		  while(g <= substrL.end){
			DMOSP_Problem::SOperation *op = jbstrL->Gene[g];
			SC->jbString->Gene[g] = op;
			jbstrGeneSel[op->i] = true;
			g++;
		  }
		}
	}

	// Inherit genes from the WS string of the two parent solutions
	DMOSP_VPBString *wsstrL = SL->wsString;
	DMOSP_VPBString *wsstrF = SF->wsString;
	for(int s=0; s<wsstrL->nSubstrings; s++){
		DMOSP_VPBSubString &substrL = wsstrL->Substring[s];
		DMOSP_VPBSubString &substrF = wsstrF->Substring[s];
		if(flip(Theta_X)){
		  int p = rnd(substrL.start, substrL.end);
		  int g = substrL.start;
		  while(g < p){
			DMOSP_Problem::SOperation *op = wsstrL->Gene[g];
			SC->wsString->Gene[g] = op;
			wsstrGeneSel[op->i] = true;
			g++;
		  }
		  int fg = substrF.start;
		  while(g <= substrL.end && fg <= substrF.end){
			DMOSP_Problem::SOperation *op = wsstrF->Gene[fg];
			while(wsstrGeneSel[op->i]){
			  fg++;
			  op = wsstrF->Gene[fg];
			}
			SC->wsString->Gene[g] = op;
			wsstrGeneSel[op->i] = true;
			g++;
			fg++;
		  }

		  // Copy machine separators from the sub-string with the highest number
		  // of inherited genes
		  DMOSP_VPBSubString &substrC = SC->wsString->Substring[s];
		  if(p >= substrL.start + (substrL.end - substrL.start) / 2)
			memcpy(substrC.SeparatorPos, substrL.SeparatorPos, substrL.nSeparators * sizeof(int));
		  else
			memcpy(substrC.SeparatorPos, substrF.SeparatorPos, substrF.nSeparators * sizeof(int));

		  if(frmMOSSOperators->bActive){  // record job crossover
			DMOSP_xOverElement *xoverel = new DMOSP_xOverElement();
			xoverel->type = 2;
			wcscpy(xoverel->ID, W[s]->ID());
			xoverel->pos = p;
			frmMOSSOperators->xPos.Add((void *)xoverel);
		  }
		}
		else {
		  int g = substrL.start;
		  while(g <= substrL.end){
			DMOSP_Problem::SOperation *op = wsstrL->Gene[g];
			SC->wsString->Gene[g] = op;
			wsstrGeneSel[op->i] = true;
			g++;
		  }
		  DMOSP_VPBSubString &substrC = SC->wsString->Substring[s];
          memcpy(substrC.SeparatorPos, substrL.SeparatorPos, substrL.nSeparators * sizeof(int));
		}
	}

		#ifdef PRNT_MOSS_DBG
		  SC->echoVPBChromosome(mossDbgFile, L"Generated child chromosome");
		#endif

	// Now, interpret the combined solutions's strings into a valid solution
	SC->bVPBConstructed = true;
	SC->GenerateVectors();

		#ifdef PRNT_MOSS_DBG
          SC->echoVechors(mossDbgFile, L"Child solution");
		#endif

	SC->GenerateNetwork();
	if(SC->InterpretNetwork()){
		#ifdef PRNT_MOSS_DBG
		  SC->echoNetwork(mossDbgFile, L"Child solution");
		#endif

	  bFeasibleFound = true;
	  break;
	}
  }

  if(!bFeasibleFound){
	delete SC;
	SC = new DMOSP_MOSS_Solution(this);
	if(flip(0.5)) SC->Copy(SL);
	else SC->Copy(SF);
  }

  return SC;
}

//--------------------------------------- General solution calling functions

void DMOSP_MOSS::ChooseNDSolution(int index)
{
  if(index < 0 || index >= ND.Count()) return;
  DMOSP_Solution *Sol = (DMOSP_Solution *)ND[index];

  Solution->Copy(Sol);
}

DMOSP_MOSS_Solution *DMOSP_MOSS::GetDSolution(int index)
{
  if(index < 0 || index >= D.Count()) return NULL;
  DMOSP_MOSS_Solution *Sol = (DMOSP_MOSS_Solution *)D[index];
  return Sol;
}

void DMOSP_MOSS::AddDSol(DMOSP_MOSS_Solution *s)
{
  s->ConvertNetToTasks();
  DMOSP_MOSS_Solution *s_ = new DMOSP_MOSS_Solution(*s);
  D.Add((void *)s_);
  DMOSP_Solution *dmosps = new DMOSP_Solution(Problem);
  dmosps->Copy(Solution);
  ND.Add((void *)dmosps);
}

int DMOSP_MOSS::dominates(DMOSP_MOSS_Solution *guy1, DMOSP_MOSS_Solution *guy2)
{
  int idomj = 0, jdomi = 0;

  if(guy1->Cmax < guy2->Cmax) idomj = 1;
  else if(guy2->Cmax < guy1->Cmax) jdomi = 1;
  if(guy1->MWFT < guy2->MWFT) idomj = 1;
  else if(guy2->MWFT < guy1->MWFT) jdomi = 1;

//#define IDOMJ        1
//#define JDOMI       -1
//#define INONDOMJ     0

  if((idomj == 1) && (jdomi == 1)) return 0;
  else if((idomj == 1)&&(jdomi == 0)) return 1;
  else if((idomj == 0)&&(jdomi == 1)) return -1;
  else return 0;
}

void DMOSP_MOSS::RankDSolutions()
{    // generate the rank values for solutions in set D
  // This function conducts the non-dominated sorting technique of NSGA-II
  // on the solutions stored in set D. It generates ranks and fronts accordingly.
  int ii, jj, fid, idomj, nif;
  int *domCount, **indDomByMe, *numIndDom;
  DMOSP_MOSS_Solution **theGuys;

  if(dSize > 0){  // delete previously allocated fronts if any
	delete [] numIndsFront;
	for(ii = 0; ii < dSize; ii++) delete [] paretoFront[ii];
	delete [] paretoFront;
  }

  dSize = D.Count();
  theGuys = new DMOSP_MOSS_Solution *[dSize];
  ii = 0;
  for(SList::SNode *nd=D.head(); nd; nd = nd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)nd->Data();
	theGuys[ii++] = sol;
  }

  numIndsFront = new int[dSize];
  paretoFront = new int*[dSize];
  for(ii = 0; ii < dSize; ii++) {
	paretoFront[ii] = new int[dSize];
  }

  domCount = new int[dSize];
  numIndDom = new int[dSize];
  indDomByMe = new int*[dSize];
  for(ii = 0; ii < dSize; ii++) {
	indDomByMe[ii] = new int[dSize];
	domCount[ii] = numIndDom[ii] = 0;
  }
  for(ii = 0; ii < dSize-1; ii++) {
	for(jj = ii+1; jj < dSize; jj++) {
	  idomj = dominates(theGuys[ii],theGuys[jj]);
	  if(idomj == 1) {
		indDomByMe[ii][numIndDom[ii]] = jj;
		numIndDom[ii] += 1;
		domCount[jj] += 1;
	  }
	  else if(idomj == -1) {
		indDomByMe[jj][numIndDom[jj]] = ii;
		numIndDom[jj] += 1;
		domCount[ii] += 1;
	  }
	}
  }

  nif = 0;     // number of individuals in the front
  for(ii = 0; ii < dSize; ii++) {
	if(domCount[ii] == 0) {
	  paretoFront[0][nif++] = ii;
	  theGuys[ii]->rank = 0;
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
		  theGuys[indDomByMe[paretoFront[fid][ii]][jj]]->rank = fid+1;
		}
	  }
	}
	fid += 1;
	numIndsFront[fid] = nif;
  }
  numFronts = fid;

  for(ii = 0; ii < dSize; ii++)
	delete indDomByMe[ii];
  delete []domCount;
  delete []numIndDom;
  delete []indDomByMe;
  delete []theGuys;
}

void DMOSP_MOSS::ComputeObjectivesStatistics()
{
  minCmax = minMWFT = MAXFLOAT;
  maxCmax = maxMWFT = 0.0;
  avgCmax = avgMWFT = 0;
  varCmax = varMWFT = 0;
  int n =  D.Count();
  for(SList::SNode *nd = D.head(); nd; nd = nd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)nd->Data();
	if(sol->Cmax < minCmax) minCmax = sol->Cmax;
	if(sol->Cmax > maxCmax) maxCmax = sol->Cmax;
	if(sol->MWFT < minMWFT) minMWFT = sol->MWFT;
	if(sol->MWFT > maxMWFT) maxMWFT = sol->MWFT;
	avgCmax += sol->Cmax;
	avgMWFT += sol->MWFT;
  }
  avgCmax /= (double)n;
  avgMWFT /= (double)n;
  for(SList::SNode *nd = D.head(); nd; nd = nd->Next()){
	DMOSP_MOSS_Solution *sol = (DMOSP_MOSS_Solution *)nd->Data();
	varCmax += pow(avgCmax - sol->Cmax, 2.0);
	varMWFT += pow(avgMWFT - sol->MWFT, 2.0);
  }
  varCmax /= (double)(n-1);
  varMWFT /= (double)(n-1);
}

void DMOSP_MOSS::ComputePerformanceMetrics()
{
  // Calculate Hypervolumd (HV)
  //  NOTE: the set D of non-dominated solutions should have been formed previously
  //
  D.QSort(MOSSIndComp);
  double CmaxLimit = 1.5 * CmaxLB;
  double MWFTLimit = 1.8 * MWFTLB;

  HV = 0.0;
  DMOSP_MOSS_Solution *prvS;
  double dCmax, dMWFT;
  bool bFirstSol = true;
  for(SList::SNode *pnd = D.head(); pnd; pnd = pnd->Next()){
	DMOSP_MOSS_Solution *S = (DMOSP_MOSS_Solution *)pnd->Data();
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
  int nP = D.Count();
  int nF = SolvedDMOSPs[prb_i].nSol;

  double **ED = new double *[nF];     // Eucludean distance matrix
  for(fs=0; fs<nF; fs++) ED[fs] = new double[nP];

  GD = IGD = HV0 = 0.0;
  for(fs=0; fs<nF; fs++){
	SList::SNode *pnd = D.head();
	for(ps=0; ps<nP; ps++){
	  DMOSP_MOSS_Solution *psol = (DMOSP_MOSS_Solution *)pnd->Data();
	  double d;
	  d = pow(psol->Cmax - SolvedDMOSPs[prb_i].Cmax[fs], 2.0)
		  + pow(psol->MWFT - SolvedDMOSPs[prb_i].MWFT[fs], 2.0);
	  ED[fs][ps] = d;
	  pnd = pnd->Next();
	}

	if(fs == 0){  // first solution with the maximum Cmax and minimum MWFT
	  HV0 += (  (CmaxLimit - SolvedDMOSPs[prb_i].Cmax[fs])
			  * (MWFTLimit - SolvedDMOSPs[prb_i].MWFT[fs]));
	}
	else {
	  HV0 += ((SolvedDMOSPs[prb_i].Cmax[fs-1] - SolvedDMOSPs[prb_i].Cmax[fs])
			  * (MWFTLimit - SolvedDMOSPs[prb_i].MWFT[fs]));
	}
  }
  HV0pr = HV0 / (0.5 * CmaxLB * 0.8 * MWFTLB) * 100.0;

  // to calculate GD
  for(ps=0; ps<nP; ps++){
	double minED = MAXFLOAT;
	for(fs=0; fs<nF; fs++) if(ED[fs][ps] < minED) minED = ED[fs][ps];
	GD += minED;
  }
  GD = pow(GD, 0.5) / nP;

  // to calculate IGD
  for(fs=0; fs<nF; fs++){
	double minED = MAXFLOAT;
	for(ps=0; ps<nP; ps++) if(ED[fs][ps] < minED) minED = ED[fs][ps];
	IGD += minED;
  }
  IGD = pow(IGD, 0.5) / nF;
  TGD = GD + IGD;

  for(fs=0; fs<nF; fs++) delete [] ED[fs];
  delete [] ED;
}

//------------------- Functions used to test N3 and N4 related operations
DMOSP_MOSS::DMOSP_MOSS(DMOSP_Problem *P, DMOSP_Solution *sol)
	: DMOSP_Metaheuristic(P, L"Multi-objective Scatter Search for DMOSP")
{
  pSize = 20;
  deltaThreshold = 20;
  TabuSize = 20;
  nTSiterations = 50;
  nImpIterations = 50;
  nBestMoves = 10;
  nRandMoves = 10;
  Theta_X = 0.8;
  nIterations = 100;

  startingSol = NULL;
  bestLocalSol = NULL;
  bestCmaxSol = NULL;
  bestMWFTSol = NULL;

  GD = IGD = -1;
  dSize = 0;  // number of individuals in set D used for generating fronts

  iSol = sol;
  NSTestSol = NULL;
}

void DMOSP_MOSS::Initialize4NSTests()
{
  if(iSol->Problem != Problem) return;

  // Copy tasks for the given initial solution to the current tasks
  for(int i=0; i<nOps; i++){
	DMOSP_Solution::SchTask *gtsk = iSol->TaskAssociatedToOperation(O[i]);
	memcpy(tsk[i], gtsk, sizeof(DMOSP_Solution::SchTask));
  }

  NSTestSol = new DMOSP_MOSS_Solution(this);
  NSTestSol->InterpretTaskList();
  NSTestSol->GenerateNetwork();
  NSTestSol->InterpretNetwork();

  // Create bestCmaxSol and bestMWFTSol just to be able to use the neighborhood
  // functions normally (Specifically ApplyN function)
  double bestCmax, Cmax, bestMWFT, MWFT;
  bestCmaxSol = new DMOSP_MOSS_Solution(this);
  DSG(MCTj, MWL, bestCmax, MWFT);
  bestCmaxSol->InterpretTaskList();
  bestMWFTSol = new DMOSP_MOSS_Solution(this);
  DSG(WLWR, MCTm, Cmax, bestMWFT);
  bestMWFTSol->InterpretTaskList();
}

void DMOSP_MOSS::FreeNSTestMemory()
{
  FreeMemory();
  if(NSTestSol) delete NSTestSol;
  if(bestCmaxSol) delete bestCmaxSol;
  if(bestMWFTSol) delete bestMWFTSol;
}

void DMOSP_MOSS::N3ExactResults(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  NSTestSol->N3ExactResults(v1, alpha, omega, deltaMWFT, minX);
}

void DMOSP_MOSS::N4ExactResults(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  NSTestSol->N4ExactResults(v2, k2, alpha, omega, deltaMWFT, minX);
}

void DMOSP_MOSS::N3Results(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  NSTestSol->N3Results(v1, alpha, omega, deltaMWFT, minX);
}

void DMOSP_MOSS::N4Results(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX)
{
  NSTestSol->N4Results(v2, k2, alpha, omega, deltaMWFT, minX);
}

// A function used for sorting solutions in set P to facilitate calculating
// the hypervolume performance measure
//
int MOSSIndComp(void *ind1, void *ind2)
{
  DMOSP_MOSS_Solution *S1 = (DMOSP_MOSS_Solution *)ind1;
  DMOSP_MOSS_Solution *S2 = (DMOSP_MOSS_Solution *)ind2;
  if(S1->Cmax < S2->Cmax) return -1;
  else if(S1->Cmax == S2->Cmax){
	if(S1->MWFT < S2->MWFT) return -1;
	else if(S1->MWFT == S2->MWFT) return 0;
  }
  return 1;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
