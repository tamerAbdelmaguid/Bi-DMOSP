//--- This code is Copyright 2019, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//---------------------------------------------------------------------------

#ifndef DMOSPSolverH
#define DMOSPSolverH

#include "Problem.h"
#include "Algorithm.h"

double max(double a, double b);
double min(double a, double b);
int BiObjSolComp(void *ind1, void *ind2);

struct DMOSP_Solution {
    struct SchTask {
	  DMOSP_Problem::SOperation *Operation;
	  DMOSP_Problem::SMC *SelectedMC;        // = NULL if not selected
	  bool bScheduled;    // Needed by the construction algorithm
	  double StartTime;
	  double EndTime;
	  SchTask(DMOSP_Problem::SOperation *op, DMOSP_Problem::SMC *mc, double s, double e)
			: Operation(op), SelectedMC(mc), StartTime(s), EndTime(e), bScheduled(true)	{};
	  SchTask(DMOSP_Problem::SOperation *op)
				: Operation(op), SelectedMC(NULL), bScheduled(false)
				{ StartTime = EndTime = MAXFLOAT; };
	};

	DMOSP_Problem *Problem;
	SList ScheduledTasks;

	DMOSP_Solution(DMOSP_Problem *P, bool bConstructTasks = true);
	~DMOSP_Solution();

	double JobCompletionTime(DMOSP_Problem::SJob *jb);

	SchTask *TaskAssociatedToOperation(DMOSP_Problem::SOperation *op);
	double ComputeObjectiveValue(DMOSP_Problem::TObjective OBJ=(DMOSP_Problem::TObjective)0);

	bool Copy(DMOSP_Solution *Sol);
	DMOSP_Solution& operator=(DMOSP_Solution &sol);
	bool operator ==(DMOSP_Solution &Sol);

	bool isComplete();
	bool isFeasible();

	double Cmax, MWFT;
};

class DMOSP_Algorithm : public Algorithm {
  protected:
	DMOSP_Algorithm(DMOSP_Problem *P, wchar_t *N, double TL=-1);

	// Data structures that are used to include all problem data from an easily
	//  accessible memory locations to subdue the effect of memory access time
	//  on the efficiency of the algorithms.
	//
  public:
	double CmaxLB;  // Cmax Lower bound for the given problem instance
	double MWFTLB;  // MWFT Lower bound for the given problem instance

	int nWCs, nMCs, nJobs, nOps;    // number of...
	DMOSP_Problem::SWC **W;          // set of all workcenters
	DMOSP_Problem::SMC **M;          // set of all machines
	int *Wa, *Wz;                   // set of indexes for workcenters to
									//   identify the starting and ending indexes
									//   of the machines that belong to a WC
	int *Mw;                        // workcenter index for a machine

	DMOSP_Problem::SJob **J;         // set of all jobs
	DMOSP_Problem::SOperation **O;   // set of all operations
	int *Ja, *Jz;                   // set of indexes for jobs to
									//   identify the starting and ending indexes
									//   of the operations that belong to a job
	int *Oj;                        // job index for an operation

	double **pt;                    // processing time matrix of a job on a machine

	DMOSP_Solution *Solution;        // Solution of the problem in tasks representation
	DMOSP_Solution::SchTask **tsk;   // set of all task pointers in Solution

	enum TSolutionStatus { INFEASIBLE, FEASIBLE } SolStatus;

	DMOSP_Algorithm(DMOSP_Problem *P);
	~DMOSP_Algorithm();

	void AllocateArrays();
	void FreeArrays();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual void Improve();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();

    void CalculateCmaxLowerBound();
	double CalculateCmaxLowerBound(double &lb1, double &lb2);
    double CalculateMWFTLowerBound();
};

//------------------- An implementation of the dense schedule (DS) generator
//

int tskComp(void * Item1, void * Item2);

class DMOSP_DSG : public DMOSP_Algorithm {
  protected:
	DMOSP_DSG(DMOSP_Problem *P, wchar_t *N);

