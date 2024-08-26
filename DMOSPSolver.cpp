//--- This code is Copyright 2019, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//#define REPORT_NETWORK_RESULTS
//#define REPORT_VECTOR_RESULTS

#ifdef REPORT_NETWORK_RESULTS
  #define REPORT_INTERMEDIATE_RESULTS
#endif

#ifdef REPORT_VECTOR_RESULTS
  #define REPORT_INTERMEDIATE_RESULTS
#endif

//#define REPORT_NSTester_RESULTS

#include "DMOSPSolver.h"
#include "random.h"
#include "SList.h"
#include "STree.h"

#ifdef REPORT_INTERMEDIATE_RESULTS
FILE *dbgFile;
#endif

double max(double a, double b)
{
  if(a > b) return a;
  return b;
}

double min(double a, double b)
{
  if(a < b) return a;
  return b;
}

// A function used for sorting solutions in a list to facilitate calculating
// the hypervolume performance measure for the bi-bjective DMOSP
//
int BiObjSolComp(void *ind1, void *ind2)
{
  DMOSP_Solution *S1 = (DMOSP_Solution *)ind1;
  DMOSP_Solution *S2 = (DMOSP_Solution *)ind2;
  if(S1->Cmax < S2->Cmax) return -1;
  else if(S1->Cmax == S2->Cmax){
	if(S1->MWFT < S1->MWFT) return -1;
	else if(S1->MWFT == S1->MWFT) return 0;
  }
  return 1;
}

//----------------------------------------------- Solution Class
DMOSP_Solution::DMOSP_Solution(DMOSP_Problem *P, bool bConstructTasks) : Problem(P)
{
  // Since scheduled tasks are well defined as related to operations
  // we will allocate necessary memory space for all of them here unless
  // bConstructTasks == false
  //
  if(bConstructTasks){
    SList::SNode *jnd = Problem->Jobs.head();
    while(jnd){
      DMOSP_Problem::SJob *job = (DMOSP_Problem::SJob *)jnd->Data();
      SList::SNode *opnd = job->Operations.head();
      while(opnd){
		DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
        DMOSP_Solution::SchTask *newTsk = new DMOSP_Solution::SchTask(op);
        ScheduledTasks.Add(newTsk);
        opnd = opnd->Next();
      }
      jnd = jnd->Next();
    }
  }
}

DMOSP_Solution::~DMOSP_Solution()
{
  // free allocated memory for scheduled tasks
  //
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
    SchTask *tsk = (SchTask *)nd->Data();
    delete tsk;
    nd = nd->Next();
  }
  ScheduledTasks.Clear();
}

double DMOSP_Solution::JobCompletionTime(DMOSP_Problem::SJob *jb)
{
  SList::SNode *tsknd = ScheduledTasks.head();

  double ct = -1;
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(!tsk->bScheduled){
	  ct = -MAXFLOAT;
	  break;
	}
	if(tsk->Operation->Job == jb){
	  if(ct < tsk->EndTime) ct = tsk->EndTime;
	}
	tsknd = tsknd->Next();
  }
  return ct;
}

DMOSP_Solution::SchTask *DMOSP_Solution::TaskAssociatedToOperation(
												  DMOSP_Problem::SOperation *op)
{
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	if(tsk->Operation == op) return tsk;
	nd = nd->Next();
  }
  return NULL;
}

double DMOSP_Solution::ComputeObjectiveValue(DMOSP_Problem::TObjective OBJ)
{
  // Calculate the objective function value for a given solution "Sol"
  //   --> the return value is -MAXFLOAT if the schedule is not complete
  //
  SList::SNode *tsknd, *jnd;
  double obj = 0;
  DMOSP_Problem::TObjective OBJECTIVE = (OBJ == 0)? Problem->Objective : OBJ;
  switch(OBJECTIVE){
	case DMOSP_Problem::MIN_MAKESPAN:
		   tsknd = ScheduledTasks.head();
		   while(tsknd){
			 DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
			 if(!tsk->bScheduled){
			   obj = -MAXFLOAT;
			   break;
			 }
			 if(obj < tsk->EndTime) obj = tsk->EndTime;
			 tsknd = tsknd->Next();
		   }
		   break;

	case DMOSP_Problem::MIN_MEAN_FLOWTIME:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (ct - jb->ReleaseTime);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case DMOSP_Problem::MIN_MEAN_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (ct - jb->DueDate);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case DMOSP_Problem::MIN_MEAN_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += max((ct - jb->DueDate), 0.0);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case DMOSP_Problem::MIN_MAX_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 double lat = ct - jb->DueDate;
			 if(obj < lat) obj = lat;
			 jnd = jnd->Next();
		   }
		   break;

	case DMOSP_Problem::MIN_MAX_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 double tard = max((ct - jb->DueDate), 0.0);
			 if(obj < tard) obj = tard;
			 jnd = jnd->Next();
		   }
		   break;

	case DMOSP_Problem::MIN_N_TARDY:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 if(ct > jb->DueDate) obj++;
			 jnd = jnd->Next();
		   }
		   break;

	case DMOSP_Problem::MIN_MEAN_W_LATENESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += ((ct - jb->DueDate) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case DMOSP_Problem::MIN_MEAN_W_TARDINESS:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += (max((ct - jb->DueDate), 0.0) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;

	case DMOSP_Problem::MIN_MEAN_W_FLOWTIME:
		   jnd = Problem->Jobs.head();
		   while(jnd){
			 DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
			 double ct = JobCompletionTime(jb);
			 if(ct<0){  obj = -MAXFLOAT;   break;  }
			 obj += ((ct - jb->ReleaseTime) * jb->Weight);
			 jnd = jnd->Next();
		   }
		   obj /= (Problem->Jobs.Count());
		   break;
  }
  return obj;
}

bool DMOSP_Solution::Copy(DMOSP_Solution *Sol)
{
  if(Problem != Sol->Problem) return false;

  SList::SNode *curnd = ScheduledTasks.head();
  SList::SNode *nd = Sol->ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	SchTask *curtsk = (SchTask *)curnd->Data();
	memcpy(curtsk, tsk, sizeof(SchTask));
	nd = nd->Next();
	curnd = curnd->Next();
  }
  return true;
}

DMOSP_Solution& DMOSP_Solution::operator=(DMOSP_Solution &sol)
{
  Copy(&sol);
  return *this;
}

bool DMOSP_Solution::operator ==(DMOSP_Solution &Sol)
{
  if(Problem != Sol.Problem) return false;
  SList::SNode *nd = Sol.ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	SchTask *thisTsk = TaskAssociatedToOperation(tsk->Operation);
	if(thisTsk->bScheduled != tsk->bScheduled || thisTsk->SelectedMC != tsk->SelectedMC ||
	   thisTsk->StartTime != tsk->StartTime) return false;
	nd = nd->Next();
  }
  return true;
}

bool DMOSP_Solution::isComplete()
{
  SList::SNode *nd = ScheduledTasks.head();
  while(nd){
	SchTask *tsk = (SchTask *)nd->Data();
	if(!tsk->SelectedMC) return false;
	nd = nd->Next();
  }
  return true;
}

bool DMOSP_Solution::isFeasible()
{
  // start by sorting all tasks in a non-decreasing order of their start times
  ScheduledTasks.QSort(tskComp, ASCENDING);

  // check for overlaps in task intervals on machines and jobs
  SList::SNode *cur = ScheduledTasks.head();
  while(cur){
	SchTask *tsk = (SchTask *)cur->Data();
	SList::SNode *nxt = cur->Next();
	while(nxt){
	  SchTask *nxttsk = (SchTask *)nxt->Data();
	  if(nxttsk->StartTime >= tsk->EndTime) break;
	  if(nxttsk->SelectedMC == tsk->SelectedMC ||
		 nxttsk->Operation->Job == tsk->Operation->Job) return false;
	  nxt = nxt->Next();
	}
	cur = cur->Next();
  }

  // check for any logical contradiction between processing sequences on machines
  // and processing orders of jobs (the case of cycles in the network representation)
  //
  bool bFeasible = true;
  int nUsedMCs = Problem->nMachines();
  int nIncludedJobs = Problem->Jobs.Count();
  int nMCs = nUsedMCs;
  int nJobs = nIncludedJobs;

  DMOSP_Problem::SMC **M = new DMOSP_Problem::SMC *[nMCs];
  SList::SNode *nd = Problem->WCs.head();
  int m=0;
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  M[m] = mc;
	  m++;
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }

  int j=0;
  DMOSP_Problem::SJob **J = new DMOSP_Problem::SJob *[nJobs];
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	J[j] = jb;
    jb->i = j;
	j++;
	nd = nd->Next();
  }

  SList::SNode **mnxt = new SList::SNode *[nMCs];    // next task node for the machine
  for(m=0; m<nMCs; m++){
	mnxt[m] = NULL;
	nd = ScheduledTasks.head();
	while(nd){
	  SchTask *tsk = (SchTask *)nd->Data();
	  if(tsk->SelectedMC == M[m]){ mnxt[m] = nd;  break; }
	  nd = nd->Next();
	}
	if(!mnxt[m]) nUsedMCs--;
  }

  SList::SNode **jnxt = new SList::SNode *[nJobs];   // next task node for the job
  for(j=0; j<nJobs; j++){
	jnxt[j] = NULL;
	nd = ScheduledTasks.head();
	while(nd){
	  SchTask *tsk = (SchTask *)nd->Data();
	  if(tsk->Operation->Job == J[j]){ jnxt[j] = nd;  break; }
	  nd = nd->Next();
	}
	if(!jnxt[j]) nIncludedJobs--;
  }

  // Check for the infeasibility now
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<nMCs; m++){
	  if(!mnxt[m]) continue;
	  SchTask *mtsk = (SchTask *)mnxt[m]->Data();
	  j = mtsk->Operation->Job->i;
	  SchTask *jtsk = (SchTask *)jnxt[j]->Data();
	  if(mtsk == jtsk){
		mnxt[m] = mnxt[m]->Next();
		while(mnxt[m]){
		  SchTask *tsk = (SchTask *)mnxt[m]->Data();
		  if(tsk->SelectedMC == M[m]) break;
		  mnxt[m] = mnxt[m]->Next();
		}
		jnxt[j] = jnxt[j]->Next();
		while(jnxt[j]){
		  SchTask *tsk = (SchTask *)jnxt[j]->Data();
		  if(tsk->Operation->Job == J[j]) break;
		  jnxt[j] = jnxt[j]->Next();
		}

		if(!mnxt[m]) nUsedMCs--;
		if(!jnxt[j]) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  delete [] J;
  delete [] M;
  delete [] mnxt;
  delete [] jnxt;

  return bFeasible;
}

//----------------------------------------------- DMOSP_Algorithm Class
DMOSP_Algorithm::DMOSP_Algorithm(DMOSP_Problem *P)
  : Algorithm(P, L"MOSP Solver")
{
  Solution = NULL;
}

DMOSP_Algorithm::DMOSP_Algorithm(DMOSP_Problem *P, wchar_t *N, double TL)
  : Algorithm(P, N, TL)
{
  Solution = NULL;
}

DMOSP_Algorithm::~DMOSP_Algorithm()
{
  if(Solution) delete Solution;
}

void DMOSP_Algorithm::AllocateArrays()
{
  SList::SNode *nd, *tsknd;

  nWCs = Problem->WCs.Count();
  nMCs = 0;
  nd = Problem->WCs.head();
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	nMCs += wc->MCs.Count();
	nd = nd->Next();
  }

  W = new DMOSP_Problem::SWC *[nWCs];
  M = new DMOSP_Problem::SMC *[nMCs];
  Wa = new int[nWCs];
  Wz = new int[nWCs];
  Mw = new int[nMCs];

  nd = Problem->WCs.head();
  int i=0, j=0;
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	W[i] = wc;
	wc->i = i;
	Wa[i] = j;

	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  M[j] = mc;
	  mc->i = j;
	  Mw[j] = i;
	  j++;
	  mcnd = mcnd->Next();
	}
	Wz[i] = j-1;

	i++;
	nd = nd->Next();
  }

  nJobs = Problem->Jobs.Count();
  nOps = 0;
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	nOps += jb->Operations.Count();
	nd = nd->Next();
  }

  J = new DMOSP_Problem::SJob *[nJobs];
  O = new DMOSP_Problem::SOperation *[nOps];
  tsk = new DMOSP_Solution::SchTask *[nOps];
  Ja = new int[nJobs];
  Jz = new int[nJobs];
  Oj = new int[nOps];

  nd = Problem->Jobs.head();
  tsknd = Solution->ScheduledTasks.head();
  i = j = 0;
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	J[i] = jb;
	jb->i = i;
	Ja[i] = j;

	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  O[j] = op;
	  op->i = j;
	  Oj[j] = i;

	  DMOSP_Solution::SchTask *task = (DMOSP_Solution::SchTask *)tsknd->Data();
	  tsk[j] = task;

	  j++;
	  opnd = opnd->Next();
	  tsknd = tsknd->Next();
	}
	Jz[i] = j-1;

	i++;
	nd = nd->Next();
  }

  pt = new double *[nJobs];
  for(i=0; i<nJobs; i++){
	pt[i] = new double[nMCs];
	memset((void *)pt[i], 0, nMCs*sizeof(double));  // if pt is zero, then a job doesn't need the machine
	for(j=0; j<nMCs; j++){
	  for(int k=Ja[i]; k<=Jz[i]; k++){
		nd = O[k]->AltMCs.head();
		while(nd){
		  DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)nd->Data();
		  if(opmc->MC == M[j]){        // Remember that a job is to visit a machine
			pt[i][j] = opmc->pt;       // only once...
			break;
		  }
		  nd = nd->Next();
		}
	  }
	}
  }
}

