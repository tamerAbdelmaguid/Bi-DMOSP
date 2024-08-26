//---------------------------------------------------------------------------

#ifndef MOGWOH
#define MOGWOH

#include "Metaheuristic.h"
#include "RKChromosome.h"

struct DMOSP_Wolf {
   DMOSP_RKChromosome *Chromosome;   // takes care of the decision variables
   union{
	 struct { double Cmax, MWFT; };
	 double Objective[2];
   };
   bool bDominated;
   int GridIndex, GridSubIndex[2];   // we have two objectives only

   DMOSP_Wolf(DMOSP_Problem *P);
   DMOSP_Wolf(DMOSP_Wolf *wlf);
   ~DMOSP_Wolf();

   bool Copy(DMOSP_Wolf *wlf);
   DMOSP_Wolf &operator =(DMOSP_Wolf &wlf){  Copy(&wlf); return *this; };

   //double Velocity;
   //DMOSP_RKChromosome *bestChromosome;          // These are defined in the
   //union{                                       // MOGWO matlab code but they
   //	 struct { double bestCmax, bestMWFT; };   // haven't been used!!!!!
   //	 double bestObjective[2];
   //};
};

struct DMOSP_Hypercube {
   double *Lower, *Upper;
};

class DMOSP_MOGWO : public DMOSP_Metaheuristic {
  public:
	enum DMOSP_MUTATION_OPERATOR {
	   SHIFT_MUTATION=0,
	   SWAP_MUTATION
	};

    double bestCmax, bestMWFT;
	double GD, IGD, TGD;  // performance indicators of final non-dominated solutions
	double HV;            // hypervolume performace indicator
	double HV0;           // hypervolume for optimal Pareto front if known
	double HVpr, HV0pr;   // percentage values for HV and HV0
	SList P;              // set of final Pareto front solutions

  private:
	int nGreyWolves;
	int maxIterations;
	int ArchiveSize;
	double alpha;       // Grid inflation parameter
	int nGrids;         // Number of grids for each dimension
	double beta;        // Leader selection pressure parameter
	double gamma;       // Extra (to be deleted) Repository Member Selection Pressure

	DMOSP_MUTATION_OPERATOR mutOper;

	bool bTerminate;    // = true for intermediate terminations
	int itr;

	DMOSP_Wolf **Wolves;
	DMOSP_Wolf **Archive;        // main set of non-dominated solutions
	DMOSP_Wolf **ExtraArchive;   // additional non-dominated solutions if more
								 // than ArchiveSize solutions exist in Archive
	int nArchiveWolves, nExtraArchiveWolves;

	DMOSP_Wolf **Combined;       // Combined set of solutions
	int CombinedSize, nCombined;

	DMOSP_Hypercube *Hypercube;

	DMOSP_Wolf **rep2, **rep3;   // subsets from the Archive to choose leaders
								 // from less crowded regions
	int nrep2, nrep3;

	DMOSP_Wolf *Alpha, *Beta, *Delta;

	int n_occ_cells;
	int *occ_cell_index;
	int *occ_cell_member_count;

	int nVar;                    // number of variables = 2*nOps
	double *c_, *D_, *A_, *X1_, *X2_, *X3_;

	// Parameters and variables used by the simulated annealing local search
	int maxSAIterations, SAIter;
	double initSATemp, SACoolingRate, curSATemp;
	double SAApplProp;
	DMOSP_Wolf *bestSAWolf;

  public:
	DMOSP_MOGWO(DMOSP_Problem *P, int nGWs=100, int nIter=1000,
				double timelimit=-1, int szArch=100, double a=0.1, int ngrd=10,
				double b=4, double g=2, DMOSP_MUTATION_OPERATOR mOpr=SHIFT_MUTATION,
				int nsaitr=50, double saitemp=1000,
				double sacoolrt=0.95, double saprop=0.3);
	~DMOSP_MOGWO();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	// ---------- Multi-objective Grey Wolf Operaizer main functions  --------
	void DetermineDomination(DMOSP_Wolf **pop);
	void GetNonDominatedWolves(DMOSP_Wolf **pop);      // Update Archive
	void CreateHypercubes(DMOSP_Wolf **pop);
	void GetGridIndexes(DMOSP_Wolf **pop);

	void GetOccupiedCells(DMOSP_Wolf **pop);
	DMOSP_Wolf *SelectLeader(DMOSP_Wolf **pop, double bta=1.0);
    void CombineArchives();
	void DeleteFromRep();     // Manage archived and extra-archived wolves

	// Supplementary functions
	double SumDeltaPosition(DMOSP_Wolf *w1, DMOSP_Wolf *w2);

	// ------------------------ Mutation operators ---------------------------
	int mutPoint1, mutPoint2;
	void SwapMutation(DMOSP_Wolf *original, bool bConstruct=true);
	void ShiftMutation(DMOSP_Wolf *original, bool bConstruct=true);

	// ------------------ Simulated Annealing Local Search -------------------
	void setSAParameters(int nsaitr, double saitemp, double sacoolrt);
	void InitializeSA(DMOSP_Wolf *ind);
	void RunSA(DMOSP_Wolf *ind);
	bool StepSA(DMOSP_Wolf *ind);   // returns true when iterations end

	int getCurSAIter() const { return SAIter; }
	double getInitSATemp() const { return initSATemp; }
	double getSACoolingRate() const { return SACoolingRate; }
	double getCurSATemp() const { return curSATemp; }

	//------------------ Storing non-dominated solutions in set P
	void StoreParetoFrontSolutions();
	void ComputePerformanceMetrics();   // Compute GD and IGD for the set of solved instances

	// ----------------- Interface functions
	DMOSP_Wolf **getWolves(){ return Wolves; };
	DMOSP_Wolf **getArchive(){ return Archive; };

	int getnArchive(){ return nArchiveWolves; }
	int getnWolves(){ return nGreyWolves; }
};


//---------------------------------------------------------------------------
#endif
