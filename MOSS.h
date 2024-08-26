//---------------------------------------------------------------------------

#ifndef MOSSH
#define MOSSH
#include "Metaheuristic.h"

//#define DEBUG_MOSS_RESULTS

#ifdef DEBUG_MOSS_RESULTS
#define PRNT_MOSS_DBG    // To save debugging info to file "mossDebug.txt"
#endif

int DMOSPMOSSSolCompCmax(void * Item1, void * Item2);
int DMOSPMOSSSolCompMWFT(void * Item1, void * Item2);
int DMOSPMOSSSolCompDomin(void * Item1, void * Item2);

struct DMOSP_MOSS_Node {  // used to represent nodes in the network representations of solutions
  int m_prv;  // index of the previous operation in machine sequence
  int m_nxt;  // index of the next operation in machine sequence
  int j_prv;  // index of the previous operation in the job route
  int j_nxt;  // index of the next operation in the job route
			  // ---- if a nxt/prv node does not exist, the values are set to -1

  double weight;  // the processing time of the corresponding operation

  double estart;    // [eta] earliest start time of an operation
  double lstart;    // [lambda] latest start time of an operation
  double tail;      // [gamma] tail of an operation, which is the remaining time to the completion of the schedule
  double slack;     // slack of an operation = lstart - estart
  bool bCritical;   // = true when estart = lstart

  int rank;      // number of all preceding nodes

  double minC;   // Minimum possible completion time given the job tour
  double mwftD;  // Deviation from the minimum possible completion time (minC)

  void Reset(){  // default node parameters
	m_prv = m_nxt = j_prv = j_nxt = -1;  // no connections with other nodes
	weight = 0;
	estart = lstart = tail = slack = -1;
	bCritical = false;
	rank = 0;
  };
};

class DMOSP_MOSS;
extern int MOSSIndComp(void *ind1, void *ind2);

struct DMOSP_SubPerumutation {
  int x1;  // starting position in the first permutation
  int x2;  // starting position in the second permutation
  int l;   // length of the subpermutation
};

class DMOSP_MOSS_Solution {
	friend class DMOSP_MOSS;
	friend int MOSSIndComp(void *ind1, void *ind2);

  private:
	DMOSP_MOSS *moss;

	// variables used for the vector of permutations based chromosome structure
	//
	DMOSP_VPBString *jbString;          // Upper string for job routing
	DMOSP_VPBString *wsString;          // Lower string for workstations' orders
	bool bVPBConstructed;   // = true if the chromosome genes are assigned values

	// variables used for representing solutions as vectors of permutations
	//
	int **tau;    // vectors of permutations of jobs' tours represented as arrays
	int **pi;     // vectors of permutations of operations on machines represented as arrays
				  //----- Important note: The operations listed in tau and pi
				  //      are not zero-based, i.e. start at tau[j][1] and pi[m][1].
				  //      This is done to simplify the references to the
				  //      postions of the operations in both permutations,
				  //      which complies with the notations in the paper.
				  //      Meanwhile tau[j][0] = | tau_j |
				  //      and       pi[m][0] = | pi_m |
				  //

	int *jb;      // jb[i] = index of the job to which operation i belongs
	int *mc;      // mc[i] = index of the machine assigned to operation i
	int *tau_pos; // tau_pos[i] = position of operation i in tau
	int *pi_pos;  // pi_pos[i] = position of operation i in pi
	bool bVectorsConstructed;

	// variables used for the network representaion of the solution
	//
	DMOSP_MOSS_Node *Nodes;   // for each operation, there is a node in the network
	int *FirstjNode;          // index of the first operation in the job tour
	int *LastjNode;           // index of the last operation in the job tour
	int *FirstmNode;          // index of the first node in the machine sequence
	int *LastmNode;           // index of the last operation in the machine sequence
	//---> if Cmax = -1, it means that the network is not constructed

	// Variables used for interpretting tasks into the strings of the VPB chromosome
	int *jpos, *wpos;

	// Variables used within the network interpretation function
	double *MCrt;  // machine's ready time
	int *mnxt;     // next node index for the machine
	double *Jnft;  // job's nearest finish time
	int *jnxt;     // next node index for the job

	// Variables used within the task list conversion function to vectors
	int *jcur;  // current operation index of job
	int *mcur;  // current operation index of machine

	double *etgmx_1;
	double *etgmx_2;

	double Cmax;    // makespan
	double MWFT;    // mean weighted flow time