  public:
	enum T_JobDispatchingRule {
		   FCFS=0,   // First Come First Serve
		   SPT,      // Shortest Processing Time
		   LPT,      // Longest Processing Time
		   MCT,      // Minimum Completion Time
		   MOR,      // Most Operations Remaining
		   LOR,      // Least Operations Remaining
		   MWR,      // Most Work Remaining
		   LWR,      // Least Work Remaining
		   RANDOM
	} JobDispatchingRule;

	enum T_MCSelectionRule {
		   MIN_LOAD=0,  // Minimum load
		   MINCT,       // Minimum completion time
		   MCRANDOM
	} MCSelectionRule;

	DMOSP_DSG(DMOSP_Problem *P, T_JobDispatchingRule DR, T_MCSelectionRule SR);
	~DMOSP_DSG();

	double *MCrt;          // Machine ready time
	bool *bMCready;     // = true if a machine is ready to process a job at a current time
	bool *bMCdone;      // = true if a machine is done with all its required jobs

	double *Jnft;          // Job's nearest finish time
	bool *bJcompleted;  // = true if all operations of a job are scheduled

	bool **bJM;              // = true if a job needs a machine
	bool *SchedulableJobs;   // = true if a job is ready to be processed on a machine
	bool *bScheduled;        // = true if an operations is scheduled and false otherwise
	int *SelJob;             // selected job for each machine
	double *SumSchedTime;       // summation of scheduled times on a machine

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//------------- Network representation of solutions
//

class DMOSP_VectorSol {
	friend class DMOSP_NSTester;

  private:
	DMOSP_Algorithm *Alg;

	SList *tau;    // vectors of permutations of jobs' tours
	SList *pi;    // vectors of permutations of operations on machines

	int *mc;       // mc[i] = index of the machine assigned to operation i

  public:
	DMOSP_VectorSol(DMOSP_Algorithm *a);
	DMOSP_VectorSol(DMOSP_VectorSol *V);
	~DMOSP_VectorSol();

	void Copy(DMOSP_VectorSol *V);

	char *u;       // inclusion status in job and machine permutations to be used
				   //   within the network construction and properties calculations
				   // u[i] = 0 if operation i is not included in neither machine nor job permutations
				   //      = 1 if operation i is included in job permutation but not in machine permutation
				   //      = 2 if operation i is not included in job permutaion but included in machien permutation
				   //      = 3 if operation i is included in both job and machine permutations

	void Assert(wchar_t *heading);    // make output to the debug file to show the vector's content

	void ConvertTasksToVect();    // Convert current solution (Alg->Solution)
								  // into vector representation 

	bool AppendOp2Mc(int m, DMOSP_Problem::SOperation *op);
	bool AppendOp2Mc(DMOSP_Problem::SMC *m, DMOSP_Problem::SOperation *op);
	bool AppendOp2Jb(DMOSP_Problem::SOperation *op);
	bool RemoveOp(DMOSP_Problem::SOperation *op);

	int nInTau(int j){ if(j<0 || j>=Alg->nJobs) return -1;  return tau[j].Count(); }
	int nInPi(int m){ if(m<0 || m>=Alg->nMCs) return -1;  return pi[m].Count(); }

	SList::SNode *headOfTau(int j){ if(j<0 || j>=Alg->nJobs) return NULL;   return tau[j].head(); }
	SList::SNode *headOfPi(int m){ if(m<0 || m>=Alg->nMCs) return NULL;  return pi[m].head(); }

	void Clear();        // clear all vectors

	bool isComplete();   // check if all operations are included
	bool isFeasible();   // check if there is any conflict. The solution does not have to be complete!

	int Psi_j(DMOSP_VectorSol *V); // dissimilarity measure between this vector and
								  // another vector V for taus only
	int Psi_w(DMOSP_VectorSol *V); // dissimilarity measure for pis

