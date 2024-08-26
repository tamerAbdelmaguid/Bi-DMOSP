//---------------------------------------------------------------------------

#ifndef NSGAIIH
#define NSGAIIH

#include "Metaheuristic.h"
#include "OBChromosome.h"
#include ".\\NSGAII\\MOGARandom.h"

#define SWAP(A,B)    (A^=B^=A^=B)
#define ZERO         1.0E-10
#define INFTY        1.0E+20;
#define IDOMJ        1
#define JDOMI       -1
#define INONDOMJ     0
#define GUYS         0
#define NEWGUYS      1
#define COMBINEDGUYS 2
#define NO           0
#define YES          1
#define OFF          0
#define ON           1
#define OBJECTIVE    0
#define FITNESS      1
#define REFLECT -1.0
#define EXPAND   2.0
#define CONTRACT 0.5
#define RECONSTRUCT 0.0

extern MOGA_Random myRandom;

struct DMOSP_Individual {
   DMOSP_OBChromosome *Chromosome;
   union{
	 struct { double Cmax, MWFT; };
	 double Objective[2];
   };
   union{
	 struct{ double fitCmax, fitMWFT; };
	 double Fitness[2];
   };
   int rank;
   double crowdingDistance;

   DMOSP_Individual(DMOSP_Problem *P);
   DMOSP_Individual(DMOSP_Individual *ind);
   ~DMOSP_Individual();

   bool Copy(DMOSP_Individual *ind);
   DMOSP_Individual &operator =(DMOSP_Individual &ind){  Copy(&ind); return *this; };

   inline double getObjective(int i) const { return Objective[i];};
   inline double getFitness(int i) const { return Fitness[i]; };
   inline void setFitness(int i, double f){ Fitness[i] = f; };

   inline int getRank(void) const { return rank; }
   inline double getCrowdingDistance (void) const { return crowdingDistance; }
   inline void setRank(int frontRank) { rank = frontRank; }
   inline void setCrowdingDistance(double distance) { crowdingDistance = distance; }
};

class DMOSP_NSGAII : public DMOSP_Metaheuristic {
  public:
	enum DMOSP_XOVER_OPERATOR {
	   SINGLE_POINT_OX = 0,   // Single-point order crossover
	   TWO_POINT_OX,          // Two-point order crossover
	   UNIFORM_OX,            // Multiple-point order crossover
	   GENPOSX,               // Generalized position crossover
	   SIMBLOCK2OX            // Similar block 2-point order crossover
	};

	struct DMOSP_xOverOpActions {
	  bool bOpInheritedToChild1;
	  bool bOpInheritedToChild2;
	  bool bFilledLocus;           // Needed for SB2OX
	};

	enum DMOSP_MUTATION_OPERATOR {
	   SHIFT_MUTATION=0,
	   SWAP_MUTATION
	};

	double GD, IGD, TGD;  // performance indicators of final non-dominated solutions
	double HV;            // hypervolume performace indicator
	double HV0;           // Hypervolume of the optimal Pareto front if known
	double HVpr, HV0pr;   // percentage values for HV and HV0
	SList P;              // set of final Pareto front solutions

  private:
	int popSize;
	DMOSP_Individual **guys;      // current population of individuals
	DMOSP_Individual **newGuys;   // individuals after crossover and mutation
	int *mpool;                   // mating pool

	double xOverProbability;
	DMOSP_XOVER_OPERATOR xOverOper;
	DMOSP_xOverOpActions *xOverActions;

	double mutProbability, RLBmutProbability;
	DMOSP_MUTATION_OPERATOR mutOper;

	int maxSAIterations, SAIter;
	double initSATemp, SACoolingRate, curSATemp;
	double SAApplProp;

	double *bestobj;              // minimum objective
	double *worstobj;             // maximum objective
	double *avgobj;               // average objective
	double *maxfit;               // maximum fitness
	double *minfit;               // minimum fitness
	double *avgfit;               // average fitness
	double *varfit;               // fitness variance

	DMOSP_Individual **combinedGuys;
	int *numIndsFront;
	int **paretoFront;
	int numFronts;
    int oldNoOfFronts;
	int numFrontChange;

	int maxGenerations, genID;
	long noOfGlobalEvals;
	long noOfLocalEvals;
	bool bTerminate;              // = true for intermediate terminations

	// variables used by the randomized load balancing operator
	int nCandidateWSs;
	int *iCandidateWSs;
	double *totalMCLoad;
	int *nOpsLoaded;
	int *iLoadedMC;

	// variables used by the simulated annealing local search
	DMOSP_Individual *bestSAInd;

  public:
	DMOSP_NSGAII(DMOSP_Problem *P, int ps=100, int maxgen=1000, double timelimit=-1,
				 double xPrb=0.8, DMOSP_XOVER_OPERATOR xOpr=SINGLE_POINT_OX,
				 double mPrb=0.1, DMOSP_MUTATION_OPERATOR mOpr=SHIFT_MUTATION,
				 double rlsMutProb=0.2, int nsaitr=50, double saitemp=1000,
				 double sacoolrt=0.95, double saprop=0.3);
	~DMOSP_NSGAII();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	void ComputePerformanceMetrics();
	void StoreParetoFrontSolutions();