void DMOSP_Algorithm::FreeArrays()
{
  delete [] W;
  delete [] M;
  delete [] Wa;
  delete [] Wz;
  delete [] Mw;

  delete [] J;
  delete [] O;
  delete [] tsk;
  delete [] Ja;
  delete [] Jz;
  delete [] Oj;

  for(int i=0; i<nJobs; i++){
	delete [] pt[i];
  }
  delete [] pt;
}

void DMOSP_Algorithm::AllocateMemory()
{
  if(Solution) delete Solution;
  Solution = new DMOSP_Solution(Problem);
  AllocateArrays();
}

void DMOSP_Algorithm::Initialize()
{
}

void DMOSP_Algorithm::Improve()
{
}

bool DMOSP_Algorithm::Stop()
{
  return true;
}

void DMOSP_Algorithm::Finalize()
{
  // do nothing
}

void DMOSP_Algorithm::FreeMemory()
{
  FreeArrays();

  // Keep the solution available for other algorithmic and interface uses
}

void DMOSP_Algorithm::ComputeObjectiveValue()
{
  // Calculate the objective function value for the current solution "Solution"
  //   --> the objective value is -MAXFLOAT if the schedule is not complete
  //
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

void DMOSP_Algorithm::CalculateCmaxLowerBound()
{
  double lb1, lb2;
  CmaxLB = CalculateCmaxLowerBound(lb1, lb2);
}

double DMOSP_Algorithm::CalculateCmaxLowerBound(double &lb1, double &lb2)
{
   // ----------------- Evaluate the makespan lower bound for the given problem
   // ---- The first lower bound is based on the maximum of each job's minimum
   //      completion time. For each job, the minimum time needed to complete
   //      it on its processing centers is evaluated. Then the maximum of these
   //      is the LB after adding the minimum possible start time of the job.
   //
   lb1 = 0;
   for(int j=0; j<nJobs; j++){
	 double sum_min_pt = 0;
	 double minat = MAXFLOAT;   // minimum ready time for all machines that can process job j
	 for(int i=Ja[j]; i<=Jz[j]; i++){
	   int w = O[i]->WC->i;
	   double minpt = MAXFLOAT;
	   for(int k=Wa[w]; k<=Wz[w]; k++){
		 if(minpt > pt[j][k]) minpt = pt[j][k];
		 if(minat > M[k]->ReadyTime) minat = M[k]->ReadyTime;
	   }
	   sum_min_pt += minpt;
	 }
	 sum_min_pt += ((J[j]->ReleaseTime > minat)? J[j]->ReleaseTime : minat);
	 if(sum_min_pt > lb1) lb1 = sum_min_pt;
   }

   // ---- The second lower bound is based on parallel machine preemptive
   //      relaxation for each wrokcenter.
   //
   double min_pt;
   double sum_pt, mxWClb = 0;
   lb2 = 0;
   SList::SNode *pcnd = Problem->WCs.head();
   while(pcnd){
	 DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)pcnd->Data();
	 double qw = wc->MCs.Count();

	 double min_rj = MAXFLOAT;
	 double min_am = MAXFLOAT;

	 sum_pt = 0;
	 min_pt = MAXFLOAT;

	 int w = wc->i;
	 for(int o=0; o<nOps; o++){
	   if(O[o]->WC != wc) continue;
	   double pk = MAXFLOAT; // Start with a large value for pk to find the minimum
	   int j = O[o]->Job->i;

	   if(min_rj > J[j]->ReleaseTime) min_rj = J[j]->ReleaseTime;

	   for(int m=Wa[w]; m<=Wz[w]; m++){
		 if(pk > pt[j][m]) pk = pt[j][m];

		 if(min_am > M[m]->ReadyTime) min_am = M[m]->ReadyTime;
	   }
	   if(min_pt > pk) min_pt = pk;
	   sum_pt += pk;
	 }

	 if(sum_pt != 0){
	   mxWClb = sum_pt / qw;
	   if(mxWClb < min_pt) mxWClb = min_pt;
	 }
	 double a_w = (min_rj > min_am)? min_rj : min_am;
	 mxWClb += a_w;

	 if(lb2 < mxWClb) lb2 = mxWClb;

	 pcnd = pcnd->Next();
   }

   double CmaxLB = (lb1 < lb2)? lb2 : lb1;
   return CmaxLB;
}

double DMOSP_Algorithm::CalculateMWFTLowerBound()
{
   // ----------------- Evaluate the MWFT lower bound for the given problem
   // ---- This lower bound is based on the maximum of each job's minimum
   //      completion time. For each job, the minimum time needed to complete
   //      it on its processing centers is evaluated. Then the maximum of these
   //      is used to evaluate the MWFT lower bound.
   //
   MWFTLB = 0;
   for(int j=0; j<nJobs; j++){
	 double sum_min_pt = 0;
	 double minat = MAXFLOAT;   // minimum ready time for all machines that can process job j
	 for(int i=Ja[j]; i<=Jz[j]; i++){
	   int w = O[i]->WC->i;
	   double minpt = MAXFLOAT;
	   for(int k=Wa[w]; k<=Wz[w]; k++){
		 if(minpt > pt[j][k]) minpt = pt[j][k];
		 if(minat > M[k]->ReadyTime) minat = M[k]->ReadyTime;
	   }
	   sum_min_pt += minpt;
	 }
	 sum_min_pt += ((J[j]->ReleaseTime > minat)? J[j]->ReleaseTime : minat);
	 sum_min_pt -= J[j]->ReleaseTime;
	 sum_min_pt *= J[j]->Weight;
	 MWFTLB += sum_min_pt;
   }

   MWFTLB /= (double)nJobs;

   return MWFTLB;
}

//------------------------ tskComp function to be used by sorting algorithms

int tskComp(void * Item1, void * Item2)
{
  DMOSP_Solution::SchTask *tsk1, *tsk2;
  tsk1 = (DMOSP_Solution::SchTask *)Item1;
  tsk2 = (DMOSP_Solution::SchTask *)Item2;
  int ret;
  double diff = tsk1->StartTime - tsk2->StartTime;
  if(diff > 0.0) ret = 1;
  else if(diff == 0.0) ret = 0;
  else ret = -1;
  return ret;
}

//--------------------------------------------- Dense Schedule Generator (DSG)

DMOSP_DSG::DMOSP_DSG(DMOSP_Problem *P, wchar_t *N)
  : DMOSP_Algorithm(P, N)
{
}

DMOSP_DSG::DMOSP_DSG(DMOSP_Problem *P, T_JobDispatchingRule DR, T_MCSelectionRule SR)
  : DMOSP_Algorithm(P, L"Dense Schedule Generator")
{
  JobDispatchingRule = DR;
  MCSelectionRule = SR;
}

DMOSP_DSG::~DMOSP_DSG()
{
}

void DMOSP_DSG::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();

  MCrt = new double[nMCs];
  for(int i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  bMCready = new bool[nMCs];
  bMCdone = new bool[nMCs];

  Jnft = new double[nJobs];
  bJcompleted = new bool[nJobs];
  for(int i=0; i<nJobs; i++){
	Jnft[i] = J[i]->ReleaseTime;
	bJcompleted[i] = false;
  }

  bJM = new bool *[nJobs];
  for (int i = 0; i < nJobs; i++){
	bJM[i] = new bool[nMCs];
	for(int m=0; m<nMCs; m++){
	  bJM[i][m] = (pt[i][m] > 0)? true : false;
	}
  }

  SchedulableJobs = new bool[nJobs];
  SelJob = new int[nMCs];
  bScheduled = new bool[nOps];
  SumSchedTime = new double[nMCs];
  for(int i=0; i<nMCs; i++) SumSchedTime[i] = 0.0;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  dbgFile = fopen("Debug.txt", "a+");
	 #endif
}