	int rank;  // This variable is not related to the current algorithmic approach
			   // it is used here as a reference and it can be used in the future
			   // algorithmic development.

	void InterpretTaskList();      // Interpret tasks list provided in sspr->Solution
								   // into VPB chromosome strings and vectors of tau and pi

	void GenerateVectors();        // Interprets VPB chromosome (jbString and wsString)
								   // into tau and pi vectors

	void GenerateVPBStrings();     // Convert vectors of tau and pi into VPB strings

	void GenerateNetwork();		   // interprets tau and pi vectors into network representation

	bool InterpretNetwork();       // Evaluate the earliest and latest start times
								   // for all operations based on a given complete
								   // network representation including all links
								   // and weights definitions.
								   //   Returns the status of the network
								   // representation if it is feasible or not.
								   // IF it is feasible, the values of Cmax and MWFT
								   // are calculated, otherwise they are set to -1.

	bool ConvertNetToTasks();      // Convert the current netowrk representation
								   // into tasks stored in sspr->Solution
								   // returns false for infeasible networks

  public:
	DMOSP_MOSS_Solution(DMOSP_MOSS *alg);
	DMOSP_MOSS_Solution(DMOSP_MOSS_Solution &Sol);
	~DMOSP_MOSS_Solution();

	void AllocateMemory(DMOSP_MOSS *alg);
	void FreeMemory();

	void Reset();
	void Copy(DMOSP_MOSS_Solution *S);
	DMOSP_MOSS_Solution &operator = (DMOSP_MOSS_Solution &S){ Copy(&S);  return (*this); }
	friend bool operator == (DMOSP_MOSS_Solution &S1, DMOSP_MOSS_Solution &S2);

	bool InterpretDMOSPSolution(DMOSP_Solution *sol);  // Reads the task lists and interpret
													   // tasks into vectors only

	void alpha_omega_N1(int v1,	int &apr_alpha, int &apr_omega);
	double phi_N1(int v1);
	void Deltas_N1(int v1, int x1, double &hat_delta, double &check_delta);

	void alpha_omega_N2(int v2, int mk2, int &apr_alpha, int &apr_omega);
	double phi_N2(int v2, int mk2);
	void Deltas_N2(int v2, int mk2, int x2, double &hat_delta, double &check_delta);

	void alpha_omega_B1(int j, int x1, int x2, int &apr_alpha, int &apr_omega);
	void alpha_omega_B2(int m1, int m2, int x1, int x2, int &apr_alpha, int &apr_omega);

	void N1Results(int v1, int &alpha, int &omega, double *&apr_Cmax, int &minX);
		 //--- apr_Cmax must be array of double with size of nWCs+1
	void N2Results(int v2, int k2, int &alpha, int &omega, double *&apr_Cmax, int &minX);
		 //--- apr_Cmax must be array of double with size of nJobs+1
	void N3ExactResults(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX);
		 //--- deltaMWFT must be array of double with size of nWCs+1
	void N4ExactResults(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX);
		 //--- deltaMWFT must be array of double with size of nJobs+1
	void N3Results(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX);
		 //--- deltaMWFT must be array of double with size of nWCs+1
	void N4Results(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX);
		 //--- deltaMWFT must be array of double with size of nJobs+1

	double CmaxValue() { return Cmax; }
	double MWFTValue() { return MWFT; }
	DMOSP_VPBString *getJobsString() { return jbString; }
	DMOSP_VPBString *getWorkstationsString() { return wsString; }
	int getRank(){  return rank; }

    // The following functions are used for debugging purposes
	void echoVPBChromosome(FILE *outf, wchar_t *heading);
	void echoVechors(FILE *outf, wchar_t *heading);
	void echoNetwork(FILE *outf, wchar_t *heading);
};

struct DMOSP_xOverElement {   // used for recording crossover element description
  // This is used for showing the crossover application and does not affect the algorithm
  int type;  // = 1 for job and 2 for worcenter
  wchar_t ID[9];  // ID of the job or the worcenter
  int pos;   // Position of the crossover point
};

class DMOSP_MOSS : public DMOSP_Metaheuristic {
  friend class TfrmMOSS;
  friend class TfrmMOSSCalcProgress;

  public:
	// Data structures for moves definitions
	enum NSFunction {
	  N1 = 1,      // N3 has the same structure as N1
	  N2           // N4 has the same structure as N2
	};             // Threfore, there is no need to define N3 and N4 explicitly