	bool RemoveTauSubpermutation(int j, int x1, int x2);
	bool RemovePiSubpermutation(int m, int x1, int x2);

	void RemoveTauOperation(DMOSP_Problem::SOperation *op){ u[op->i] &= 2; };
	void RemovePiOperation(DMOSP_Problem::SOperation *op){ u[op->i] &= 1; };

	void UnremoveAll();

	bool MoveOpToTauPos(DMOSP_Problem::SOperation *op, int x, bool bchkpos=true);
	bool MoveOpToPiPos(DMOSP_Problem::SOperation *op, int k, int x, bool bchkpos=true);

	bool MoveTauSubpermutation(int j, int x1, int x2, int x, bool bchkpos=true);
	bool MovePiSubpermutation(int from_m, int x1, int x2, int to_m, int x, bool bchkpos=true);

	int TauPosition(DMOSP_Problem::SOperation *op);
	int PiPosition(DMOSP_Problem::SOperation *op);

	DMOSP_Problem::SMC *MCofOp(DMOSP_Problem::SOperation *op);
};

struct DMOSP_Node {  // used to represent nodes in the network representations of solutions
  int mc;     // index of the selected machine, = -1 if no machine is assigned

  int m_prv;  // index of the previous operation in machine sequence
  int m_nxt;  // index of the next operation in machine sequence
  int j_prv;  // index of the previous operation in the job route
  int j_nxt;  // index of the next operation in the job route
			  // ---- if a nxt/prv node does not exist, the values are set to -1

  double weight;    // the processing time of the corresponding operation

  double estart;    // [eta] earliest start time of an operation
					// ---> = -1 if the operation is not included in the job tour and machine sequence
  double lstart;    // [lambda] latest start time of an operation
  double tail;      // [gamma] tail of an operation, which is the remaining time to the completion of the schedule
  double slack;     // slack of an operation = lstart - estart
  bool bCritical;   // = true when estart = lstart

  int rank;         // number of all preceding nodes

  void Reset(){  // default node parameters
	mc = -1;     // means no machine is assigned
	m_prv = m_nxt = j_prv = j_nxt = -1;  // no connections with other nodes
	weight = 0;
	estart = lstart = tail = slack = -1;
	bCritical = false;
	rank = 0;
  };
};

class DMOSP_Network {
	friend class DMOSP_NSTester;

  private:
	DMOSP_Algorithm *Alg;         // a pointer to the tabu search algorithm for problem
								 // and algorithm parameters

	// Variables used within the network interpretation function
	double *MCrt;  // machine's ready time
	int *mnxt;  // next node index for the machine
	double *Jnft;  // job's nearest finish time
	int *jnxt;  // next node index for the job

	// Variables used within the task conversion function to network
	int *jcur;  // current operation index of job
	int *mcur;  // current operation index of machine

  public:
	DMOSP_Node *Nodes;    // for each operation, there is a node in the network
	int *FirstjNode;     // index of the first operation in the job tour
	int *LastjNode;      // index of the last operation in the job tour
	int *FirstmNode;     // index of the first node in the machine sequence
	int *LastmNode;      // index of the last operation in the machine sequence

	double Cmax;
	double LB;              // approximate lower bound for an interchange of nodes

	DMOSP_Network(DMOSP_Algorithm *a);
	DMOSP_Network(DMOSP_Network *nt);
	~DMOSP_Network();

	void Assert(wchar_t *);  // make output into dbgFile to show the network elements

	void Copy(DMOSP_Network *nt);

	int GetCPath(int *pth);   // Find an arbitrary critical path and put the node
							  // indexes in that path in "pth" and return the number
							  // of nodes in that path


	void ConvertTasksToNet();      // Convert current solution (Alg->Solution)
								   // into network representation

	void InterpretNetwork(bool &bFeasible, bool &bComplete);
								   // Evaluate the earliest and latest start times
								   // for all operations based on a given complete
								   // network representation including all links
								   // and weights definitions.
								   //   Returns the status of the network
								   // representation if it is feasible or not
								   // and if it represents a complete solution or not