void DMOSP_DSG::Initialize()
{
  int i, j, m;
  double t;   // Current scheduling time

  // initialize random number generator
  randomize();
  warmup_random(random(1000)/1000.);

  // Start with all operations unscheduled
  for(i=0; i<nOps; i++) bScheduled[i] = false;

  while(true){ // Main loop of the algorithm
	// Check for each machine if it is needed by any job or not
	for(m=0; m<nMCs; m++){
	  bool b = true;
	  for(j=0; j<nJobs; j++){
		if(bJM[j][m]){
		  b = false;
		  break;
		}
	  }
	  bMCdone[m] = b;
	}

	// If all machines are done, then stop the algorithm
	bool bAllDone = true;
	for(m=0; m<nMCs; m++){
	  if(!bMCdone[m]){
		bAllDone = false;
		break;
	  }
	}
	if(bAllDone) break;

	// Find the minimum machine ready time which is the current schedule time
	t = MAXFLOAT;
	for(m=0; m<nMCs; m++){
	  if(!bMCdone[m] && MCrt[m] < t) t = MCrt[m];
	}

	// Define the set of schedulable machines at time t
	for(m=0; m<nMCs; m++){
	  bMCready[m] = (MCrt[m] == t && !bMCdone[m])? true : false;
	}

	// For each machine ready in time t, select a job
	//
	for(m=0; m<nMCs; m++){
		if(!bMCready[m]) continue;

		// Check to see if there is any job that needs to be scheduled on machine m
		//   preference is made first to jobs that has nearest finish time less than
		//   or equal t. IF there isn't any job with that condition, all schedulable
		//   jobs are considered.
		//
		int nsj = 0;  // start with zero number of schedulable jobs
		for(j=0; j<nJobs; j++){
		  if(bJM[j][m] && Jnft[j] <= t){
			SchedulableJobs[j] = true;
			nsj++;
		  }
		  else SchedulableJobs[j] = false;
		}
		if(nsj == 0){
		  for(j=0; j<nJobs; j++){
			if(bJM[j][m]){
			  SchedulableJobs[j] = true;
			  nsj++;
			}
			else SchedulableJobs[j] = false;
		  }
		  if(nsj == 0){  // Which shouldn't happen anyway!!!
			bMCdone[m] = true;
			continue;
		  }
		}

		// Now apply the job dispatching rule to select a job from all schedulable
		// jobs.
		//
		double Mr;  // The measure based upon which a job will be selected
		for(j=0; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  SelJob[m] = j;
		  switch(JobDispatchingRule){
			case FCFS:   // First Come First Serve
						 Mr = Jnft[j];  // use the job's nearest finish time for arrival order to a machine
						 break;
			case SPT:    // Shortest Processing Time
			case LPT:    // Longest Processing Time
						 Mr = pt[j][m];
						 break;
			case MCT:    // Minimum Completion Time
						 Mr = Jnft[j] + pt[j][m];
						 break;
			case MOR:    // Most Operations Remaining
			case LOR:    // Least Operations Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) Mr++;
						 }
						 break;
			case MWR:    // Most Work Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   Mr += O[i]->maxpt();
						 }
						 break;
			case LWR:    // Lease Work Remaining
						 Mr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   Mr += O[i]->minpt();
						 }
						 break;
		  }
		  break;
		}
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  int opsr;
		  double wrk;
		  switch(JobDispatchingRule){
			case FCFS:   // First Come First Serve
						 if(Jnft[j] < Mr){
						   SelJob[m] = j;
						   Mr = Jnft[j];
						 }
						 break;
			case SPT:    // Shortest Processing Time
						 if(pt[j][m] < Mr){
						   SelJob[m] = j;
						   Mr = pt[j][m];
						 }
						 break;
			case LPT:    // Longest Processing Time
						 if(pt[j][m] > Mr){
						   SelJob[m] = j;
						   Mr = pt[j][m];
						 }
						 break;
			case MCT:    // Minimum Completion Time
						 if(Jnft[j] + pt[j][m] < Mr){
						   SelJob[m] = j;
						   Mr = Jnft[j] + pt[j][m];
						 }
						 break;
			case MOR:    // Most Operations Remaining
						 opsr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) opsr++;
						 }
						 if(opsr > Mr){
						   Mr = opsr;
						   SelJob[m] = j;
						 }
						 break;
			case LOR:    // Least Operations Remaining
						 opsr = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(!bScheduled[i]) opsr++;
						 }
						 if(opsr < Mr){
						   Mr = opsr;
						   SelJob[m] = j;
						 }
						 break;
			case MWR:    // Most Work Remaining
						 wrk = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   wrk += O[i]->maxpt();
						 }
						 if(wrk > Mr){
						   Mr = wrk;
						   SelJob[m] = j;
						 }
						 break;
			case LWR:    // Lease Work Remaining
						 wrk = 0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   wrk += O[i]->minpt();
						 }
						 if(wrk < Mr){
						   Mr = wrk;
						   SelJob[m] = j;
						 }
						 break;
			case RANDOM: // Random
						 if(flip(0.5)) SelJob[m] = j;
		  }
		}
	}

	// Now, for each machine there is a selected job. We need to filter these
	//   selections by removing duplicates in job selections using the machines'
	//   selection rule
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
      if(j==-1) continue;
	  for(int m2=m+1; m2<nMCs; m2++){
        if(!bMCready[m2] || SelJob[m2] == -1) continue;
		if(j == SelJob[m2]){
		  int SelMC;
		  if(MCSelectionRule == MIN_LOAD){
			// Select the machine with the minimum load
			if(SumSchedTime[m] <= SumSchedTime[m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else if(MCSelectionRule == MINCT){
			// Select the machine that results in lower completion time
			if(pt[j][m] <= pt[j][m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else{
			// Select a machine randomly
			SelMC = (flip(0.5))? m : m2;
		  }

		  if(SelMC == m) SelJob[m2] = -1;
		  else{
			SelJob[m] = -1;
			break;
		  }
		}
	  }
	}

	// Now, for all machines that have assigned values in SelJob and in the current
	// list of ready machines, schedule their jobs and update all necessary varaibles.
	//
	for(m=0; m<nMCs; m++){
	  if(!bMCready[m]) continue;
	  j = SelJob[m];
	  if(j == -1) continue;

	  // define which operation index
	  for(i=Ja[j]; i<=Jz[j]; i++) if(O[i]->WC == M[m]->WC) break;
	  if(i > Jz[j]) continue;  // <-- this should not happen!!

	  // Update machine and job times and set operation and task related variables
	  tsk[i]->SelectedMC = M[m];
	  tsk[i]->StartTime = (Jnft[j] > MCrt[m])? Jnft[j] : MCrt[m];
	  Jnft[j] = tsk[i]->StartTime + pt[j][m];
	  MCrt[m] = Jnft[j];
	  tsk[i]->EndTime = Jnft[j];
	  SumSchedTime[m] += pt[j][m];
	  tsk[i]->bScheduled = bScheduled[i] = true;

	  // Now job j does not need any machine in the workcenter p
	  int p = Mw[m];  // index of the WC to which machine m belongs
	  for(int m2=Wa[p]; m2<=Wz[p]; m2++) bJM[j][m2] = false;

	  // Check if all operations of job j are completed
	  bool bAllDone = true;
	  for(int i2=Ja[j]; i2<=Jz[j]; i2++){
		if(!bScheduled[i2]){
		  bAllDone = false;
		  break;
		}
	  }
	  bJcompleted[j] = bAllDone;
	}
  } // End of main loop
}

bool DMOSP_DSG::Stop()
{
  return true;
}

void DMOSP_DSG::Finalize()
{
  DMOSP_Algorithm::Finalize();
  SolStatus = FEASIBLE;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  wchar_t buf[256];
		  swprintf(buf, L"Initial solution from DSG:\n\n");
		  fwprintf(dbgFile, buf);
		  swprintf(buf, L"\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fwprintf(dbgFile, buf);
		  for(int i=0; i<nOps; i++){
			swprintf(buf, L"\t%i\t%s\t%s\t%s\t%0.2f\t%0.2f\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), tsk[i]->StartTime,
						 tsk[i]->EndTime);
			fwprintf(dbgFile, buf);
		  }
		  swprintf(buf, L"\n\tCmax = %0.2f\n\n", Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN));
		  fwprintf(dbgFile, buf);
	 #endif
}

void DMOSP_DSG::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  delete [] MCrt;
  delete [] bMCready;
  delete [] bMCdone;

  delete [] Jnft;
  delete [] bJcompleted;
  for (int i = 0; i < nJobs; i++) delete [] bJM[i];
  delete [] bJM;
  delete [] SchedulableJobs;
  delete [] SelJob;
  delete [] bScheduled;
  delete [] SumSchedTime;

	 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fclose(dbgFile);
	 #endif
}

void DMOSP_DSG::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//---------------------------------------- Vector representation of solutions

DMOSP_VectorSol::DMOSP_VectorSol(DMOSP_Algorithm *a)
{
  Alg = a;
  tau = new SList[Alg->nJobs];
  pi = new SList[Alg->nMCs];
  mc = new int[Alg->nOps];
  memset(mc, -1, Alg->nOps * sizeof(int));
  u = new char[Alg->nOps];
  memset(u, 0, Alg->nOps * sizeof(char));
}

DMOSP_VectorSol::DMOSP_VectorSol(DMOSP_VectorSol *V)
{
  Alg = V->Alg;
  tau = new SList[Alg->nJobs];
  pi = new SList[Alg->nMCs];
  mc = new int[Alg->nOps];
  u = new char[Alg->nOps];
  Copy(V);
}

DMOSP_VectorSol::~DMOSP_VectorSol()
{
  delete [] tau;
  delete [] pi;
  delete [] mc;
  delete [] u;
}

void DMOSP_VectorSol::Copy(DMOSP_VectorSol *V)
{
  if(Alg != V->Alg) return;
  for(int j=0; j<Alg->nJobs; j++) tau[j] = V->tau[j];
  for(int m=0; m<Alg->nMCs; m++) pi[m] = V->pi[m];
  memcpy(mc, V->mc, sizeof(int)*Alg->nOps);
  memcpy(u, V->u, sizeof(char)*Alg->nOps);
}

#ifdef REPORT_VECTOR_RESULTS
void DMOSP_VectorSol::Assert(wchar_t *heading)
{
  int m, j;
  wchar_t buf[256];
  fwprintf(dbgFile, L"\n\n%s:\n\n", heading);
  fwprintf(dbgFile, L"\ta) pi\'s:\n");
  for(m=0; m<Alg->nMCs; m++){
	swprintf(buf, L"\t\t%s: ", Alg->M[m]->ID());
	fwprintf(dbgFile, buf);
	SList::SNode *pind = headOfPi(m);
	if(!pind) fwprintf(dbgFile, L"\n");
	while(pind){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)pind->Data();
	  if(u[op->i] & 2){
		swprintf(buf, L" %s ", op->ID());
		fwprintf(dbgFile, buf);
	  }
	  pind = pind->Next();
	  if(pind) swprintf(buf, L"->");
	  else swprintf(buf, L"\n");
	  fwprintf(dbgFile, buf);
	}
  }
  fwprintf(dbgFile, L"\n\tb) taus:\n");
  for(j=0; j<Alg->nJobs; j++){
	swprintf(buf, L"\t\t%s: ", Alg->J[j]->ID());
	fwprintf(dbgFile, buf);
	SList::SNode *tund = headOfTau(j);
	if(!tund) fwprintf(dbgFile, L"\n");
	while(tund){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)tund->Data();
	  if(u[op->i] & 1){
		swprintf(buf, L" %s ", op->ID());
		fwprintf(dbgFile, buf);
	  }
	  tund = tund->Next();
	  if(tund) swprintf(buf, L"->");
	  else swprintf(buf, L"\n");
	  fwprintf(dbgFile, buf);
	}
  }
  fwprintf(dbgFile, L"\n\n");
  fflush(dbgFile);
}
#endif

void DMOSP_VectorSol::ConvertTasksToVect()
{
  Clear();

  SList &Tasks = Alg->Solution->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  for(SList::SNode *tsknd = Tasks.head(); tsknd; tsknd = tsknd->Next()){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC){
	  AppendOp2Mc(tsk->SelectedMC, tsk->Operation);
	  AppendOp2Jb(tsk->Operation);
	  u[tsk->Operation->i] = 3;
	}
  }

		  #ifdef REPORT_VECTOR_RESULTS
			  Assert(L"Vector representation after interpreting tasks");
		  #endif
}

bool DMOSP_VectorSol::AppendOp2Mc(int m, DMOSP_Problem::SOperation *op)
{
  if(mc[op->i] != -1) return false;
  if(m < Alg->Wa[op->WC->i] || m > Alg->Wz[op->WC->i]) return false;

  pi[m].Add(op);
  mc[op->i] = m;
  return true;
}

bool DMOSP_VectorSol::AppendOp2Mc(DMOSP_Problem::SMC *m, DMOSP_Problem::SOperation *op)
{
  return AppendOp2Mc(m->i, op);
}

bool DMOSP_VectorSol::AppendOp2Jb(DMOSP_Problem::SOperation *op)
{
  if(tau[op->Job->i].IndexOf((void *)op) != -1) return false;
  tau[op->Job->i].Add(op);
  return true;
}

bool DMOSP_VectorSol::RemoveOp(DMOSP_Problem::SOperation *op)
{
  if(mc[op->i] == -1) return false;
  pi[mc[op->i]].Remove((void *)op);
  mc[op->i] = -1;
  tau[op->Job->i].Remove((void *)op);
  u[op->i] = 0;
  return true;
}

void DMOSP_VectorSol::Clear()
{
  for(int j=0; j<Alg->nJobs; j++) tau[j].Clear();
  for(int m=0; m<Alg->nMCs; m++) pi[m].Clear();
  memset(mc, -1, Alg->nOps * sizeof(int));
  memset(u, 0, Alg->nOps * sizeof(char));
}

bool DMOSP_VectorSol::isComplete()   // assuming no replication of operations anywhere
{                                   // in taus and pi's
  int sum = Alg->nOps;
  for(int i=0; i<Alg->nOps; i++) sum -= (int)u[i] / 3;

  if(sum != 0) return false;

  /*for(int j=0; j<Alg->nJobs; j++) sum += tau[j].Count();
  if(sum != Alg->nOps) return false;

  for(int m=0; m<Alg->nMCs; m++) sum -= pi[m].Count();
  if(sum != 0) return false;*/

  return true;
}