	// Generate a solution based on the chromosome of individual i, save
	// the resultant solution in "Solution" and return the chromosome itself
	DMOSP_OBChromosome *ChooseIndividual(int i);
	DMOSP_OBChromosome *ChooseParetoIndividual(int i);

    // statistics calculation functions
	void computeObjStatistics(int whichGuys);
	void mapObjectiveToFitness(int whichGuys);
	void computeFitnessStatistics(int whichGuys);

	// ----------------------------------------------- NSGA-II search operators
	void doNonDominatedSort(int whichGuys);
	void computeCrowdingDistance(int whichGuys);
	void quickSort(DMOSP_Individual **theGuys, int *output, int left, int right, int objID);
	void regQSort(double *crowdingDistance, int *output, int left, int right);
    void swap(int &ii, int &jj);

	int dominates(const DMOSP_Individual *guy1, const DMOSP_Individual *guy2);
	bool crowdingComp (const DMOSP_Individual *guy1, const DMOSP_Individual *guy2);
    int betterIndividual(DMOSP_Individual *guy1, DMOSP_Individual *guy2);
	void TournamentSelection(int tourSize=2);
	                // sets values of the mpool based on tournment selector

	// Crossover operators
	int nXOverPoints;
	int *xOverLocus;
	void SinglePointOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2, bool bConstruct=true);
	void TwoPointOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2, bool bConstruct=true);
	void UniformOX(DMOSP_Individual *parent1, DMOSP_Individual *parent2, bool bConstruct=true);
	void GPX(DMOSP_Individual *parent1, DMOSP_Individual *parent2, bool bConstruct=true);
	void SB2OX(DMOSP_Individual *parent1, DMOSP_Individual *parent2, bool bConstruct=true);

	// Mutation operators
	int mutPoint1, mutPoint2;
	DMOSP_OBGene *tmpGene;
	void SwapMutation(DMOSP_Individual *original, bool bConstruct=true);
	void ShiftMutation(DMOSP_Individual *original, bool bConstruct=true);
	void RLSMutation(DMOSP_Individual *original, bool bConstruct=true);

	// ------------------ Simulated Annealing Local Search -------------------
	void setSAParameters(int nsaitr, double saitemp, double sacoolrt);
	void InitializeSA(DMOSP_Individual *ind);
	void RunSA(DMOSP_Individual *ind);
	bool StepSA(DMOSP_Individual *ind);   // returns true when iterations end

	int getCurSAIter() const { return SAIter; }
	double getInitSATemp() const { return initSATemp; }
	double getSACoolingRate() const { return SACoolingRate; }
	double getCurSATemp() const { return curSATemp; }

	// ---------------------------------------------------- Interface functions
	inline int getPopSize(void) const { return popSize; }
	inline DMOSP_Individual *getGuy(int i) const { return guys[i]; }
	inline int getGenID (void) const {return genID;}
	inline int getMatingPool(int i) const { return mpool[i]; }
	inline long getNoOfGlobalEvals(void) const {return noOfGlobalEvals;}
	inline long getNoOfLocalEvals(void) const {return noOfLocalEvals;}

	inline void setGenID (int iValue) {genID = iValue;}
	inline void setNoOfGlobalEvals(long lValue) {noOfGlobalEvals = lValue;}
	inline void setNoOfLocalEvals(long lValue) {noOfLocalEvals = lValue;}

	inline double getMaxObj(int index) const { return worstobj[index]; }
	inline double getMinObj(int index) const { return bestobj[index]; }
	inline double getAvgObj(int index) const { return avgobj[index]; }
	inline double getMaxFit(int index) const { return maxfit[index]; }
	inline double getMinFit(int index) const { return minfit[index]; }
	inline double getAvgFit(int index) const { return avgfit[index]; }
	inline double getFitVar(int index) const { return varfit[index]; }

	inline void setMaxObj(int index, double dValue) { worstobj[index] = dValue; }
	inline void setMinObj(int index, double dValue) { bestobj[index] = dValue; }
	inline void setAvgObj(int index, double dValue) { avgobj[index] = dValue; }
	inline void setMaxFit(int index, double dValue) { maxfit[index] = dValue; }
	inline void setMinFit(int index, double dValue) { minfit[index] = dValue; }
	inline void setAvgFit(int index, double dValue) { avgfit[index] = dValue; }
	inline void setFitVar(int index, double dValue) { varfit[index] = dValue; }

	inline int getNoOfIndsBestFront(void) const { return numIndsFront[0]; }
	inline int* getNoOfIndsFront(void) { return numIndsFront; }
	inline int** getParetoFront(void) { return paretoFront; }
	inline DMOSP_Individual** getCombinedGuys(void) { return combinedGuys; }

	inline int getNoOfFronts(void) const { return numFronts; }
    inline int getNoOfFrontChange(void) const { return numFrontChange; }

	inline void setNoOfFronts(int iValue) { numFronts = iValue; }
	inline void setNoOfFrontChange(int iValue) { numFrontChange = iValue; }

    double getCrowdingDistance(int index) {return combinedGuys[index]->getCrowdingDistance();}
};

//---------------------------------------------------------------------------
#endif
