//---------------------------------------------------------------------------


#pragma hdrstop

#include "RKChromosome.h"
#include "SList.h"
#include "Random.h"

//-------------------------------------- Definition of the quick sort function

void RKQSort(DMOSP_Problem::SOperation **Ops, DMOSP_Problem::SMC **MCs, double *key, int l, int r)
{
  int i,j;
  double v;
  double tk;
  DMOSP_Problem::SOperation *top;
  DMOSP_Problem::SMC *tmc;

  if(r > l){
	v = key[r]; i = l-1; j = r;
	for(;;){
	  while(key[++i] > v);
	  while(key[--j] < v && j > 0);
	  if(i >= j) break;
	  top = Ops[i]; Ops[i] = Ops[j]; Ops[j] = top;
	  tmc = MCs[i]; MCs[i] = MCs[j]; MCs[j] = tmc;
	  tk = key[i]; key[i] = key[j]; key[j] = tk;
	}
	top = Ops[i]; Ops[i] = Ops[r]; Ops[r] = top;
	tmc = MCs[i]; MCs[i] = MCs[r]; MCs[r] = tmc;
	tk = key[i]; key[i] = key[r]; key[r] = tk;
	RKQSort(Ops, MCs, key, l, i-1);
	RKQSort(Ops, MCs, key, i+1, r);
  }
}

//--------------------------- Decoder algorithm for the random keys chromosome

DMOSP_RKChromosome::Decoder::Decoder(DMOSP_RKChromosome *chrom)
  : DMOSP_Algorithm(chrom->getProblem(), L"Decoder for the random keys chromosome"),
	Chromosome(chrom)
{
}

DMOSP_RKChromosome::Decoder::~Decoder()
{
}

void DMOSP_RKChromosome::Decoder::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();
  mcrt = new double[Problem->nMachines()];
  jbrt = new double[Problem->Jobs.Count()];
}

DMOSP_Problem::SMC *DMOSP_RKChromosome::Decoder::SelectedMachine(int opIndex, double key)
{
  DMOSP_Problem::SMC *mc = NULL;
  double step = 1.0 / (double)(O[opIndex]->AltMCs.Count());
  double pos = step;
  for(SList::SNode *mcnd = O[opIndex]->AltMCs.head(); mcnd; mcnd = mcnd->Next()){
	 if(key <= pos){
	   DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
	   mc = opmc->MC;
	   break;
	 }
	 pos += step;
  }
  return mc;
}

void DMOSP_RKChromosome::Decoder::Initialize()
{
  // This procedure interprets the given keys into a feasible schedule. All keys
  //   must be double numbers from 0 to 1, otherwise an incomplete schedule
  //   will result.

  // Note: The procedure used herein will result in any sort of infeasibility,
  //  therefore, there is no need to check it.

  int i, j, m, l;

  // First, we need to convert the random keys stored in Genes as double values
  // into the corresponding list of operations with their assigned machines
  //
  double *keys = new double[nOps];
  DMOSP_Problem::SOperation **Operations = new DMOSP_Problem::SOperation *[nOps];
  DMOSP_Problem::SMC **Machines = new DMOSP_Problem::SMC *[nOps];

  double *Genes = Chromosome->getGenes();
  for(i=0; i<nOps; i++){
	keys[i] = Genes[i*2];
	Operations[i] = O[i];
	Machines[i] = SelectedMachine(i, Genes[i*2+1]);
  }
  RKQSort(Operations, Machines, keys, 0, nOps-1);

  // Now, we can construct the semi-active solution using the generated ordered
  // list of operations and assigned machines.
  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
  }

  for(l=0; l<nOps; l++){
	 DMOSP_Problem::SOperation *op = Operations[l];
	 DMOSP_Problem::SMC *mc = Machines[l];

	 j = op->Job->i;  // job index
	 i = op->i; // operation index
	 DMOSP_Solution::SchTask *task = tsk[i];

	 if(mc){
	   m = mc->i;  // machine index

	   task->SelectedMC = mc;
	   task->StartTime = max(jbrt[j], mcrt[m]);
	   task->EndTime = task->StartTime + pt[j][m];
	   task->bScheduled = true;
	   jbrt[j] = task->EndTime;
	   mcrt[m] = task->EndTime;
	 }
	 else{
	   task->SelectedMC = mc;
	   task->StartTime = -1;
	   task->EndTime = -1;
	   task->bScheduled = false;
	 }
  }

  delete [] Operations;
  delete [] Machines;
  delete [] keys;

  Solution->Cmax = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  Solution->MWFT = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);

  SolStatus = FEASIBLE;
}

