//---------------------------------------------------------------------------

#ifndef MetaheuristicH
#define MetaheuristicH

#include "OBChromosome.h"
#include "RKChromosome.h"
#include "VPBChromosome.h"

class DMOSP_Metaheuristic : public DMOSP_Algorithm {
   protected:
	 // Types and variables used by the dense schedule generator, which is used for
	 // constructing the initial population
	 enum DSG_JSR {
		   RANDj=0,  // Random
		   MCTj,     // Minimum Completion Time
		   WSPT,     // Weighted shortest Processing Time
		   WLWR      // Weighted least Work Remaining
	 };

	 enum DSG_MSR {
		   RANDm=0,  // Random
		   MCTm,     // Minimum completion time
		   MWL       // Minimum work load
	 };

	 double *MCrt;       // Machine ready time
	 bool *bMCready;     // = true if a machine is ready to process a job at a current time
	 bool *bMCdone;      // = true if a machine is done with all its required jobs

	 double *Jnft;          // Job's nearest finish time
	 bool *bJcompleted;     // = true if all operations of a job are scheduled

	 bool **bJM;              // = true if a job needs a machine
	 bool *SchedulableJobs;   // = true if a job is ready to be processed on a machine
	 bool *bScheduled;        // = true if an operations is scheduled and false otherwise
	 int *SelJob;             // selected job for each machine
	 double *SumSchedTime;       // summation of scheduled times on a machine

	 void DSG(DSG_JSR jsr, DSG_MSR msr, double &Makespan, double &MWFT);
	                      // Apply the DSG heuristic to construct "Solution"

	 // Variables used for constructing solutions and decoding chromosomes
	 double *mcrt,   // an array for the machine ready times
			*jbrt;   // an array for the jobs release times which represent the
					 // minimum time at which the next unscheduled operation can start
	 int *njOps;     // number of operations not yet scheduled for a job

	 // Variables used for interpretting random keys chromosomes
	 double *keys;
	 DMOSP_Problem::SOperation **key_Operations;
	 DMOSP_Problem::SMC **key_Machines;

	 // Variables used for interpretting VPB cohromosomes
	 SList *jbList; // Lists for operations in the order of processing for the jobs
	 SList *mcList; // lists for operations in the order of processing on the machines
	 int *nxt;      // Next schedulable operation index

	 // Variables used for converting operations based chromosome to random keys
	 int *rank, *mcindx, *nmcs;

   public:
	 DMOSP_Metaheuristic(DMOSP_Problem *prob);
	 DMOSP_Metaheuristic(DMOSP_Problem *prob, wchar_t *Name, double TL=-1);
	 ~DMOSP_Metaheuristic();

	 virtual void AllocateMemory();
	 virtual void Initialize();
	 virtual bool Stop();
	 virtual void Finalize();
	 virtual void FreeMemory();
	 virtual void ComputeObjectiveValue();

	 // The following function can be called indpendently but make sure that the
	 // memory has been allocated before calling it
	 bool DecodeOBChromosome(DMOSP_OBChromosome *chrom, DMOSP_Solution *sol,
							 double &Cmax, double &MWFT);
	 bool DecodeRKChromosome(DMOSP_RKChromosome *chrom, DMOSP_Solution *sol,
							 double &Cmax, double &MWFT);
	 bool DecodeVPBChromosome(DMOSP_VPBChromosome *chrom, DMOSP_Solution *sol,
							 double &Cmax, double &MWFT);

	 void ConvertOBtoRK(DMOSP_OBChromosome *ob, DMOSP_RKChromosome *rk);
	 void ConvertRKtoOB(DMOSP_RKChromosome *rk, DMOSP_OBChromosome *ob);
};

//---------------------------------------------------------------------------
#endif