	struct NSMove {
	  NSFunction F;
	  int v;   // operation index
	  int x;   // move to position
	  int m2;  // move to machine for N2
	  union{
		double aprCmax;    // the value of phi + delta(x)
		double deltaMWFT;  // approximate change in the MWFT value
	  };
	};

	double GD, IGD, TGD;  // performance indicators of final non-dominated solutions
	double HV;            // hypervolume performace indicator
	double HV0;           // Optimal hypervolume
	double HVpr, HV0pr;   // percentage values for HV and HV0

  private:
	int pSize;  		 // Reference set size
	int deltaThreshold;  // Threshold distance value for accepting new solutions
						 // in the reference set of solutions

    // The following sets store solutions in the DMOSP_MOSS_Solution structure
	SList R; // reference set of solutions
	SList T; // set of generated trial solutions in each iteration
	SList D; // set of non-dominated solutions

	SList ND;  // final set of non-dominated solutions in the DMOSP_Solution

	int mossitr;

	double Theta_X;

	bool bTerminate;  // = true for intermediate terminations

  protected:
	double bestCmax;     // Best found makespan updated throughout iterations
	double bestMWFT;     // Best found mean weighted flow time

	DMOSP_MOSS_Solution *bestCmaxSol;   // Best found makespan solution throughout iterations
	DMOSP_MOSS_Solution *bestMWFTSol;   // Best found MWFT solution throughout iterations

	bool bOptSolFound;  // = true if a solution with Cmax = CmaxLB and MWFT = MWFTLB is found
	bool bRefSetUpdated;  // = ture if the reference set is updated during the improvement iterations
	bool bNonDominatedSetUpdated; // = true if set D is updated during the improvement iterations

	int nBestMoves;   // Maximum limit on the number of moves to be stored for improvement

	// The following variables are used for calculating distances ---------------------
	int mxsz;  // maximum size for the precedence relationship matrix
	int nr;    // number of subpermutations in the intersection set of two permutations
	DMOSP_SubPerumutation *r;  // list of all subpermutations (intersection set)
	char **a;  // precedence relationship matrix to be used internally
			   // in calculating distance between two solutions
	int *Suma_i, *Suma_j; // summations in "a" matrix along the row and along the column
	//---------------------------------------------------------------------------------

	// The following variables are used for the implementation of Tabu Search (TS)
	int TabuSize;        // Allowed maximum tabu size
	int nTSiterations;   // maximum number of tabu search iterations allowed
	int nImpIterations;  // maximum number of improvement iterations allowed
	int nRandMoves;      // number of random moves to be conducted for diversification
	int nTabus;          // current number of tabus in the list
	int TabuPntr;        // a pointer to the current position in the circular tabu list
	NSMove *TabuMoves;
	double initialLocalCmax;  // Initial makespan passed to the tabu search
	double initialLocalMWFT;  // Initial MWFT passed to the tabu search
	DMOSP_MOSS_Solution *startingSol;    // Initial solution sent to TabuSearch
	DMOSP_MOSS_Solution *bestLocalSol;   // Best found Cmax solution throughout iterations
	double bestLocalCmax;   // The best makespan value for the current solution being improved by TS
	double bestLocalMWFT;   // The best MWFT value for the current solution being improved by TS
	double *apr_Cmax;   // Stores approximate Cmax values for neighborhood moves
	double *deltaMWFT;  // stores actual changes in MWFT for neighborhood moves
	//---------------------------------------------------------------------------

	// The following variables are used for the solution combination related tasks
	int CombinedSolCounter;
	bool *jbstrGeneSel;
	bool *wsstrGeneSel;

	//---------------------------------------------------------------------------

	int nIterations;      // Maximum number of improvement iterations allowed

	double **TrackingResults;
	double TrackTime[5];
	int iTrack;

  public:
	DMOSP_MOSS(DMOSP_Problem *P, int ps=15, int dth=-1, int ts=10, int ntsitr=40,
							   int ntsimpitr=20, int nbstmvs=5, int nrndmvs=5,
							   double probX=0.5, int nitr=300, double TL=60000);
	DMOSP_MOSS(DMOSP_Problem *P, int ps, int ntsitr, int dth, double **tr=NULL);
	~DMOSP_MOSS();

	int nMoves;          // Actual number of best improvement moves stored
	NSMove *bestMoves;   // a list of best improvement moves in an iteration

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

	void ClearND();
	void RemoveDuplicatesInD();
	void ConvertDtoND();

	int SuggestDeltaThreshold();
	int ConstructInitialPopulation();  // returns the number of solutions constructed
	DMOSP_MOSS_Solution *ConstructSolution();

