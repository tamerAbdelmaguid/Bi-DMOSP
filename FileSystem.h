//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

#include "Problem.h"
#include "DMOSPSolver.h"

enum DMOSP_FIO_RESULT {
  SUCCESS,
  FAIL_TO_OPEN,
  UNKNOWN_FILE_TYPE,
  DATA_ERROR,
  INCOMPATIBLE_PROBLEM,
  INFEASIBLE_SOLUTION
};

struct DMOSP_DataHeader {
  char ID[8];
  int nWCs;
  int nMCs;
  int nJobs;
  int nOperations;
};

struct DMOSP_WCData {
  wchar_t ID[8];
  wchar_t Description[64];
  int nMCs;
};

struct DMOSP_MCData {
  wchar_t ID[8];
  wchar_t Description[64];
  double ReadyTime;
};

struct DMOSP_JobData {
  wchar_t ID[8];
  wchar_t Description[64];
  double Weight;
  double ReleaseTime;
  double DueDate;
  int nOperations;  // used to verify data compatibility
};

struct DMOSP_OperationData {
  wchar_t ID[8];
  wchar_t Description[64];
  wchar_t WCID[8];
  int nAltMCs;
};

struct DMOSP_ProcessingTimeData{
  wchar_t MCID[8];
  double ProcessingTime;
};

struct DMOSP_WCData_s {
  int nMCs;
};

struct DMOSP_MCData_s {
  double ReadyTime;
};

struct DMOSP_JobData_s {
  double Weight;
  double ReleaseTime;
  double DueDate;
  int nOperations;  // used to verify data compatibility
};

struct DMOSP_OperationData_s {
  int WCIndex;
  int nAltMCs;
};

struct DMOSP_ProcessingTimeData_s {
  int MCIndex;
  double ProcessingTime;
};

struct DMOSP_ScheduledTask {
  int JobIndex;
  int OperationIndex;
  int MCIndex;
  double Start, End;
};

void SaveDMOSP(DMOSP_Problem *Problem, wchar_t *fileName, DMOSP_FIO_RESULT &Result);
void SaveDMOSPSolution(DMOSP_Problem *Problem, DMOSP_Solution *Sol, wchar_t *fileName, DMOSP_FIO_RESULT &Result);

DMOSP_Problem *LoadDMOSP(wchar_t *fileName, DMOSP_FIO_RESULT &Result);
DMOSP_Solution *LoadDMOSPSolution(DMOSP_Problem *Problem, wchar_t *fileName, DMOSP_FIO_RESULT &Result);

void SaveDMOSP_DAT(DMOSP_Problem *Problem, wchar_t *fileName, DMOSP_FIO_RESULT &Result);

//---------------------------------------------------------------------------
#endif
