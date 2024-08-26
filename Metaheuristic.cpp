//---------------------------------------------------------------------------


#pragma hdrstop

#include "Metaheuristic.h"
#include "random.h"


DMOSP_Metaheuristic::DMOSP_Metaheuristic(DMOSP_Problem *prob)
  : DMOSP_Algorithm(prob, L"Metaheuristic Algorithm")
{
}

DMOSP_Metaheuristic::DMOSP_Metaheuristic(DMOSP_Problem *prob, wchar_t *name, double TL)
  : DMOSP_Algorithm(prob, name, TL)
{
}

DMOSP_Metaheuristic::~DMOSP_Metaheuristic()
{
}

void DMOSP_Metaheuristic::DSG(DSG_JSR jsr, DSG_MSR msr,
                              double &Makespan, double &MWFT)
{
  // Initialize scheduling parameters
  //
  Makespan = MWFT = 0;
  int i, j, m;
  for(i=0; i<nMCs; i++) MCrt[i] = M[i]->ReadyTime;
  for(j=0; j<nJobs; j++){
	Jnft[j] = J[j]->ReleaseTime;
	bJcompleted[j] = false;
  }
  for(m=0; m<nMCs; m++) SumSchedTime[m] = 0;
  for (j = 0; j < nJobs; j++)
	for(m=0; m<nMCs; m++)
	  bJM[j][m] = (pt[j][m] > 0)? true : false;

  // Construct a new solution using the DGS heuristic using the given JSR and MSR
  // --> the constructed solution is stored in "Solution"
  double t;   // Current scheduling time

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
	t = MAXDOUBLE;
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
		  switch(jsr){
			case WSPT:   // Weighted shortest Processing Time
						 Mr = pt[j][m] / J[j]->Weight;
						 break;
			case MCTj:   // Minimum Completion Time
						 Mr = Jnft[j] + pt[j][m];
						 break;
			case WLWR:   // Weighted least Work Remaining
						 Mr = 0.0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   Mr += O[i]->minpt();
						 }
						 Mr /= J[j]->Weight;
						 break;
		  }
		  break;
		}
		double CandMr;
		for(j++; j<nJobs; j++){
		  if(!SchedulableJobs[j]) continue;
		  switch(jsr){
			case WSPT:   // Shortest Processing Time
						 CandMr = pt[j][m] / J[j]->Weight;
						 if(CandMr < Mr){
						   SelJob[m] = j;
						   Mr = CandMr;
						 }
						 break;
			case MCTj:   // Minimum Completion Time
						 CandMr = Jnft[j] + pt[j][m];
						 if(CandMr < Mr){
						   SelJob[m] = j;
						   Mr = CandMr;
						 }
						 break;
			case WLWR:   // Lease Work Remaining
						 CandMr = 0.0;
						 for(i=Ja[j]; i<=Jz[j]; i++){
						   if(bScheduled[i]) continue;
						   CandMr += O[i]->minpt();
						 }
						 CandMr /= J[j]->Weight;
						 if(CandMr < Mr){
						   Mr = CandMr;
						   SelJob[m] = j;
						 }
						 break;
			case RANDj:  // Random
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
		  if(msr == MWL){
			// Select the machine with the minimum load
			if(SumSchedTime[m] <= SumSchedTime[m2]) SelMC = m;
			else SelMC = m2;
		  }
		  else if(msr == MCTm){
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
	  if(Jnft[j] > Makespan) Makespan = Jnft[j];  // <--------------------- Update Cmax

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

  for(j=0; j<nJobs; j++){
	MWFT += ((Jnft[j] - J[j]->ReleaseTime) * J[j]->Weight);
  }
  MWFT /= (double)(nJobs);
}

void DMOSP_Metaheuristic::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();

  // Allocate memory for the DSG constructive heuristic
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

  // Arrays used for constructing solutions and interpretting chromosomes
  mcrt = new double[nMCs];
  jbrt = new double[nJobs];
  njOps = new int[nJobs];

  // Arrays used for interpretting random keys chromosomes
  keys = new double[nOps];
  key_Operations = new DMOSP_Problem::SOperation *[nOps];
  key_Machines = new DMOSP_Problem::SMC *[nOps];

  // arrays used for converting OB chromosome to RK chromosome
  rank = new int[nOps];
  mcindx = new int[nOps];
  nmcs = new int[nOps];

  // arrays used for BPB chromosome
  jbList = new SList[nJobs];
  mcList = new SList[nMCs];
  nxt = new int[nJobs];
}

void DMOSP_Metaheuristic::Initialize()
{
}

bool DMOSP_Metaheuristic::Stop()
{
  SolStatus = FEASIBLE;
  return true;
}

void DMOSP_Metaheuristic::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_Metaheuristic::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  // free allocated memroy for the DSG constructive heuristic
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

  // free allocated memory for chromosomes coding/decoding
  delete [] mcrt;
  delete [] jbrt;
  delete [] njOps;

  delete [] key_Operations;
  delete [] key_Machines;
  delete [] keys;

  delete [] rank;
  delete [] mcindx;
  delete [] nmcs;

  for(int j=0; j<nJobs; j++) jbList[j].Clear();
  for(int m=0; m<nMCs; m++) mcList[m].Clear();
  delete [] jbList;
  delete [] mcList;
  delete [] nxt;
}

