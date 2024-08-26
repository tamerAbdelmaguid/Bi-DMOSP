//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef ALGORITHM
#define ALGORITHM

#include <vcl.h>
#pragma hdrstop

#include <sys\timeb.h>
#include <stdlib.h>
#include <time.h>
#include "Problem.h"

//------------------------------------------ Generic definitions for algorithms

class Algorithm {
  protected:
	DMOSP_Problem *Problem;
	wchar_t Name[64];
	double ObjectiveValue;
	double ComputationalTime, MaxTimeLimit;  // defined in milli-seconds
	bool bCompuTimeLimitReached;
	timeb start_time, cur_time, end_time;

  public:
	bool bInterrupt;

	Algorithm(DMOSP_Problem *P, wchar_t *N=L"", double TL=-1){
	  wcsncpy(Name, N, 64);
	  Problem = P;
	  MaxTimeLimit = TL;
	  ComputationalTime = 0;
	  bInterrupt = false;
	};
	~Algorithm() {};

	const wchar_t *GetName(){ return Name; }
	const double GetObjectiveValue(){ return ObjectiveValue; }
	const double GetTime(){ return ComputationalTime; }
	const double GetMaxTime(){ return MaxTimeLimit; }
	void SetMaxTime(double mxt){ MaxTimeLimit = mxt; }
	DMOSP_Problem *GetProblem(){ return Problem; }

	virtual void AllocateMemory() = 0;
	virtual void Initialize() = 0;
	virtual void Improve() = 0;
	virtual bool Stop() = 0;
	virtual void Finalize() = 0;
	virtual void FreeMemory() = 0;
	virtual void ComputeObjectiveValue() = 0;

	void Run(){
      bCompuTimeLimitReached = false;
	  ObjectiveValue = MAXFLOAT;
	  AllocateMemory();              // 1. allocate necessary memory space
	  ::ftime(&start_time);          // 2. record start time
      ComputationalTime = 0.0;
	  Initialize();                  // 3. do initialization step
      ::ftime(&cur_time);
	  while(!Stop()){                // 4. conduct iterations
		Improve();
		Application->ProcessMessages();
		if(MaxTimeLimit > 0){
		  ::ftime(&cur_time);
		  ComputationalTime = (cur_time.time - start_time.time)*1000.0 +
								   (cur_time.millitm - start_time.millitm);
		  if(ComputationalTime >= MaxTimeLimit){
            bCompuTimeLimitReached = true;
			break;
		  }
		}
		if(bInterrupt) break;
	  }
	  Finalize();                    // 5. do finalization step
	  ::ftime(&end_time);            // 6. record end time
	  ComputeObjectiveValue();       // 7. Compute final solution O.F. value
	  FreeMemory();                  // 8. free up allocated memory space

	  ComputationalTime = (end_time.time - start_time.time)*1000.0 +
						  (end_time.millitm - start_time.millitm);
	};
};

//------------------------------------------------------------------------

#endif