bool DMOSP_VectorSol::isFeasible()
{
  bool bFeasible = true;
  int j, m;

  int nUsedMCs = Alg->nMCs;          // number of machines that have nodes
  int nIncludedJobs = Alg->nJobs;    // number of jobs with nodes included

  SList::SNode **mnxt = new SList::SNode *[Alg->nMCs];    // next node index for the machine
  for(m=0; m<Alg->nMCs; m++){
	mnxt[m] = pi[m].head();
	if(!mnxt[m]) nUsedMCs--;
  }

  SList::SNode **jnxt = new SList::SNode *[Alg->nJobs];   // next node index for the job
  for(j=0; j<Alg->nJobs; j++){
	jnxt[j] = tau[j].head();
	if(!jnxt[j]) nIncludedJobs--;
  }

  // Check for the infeasibility now
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<Alg->nMCs; m++){
	  if(!mnxt[m]) continue;
	  DMOSP_Problem::SOperation *mop = (DMOSP_Problem::SOperation *)mnxt[m]->Data();
	  j = mop->Job->i;
	  DMOSP_Problem::SOperation *jop = (DMOSP_Problem::SOperation *)jnxt[j]->Data();
	  if(mop == jop){
		mnxt[m] = mnxt[m]->Next();
		jnxt[j] = jnxt[j]->Next();
		if(!mnxt[m]) nUsedMCs--;
		if(!jnxt[j]) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  delete [] mnxt;
  delete [] jnxt;

  return bFeasible;
}

int DMOSP_VectorSol::Psi_j(DMOSP_VectorSol *V)
{
  int ret = 0;
  for(int j=0; j<Alg->nJobs; j++){
	int p = psi(tau[j], V->tau[j]);
	if(p >= 0) ret += p;
	else return -1;
  }
  return ret;
}

int DMOSP_VectorSol::Psi_w(DMOSP_VectorSol *V)
{
  int ret = 0;
  SList P1, P2;
  for(int w=0; w<Alg->nWCs; w++){
	P1.Clear();
	P2.Clear();
	for(int m=Alg->Wa[w]; m<=Alg->Wz[w]; m++){
	  P1.Add(pi[m]);
	  P2.Add(V->pi[m]);
	  P1.Add((void *)Alg->M[m]);
	  P2.Add((void *)Alg->M[m]);
	}
	int p = psi(P1, P2);
	if(p >= 0) ret += p;
	else return -1;
  }
  return ret;
}

bool DMOSP_VectorSol::RemoveTauSubpermutation(int j, int x1, int x2)
{
  if(x1 > x2) return false;
  SList &tau_j = tau[j];
  int l = tau_j.Count();
  if(x2 > l) return false;

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  for(; i<=x2; i++){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	u[op->i] &= 2;
	nd = nd->Next();
  }
  return true;
}

bool DMOSP_VectorSol::RemovePiSubpermutation(int m, int x1, int x2)
{
  if(x1 > x2) return false;
  SList &pi_m = pi[m];
  int l = pi_m.Count();
  if(x2 > l) return false;

  int i=1;
  SList::SNode *nd = pi_m.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  for(; i<=x2; i++){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	u[op->i] &= 1;
	nd = nd->Next();
  }
  return true;
}

void DMOSP_VectorSol::UnremoveAll()
{
  memset(u, 3, Alg->nOps * sizeof(char));
}

bool DMOSP_VectorSol::MoveOpToTauPos(DMOSP_Problem::SOperation *op, int x, bool bchkpos)
{
  SList &tau_j = tau[op->Job->i];
  int l = tau_j.Count();

  if(bchkpos) if(x < 1 || x > l) return false;

  tau_j.Remove((void *)op);

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  if(nd) tau_j.AddBefore(nd, op);
  else tau_j.Add(op);

		 #ifdef REPORT_VECTOR_RESULTS
			Assert(L"Vector solution after moving operation in job tau");
		 #endif

  return true;
}

bool DMOSP_VectorSol::MoveOpToPiPos(DMOSP_Problem::SOperation *op, int k, int x, bool bchkpos)
{
  int m = mc[op->i];
  SList &pi_m = pi[m];
  SList &pi_k = pi[k];
  int lm = pi_m.Count();
  int lk = pi_k.Count();

  if(bchkpos){
	if(x < 1) return false;
	if(m == k) if(x > lm) return false;
	else if(x > lk+1) return false;
  }

  pi_m.Remove((void *)op);

  int i=1;
  SList::SNode *nd = pi_k.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  if(nd) pi_k.AddBefore(nd, (void *)op);
  else pi_k.Add((void *)op);

		 #ifdef REPORT_VECTOR_RESULTS
			Assert(L"Vector solution after moving operation between machine permutations");
		 #endif

  return true;
}

bool DMOSP_VectorSol::MoveTauSubpermutation(int j, int x1, int x2, int x, bool bchkpos)
{
  SList &tau_j = tau[j];
  int l = tau_j.Count();

  if(bchkpos){
	if(x1 < 1 || x1 > x2 || x2 > l) return false;
	if(x1 == 1 && x2 == l){
	  if(x == 1) return true; // trivial!!
	  else return false;
	}
	if(x < 1 || x > l - (x2 - x1)) return false;
  }

  int i=1;
  SList::SNode *nd = tau_j.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  SList tmpOp;
  for(; i<=x2; i++){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	tmpOp.Add((void *)op);
	SList::SNode *nxtnd = nd->Next();
	tau_j.Remove(nd);
	nd = nxtnd;
  }

  i=1;
  nd = tau_j.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  for(SList::SNode *tmpnd = tmpOp.head(); tmpnd; tmpnd = tmpnd->Next()){
	if(nd) tau_j.AddBefore(nd, tmpnd->Data());
	else tau_j.Add(tmpnd->Data());
  }

		 #ifdef REPORT_VECTOR_RESULTS
			Assert(L"Vector solution after moving job subpermutation");
		 #endif

  return true;
}

bool DMOSP_VectorSol::MovePiSubpermutation(int m, int x1, int x2, int k, int x, bool bchkpos)
{
  SList &pi_m = pi[m];
  SList &pi_k = pi[k];
  int lm = pi_m.Count();
  int lk = pi_k.Count();

  if(bchkpos){
	if(x1 < 1 || x1 > x2 || x2 > lm || x < 1) return false;
	if(m == k){
	  if(x1 == 1 && x2 == lm){
		if(x == 1) return true; // trivial!!
		else return false;
	  }
	  if(x > lm - (x2 - x1)) return false;
	}
	else{
	  if(x > lk+1) return false;
	}
  }

  int i=1;
  SList::SNode *nd = pi_m.head();
  while(i < x1){
	nd = nd->Next();
	i++;
  }

  SList tmpOp;
  for(; i<=x2; i++){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	tmpOp.Add((void *)op);
	SList::SNode *nxtnd = nd->Next();
	pi_m.Remove(nd);
	nd = nxtnd;
  }

  i=1;
  nd = pi_k.head();
  while(i < x){
	nd = nd->Next();
	i++;
  }

  for(SList::SNode *tmpnd = tmpOp.head(); tmpnd; tmpnd = tmpnd->Next()){
	if(nd) pi_k.AddBefore(nd, tmpnd->Data());
	else pi_k.Add(tmpnd->Data());
  }

		 #ifdef REPORT_VECTOR_RESULTS
			Assert(L"Vector solution after moving machine subpermutation");
		 #endif

  return true;
}

int DMOSP_VectorSol::TauPosition(DMOSP_Problem::SOperation *op)
{
  SList &tau_j = tau[op->Job->i];
  return tau_j.IndexOf((void *)op) + 1;
}

int DMOSP_VectorSol::PiPosition(DMOSP_Problem::SOperation *op)
{
  SList &pi_m = pi[mc[op->i]];
  return pi_m.IndexOf((void *)op) +  1;
}

DMOSP_Problem::SMC *DMOSP_VectorSol::MCofOp(DMOSP_Problem::SOperation *op)
{
  return Alg->M[mc[op->i]];
}

//---------------------------------------- Network representation of solutions

DMOSP_Network::DMOSP_Network(DMOSP_Algorithm *a)
{
  Alg = a;
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  Nodes = new DMOSP_Node[nOps];
  FirstjNode = new int[nJobs];
  LastjNode = new int[nJobs];
  FirstmNode = new int[nMCs];
  LastmNode = new int[nMCs];

  MCrt = new double[nMCs];    // machine's ready time
  mnxt = new int[nMCs];    // next node index for the machine
  Jnft = new double[nJobs];   // job's nearest finish time
  jnxt = new int[nJobs];   // next node index for the job

  jcur = new int[nJobs];
  mcur = new int[nMCs];
}

DMOSP_Network::DMOSP_Network(DMOSP_Network *nt)
{
  Alg = nt->Alg;
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  Nodes = new DMOSP_Node[nOps];
  FirstjNode = new int[nJobs];
  LastjNode = new int[nJobs];
  FirstmNode = new int[nMCs];
  LastmNode = new int[nMCs];

  MCrt = new double[nMCs];    // machine's ready time
  mnxt = new int[nMCs];    // next node index for the machine
  Jnft = new double[nJobs];   // job's nearest finish time
  jnxt = new int[nJobs];   // next node index for the job

  jcur = new int[nJobs];
  mcur = new int[nMCs];

  Copy(nt);
}

DMOSP_Network::~DMOSP_Network()
{
  delete [] Nodes;
  delete [] FirstjNode;
  delete [] LastjNode;
  delete [] FirstmNode;
  delete [] LastmNode;

  delete [] MCrt;
  delete [] mnxt;
  delete [] Jnft;
  delete [] jnxt;
  delete [] jcur;
  delete [] mcur;
}

#ifdef REPORT_NETWORK_RESULTS
void DMOSP_Network::Assert(wchar_t *heading)
{
  wchar_t buf[256];
  fwprintf(dbgFile, L"\n\n%s:\n\n", heading);
  fwprintf(dbgFile, L"\ta) List of Nodes:\n\n");
  fwprintf(dbgFile, L"\tNo.\tID\tj_prv\tj_nxt\tM/C\tm_prv\tm_nxt\tweight\testart\tlstart\ttail\tslack\tRank\tCritical\n");
  for(int i=0; i<Alg->nOps; i++){
	if(Nodes[i].mc != -1)
		 swprintf(buf, L"\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%i\t%s\n",
				 i, Alg->O[i]->ID(), Nodes[i].j_prv, Nodes[i].j_nxt,
				 Alg->M[Nodes[i].mc]->ID(), Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? L"*" : L" "));
	else
		 swprintf(buf, L"\t%i\t%s\t%i\t%i\t%s\t%i\t%i\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%i\t%s\n",
				 i, Alg->O[i]->ID(), Nodes[i].j_prv, Nodes[i].j_nxt,
				 "N/A", Nodes[i].m_prv,
				 Nodes[i].m_nxt, Nodes[i].weight, Nodes[i].estart,
				 Nodes[i].lstart, Nodes[i].tail, Nodes[i].slack,
				 Nodes[i].rank, ((Nodes[i].bCritical)? L"*" : L" "));
	fwprintf(dbgFile, buf);
  }
  fwprintf(dbgFile, L"\n\n\tb) List of Jobs:\n\n");
  fwprintf(dbgFile, L"\tID\tFirst\tLast\n");
  for(int j=0; j<Alg->nJobs; j++){
	swprintf(buf, L"\t%s\t%i\t%i\n", Alg->J[j]->ID(), FirstjNode[j], LastjNode[j]);
	fwprintf(dbgFile, buf);
  }
  fwprintf(dbgFile, L"\n\n\tc) List of Machines:\n\n");
  fwprintf(dbgFile, L"\tID\tFirst\tLast\n");
  for(int m=0; m<Alg->nMCs; m++){
	swprintf(buf, L"\t%s\t%i\t%i\n", Alg->M[m]->ID(), FirstmNode[m], LastmNode[m]);
	fwprintf(dbgFile, buf);
  }
  swprintf(buf, L"\n\t\tCmax = %0.2f\n\n", Cmax);
  fwprintf(dbgFile, buf);
  fflush(dbgFile);
}
#endif

