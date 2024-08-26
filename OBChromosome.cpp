//---------------------------------------------------------------------------


#pragma hdrstop

#include "OBChromosome.h"
#include "SList.h"

//------------------------------------------------------------ Gene structure

DMOSP_OBGene::DMOSP_OBGene(DMOSP_Problem::SOperation *op, DMOSP_Problem::SMC *mc)
{
  Operation = op;
  Machine = mc;
}

DMOSP_OBGene::DMOSP_OBGene(DMOSP_OBGene *g)
{
  Copy(g);
}

DMOSP_OBGene::DMOSP_OBGene()
{
  Operation = NULL;
  Machine = NULL;
}

void DMOSP_OBGene::Copy(DMOSP_OBGene *g)
{
  Operation = g->Operation;
  Machine = g->Machine;
}

DMOSP_OBGene &DMOSP_OBGene::operator = (DMOSP_OBGene &g)
{
  Operation = g.Operation;
  Machine = g.Machine;
  return *this;
}

//---------------------- Decoder algorithm for the operations-based chromosome

DMOSP_OBChromosome::Decoder::Decoder(DMOSP_OBChromosome *chrom)
  : DMOSP_Algorithm(chrom->getProblem(), L"Decoder for the operations-based chromosome"),
	Chromosome(chrom)
{
}

DMOSP_OBChromosome::Decoder::~Decoder()
{
}

void DMOSP_OBChromosome::Decoder::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();
  mcrt = new double[Problem->nMachines()];
  jbrt = new double[Problem->Jobs.Count()];
}

void DMOSP_OBChromosome::Decoder::Initialize()
{
  // This decoding algorithm assumes that the given chromosome does not have
  // duplicate operations in the list and the assigned machines to operations
  // are acceptable candidates.
  // Note: The procedure used herein will result in any sort of infeasibility,
  //  therefore, there is no need to check it.

  int i, j, m, l;

  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
  }

  DMOSP_OBGene *Genes = Chromosome->getGenes();
  for(l=0; l<Chromosome->getLength(); l++){
	 DMOSP_Problem::SOperation *op = Genes[l].Operation;
	 DMOSP_Problem::SMC *mc = Genes[l].Machine;

	 j = op->Job->i;  // job index
	 i = op->i; // operation index
	 m = mc->i;  // machine index

	 DMOSP_Solution::SchTask *task = tsk[i];
	 task->SelectedMC = mc;
	 task->StartTime = max(jbrt[j], mcrt[m]);
	 task->EndTime = task->StartTime + pt[j][m];
	 task->bScheduled = true;
	 jbrt[j] = task->EndTime;
	 mcrt[m] = task->EndTime;
  }

  Solution->Cmax = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  Solution->MWFT = Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);

  SolStatus = FEASIBLE;
}

bool DMOSP_OBChromosome::Decoder::Stop()
{
  return true;
}

void DMOSP_OBChromosome::Decoder::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_OBChromosome::Decoder::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  delete [] mcrt;
  delete [] jbrt;
}

void DMOSP_OBChromosome::Decoder::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//------------------------------------- Operations-based chromosome operations

DMOSP_OBChromosome::DMOSP_OBChromosome(DMOSP_Problem *Prob)
{
  Problem = Prob;
  length = Problem->nOperations();
  Genes = new DMOSP_OBGene[length];
  bConstructed = false;
}

DMOSP_OBChromosome::DMOSP_OBChromosome(DMOSP_OBChromosome *chrom)
{
  Problem = chrom->Problem;
  length = Problem->nOperations();
  Genes = new DMOSP_OBGene[length];
  bConstructed = false;
  Copy(chrom);
}

DMOSP_OBChromosome::~DMOSP_OBChromosome()
{
  delete [] Genes;
}

bool DMOSP_OBChromosome::Copy(DMOSP_OBChromosome *chrom)
{
  if(Problem != chrom->Problem) return false;

  memcpy((void *)Genes, (void *)chrom->Genes, length * sizeof(DMOSP_OBGene));
  bConstructed = chrom->bConstructed;
  return true;
}

bool DMOSP_OBChromosome::Encode(DMOSP_Solution *Sol)
{
  // Convert a given solution into the operations-based chromosome representation

  // if different problem, return false
  if(Sol->Problem != Problem) return false;

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  int i = 0;
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL){
	  Genes[i].Operation = tsk->Operation;
	  Genes[i].Machine = tsk->SelectedMC;
	  ++i;
	}
	tsknd = tsknd->Next();
  }

  // If the coding is successful
  bConstructed = true;
  return true;
}

bool DMOSP_OBChromosome::Decode(DMOSP_Solution *Sol)
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