bool DMOSP_RKChromosome::Decoder::Stop()
{
  return true;
}

void DMOSP_RKChromosome::Decoder::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_RKChromosome::Decoder::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  delete [] mcrt;
  delete [] jbrt;
}

void DMOSP_RKChromosome::Decoder::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//----------------------------------------- Random keys chromosome operations

DMOSP_RKChromosome::DMOSP_RKChromosome(DMOSP_Problem *Prob)
{
  Problem = Prob;
  length = 2 * Problem->nOperations();
  Genes = new double[length];
  bConstructed = false;
}

DMOSP_RKChromosome::DMOSP_RKChromosome(DMOSP_RKChromosome *chrom)
{
  Problem = chrom->Problem;
  length = 2 * Problem->nOperations();
  Genes = new double[length];
  bConstructed = false;
  Copy(chrom);
}

DMOSP_RKChromosome::~DMOSP_RKChromosome()
{
  delete [] Genes;
}

bool DMOSP_RKChromosome::Copy(DMOSP_RKChromosome *chrom)
{
  if(Problem != chrom->Problem) return false;

  memcpy((void *)Genes, (void *)(chrom->Genes), length * sizeof(double));
  bConstructed = chrom->bConstructed;
  return true;
}

bool DMOSP_RKChromosome::Encode(DMOSP_Solution *Sol)
{
  // Convert a given solution into the random keys chromosome representation

  // if different problem, return false
  if(Sol->Problem != Problem) return false;

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  // first make sure that indexes for operations are assigned to keep their
  // arbitrary order
  SList::SNode *nd = Problem->Jobs.head();
  int i = 0;
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  op->i = i;
	  i++;
	  opnd = opnd->Next();
	}
	nd = nd->Next();
  }

  // Now get the rank of each operation and its assigned machine index
  int *rank = new int[i];      // Note: i = number of operations
  int *mcindx = new int[i];
  int *nmcs = new int[i];

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  i = 0;
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL){
	  rank[tsk->Operation->i] = i;
	  mcindx[tsk->Operation->i] = 0;
	  for(SList::SNode *mcnd = tsk->Operation->AltMCs.head(); mcnd; mcnd = mcnd->Next()){
		DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		if(tsk->SelectedMC == opmc->MC) break;
		mcindx[tsk->Operation->i]++;
	  }
	  nmcs[tsk->Operation->i] = tsk->Operation->AltMCs.Count();
	  ++i;
	}
	tsknd = tsknd->Next();
  }

  // Next, we convert the recorded ranks into random keys
  for(i=0; i<length; i+=2){
	double r = rndreal(0, 2.0 / (double)length);
	Genes[i] = 1.0 - rank[i/2] * 2.0 / (double)length - r;
	r = rndreal(0, 1.0 / (double)nmcs[i/2]);
	Genes[i+1] = (double)mcindx[i/2] / (double)nmcs[i/2] + r;
  }

  // If the coding is successful
  bConstructed = true;

  delete [] rank;
  delete [] mcindx;
  delete [] nmcs;
  return true;
}

bool DMOSP_RKChromosome::Decode(DMOSP_Solution *Sol)
{
  // Convert the current operations-based chromosome representation to a schedule
  // in the form of list of tasks

  // if different problem, return false
  if(Sol->Problem != Problem) return false;
  if(!bConstructed) return false;

  Decoder *decoder = new Decoder(this);
  decoder->Run();

  // Note that the solution will always be feasible
  Sol->Copy(decoder->Solution);
  delete decoder;

  return true;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