void DMOSP_Network::Copy(DMOSP_Network *nt)
{     // Assuming no change in the problem (nJobs, nMCs, and nOps)
  if(Alg != nt->Alg) return;
  memcpy(Nodes, nt->Nodes, Alg->nOps * sizeof(DMOSP_Node));
  memcpy(FirstjNode, nt->FirstjNode, Alg->nJobs * sizeof(int));
  memcpy(LastjNode, nt->LastjNode, Alg->nJobs * sizeof(int));
  memcpy(FirstmNode, nt->FirstmNode, Alg->nMCs * sizeof(int));
  memcpy(LastmNode, nt->LastmNode, Alg->nMCs * sizeof(int));
  Cmax = nt->Cmax;
}

int DMOSP_Network::GetCPath(int *pth)
{
  int m, i, l=0;  // There should be at least on critical operation

  // Find a first critical node
  for(i=0; i<Alg->nOps; i++){
	if(Nodes[i].bCritical && Nodes[i].m_prv == -1 && Nodes[i].j_prv == -1) break;
  }
  pth[l] = i;

  while(true){
	int jnxt = Nodes[i].j_nxt;
	int mnxt = Nodes[i].m_nxt;

	if(jnxt == -1 && mnxt == -1) break;

	if(jnxt != -1 && mnxt == -1){  // the next operation in job route must be critical
	  i = jnxt;
	  pth[++l] = i;
	}

	if(jnxt == -1 && mnxt != -1){  // the next operation in machine sequence must be critical
	  i = mnxt;
	  pth[++l] = i;
	}

	if(jnxt != -1 && mnxt != -1){
	  if(Nodes[jnxt].bCritical && Nodes[mnxt].bCritical){
		if(Nodes[jnxt].estart <= Nodes[mnxt].estart){
		  i = jnxt;
		  pth[++l] = i;
		}
		else{
		  i = mnxt;
		  pth[++l] = i;
		}
	  }
	  else if(Nodes[mnxt].bCritical){     // It must be the next operation in machine sequence critical
		i = mnxt;
		pth[++l] = i;
	  }
	  else{
		i = jnxt;
		pth[++l] = i;
	  }
	}
  }

  return l+1;
}

void DMOSP_Network::ConvertTasksToNet()    // Assuming the the solution is feasible
                                          // and task start times are their earliest starts
{                                         // however it can be partial (incomplete)
  Reset();                                // Note: node ranks and latest start times are
                                          //       not always correct whenever
                                          //       task durations are zeros.....
  memset(jcur, -1, Alg->nJobs * sizeof(int));
  memset(mcur, -1, Alg->nMCs * sizeof(int));

  SList &Tasks = Alg->Solution->ScheduledTasks;

  // All tasks must be sorted in increasing order of their earliest start times
  Tasks.QSort(tskComp, ASCENDING);

  int rem = Alg->nOps; // number of remaining operations which still need to define their tails

  Cmax = 0;
  for(SList::SNode *nd = Tasks.head(); nd; nd = nd->Next()){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)nd->Data();
	int i = tsk->Operation->i;
	if(tsk->SelectedMC){
	  int m = tsk->SelectedMC->i;
	  int j = Alg->Oj[i];

	  Nodes[i].mc = m;
	  Nodes[i].estart = tsk->StartTime;
	  Nodes[i].weight = tsk->EndTime - tsk->StartTime;
	  if(tsk->EndTime > Cmax) Cmax = tsk->EndTime;

	  Nodes[i].m_prv = mcur[m];
	  if(mcur[m] != -1){
		Nodes[mcur[m]].m_nxt = i;
		Nodes[i].rank = Nodes[mcur[m]].rank + 1;
	  }
	  else FirstmNode[m] = i;     // the rank of the first machine node is determined by job nodes
	  LastmNode[m] = i;
	  mcur[m] = i;

	  Nodes[i].j_prv = jcur[j];
	  if(jcur[j] != -1){
		Nodes[jcur[j]].j_nxt = i;
		Nodes[i].rank += (Nodes[jcur[j]].rank + 1);
	  }
	  else{
		FirstjNode[j] = i;
		Nodes[i].rank += 1;
	  }
	  LastjNode[j] = i;
	  jcur[j] = i;
	}
	else rem--;
  }

  // Evaluate the latest start times of the nodes and determine their criticality conditions
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	double maxEFT = -1;
	int imaxEFT = -1;
	for(int i = 0; i < Alg->nOps; i++){
	  if(Nodes[i].lstart != -1) continue;  // tail is already defined
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

	 #ifdef REPORT_NETWORK_RESULTS
		Assert(L"Network representation based on tasks list");
	 #endif
}

