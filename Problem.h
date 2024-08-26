//--- This code is Copyright 2019, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef SCHProblemH
#define SCHProblemH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "values.h"
#include "SList.h"
#include "STree.h"

//---- Simple definition class used as the parent for the other classes
class SCH_Entity {
  wchar_t _ID[9];
  wchar_t _Description[65];

public:
  int i;  // an index for the entity to be used by algorithms

  SCH_Entity(wchar_t *ID, wchar_t *Des){
	wcsncpy(_ID, ID, 8);             _ID[8] = 0;
	wcsncpy(_Description, Des, 64);  _Description[64] = 0;
  }

  wchar_t *ID(void) { return _ID; }
  wchar_t *Description(void) { return _Description; }
};

//----------------------------------------- The problem definition class
class DMOSP_Problem : public SCH_Entity {    // If a problem belongs to the solved set, i = Solved problem no.
public:                               // otherwise, i = -1;
  class SWC : public SCH_Entity {    // A class for workcenters
	public:
	  SList MCs;

	  SWC(wchar_t *ID, wchar_t *Des) :  SCH_Entity (ID, Des) {};
	  ~SWC();
  };

  class SMC : public SCH_Entity {    // A class for machines
	public:
	  double ReadyTime;   // Machine ready time
	  SWC *WC;           // A pointer to the worcenter to which it belongs

	  SMC(wchar_t *ID, wchar_t *Des, SWC *wc, double rt)
		  : SCH_Entity (ID, Des), WC(wc), ReadyTime(rt) {};
  };

  class SJob : public SCH_Entity {   // A class for jobs
	public:
	  double ReleaseTime, DueDate, Weight;
	  SList Operations;

	  SJob(wchar_t *ID, wchar_t *Des, double rt=0, double dd=MAXFLOAT, double w=1) :
			SCH_Entity(ID, Des), ReleaseTime(rt), DueDate(dd), Weight (w) {};
	  ~SJob();

	  bool bUsedWC(SWC *wc);    // true if wc is used by the job
  };

  struct SOpMC {  // A structure for defining the operation-machine relationship
	SMC *MC;      // machine
	double pt;    // Corresponding processing time

	SOpMC(SMC *mc, double t=0) : MC(mc), pt(t) {};
  };

  class SOperation : public SCH_Entity {  // A class for operations
	public:
	  SJob *Job;     // Its job
	  SWC *WC;       // Its workcenter
	  SList AltMCs;  // List of alternative machines belonging to WC
					 // with the SOpMC structure

	  SOperation(wchar_t *ID, wchar_t *Des, SJob *jb, SWC *wc, double *t=NULL);
	  SOperation(wchar_t *ID, wchar_t *Des, SJob *jb, SWC *wc, double t);  // special constructor for Matta's problems
	  ~SOperation();

	  double pt(SMC *mc);   // returns processing time on machine mc
	  double minpt();       // returns the minimum processing time for all machines of the WC
	  double maxpt();       // returns the maximum processing time for all machines of the WC
  };

  enum TObjective {
	MIN_MAKESPAN=1,
	MIN_MEAN_FLOWTIME,
	MIN_MEAN_LATENESS,
	MIN_MEAN_TARDINESS,
	MIN_MAX_LATENESS,
	MIN_MAX_TARDINESS,
	MIN_N_TARDY,
	MIN_MEAN_W_LATENESS,
	MIN_MEAN_W_TARDINESS,
	MIN_MEAN_W_FLOWTIME
  };

  // Original problem data - lists of the SCH classes
  TObjective Objective;   // Objective function
  SList WCs;              // List of workcenters
  SList Jobs;             // List of jobs

  DMOSP_Problem(wchar_t *ID, wchar_t *Des, TObjective O=MIN_MAKESPAN);
  DMOSP_Problem(wchar_t *ID, wchar_t *Des, int nWCs, int nJobs, int *nMCs,
                double *pt); // Matta's PMOSP instances
  DMOSP_Problem(wchar_t *ID, wchar_t *Des, int nWCs, int nJobs, int qwLevel,
				double LL, int PTFLevel, int PLJLevel, int JRTFLevel,
				int PBMCLEVEL, int MCATFLevel);  // Specially designed DMOSP instances
  ~DMOSP_Problem();

  SWC *AddWC(wchar_t *ID, wchar_t *Des);
  SWC *InsertWC(SWC *curWC, wchar_t *ID, wchar_t *Des);
  SWC *InsertWC(int icurWC, wchar_t *ID, wchar_t *Des);
  SMC *AddMC(wchar_t *ID, wchar_t *Des, SWC *wc, double rt);
  SMC *InsertMC(SMC *curMC, wchar_t *ID, wchar_t *Des, SWC *wc, double rt);
  SMC *InsertMC(int icurMC, wchar_t *ID, wchar_t *Des, SWC *wc, double rt);
  SJob *AddJob(wchar_t *ID, wchar_t *Des, double rt=0, double dd=0, double w=1);
  SJob *InsertJob(SJob *curJob, wchar_t *ID, wchar_t *Des, double rt=0, double dd=0, double w=1);
  SJob *InsertJob(int icurJob, wchar_t *ID, wchar_t *Des, double rt=0, double dd=0, double w=1);
  SOperation *AddOperation(wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t);
  SOperation *InsertOperation(SOperation *curOp, wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t);
  SOperation *InsertOperation(int icurOp, wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t);

  bool RemoveWC(SWC *wc);
  bool RemoveMC(SMC *mc);
  bool RemoveJob(SJob *jb);
  bool RemoveOperation(SOperation *op);

  bool ModifyWC(SWC *wc, wchar_t *id, wchar_t *des);
  bool ModifyMC(SMC *mc, wchar_t *id, wchar_t *des, double rt);
  bool ModifyJob(SJob *jb, wchar_t *id, wchar_t *des, double rt, double dd, double w, double l=1);
  bool ModifyOperation(SOperation *op, wchar_t *id, wchar_t *des, SWC* wc, double *t);

  bool UsedIDinWCs(wchar_t *id, SWC *except=NULL);
  bool UsedIDinMCs(wchar_t *id, SMC *except=NULL);
  bool UsedIDinJOs(wchar_t *id, SJob *except=NULL);
  bool UsedIDinOps(wchar_t *id, SOperation *except=NULL);
  bool UsedID(wchar_t *id);

  SWC *WCbyID(wchar_t *id);
  SMC *MCbyID(wchar_t *id);
  SJob *JObyID(wchar_t *id);
  SOperation *OpByID(wchar_t *id);

  int nOperations();
  int nOperationsByWC(SWC *wc);
  int nMachines();

  bool isProportionate();
};

struct DMOSP_SolvedInstance {
  int nSol;  // number of solutions in the optimal Pareto front
  double Cmax[23];
  double MWFT[23];  // There are at most 23 Pareto Front solutions for problems in the solved set
};

extern DMOSP_SolvedInstance SolvedDMOSPs[];

#endif