	bool UpdateNonDominatedSolutions(DMOSP_MOSS_Solution *Sol);
	bool UpdateNonDominatedSolutions(SList &Sols);

	void UpdateRefSet();
	void DiversifyR();
	void InjectPinR();

	double ApplyN(DMOSP_MOSS_Solution *Sol, NSMove mv);  // returns resultant Cmax
	double MWFTIfApplyN(DMOSP_MOSS_Solution *Sol, NSMove mv);
			 // Incomplete network change to get the effect on MWFT
	void MWFTBasedOnNetwork(DMOSP_MOSS_Solution *Sol);
			 // Based on the current network of a solution, update earliest start
			 // times for the estimation of MWFT when N3 is applied
	double MWFTIfApplyN_Complete(DMOSP_MOSS_Solution *Sol, NSMove mv);
			 // Completely modify solution to get the effect on MWFT

	int TauIntersectionSize(int j, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2);
	int TauDistance(int j, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2);
	int WCIntersectionSize(int w, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2);
	int WCDistance(int w, DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2);
	int Distance(DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2, int &jD, int &wcD);
												   // Dissimilarity measure
	bool isDistanceGreaterThan(DMOSP_MOSS_Solution *S1, DMOSP_MOSS_Solution *S2, int DistLimit);

	// Functions used by Tabu Search
	int SuggestTabuSize();
	void SetTabuSize(int ts);
	void SetTSnIterations(int nitr);
	void SetTSnBestMoves(int nbst);
	void SetTSnRandMoves(int nrnd);
	void TabuSearch(DMOSP_MOSS_Solution *Sol, bool bAllRandom=false, bool bGenerateNet=true);
	void AddTabu(NSFunction F, int v, int x, int m=-1);
	void ClearTabus();
	bool isTabu(NSFunction F, int v, int x, int m=-1);
	void EnumerateCmaxNonTabuMoves(DMOSP_MOSS_Solution *Sol);
	void EnumerateMWFTNonTabuMoves(DMOSP_MOSS_Solution *Sol);
	int ApplyNandUpdateTabuList(DMOSP_MOSS_Solution *Sol, NSMove mv);  // returns resultant Cmax

	// Functions used by solution combination
    void setTheta_X(double th){ Theta_X = th; }
	double getTheta_X(){ return Theta_X; }
	DMOSP_MOSS_Solution *CombinedFrom(DMOSP_MOSS_Solution *SL, DMOSP_MOSS_Solution *SF);

    // General functions
	void ChooseNDSolution(int index);   // Set "Solution" to the indexed one from set D
	int getNDSize() { return ND.Count(); }
	int getDSize() { return D.Count(); }
	DMOSP_MOSS_Solution *GetDSolution(int index);  // return DMOSP_MOSS_Solution from set D
	void AddDSol(DMOSP_MOSS_Solution *s);

	// Variables and functions related to the fronts determination and the rank calculations
	int dSize;  // size of individuals in set D used for generating fronts
	int *numIndsFront;
	int **paretoFront;
	int numFronts;
	int dominates(DMOSP_MOSS_Solution *guy1, DMOSP_MOSS_Solution *guy2);
	void RankDSolutions();    // generate the rank values for solutions in set D
	int* getNoOfIndsFront(void) { return numIndsFront; }
	int** getParetoFront(void) { return paretoFront; }
	int getNoOfFronts(void) const { return numFronts; }

	// statistics for the objective values for the initial population
    int nGenerated, nExcluded;
	double avgCmax, minCmax, maxCmax, varCmax;
	double avgMWFT, minMWFT, maxMWFT, varMWFT;
	void ComputeObjectivesStatistics();

	void ComputePerformanceMetrics();  // Compute GD, IGD, HV and HV%

	//------------------- Functions used to test N3 and N4 related operations
	DMOSP_Solution *iSol;
	DMOSP_MOSS_Solution *NSTestSol;
	DMOSP_MOSS(DMOSP_Problem *P, DMOSP_Solution *sol);
	void Initialize4NSTests();
	void FreeNSTestMemory();
	void N3ExactResults(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX);
	void N4ExactResults(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX);
	void N3Results(int v1, int &alpha, int &omega, double *&deltaMWFT, int &minX);
	void N4Results(int v2, int k2, int &alpha, int &omega, double *&deltaMWFT, int &minX);
	//-----------------------------------------------------------------------
};


//---------------------------------------------------------------------------
#endif