void DMOSP_Network::InterpretNetwork(bool &bFeasible, bool &bComplete)
{
  int i, j, m;

  bFeasible = bComplete = true;

  for(i=0; i<Alg->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  int nUsedMCs = Alg->nMCs;          // number of machines that have nodes
  int nIncludedJobs = Alg->nJobs;    // number of jobs with nodes included

  for(m=0; m<Alg->nMCs; m++){
	MCrt[m] = Alg->M[m]->ReadyTime;
	mnxt[m] = FirstmNode[m];
	if(mnxt[m] == -1) nUsedMCs--;
  }

  for(j=0; j<Alg->nJobs; j++){
	Jnft[j] = Alg->J[j]->ReleaseTime;
	jnxt[j] = FirstjNode[j];
	if(jnxt[j] == -1) nIncludedJobs--;
  }

  if(nIncludedJobs != Alg->nJobs) bComplete = false;

  // Determine the earliest start times for all operations, and determine Cmax
  Cmax = 0;
  while(!(nUsedMCs == 0 && nIncludedJobs == 0)){
	bool bNoAction = true;
	for(m=0; m<Alg->nMCs; m++){
	  i = mnxt[m];
	  if(i==-1) continue;
	  j = Alg->O[i]->Job->i;
	  if(jnxt[j] == i){
		Nodes[i].estart = (MCrt[m] > Jnft[j])? MCrt[m] : Jnft[j];
		MCrt[m] = Jnft[j] = Nodes[i].estart + Nodes[i].weight;
		if(Cmax < MCrt[m]) Cmax = MCrt[m];
		mnxt[m] = Nodes[i].m_nxt;
		jnxt[j] = Nodes[i].j_nxt;

		DMOSP_Node &ndi = Nodes[i];
		if(ndi.m_prv != -1) ndi.rank = Nodes[ndi.m_prv].rank + 1;
		if(ndi.j_prv != -1) ndi.rank += (Nodes[ndi.j_prv].rank + 1);
		else ndi.rank += 1;

		if(mnxt[m] == -1) nUsedMCs--;
		if(jnxt[j] == -1) nIncludedJobs--;
		bNoAction = false;
		break;
	  }
	}
	if(bNoAction){ bFeasible = false;  break; }  // there is a loop in the network
  }

  int rem = Alg->nOps; // number of remaining operations which need to define start/end times
  for(i=0; i<Alg->nOps; i++)
	if(Nodes[i].estart == -1){
		 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fprintf(dbgFile, "\n\n\t------------ Incomplete schedule is encountered!!!");
		 #endif
	   rem--;
	}
  bComplete = (rem == Alg->nOps)? true : false;

  if(!bFeasible){
	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  fprintf(dbgFile, "\n\n\t------------ Infeasible solution is encountered!!!");
	 #endif

	Cmax = MAXFLOAT;
  }
  else{
	// Now, we define the latest start and criticality conditions for nodes
	while(rem > 0){
	  // Search for an operation with the most earliest finish time
	  double maxEFT = -1;
	  int imaxEFT = -1;
	  for(i = 0; i < Alg->nOps; i++){
		if(Nodes[i].estart == -1) continue;  // the operation is not in the network
		if(Nodes[i].lstart != -1) continue;  // latest start is already defined
		int eft = Nodes[i].estart + Nodes[i].weight;
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
	  if(Nodes[imaxEFT].slack == 0) Nodes[imaxEFT].bCritical = true;
	  else Nodes[imaxEFT].bCritical = false;
	  rem--;
	}
  }

	 #ifdef REPORT_NETWORK_RESULTS
	   Assert(L"Network representation");
	 #endif
}

bool DMOSP_Network::ConvertNetToTasks()
{
  bool bF, bC;
  InterpretNetwork(bF, bC);
  if(!bF) return false;

  for(int i=0; i<Alg->nOps; i++){
	if(Nodes[i].estart != -1){
	  Alg->tsk[i]->SelectedMC = Alg->M[Nodes[i].mc];
	  Alg->tsk[i]->StartTime = Nodes[i].estart;
	  Alg->tsk[i]->EndTime = Nodes[i].estart + Nodes[i].weight;
	}
	else{
	  Alg->tsk[i]->SelectedMC = NULL;
	  Alg->tsk[i]->StartTime = 0;
	  Alg->tsk[i]->EndTime = 0;
	}
  }

  return true;
}

bool DMOSP_Network::InterpretIncompleteNetwork()
{
  int i, j, k, m;
  int jprv, mprv;

  for(i=0; i<Alg->nOps; i++)  Nodes[i].estart = Nodes[i].lstart = -1;

  // Determine nodes' ranks to be used in an ordered assignment of
  // of earliest start times
  //
  // First, define initial rank values for nodes that do not have both machine
  // and job prdecessors.
  //
  int r=0;
  for(i=0; i<Alg->nOps; i++){
	if(Nodes[i].m_prv == -1 && Nodes[i].j_prv == -1){
	  Nodes[i].rank = 1;
	  r++;
	}
	else{
	  Nodes[i].rank = 0;
	}
  }

  // Second, loop through the remaining nodes to define their ranks and check for
  // cycles...
  while(r < Alg->nOps){
	bool bFound;
	for(i=0; i<Alg->nOps; i++){
	  if(Nodes[i].rank != 0) continue;
	  bool bpm = false, bpj = false;
	  jprv = Nodes[i].j_prv;
	  mprv = Nodes[i].m_prv;
	  if(jprv != -1){
		if(Nodes[jprv].rank > 0){
		  Nodes[i].rank += (Nodes[jprv].rank + 1);
		  bpj = true;
		}
	  }
	  else bpj = true;
	  if(mprv != -1){
		if(Nodes[mprv].rank > 0){
		  Nodes[i].rank += (Nodes[mprv].rank + 1);
		  bpm = true;
		}
	  }
	  else bpm = true;
	  if(!bpj || !bpm){
		Nodes[i].rank = 0;
		bFound = false;
		continue;
	  }
	  bFound = true;
	  r++;
	  break;
	}
	if(!bFound) return false;      // there is a cycle...
  }

  // Determine the earliest start times for all operations, and determine Cmax
  int rem = 0;
  Cmax = 0;
  for(r=0; r<Alg->nOps; r++){
	// find operation i with the lowest rank
	i = -1;
	int lowest_rank = MAXINT;
	for(k=0; k<Alg->nOps; k++){
	  if(Nodes[k].estart == -1 && Nodes[k].rank != -1){
		int k_rank = Nodes[k].rank;
		if(k_rank < lowest_rank){
		  lowest_rank = k_rank;
		  i = k;
		}
	  }
	}
	if(i == -1) break;

	jprv = Nodes[i].j_prv;
	mprv = Nodes[i].m_prv;

	double jes, mes;
	if(jprv == -1) jes = Alg->J[Alg->O[i]->Job->i]->ReleaseTime;
	else jes = Nodes[jprv].estart + Nodes[jprv].weight;
	if(mprv == -1) mes = Alg->M[Nodes[i].mc]->ReadyTime;
	else mes = Nodes[mprv].estart + Nodes[mprv].weight;

	Nodes[i].estart = (jes > mes)? jes : mes;
	rem++;

	double cti = Nodes[i].estart + Nodes[i].weight;
	if(Cmax < cti) Cmax = cti;
  }

  // Now, we define the latest start and criticality conditions for nodes
  while(rem > 0){
	// Search for an operation with the most earliest finish time
	double maxEFT = -1;
	int imaxEFT = -1;
	for(i = 0; i < Alg->nOps; i++){
	  if(Nodes[i].estart == -1) continue;  // the operation is not in the network
	  if(Nodes[i].lstart != -1) continue;  // latest start is already defined
	  int eft = Nodes[i].estart + Nodes[i].weight;
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
	if(Nodes[imaxEFT].slack == 0) Nodes[imaxEFT].bCritical = true;
	else Nodes[imaxEFT].bCritical = false;
	rem--;
  }

	 #ifdef REPORT_NETWORK_RESULTS
	   Assert(L"Incomplete network representation");
	 #endif

  return true;
}

bool DMOSP_Network::ConvertIncompleteNetToTasks()
{
  if(!InterpretIncompleteNetwork()) return false;

  for(int i=0; i<Alg->nOps; i++){
	Alg->tsk[i]->SelectedMC = (Nodes[i].mc == -1)? NULL : Alg->M[Nodes[i].mc];
	Alg->tsk[i]->StartTime = Nodes[i].estart;
	Alg->tsk[i]->EndTime = Nodes[i].estart + Nodes[i].weight;
  }

  return true;
}


bool DMOSP_Network::InterpretVectors(DMOSP_VectorSol *V, bool bckf)
{
  if(bckf)  // If we have to check the feasibility of the vector solution first
	if(!V->isFeasible()) return false;

  Reset();

  // interpret vectors into nodes and arcs only and assign node weights
  int i, k, j, m;

  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;

  for(m=0; m<nMCs; m++){
	SList::SNode *pind = V->headOfPi(m);
	while(pind){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)pind->Data();
	  i = op->i;
	  if(V->u[i] & 2){
		FirstmNode[m] = i;
		Nodes[i].mc = m;
		Nodes[i].m_prv = -1;
		Nodes[i].weight = Alg->pt[op->Job->i][m];
		while(true){
		  pind = pind->Next();
		  if(pind){
			op = (DMOSP_Problem::SOperation *)pind->Data();
			k = op->i;
			if(V->u[k] & 2){
			  Nodes[i].m_nxt = k;
			  Nodes[k].mc = m;
			  Nodes[k].m_prv = i;
			  Nodes[k].weight = Alg->pt[op->Job->i][m];
			  i = k;
			}
		  }
		  else{
			Nodes[i].m_nxt = -1;
			LastmNode[m] = i;
			break;
		  }
		}
	  }
	  else{
		pind = pind->Next();
	  }
	}
  }

  for(j=0; j<nJobs; j++){
	SList::SNode *tund = V->headOfTau(j);
	while(tund){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)tund->Data();
	  i = op->i;
	  if(V->u[i] & 1){
		FirstjNode[j] = i;
		Nodes[i].j_prv = -1;
		while(true){
		  tund = tund->Next();
		  if(tund){
			op = (DMOSP_Problem::SOperation *)tund->Data();
			k = op->i;
			if(V->u[k] & 1){
			  Nodes[i].j_nxt = k;
			  Nodes[k].j_prv = i;
			  i = k;
			}
		  }
		  else{
			Nodes[i].j_nxt = -1;
			LastjNode[j] = i;
			break;
		  }
		}
	  }
	  else tund = tund->Next();
	}
  }

	 #ifdef REPORT_NETWORK_RESULTS
		  V->Assert(L"Vector representation as passed to network");
		  //Assert(L"Corresponding network representation based on the given vectors");
	 #endif

  return true;
}

void DMOSP_Network::Reset()
{
  int nJobs = Alg->nJobs;
  int nMCs = Alg->nMCs;
  int nOps = Alg->nOps;

  memset(FirstjNode, -1, nJobs*sizeof(int));
  memset(LastjNode, -1, nJobs*sizeof(int));
  memset(FirstmNode, -1, nMCs*sizeof(int));
  memset(LastmNode, -1, nMCs*sizeof(int));

  Cmax = MAXFLOAT;   // Just to notify that the network solution has not been
				      // constructed yet

  for(int i=0; i<nOps; i++) Nodes[i].Reset();
}

int DMOSP_Network::sizeof_tau(int j)
{
  int n = 0;
  int nxt = FirstjNode[j];
  while(nxt != -1){
	 nxt = Nodes[nxt].j_nxt;
	 n++;
  }
  return n;
}

int DMOSP_Network::sizeof_pi(int m)
{
  int n = 0;
  int nxt = FirstmNode[m];
  while(nxt != -1){
	nxt = Nodes[nxt].m_nxt;
	n++;
  }
  return n;
}

DMOSP_Node *DMOSP_Network::tauNode_at(int j, int x)
{
  int n = 1;
  int nxt = FirstjNode[j];
  while(n < x && nxt != -1){
	 nxt = Nodes[nxt].j_nxt;
	 n++;
  }
  if(nxt == -1) return NULL;
  return &Nodes[nxt];
}

DMOSP_Node *DMOSP_Network::piNode_at(int m, int x)
{
  int n = 1;
  int nxt = FirstmNode[m];
  while(n < x && nxt != -1){
	 nxt = Nodes[nxt].m_nxt;
	 n++;
  }
  if(nxt == -1) return NULL;
  return &Nodes[nxt];
}

int DMOSP_Network::tau_pos(DMOSP_Node *nd)
{
  DMOSP_Node *v = nd;
  int p=1;
  while(v->j_prv != -1){
	v = &Nodes[v->j_prv];
	p++;
  }
  return p;
}

int DMOSP_Network::pi_pos(DMOSP_Node *nd)
{
  DMOSP_Node *v = nd;
  int p=1;
  while(v->m_prv != -1){
	v = &Nodes[v->m_prv];
	p++;
  }
  return p;
}


bool DMOSP_Network::isThereAPath(DMOSP_Node *nd1, DMOSP_Node *nd2)
{
  if(nd1->m_nxt == -1 && nd1->j_nxt == -1) return false;

  STree T((void *)nd1);
  DMOSP_Node *cur = nd1;

  while(true){
	if(cur->j_nxt != -1){
	  if(&Nodes[cur->j_nxt] == nd2) return true;
	  T.AddChild((void *)&Nodes[cur->j_nxt]);
	  if(cur->m_nxt != -1){
		if(&Nodes[cur->m_nxt] == nd2) return true;
		T.AddLeftNode((void *)&Nodes[cur->m_nxt]);
	  }
	}
	else if(cur->m_nxt != -1){
	  if(&Nodes[cur->m_nxt] == nd2) return true;
	  T.AddChild((void *)&Nodes[cur->m_nxt]);
	}
	else{
	  STree::SNode *itrnd;
	  while(true){
		itrnd = T.MoveRight();
		if(!itrnd){
		  itrnd = T.MoveUp();
		  if(!itrnd) return false;
		}
		else break;
	  }
	}
	STree::SNode *curnd = T.GetCurrentNode();
	cur = (DMOSP_Node *)curnd->Data();
  }
}

void DMOSP_Network::alpha_omega_N1(DMOSP_Problem::SOperation *v1,
					int &alpha, int &omega, int &apr_alpha, int &apr_omega)
{
  DMOSP_Node *nd_v1 = &Nodes[v1->i];
  int l = sizeof_tau(v1->Job->i);

  if(nd_v1->m_prv == -1) alpha = apr_alpha = 1;
  else{
	alpha = tau_pos(nd_v1);
	apr_alpha = alpha;
	if(alpha != 1){
	  DMOSP_Node *mprv = &Nodes[nd_v1->m_prv];
	  DMOSP_Node *cur = nd_v1;

	  // Find exact value
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(isThereAPath(cur, mprv)) break;
		alpha--;
	  }

	  // Find approximate value
	  apr_alpha = l;
	  cur = &Nodes[LastjNode[v1->Job->i]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		apr_alpha--;
	  }
	}
  }

  if(nd_v1->m_nxt == -1) omega = apr_omega = l;
  else{
	omega = tau_pos(nd_v1);
	if(omega != l){
	  DMOSP_Node *mnxt = &Nodes[nd_v1->m_nxt];
	  DMOSP_Node *cur = nd_v1;

	  // Find exact value
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(isThereAPath(mnxt, cur)) break;
		omega++;
	  }

	  // Find approximate value
	  apr_omega = 1;
	  cur = &Nodes[FirstjNode[v1->Job->i]];
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		apr_omega++;
	  }
	}
	else apr_omega = l;
  }
}

double DMOSP_Network::phi_N1(DMOSP_Problem::SOperation *v1)
{
  double phi;

  DMOSP_Node &v = Nodes[v1->i];
  phi =   ((v.m_prv == -1)? 0 : (Nodes[v.m_prv].estart + Nodes[v.m_prv].weight))
		+ v.weight + ((v.m_nxt == -1)? 0 : Nodes[v.m_nxt].tail);

  return phi;
}