void DMOSP_Metaheuristic::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

bool DMOSP_Metaheuristic::DecodeOBChromosome(DMOSP_OBChromosome *Chromosome,
											 DMOSP_Solution *Sol,
											 double &Cmax, double &MWFT)
{
  // This decoding algorithm assumes that the given chromosome does not have
  // duplicate operations in the list and the assigned machines to operations
  // are acceptable candidates.
  // Note: The procedure used herein will not result in any sort of infeasibility,
  //  therefore, there is no need to check it.

  Cmax = MWFT = 0.0;
  if(Problem != Sol->Problem || Problem != Chromosome->getProblem()) return false;

  int i, j, m, l;

  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
	njOps[j] = J[j]->Operations.Count();
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
	 njOps[j]--;
	 if(njOps[j] == 0){
	   if(Cmax < jbrt[j]) Cmax = jbrt[j];
       MWFT += J[j]->Weight * (jbrt[j] - J[j]->ReleaseTime);
	 }
	 mcrt[m] = task->EndTime;
  }

  if(Sol != Solution) Sol->Copy(Solution);
  MWFT /= (double)nJobs;
  Sol->Cmax = Cmax;
  Sol->MWFT = MWFT;
  Chromosome->setConstructed();
  return true;
}

bool DMOSP_Metaheuristic::DecodeRKChromosome(DMOSP_RKChromosome *Chromosome,
											 DMOSP_Solution *Sol,
											 double &Cmax, double &MWFT)
{
  // Note: The procedure used herein will not result in any sort of infeasibility,
  //  therefore, there is no need to check it.

  Cmax = MWFT = 0.0;
  if(Problem != Sol->Problem || Problem != Chromosome->getProblem()) return false;

  // This procedure interprets the given keys into a feasible schedule. All keys
  //   must be double numbers from 0 to 1, otherwise an incomplete schedule
  //   will result.

  // Note: The procedure used herein will result in any sort of infeasibility,
  //  therefore, there is no need to check it.

  int i, j, m, l;

  // First, we need to convert the random keys stored in Genes as double values
  // into the corresponding list of operations with their assigned machines
  //
  double *Genes = Chromosome->getGenes();
  for(i=0; i<nOps; i++){
	keys[i] = Genes[i*2];
	key_Operations[i] = O[i];

	// determine selected machine
	DMOSP_Problem::SMC *mc = NULL;
	double step = 1.0 / (double)(O[i]->AltMCs.Count());
	double pos = step;
	double key = Genes[i*2+1];
	for(SList::SNode *mcnd = O[i]->AltMCs.head(); mcnd; mcnd = mcnd->Next()){
	  if(key <= pos || mcnd->Next() == NULL){
		DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		mc = opmc->MC;
		break;
	  }
	  pos += step;
	}
	key_Machines[i] = mc;
  }
  RKQSort(key_Operations, key_Machines, keys, 0, nOps-1);

  // Now, we can construct the semi-active solution using the generated ordered
  // list of operations and assigned machines.
  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
	njOps[j] = J[j]->Operations.Count();
  }

  for(l=0; l<nOps; l++){
	 DMOSP_Problem::SOperation *op = key_Operations[l];
	 DMOSP_Problem::SMC *mc = key_Machines[l];

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
	   njOps[j]--;
	   if(njOps[j] == 0){
		 if(Cmax < jbrt[j]) Cmax = jbrt[j];
		 MWFT += J[j]->Weight * (jbrt[j] - J[j]->ReleaseTime);
	   }
	   mcrt[m] = task->EndTime;
	 }
	 else{
	   task->SelectedMC = mc;
	   task->StartTime = -1;
	   task->EndTime = -1;
	   task->bScheduled = false;
	 }
  }

  if(Sol != Solution) Sol->Copy(Solution);
  MWFT /= (double)nJobs;
  Sol->Cmax = Cmax;
  Sol->MWFT = MWFT;
  return true;
}

