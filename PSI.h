//---------------------------------------------------------------------------

#ifndef PSIH
#define PSIH

#include "DMOSPSolver.h"

//------------- Definitions of classes used to manage processing sequences
//              of operations on machines
//

struct DMOSP_PS {  // a class for managing processing sequence on a given machine
  DMOSP_Problem::SMC *MC;
  SList PS;  // a processing sequence of operations

  DMOSP_PS(DMOSP_Problem::SMC *mc) : MC(mc) {};

  bool AppendOp(DMOSP_Problem::SOperation *op);
  bool InsertOp(DMOSP_Problem::SOperation *newop, DMOSP_Problem::SOperation *op);
  bool InsertOp(DMOSP_Problem::SOperation *newop, int pos);
  bool Swap(DMOSP_Problem::SOperation *op1, DMOSP_Problem::SOperation *op2);
  bool Swap(int pos1, int pos2);
};

struct DMOSP_VPS {  // a class for managing vectors of processing sequences on machines
  DMOSP_Problem *Problem;
  SList PSs;

  DMOSP_VPS(DMOSP_Problem *P);
  DMOSP_VPS(DMOSP_VPS *vps);
  ~DMOSP_VPS();

  bool AppendOp(DMOSP_Problem::SMC *mc, DMOSP_Problem::SOperation *op);
  bool isIncluded(DMOSP_Problem::SOperation *op);
  bool Copy(DMOSP_VPS *vps);

  bool operator ==(DMOSP_VPS &vps);
  bool operator !=(DMOSP_VPS &vps);
  DMOSP_VPS &operator =(DMOSP_VPS &vps);
  bool isComplete();   // Means all operations are included

  DMOSP_Problem::SMC *MCAssignedToOP(DMOSP_Problem::SOperation *op);
  DMOSP_PS *PSContainingOP(DMOSP_Problem::SOperation *op);

  DMOSP_PS *PS(DMOSP_Problem::SMC *mc);
};

//-------------- Definition of classes used to manage operations' orders within
//               jobs.
//

struct DMOSP_OO {   // operations' order within a job
  DMOSP_Problem::SJob *job;
  SList OO;    // Operation's order

  DMOSP_OO(DMOSP_Problem::SJob *j) : job(j) {};

  bool AppendOp(DMOSP_Problem::SOperation *op);
  bool InsertOp(DMOSP_Problem::SOperation *newop, DMOSP_Problem::SOperation *op);
  bool InsertOp(DMOSP_Problem::SOperation *newop, int pos);
  bool Swap(DMOSP_Problem::SOperation *op1, DMOSP_Problem::SOperation *op2);
  bool Swap(int pos1, int pos2);
};

struct DMOSP_VOO {  // a class for managing vectors of operations orders within jobs
  DMOSP_Problem *Problem;
  SList OOs;

  DMOSP_VOO(DMOSP_Problem *P);
  DMOSP_VOO(DMOSP_VOO *voo);
  ~DMOSP_VOO();

  bool AppendOp(DMOSP_Problem::SOperation *op);
  bool isIncluded(DMOSP_Problem::SOperation *op);
  bool Copy(DMOSP_VOO *voo);

  bool operator ==(DMOSP_VOO &voo);
  bool operator !=(DMOSP_VOO &voo);
  DMOSP_VOO &operator =(DMOSP_VOO &voo);
  bool isComplete();   // Means all operations are included

  DMOSP_OO *OO(DMOSP_Problem::SJob *jb);
};

//-------------- This is a simple algorithm that interprets a given vector of
//               processing sequences and operations' orders into semi-active
//               schedule
//

class DMOSP_PSI : public DMOSP_Algorithm {
  protected:
	DMOSP_PSI(DMOSP_Problem *P, DMOSP_VPS *vps, DMOSP_VOO *voo, wchar_t *N);

	int *nxt;   // Next schedulable operation index

	double *mcrt,  // an array for the machine ready times
		   *jbrt;  // an array for the jobs release times which represent the
				   // minimum time at which the next unscheduled operation can start

  public:
	DMOSP_VPS VPS;  // vector of processing sequences on machines
	DMOSP_VOO VOO;  // vector of operations' orders within jobs
	DMOSP_VPS *tmpVPS;
	DMOSP_VOO *tmpVOO;

	DMOSP_PSI(DMOSP_Problem *P, DMOSP_VPS *vps, DMOSP_VOO *voo);
	~DMOSP_PSI();

	virtual void AllocateMemory();
	virtual void Initialize();
	virtual bool Stop();
	virtual void Finalize();
	virtual void FreeMemory();
	virtual void ComputeObjectiveValue();
};

//---------------------------------------------------------------------------
#endif