void DMOSP_Network::ApproxDeltas_N1(DMOSP_Problem::SOperation *v1, int x1,
					   double &hat_delta, double &check_delta)
{
  DMOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 == x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  DMOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	DMOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd) check_delta += jx1nd->tail;
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0;
	DMOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd) hat_delta += (jx1nd->estart + jx1nd->weight);
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  DMOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void DMOSP_Network::EstDeltas_N1(DMOSP_Problem::SOperation *v1, int x1,
					   double &hat_delta, double &check_delta)
{
  DMOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 == x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  DMOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else if(x1 < x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	DMOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate estimate of the nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  DMOSP_Node *j_xnd = &Nodes[v.j_prv];
	  double gjv1nxt = (v.j_nxt != -1)? Nodes[v.j_nxt].tail : 0;
	  double gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
	  double g = j_xnd->weight + ((gjv1nxt > gmx1)? gjv1nxt : gmx1);
	  if(x1 < x - 1){
		for(int _x = x - 2; _x >= x1; _x--){
		  j_xnd = &Nodes[j_xnd->j_prv];
		  gmx1 = (j_xnd->m_nxt != -1)? Nodes[j_xnd->m_nxt].tail : 0;
		  g = j_xnd->weight + ((g > gmx1)? g : gmx1);
		}
	  }

	  check_delta += g;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0.0;
	DMOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate estimated value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  DMOSP_Node *j_xnd = &Nodes[v.j_nxt];
	  double etjv1prv = (v.j_prv != -1)? Nodes[v.j_prv].estart + Nodes[v.j_prv].weight : 0;
	  double etmx1 = (j_xnd->m_prv != -1)? Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight : 0;
	  double e = (etjv1prv > etmx1)? etjv1prv : etmx1;
	  if(x1 > x + 1){
		for(int _x = x + 2; _x <= x1; _x++){
		  double prvwt = j_xnd->weight;
		  j_xnd = &Nodes[j_xnd->j_nxt];
		  etmx1 = (j_xnd->m_prv != -1)? (Nodes[j_xnd->m_prv].estart + Nodes[j_xnd->m_prv].weight) : 0;
		  e = (e + prvwt > etmx1)? (e + prvwt) : etmx1;
		}
	  }
	  hat_delta += (e + jx1nd->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  DMOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void DMOSP_Network::ExactDeltas_N1(DMOSP_Network *Net_, DMOSP_Problem::SOperation *v1, int x1,
					   double &hat_delta, double &check_delta)
{
  DMOSP_Node &v = Nodes[v1->i];
  int x = tau_pos(&v);
  if(x1 <= x){
	hat_delta = 0.0;
	if(x1 != 1){
	  DMOSP_Node *jx1prv = tauNode_at(v1->Job->i, x1-1);
	  hat_delta += (jx1prv->estart + jx1prv->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	DMOSP_Node *jx1nd = Net_->tauNode_at(v1->Job->i, x1);
	if(jx1nd){
	  // Get an accurate  nodes tail value in network Gamma-1
	  // based on the values provided in Gamma network....
	  //
	  check_delta += jx1nd->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
  else{
	hat_delta = 0.0;
	DMOSP_Node *jx1nd = tauNode_at(v1->Job->i, x1);
	DMOSP_Node *jx1nd_ = Net_->tauNode_at(v1->Job->i, x1-1);
	if(jx1nd_){
	  // Get an accurate value of eta in the network Gamma-1
	  // based on the values available in network Gamma...
	  //
	  hat_delta += (jx1nd_->estart + jx1nd->weight);
	}
	if(v.m_prv != -1){
	  DMOSP_Node *mprv = &Nodes[v.m_prv];
	  hat_delta -= (mprv->estart + mprv->weight);
	}

	check_delta = 0.0;
	if(x1 < sizeof_tau(v1->Job->i)){
	  DMOSP_Node *jx1nxt = tauNode_at(v1->Job->i, x1+1);
	  check_delta += jx1nxt->tail;
	}
	if(v.m_nxt != -1){
	  DMOSP_Node *mnxt = &Nodes[v.m_nxt];
	  check_delta -= mnxt->tail;
	}
  }
}

void DMOSP_Network::alpha_omega_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2,
					int &alpha, int &omega, int &apr_alpha, int &apr_omega)
{
  DMOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i == nd_v2->mc){ // if same machine
	int l = sizeof_pi(nd_v2->mc);

	if(nd_v2->j_prv == -1) alpha = apr_alpha = 1;
	else{
	  alpha = pi_pos(nd_v2);
	  apr_alpha = alpha;
	  if(alpha != 1){
		DMOSP_Node *jprv = &Nodes[nd_v2->j_prv];
		DMOSP_Node *cur = nd_v2;

		// Find exact value
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(isThereAPath(cur, jprv)) break;
		  alpha--;
		}

		// Find approximate value
		apr_alpha = l;
		cur = &Nodes[LastmNode[nd_v2->mc]];
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		}
	  }
	}

	if(nd_v2->j_nxt == -1) omega = apr_omega = l;
	else{
	  omega = pi_pos(nd_v2);
	  if(omega != l){
		DMOSP_Node *jnxt = &Nodes[nd_v2->j_nxt];
		DMOSP_Node *cur = nd_v2;

		// Find exact value
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(isThereAPath(jnxt, cur)) break;
		  omega++;
		}

		// Find approximate value
		apr_omega = 1;
		cur = &Nodes[FirstmNode[nd_v2->mc]];
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  apr_omega++;
		}
	  }
	  else apr_omega = l;
	}
  }
  else{
	int l = sizeof_pi(mk2->i);

	if(nd_v2->j_prv == -1) alpha = apr_alpha = 1;
	else{
	  alpha = l + 1;
	  apr_alpha = alpha;
	  DMOSP_Node *jprv = &Nodes[nd_v2->j_prv];
	  DMOSP_Node *cur;

	  if(LastmNode[mk2->i] != -1){
	    // Find exact value
		cur = &Nodes[LastmNode[mk2->i]];
		do{
		  if(isThereAPath(cur, jprv)) break;
		  alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);

		// Find approximate value
		cur = &Nodes[LastmNode[mk2->i]];
		do{
		  if(cur->estart + cur->weight < jprv->estart) break;
		  apr_alpha--;
		  if(cur->m_prv == -1) break;
		  cur = &Nodes[cur->m_prv];
		}while(true);
	  }
	}

	if(nd_v2->j_nxt == -1) omega = apr_omega = l + 1;
	else{
	  omega = apr_omega = 1;
	  DMOSP_Node *jnxt = &Nodes[nd_v2->j_nxt];
	  DMOSP_Node *cur;

	  if(FirstmNode[mk2->i] != -1){
		// Find exact value
		cur = &Nodes[FirstmNode[mk2->i]];
		do{
		  if(isThereAPath(jnxt, cur)) break;
		  omega++;
		  if(cur->m_nxt == -1) break;
		  cur = &Nodes[cur->m_nxt];
		}while(true);

		// Find approximate value
		cur = &Nodes[FirstmNode[mk2->i]];
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

double DMOSP_Network::phi_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2)
{
  double phi;

  DMOSP_Node &v = Nodes[v2->i];
  phi =   ((v.j_prv == -1)? 0 : (Nodes[v.j_prv].estart + Nodes[v.j_prv].weight))
		+ ((v.j_nxt == -1)? 0 : Nodes[v.j_nxt].tail);

  double d = Alg->pt[v2->Job->i][mk2->i];
  phi += d;

  return phi;
}

void DMOSP_Network::ApproxDeltas_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2, int x2,
							double &hat_delta, double &check_delta)
{
  DMOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0.0;
	check_delta = 0.0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  DMOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

	int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  DMOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	DMOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 == x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < sizeof_pi(mk2->i)){
		DMOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  DMOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd) check_delta += mx2nd->tail;
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0.0;
	  DMOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd) hat_delta += (mx2nd->estart + mx2nd->weight);
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < sizeof_pi(mk2->i)){
		DMOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void DMOSP_Network::EstDeltas_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2, int x2,
							double &hat_delta, double &check_delta)
{
  DMOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0.0;
	check_delta = 0.0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  DMOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

    int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  DMOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	DMOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 == x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < sizeof_pi(mk2->i)){
		DMOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else if(x2 < x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  DMOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate estimate of the nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		DMOSP_Node *m_xnd = &Nodes[v.m_prv];
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
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0.0;
	  DMOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate estimated value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
		DMOSP_Node *m_xnd = &Nodes[v.m_nxt];
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
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < sizeof_pi(mk2->i)){
		DMOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void DMOSP_Network::ExactDeltas_N2(DMOSP_Network *Net_, DMOSP_Problem::SOperation *v2,
					 DMOSP_Problem::SMC *mk2, int x2, double &hat_delta, double &check_delta)
{
  DMOSP_Node *nd_v2 = &Nodes[v2->i];

  if(mk2->i != nd_v2->mc){ // if different machine
	hat_delta = 0.0;
	check_delta = 0.0;

	if(nd_v2->j_prv != -1) hat_delta -= (Nodes[nd_v2->j_prv].estart +
										 Nodes[nd_v2->j_prv].weight);

	if(x2 != 1){
	  DMOSP_Node *ndx2prv = piNode_at(mk2->i, x2-1);
	  hat_delta += (ndx2prv->estart + ndx2prv->weight);
	}

	if(nd_v2->j_nxt != -1) check_delta -= Nodes[nd_v2->j_nxt].tail;

	int l = sizeof_pi(mk2->i);
	if(x2 != l+1){
	  DMOSP_Node *ndx2nxt = piNode_at(mk2->i, x2);
	  check_delta += ndx2nxt->tail;
	}
  }
  else{
	DMOSP_Node &v = Nodes[v2->i];
	int x = pi_pos(&v);
	if(x2 <= x){
	  hat_delta = 0.0;
	  if(x2 != 1){
		DMOSP_Node *mx2prv = piNode_at(mk2->i, x2-1);
		hat_delta += (mx2prv->estart + mx2prv->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  DMOSP_Node *mx2nd = Net_->piNode_at(mk2->i, x2);
	  if(mx2nd){
		// Get an accurate  nodes tail value in network Gamma-1
		// based on the values provided in Gamma network....
		//
		check_delta += mx2nd->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
	else{
	  hat_delta = 0.0;
	  DMOSP_Node *mx2nd = piNode_at(mk2->i, x2);
	  DMOSP_Node *mx2nd_ = Net_->piNode_at(mk2->i, x2-1);
	  if(mx2nd_){
		// Get an accurate value of eta in the network Gamma-1
		// based on the values available in network Gamma...
		//
		hat_delta += (mx2nd_->estart + mx2nd->weight);
	  }
	  if(v.j_prv != -1){
		DMOSP_Node *jprv = &Nodes[v.j_prv];
		hat_delta -= (jprv->estart + jprv->weight);
	  }

	  check_delta = 0.0;
	  if(x2 < sizeof_pi(mk2->i)){
		DMOSP_Node *mx2nxt = piNode_at(mk2->i, x2+1);
		check_delta += mx2nxt->tail;
	  }
	  if(v.j_nxt != -1){
		DMOSP_Node *jnxt = &Nodes[v.j_nxt];
		check_delta -= jnxt->tail;
	  }
	}
  }
}

void DMOSP_Network::alpha_omega_B1(int j, int x1, int x2, int &alpha, int &omega,
								  int &apr_alpha, int &apr_omega)
{
  int l = sizeof_tau(j);

  alpha = apr_alpha = 1;
  omega = apr_omega = l - (x2 - x1);

  for(int x=x1; x <= x2; x++){  // of course x1 <= x2
	DMOSP_Node *v = tauNode_at(j, x);
	int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	if(v->m_prv == -1 || x1 == 1) v_alpha = v_apr_alpha = 1;
	else{
	  v_alpha = v_apr_alpha = x1;
	  DMOSP_Node *mprv = &Nodes[v->m_prv];
	  DMOSP_Node *cur = tauNode_at(j, x1);

	  // Find exact value
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(isThereAPath(cur, mprv)) break;
		v_alpha--;
	  }

	  // Find approximate value
	  cur = tauNode_at(j, x1);  //&Nodes[LastjNode[j]];
	  while(cur->j_prv != -1){
		cur = &Nodes[cur->j_prv];
		if(cur->estart + cur->weight < mprv->estart) break;
		v_apr_alpha--;
	  }
	}

	if(v->m_nxt == -1 || x2 == l) v_omega = v_apr_omega = l - (x2 - x1);
	else{
	  v_omega = v_apr_omega = x1;
	  DMOSP_Node *mnxt = &Nodes[v->m_nxt];
	  DMOSP_Node *cur = tauNode_at(j, x2);

	  // Find exact value
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(isThereAPath(mnxt, cur)) break;
		v_omega++;
	  }

	  // Find approximate value
	  cur = tauNode_at(j, x2);
	  while(cur->j_nxt != -1){
		cur = &Nodes[cur->j_nxt];
		if(mnxt->tail - mnxt->weight > cur->tail) break;
		v_apr_omega++;
	  }
	}

	if(v_alpha > alpha) alpha = v_alpha;
	if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	if(v_omega < omega) omega = v_omega;
	if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
  }
}

void DMOSP_Network::alpha_omega_B2(int m1, int m2, int x1, int x2, int &alpha,
								  int &omega, int &apr_alpha, int &apr_omega)
{
  if(m1 == m2){ // if same machine
	int l = sizeof_pi(m1);

	alpha = apr_alpha = 1;
	omega = apr_omega = l - (x2 - x1);

	for(int x = x1; x <= x2; x++){
	  DMOSP_Node *v = piNode_at(m1, x);
	  int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	  if(v->j_prv == -1 || x1 == 1) v_alpha = v_apr_alpha = 1;
	  else{
		v_alpha = v_apr_alpha = x1;
		DMOSP_Node *jprv = &Nodes[v->j_prv];
		DMOSP_Node *cur = piNode_at(m1, x1);

		// Find exact value
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(isThereAPath(cur, jprv)) break;
		  v_alpha--;
		}

		// Find approximate value
		cur = piNode_at(m1, x1);
		while(cur->m_prv != -1){
		  cur = &Nodes[cur->m_prv];
		  if(cur->estart + cur->weight < jprv->estart) break;
		  v_apr_alpha--;
		}
	  }

	  if(v->j_nxt == -1 || x2 == l) v_omega = v_apr_omega = l - (x2 - x1);
	  else{
		v_omega = x1;
		DMOSP_Node *jnxt = &Nodes[v->j_nxt];
		DMOSP_Node *cur = piNode_at(m1, x2);

		// Find exact value
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(isThereAPath(jnxt, cur)) break;
		  v_omega++;
		}

		// Find approximate value
		v_apr_omega = x1;
		cur = piNode_at(m1, x2);
		while(cur->m_nxt != -1){
		  cur = &Nodes[cur->m_nxt];
		  if(jnxt->tail - jnxt->weight > cur->tail) break;
		  v_apr_omega++;
		}
	  }

	  if(v_alpha > alpha) alpha = v_alpha;
	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_omega < omega) omega = v_omega;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
  else{ // in case of different machines
	int l = sizeof_pi(m2);

	alpha = apr_alpha = 1;
	omega = apr_omega = l + 1;

	for(int x = x1; x <= x2; x++){
	  DMOSP_Node *v = piNode_at(m1, x);
	  int v_alpha, v_apr_alpha, v_omega, v_apr_omega;

	  if(v->j_prv == -1) v_alpha = v_apr_alpha = 1;
	  else{
		v_alpha = v_apr_alpha = l + 1;
		DMOSP_Node *jprv = &Nodes[v->j_prv];
		DMOSP_Node *cur;

		if(LastmNode[m2] != -1){
		  // Find exact value
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(isThereAPath(cur, jprv)) break;
			v_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);

		  // Find approximate value
		  cur = &Nodes[LastmNode[m2]];
		  do{
			if(cur->estart + cur->weight < jprv->estart) break;
			v_apr_alpha--;
			if(cur->m_prv == -1) break;
			cur = &Nodes[cur->m_prv];
		  }while(true);
		}
	  }

	  if(v->j_nxt == -1) v_omega = v_apr_omega = l + 1;
	  else{
		v_omega = v_apr_omega = 1;
		DMOSP_Node *jnxt = &Nodes[v->j_nxt];
		DMOSP_Node *cur;

		if(FirstmNode[m2] != -1){
		  // Find exact value
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(isThereAPath(jnxt, cur)) break;
			v_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);

		  // Find approximate value
		  cur = &Nodes[FirstmNode[m2]];
		  do{
			if(jnxt->tail - jnxt->weight > cur->tail) break;
			v_apr_omega++;
			if(cur->m_nxt == -1) break;
			cur = &Nodes[cur->m_nxt];
		  }while(true);
		}
	  }

	  if(v_alpha > alpha) alpha = v_alpha;
	  if(v_apr_alpha > apr_alpha) apr_alpha = v_apr_alpha;
	  if(v_omega < omega) omega = v_omega;
	  if(v_apr_omega < apr_omega) apr_omega = v_apr_omega;
	}
  }
}

//------------------------------------------ Hill climbing neighborhood search

DMOSP_NSTester::DMOSP_NSTester(DMOSP_Problem *P, wchar_t *N)
  : DMOSP_Algorithm(P, N)
{
}

DMOSP_NSTester::DMOSP_NSTester(DMOSP_Problem *P, DMOSP_Solution *is)
  : DMOSP_Algorithm(P, L"Neighborhood Search Tester")
{
  iSol = is;
}

DMOSP_NSTester::~DMOSP_NSTester()
{
}

void DMOSP_NSTester::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();

	 #ifdef REPORT_INTERMEDIATE_RESULTS
		  dbgFile = fopen("Debug.txt", "a+");
	 #endif
}

void DMOSP_NSTester::Initialize()
{
  if(iSol->Problem != Problem) return;

  // Copy tasks for the given initial solution to the current tasks
  for(int i=0; i<nOps; i++){
	DMOSP_Solution::SchTask *gtsk = iSol->TaskAssociatedToOperation(O[i]);
	memcpy(tsk[i], gtsk, sizeof(DMOSP_Solution::SchTask));
  }

	 #ifdef REPORT_NSTester_RESULTS
		  wchar_t buf[256];
		  swprintf(buf, L"Initial solution as kept in NS Tester:\n\n");
		  fwprintf(dbgFile, buf);
		  swprintf(buf, L"\tNo.\tID\tJob\tM/C\tStart\tEnd\n");
		  fwprintf(dbgFile, buf);
		  for(int i=0; i<nOps; i++){
			swprintf(buf, L"\t%i\t%s\t%s\t%s\t%0.2f\t%0.2f\n",
						 i, O[i]->ID(), O[i]->Job->ID(),
						 tsk[i]->SelectedMC->ID(), tsk[i]->StartTime,
						 tsk[i]->EndTime);
			fwprintf(dbgFile, buf);
		  }
		  swprintf(buf, L"\n\tCmax = %0.2f\n\n", Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN));
		  fwprintf(dbgFile, buf);
		  fflush(dbgFile);
	 #endif
}