	bool ConvertNetToTasks();      // Convert the current netowrk representation
								   // into tasks stored in Alg->Solution
								   // returns false for infeasible networks

	bool InterpretVectors(DMOSP_VectorSol *V, bool bckf=true);
								   // interprets job orders and processing
								   // sequences into network representation
								   // return false if infeasible

	bool InterpretIncompleteNetwork();   // all nodes are included by some arcs are not
	bool ConvertIncompleteNetToTasks();

	void Reset();

	int sizeof_tau(int j);
	int sizeof_pi(int m);

	DMOSP_Node *tauNode_at(int j, int x);
	DMOSP_Node *piNode_at(int m, int x);

	int tau_pos(DMOSP_Node *nd);
	int pi_pos(DMOSP_Node *nd);

	bool isThereAPath(DMOSP_Node *nd1, DMOSP_Node *nd2);

	// The following functions work with complete networks of feasible solutions
	//     only
	//
	void alpha_omega_N1(DMOSP_Problem::SOperation *v1,
							int &alpha, int &omega, int &apr_alpha, int &apr_omega);
	double phi_N1(DMOSP_Problem::SOperation *v1);
	void ApproxDeltas_N1(DMOSP_Problem::SOperation *v1, int x1,
							double &hat_delta, double &check_delta);
	void EstDeltas_N1(DMOSP_Problem::SOperation *v1, int x1,
							double &hat_delta, double &check_delta);
	void ExactDeltas_N1(DMOSP_Network *Net_, DMOSP_Problem::SOperation *v1, int x1,
							double &hat_delta, double &check_delta);

	void alpha_omega_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2,
							int &alpha, int &omega, int &apr_alpha, int &apr_omega);
	double phi_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2);
	void ApproxDeltas_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2, int x2,
							double &hat_delta, double &check_delta);
	void EstDeltas_N2(DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2, int x2,
							double &hat_delta, double &check_delta);
	void ExactDeltas_N2(DMOSP_Network *Net_, DMOSP_Problem::SOperation *v2, DMOSP_Problem::SMC *mk2,
							int x2, double &hat_delta, double &check_delta);

	void alpha_omega_B1(int j, int x1, int x2, int &alpha, int &omega,
						int &apr_alpha, int &apr_omega);
	void alpha_omega_B2(int m1, int m2, int x1, int x2, int &alpha,
	                    int &omega, int &apr_alpha, int &apr_omega);
};

// An algorithm used for demonstrating neighborhood search moves to improve
// a given initial solution
//

class DMOSP_NSTester : public DMOSP_Algorithm {
  protected:
	DMOSP_Solution *iSol;    // pointer to initial solution passed to the algorithm

	DMOSP_NSTester(DMOSP_Problem *P, wchar_t *N);

  public:
	DMOSP_NSTester(DMOSP_Problem *P, DMOSP_Solution *is);
	~DMOSP_NSTester();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//--------------------------------- Arrays resulting from CPLEX MILP solution

struct DMOSP_MILPSolution {
  // Given data based on the corresponding problem
  DMOSP_Problem *Problem;
  DMOSP_Solution *Solution;
  DMOSP_Solution::SchTask **Tasks;
  bool bTasksModified;

  int nj, nm, no;    // number of jobs, machines and operations
  unsigned int xsz, ysz, zsz; // number of x, y and z decision variables
  int *OJ;           // index of job of an operation
  int *OW, *MW;      // indexes of workstations for operations and machines

  // Results from the CPLEX MILP solution
  double *c;         // jobs completion times
  double *s;         // operations start times
  char *x, *y, *z;   // values of binary decision variables

  DMOSP_MILPSolution(DMOSP_Problem *prb);
  DMOSP_MILPSolution(DMOSP_Solution *sol);
  ~DMOSP_MILPSolution();
  void AllocateMemory();
  void ModifyTasks();
};


//---------------------------------------------------------------------------
#endif