bool DMOSP_Metaheuristic::DecodeVPBChromosome(DMOSP_VPBChromosome *Chrom,
								DMOSP_Solution *Sol,
								double &Cmax, double &MWFT)
{
  // This procedure interprets the given strings into a schedule.
  // Note: the given VPB chromosome may result in an infeasible solution

  Cmax = MWFT = 0.0;
  if(Problem != Sol->Problem || Problem != Chrom->getProblem()) return false;

  // The decoding procedure converts the given strings into vectors of
  // processing sequences on jobs and machines, then it interprets these vectors
  // into a schedule after checking the feasibility conditions.
  //

  // First: convert the chromosomes into lists (vectors) of operations for
  // the jobs and the machines
  //
  int i, j;
  for(j=0; j<Chrom->getJobsString()->nSubstrings; j++){
	jbList[j].Clear();
	DMOSP_VPBSubString& jSubString = Chrom->getJobsString()->Substring[j];
	for(i = jSubString.start; i <= jSubString.end; i++){
	  DMOSP_Problem::SOperation *op = Chrom->getJobsString()->Gene[i];
	  jbList[j].Add((void *)op);
	}
  }
  int w, m;
  for(w=0, m=0; w<Chrom->getWorkstationsString()->nSubstrings; w++){
	DMOSP_VPBSubString& wSubString = Chrom->getWorkstationsString()->Substring[w];
	i = wSubString.start;
	int remMachines = wSubString.nSeparators + 1;
	for(int s=0; s<wSubString.nSeparators; s++){
	  mcList[m].Clear();
	  while(i < wSubString.SeparatorPos[s] && i <= wSubString.end){
		DMOSP_Problem::SOperation *op = Chrom->getWorkstationsString()->Gene[i];
		mcList[m].Add((void *)op);
		i++;
	  }
	  remMachines--;
	  m++;
	}
	// Add remaining operations in the sub-string to the last machine
	if(i <= wSubString.end){
	  while(i <= wSubString.end){
		DMOSP_Problem::SOperation *op = Chrom->getWorkstationsString()->Gene[i];
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
		  else{
			nxt[j] = -1;
			if(Cmax < jbrt[j]) Cmax = jbrt[j];
		    MWFT += J[j]->Weight * (jbrt[j] - J[j]->ReleaseTime);
		  }

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
  if(bComplete){
	SolStatus = FEASIBLE;
	if(Sol != Solution) Sol->Copy(Solution);
	MWFT /= (double)nJobs;
	Sol->Cmax = Cmax;
	Sol->MWFT = MWFT;
	return true;
  }
  SolStatus = INFEASIBLE;
  return false;
}

void DMOSP_Metaheuristic::ConvertOBtoRK(DMOSP_OBChromosome *ob, DMOSP_RKChromosome *rk)
{
  if(Problem != ob->getProblem() || Problem != rk->getProblem()) return;

  // Get the rank of each operation and its assigned machine index
  DMOSP_OBGene *obGenes = ob->getGenes();
  int i;
  for(i=0; i<nOps; i++){
	rank[obGenes[i].Operation->i] = i;
	mcindx[obGenes[i].Operation->i] = 0;
	for(SList::SNode *mcnd = obGenes[i].Operation->AltMCs.head(); mcnd; mcnd = mcnd->Next()){
	  DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
	  if(obGenes[i].Machine == opmc->MC) break;
	  mcindx[obGenes[i].Operation->i]++;
	}
	nmcs[obGenes[i].Operation->i] = obGenes[i].Operation->AltMCs.Count();
  }

  // Next, we convert the recorded ranks into random keys
  double *rkGenes = rk->getGenes();
  int length = 2*nOps;
  for(i=0; i<length; i+=2){
	double r = rndreal(0, 2.0 / (double)length);
	rkGenes[i] = 1.0 - rank[i/2] * 2.0 / (double)length - r;
	r = rndreal(0, 1.0 / (double)nmcs[i/2]);
	rkGenes[i+1] = (double)mcindx[i/2] / (double)nmcs[i/2] + r;
  }
}

void DMOSP_Metaheuristic::ConvertRKtoOB(DMOSP_RKChromosome *rk, DMOSP_OBChromosome *ob)
{
  if(Problem != rk->getProblem() || Problem != ob->getProblem()) return;

  // This procedure interprets the given random keys representation into an
  //   operations based representation. All keys must be double numbers from 0
  //   to 1.

  int i, l;
  double *rkGenes = rk->getGenes();
  for(i=0; i<nOps; i++){
	keys[i] = rkGenes[i*2];
	key_Operations[i] = O[i];

	// determine selected machine
	DMOSP_Problem::SMC *mc = NULL;
	double step = 1.0 / (double)(O[i]->AltMCs.Count());
	double pos = step;
	double key = rkGenes[i*2+1];
	for(SList::SNode *mcnd = O[i]->AltMCs.head(); mcnd; mcnd = mcnd->Next()){
	  if(key <= pos || mcnd->Next() == NULL){
		DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		mc = opmc->MC;
		break;
	  }
	   pos += step;
	}
	key_Machines[i] = mc;
  }
  RKQSort(key_Operations, key_Machines, keys, 0, nOps-1);

  DMOSP_OBGene *obGenes = ob->getGenes();
  for(l=0; l<nOps; l++){
	obGenes[l].Operation = key_Operations[l];
	obGenes[l].Machine = key_Machines[l];
  }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