bool DMOSP_NSTester::Stop()
{
  return true;
}

void DMOSP_NSTester::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_NSTester::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

	 #ifdef REPORT_INTERMEDIATE_RESULTS
			  fclose(dbgFile);
	 #endif
}

void DMOSP_NSTester::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//------------------------------------------------------- DMOSP_MILPSolution

DMOSP_MILPSolution::DMOSP_MILPSolution(DMOSP_Problem *prb)
{
  Problem = prb;
  AllocateMemory();
  Solution = NULL;
  bTasksModified = false;
}

DMOSP_MILPSolution::DMOSP_MILPSolution(DMOSP_Solution *sol)
{
  sol->ScheduledTasks.QSort(tskComp, ASCENDING);
  Problem = sol->Problem;
  AllocateMemory();
  Solution = sol;

  Tasks = new DMOSP_Solution::SchTask*[no];

  SList::SNode *tsknd = sol->ScheduledTasks.head();
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	Tasks[tsk->Operation->i] = tsk;
	s[tsk->Operation->i] = tsk->StartTime;
	if(tsk->EndTime > c[tsk->Operation->Job->i])
	  c[tsk->Operation->Job->i] = tsk->EndTime;

	int ypos = 0;
	for(int o=0; o<no; o++){
	  for(int m=0; m<nm; m++){
		if(o == tsk->Operation->i && m == tsk->SelectedMC->i){
		  y[ypos] = 1;
		  break;
		}
		if(OW[o] == MW[m]) ypos++;
	  }
	}
	tsknd = tsknd->Next();
  }

  int xpos = 0;
  for(SList::SNode *jnd = Problem->Jobs.head(); jnd; jnd = jnd->Next()){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  SList::SNode *nxtopnd = opnd->Next();
	  while(nxtopnd){
		DMOSP_Problem::SOperation *nxtop = (DMOSP_Problem::SOperation *)nxtopnd->Data();
		if(Tasks[op->i]->StartTime < Tasks[nxtop->i]->StartTime) x[xpos] = 1;
		xpos++;
		nxtopnd = nxtopnd->Next();
	  }
	  opnd = opnd->Next();
	}
  }

  int j, k, m, zpos = 0;
  for(j=0; j<no; j++){
	for(k=0; k<no; k++){
	  for(m=0; m<nm; m++){
		if(j != k && OW[j] == OW[k] && OW[j] == MW[m]){
		  if(Tasks[j]->SelectedMC->i == Tasks[k]->SelectedMC->i &&
			 Tasks[j]->SelectedMC->i == m){
			if(Tasks[j]->StartTime < Tasks[k]->StartTime) z[zpos] = 1;
		  }
		  zpos++;
		}
	  }
	}
  }

  bTasksModified = false;
}

DMOSP_MILPSolution::~DMOSP_MILPSolution()
{
  delete [] OJ;
  delete [] OW;
  delete [] MW;
  delete [] c;
  delete [] s;
  delete [] x;
  delete [] y;
  delete [] z;
  if(Solution) delete [] Tasks;
}

void DMOSP_MILPSolution::AllocateMemory()
{
  nj = Problem->Jobs.Count();

  no = 0;
  unsigned int noj;
  xsz = 0;
  for(SList::SNode *jnd = Problem->Jobs.head(); jnd; jnd = jnd->Next()){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	noj = jb->Operations.Count();
	no += noj;
    xsz += ((noj * (noj - 1)) / 2);
  }

  int j = 1, o = 0;
  OJ = new int[no];
  OW = new int[no];
  for(SList::SNode *jnd = Problem->Jobs.head(); jnd; jnd = jnd->Next()){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	jb->i = j-1;
	for(SList::SNode *ond = jb->Operations.head(); ond; ond = ond->Next()){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)ond->Data();
	  op->i = o;
	  OJ[o] = j;
	  int w = Problem->WCs.IndexOf((void *)op->WC) + 1;
	  OW[o] = w;
	  ++o;
	}
	++j;
  }

  nm = 0;
  for(SList::SNode *wnd = Problem->WCs.head(); wnd; wnd = wnd->Next()){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)wnd->Data();
	nm += wc->MCs.Count();
  }

  int w = 1, m = 0;
  MW = new int[nm];
  for(SList::SNode *wnd = Problem->WCs.head(); wnd; wnd = wnd->Next()){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)wnd->Data();
	for(SList::SNode *mnd = wc->MCs.head(); mnd; mnd = mnd->Next()){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mnd->Data();
	  mc->i = m;
	  MW[m] = w;
	  ++m;
	}
	++w;
  }

  c = new double[nj];
  s = new double[no];
  x = new char[xsz];

  ysz = 0;
  for(o=0; o<no; o++)
	for(m=0; m<nm; m++)
	  if(OW[o] == MW[m]) ysz++;

  y = new char[ysz];

  zsz = 0;
  for(j=0; j<no; j++)
	for(o=0; o<no; o++)
	  for(m=0; m<nm; m++)
		if(j != o && OW[j] == OW[o] && OW[j] == MW[m]) zsz++;

  z = new char[zsz];

  memset(c, 0, nj * sizeof(double));
  memset(s, 0, no * sizeof(double));
  memset(x, 0, xsz * sizeof(char));
  memset(y, 0, ysz * sizeof(char));
  memset(z, 0, zsz * sizeof(char));
}

void DMOSP_MILPSolution::ModifyTasks()
{
  if(Solution == NULL) return;

  // Modify the tasks of the current solution to represent the given
  // MILP solution. It is assumed that the CPLEX MILP solution is complete
  // and feasible.
  //
  int ypos = 0;
  for(int o=0; o<no; o++){
	// Task start time
	Tasks[o]->StartTime = s[o];

	for(int m=0; m<nm; m++){
	  if(OW[o] != MW[m]) continue;

	  if(y[ypos] == 1){
	    DMOSP_Problem::SMC *SelMC = NULL;
		for(SList::SNode *wnd = Problem->WCs.head(); wnd; wnd = wnd->Next()){
		  DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)wnd->Data();
		  for(SList::SNode *mnd = wc->MCs.head(); mnd; mnd = mnd->Next()){
			DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mnd->Data();
			if(mc->i == m){
			  SelMC = mc;
			  break;
			}
		  }
		  if(SelMC) break;
		}

		Tasks[o]->SelectedMC = SelMC;

		// Task completion time
		DMOSP_Problem::SOperation *op = NULL;
		for(SList::SNode *jnd = Problem->Jobs.head(); jnd; jnd = jnd->Next()){
		  DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
		  SList::SNode *opnd = jb->Operations.head();
		  while(opnd){
			DMOSP_Problem::SOperation *thisop = (DMOSP_Problem::SOperation *)opnd->Data();
			if(thisop->i == o){
			  op = thisop;
			  break;
			}
			opnd = opnd->Next();
		  }
		  if(op != NULL) break;
		}

		for(SList::SNode *altmcnd = op->AltMCs.head(); altmcnd; altmcnd = altmcnd->Next()){
		  DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)altmcnd->Data();
		  if(opmc->MC == SelMC){
			Tasks[o]->EndTime = Tasks[o]->StartTime + opmc->pt;
			break;
		  }
		}
	  }
	  ypos++;
	}
  }
  bTasksModified = (Solution->isFeasible())? true : false;
}

//-----------------------------------------------------------------------------
#pragma package(smart_init)
