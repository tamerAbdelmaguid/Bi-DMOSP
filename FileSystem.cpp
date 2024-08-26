//--- This code is Copyright 2014, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <dos.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include <mem.h>

#include "FileSystem.h"
#include "SList.h"

const wchar_t *DMOSP_FileID = L"DMOSP  ";
const wchar_t *DMOSP_Solution_FileID = L"DMOSPSL";

void SaveDMOSP(DMOSP_Problem *Problem, wchar_t *fileName, DMOSP_FIO_RESULT &Result)
{
  int hfout = _wrtl_creat(fileName, FA_NORMAL);
  if(hfout==-1){ Result = FAIL_TO_OPEN;   return; }

  SList::SNode *nd;
  unsigned int i;
  DMOSP_DataHeader header;
  DMOSP_WCData pc_data;
  DMOSP_MCData mc_data;
  DMOSP_JobData job_data;
  DMOSP_OperationData op_data;
  DMOSP_ProcessingTimeData pt_data;

  //--------------------- Writing header information
  //
  for(i=0; i<8; i++) header.ID[i] = DMOSP_FileID[i];
  header.nWCs = Problem->WCs.Count();
  header.nMCs = Problem->nMachines();
  header.nJobs = Problem->Jobs.Count();
  header.nOperations = Problem->nOperations();
  write(hfout, &header, sizeof(DMOSP_DataHeader));

  //--------------------- Writing Processing Centers' and Machines' information
  //
  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	DMOSP_Problem::SWC *pc = (DMOSP_Problem::SWC *)pcnd->Data();
	wchar_t *s = pc->ID();
	for(i=0; i<wcslen(s); i++) pc_data.ID[i] = s[i];
	for(; i<8; i++) pc_data.ID[i] = 0;
	s = pc->Description();
	for(i=0; i<wcslen(s); i++) pc_data.Description[i] = s[i];
	for(; i<64; i++) pc_data.Description[i] = 0;
	pc_data.nMCs = pc->MCs.Count();
	write(hfout, &pc_data, sizeof(DMOSP_WCData));

	nd = pc->MCs.head();
	while(nd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)nd->Data();
	  wchar_t *s = mc->ID();
	  for(i=0; i<wcslen(s); i++) mc_data.ID[i] = s[i];
	  for(; i<8; i++) mc_data.ID[i] = 0;
	  s = mc->Description();
	  for(i=0; i<wcslen(s); i++) mc_data.Description[i] = s[i];
	  for(; i<64; i++) mc_data.Description[i] = 0;
	  mc_data.ReadyTime = mc->ReadyTime;
	  write(hfout, &mc_data, sizeof(DMOSP_MCData));
	  nd = nd->Next();
	}
	pcnd = pcnd->Next();
  }

  //--------------------- Writing Jobs and Operations data
  //
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	DMOSP_Problem::SJob *job = (DMOSP_Problem::SJob *)jbnd->Data();
	wchar_t *s = job->ID();
	for(i=0; i<wcslen(s); i++) job_data.ID[i] = s[i];
	for(; i<8; i++) job_data.ID[i] = 0;
	s = job->Description();
	for(i=0; i<wcslen(s); i++) job_data.Description[i] = s[i];
	for(; i<64; i++) job_data.Description[i] = 0;
	job_data.Weight = job->Weight;
	job_data.ReleaseTime = job->ReleaseTime;
	job_data.DueDate = job->DueDate;
	job_data.nOperations = job->Operations.Count();
	write(hfout, &job_data, sizeof(DMOSP_JobData));

	nd = job->Operations.head();
	while(nd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	  wchar_t *s = op->ID();
	  for(i=0; i<wcslen(s); i++) op_data.ID[i] = s[i];
	  for(; i<8; i++) op_data.ID[i] = 0;
	  s = op->Description();
	  for(i=0; i<wcslen(s); i++) op_data.Description[i] = s[i];
	  for(; i<64; i++) op_data.Description[i] = 0;
	  s = op->WC->ID();
	  for(i=0; i<wcslen(s); i++) op_data.WCID[i] = s[i];
	  for(; i<8; i++) op_data.WCID[i] = 0;
	  op_data.nAltMCs = op->AltMCs.Count();
	  write(hfout, &op_data, sizeof(DMOSP_OperationData));

	  // Writing the processing time vector
	  SList::SNode *altmcnd = op->AltMCs.head();
	  while(altmcnd){
		DMOSP_Problem::SOpMC *altmc = (DMOSP_Problem::SOpMC *)altmcnd->Data();
		s = altmc->MC->ID();
		for(i=0; i<wcslen(s); i++) pt_data.MCID[i] = s[i];
		for(; i<8; i++) pt_data.MCID[i] = 0;
		pt_data.ProcessingTime = altmc->pt;
		write(hfout, &pt_data, sizeof(DMOSP_ProcessingTimeData));
		altmcnd = altmcnd->Next();
	  }
	  nd = nd->Next();
	}
	jbnd = jbnd->Next();
  }

  close(hfout);
  Result = SUCCESS;
}

void SaveDMOSPSolution(DMOSP_Problem *Problem, DMOSP_Solution *Sol, wchar_t *fileName, DMOSP_FIO_RESULT &Result)
{
  int hfout = _wrtl_creat(fileName, FA_NORMAL);
  if(hfout==-1){ Result = FAIL_TO_OPEN;   return; }

  // Save problem structure first ---------------------------------------------
  //
  SList::SNode *nd, *mcnd, *opnd;
  unsigned int i;
  DMOSP_DataHeader header;
  DMOSP_WCData_s pc_data;
  DMOSP_MCData_s mc_data;
  DMOSP_JobData_s job_data;
  DMOSP_OperationData_s op_data;
  DMOSP_ProcessingTimeData_s pt_data;

  //--------------------- Writing header information
  //
  for(i=0; i<8; i++) header.ID[i] = DMOSP_Solution_FileID[i];
  header.nWCs = Problem->WCs.Count();
  header.nMCs = Problem->nMachines();
  header.nJobs = Problem->Jobs.Count();
  header.nOperations = Problem->nOperations();
  write(hfout, &header, sizeof(DMOSP_DataHeader));

  //--------------------- Writing Processing Centers' and machines' information
  //
  nd = Problem->WCs.head();
  while(nd){
	DMOSP_Problem::SWC *pc = (DMOSP_Problem::SWC *)nd->Data();
	pc_data.nMCs = pc->MCs.Count();
	write(hfout, &pc_data, sizeof(DMOSP_WCData_s));
	mcnd = pc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  mc_data.ReadyTime = mc->ReadyTime;
	  write(hfout, &mc_data, sizeof(DMOSP_MCData_s));
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }

  //--------------------- Writing Jobs' and operations' data
  //
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *job = (DMOSP_Problem::SJob *)nd->Data();
	job_data.Weight = job->Weight;
	job_data.ReleaseTime = job->ReleaseTime;
	job_data.DueDate = job->DueDate;
	job_data.nOperations = job->Operations.Count();
	write(hfout, &job_data, sizeof(DMOSP_JobData_s));
	opnd = job->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  op_data.WCIndex = Problem->WCs.IndexOf((void *)op->WC);
	  op_data.nAltMCs = op->AltMCs.Count();
	  write(hfout, &op_data, sizeof(DMOSP_OperationData_s));
	  mcnd = op->AltMCs.head();
	  while(mcnd){
		DMOSP_Problem::SOpMC *altmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		pt_data.MCIndex = op->WC->MCs.IndexOf((void *)altmc->MC);
		pt_data.ProcessingTime = altmc->pt;
		write(hfout, &pt_data, sizeof(DMOSP_ProcessingTimeData_s));
		mcnd = mcnd->Next();
	  }
	  opnd = opnd->Next();
	}
	nd = nd->Next();
  }

  // Save solution ------------------------------------------------------------
  //
  DMOSP_ScheduledTask schtsk;
  nd = Sol->ScheduledTasks.head();
  while(nd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)nd->Data();
	schtsk.JobIndex = Problem->Jobs.IndexOf((void *)(tsk->Operation->Job));
	schtsk.OperationIndex = tsk->Operation->Job->Operations.IndexOf((void *)tsk->Operation);
	schtsk.MCIndex = tsk->Operation->WC->MCs.IndexOf((void *)tsk->SelectedMC);
	schtsk.Start = tsk->StartTime;
	schtsk.End = tsk->EndTime;
	write(hfout, &schtsk, sizeof(DMOSP_ScheduledTask));
	nd = nd->Next();
  }

  //---> Important Note: here the order by which jobs and processing centers
  //     are stored is important. We consider this order part of the problem
  //     structure.

  close(hfout);
  Result = SUCCESS;
}

DMOSP_Problem *LoadDMOSP(wchar_t *fileName, DMOSP_FIO_RESULT &Result)
{
  int hfin = _wrtl_open(fileName, O_RDONLY);
  if(hfin==-1){ Result = FAIL_TO_OPEN;   return NULL; }

  int i, j, l;
  DMOSP_DataHeader header;
  DMOSP_WCData pc_data;
  DMOSP_MCData mc_data;
  DMOSP_JobData job_data;
  DMOSP_OperationData op_data;
  DMOSP_ProcessingTimeData pt_data;

  read(hfin, &header, sizeof(DMOSP_DataHeader));
  for(i=0; i<8; i++)
	if(header.ID[i] != DMOSP_FileID[i]){
	  Result = UNKNOWN_FILE_TYPE;
	  return NULL;
	}

  wchar_t ProbDes[64];
  l = wcslen(fileName);
  if(l > 64) l = 64;
  for(i=0; i<l; i++) ProbDes[i] = fileName[i];
  for(; i<65; i++) ProbDes[i] = 0;
  DMOSP_Problem *newProblem = new DMOSP_Problem(L"DMOSP", ProbDes);

  //--------------------- Reading Processing Centers and Machines' data
  //
  for(i=0; i<header.nWCs; i++){
	read(hfin, &pc_data, sizeof(DMOSP_WCData));
	DMOSP_Problem::SWC *newpc = newProblem->AddWC(pc_data.ID, pc_data.Description);
	for(j=0; j<pc_data.nMCs; j++){
	  read(hfin, &mc_data, sizeof(DMOSP_MCData));
	  newProblem->AddMC(mc_data.ID, mc_data.Description, newpc, mc_data.ReadyTime);
	}
  }

  //--------------------- Reading Jobs and operations data
  //
  for(i=0; i<header.nJobs; i++){
	read(hfin, &job_data, sizeof(DMOSP_JobData));
	DMOSP_Problem::SJob *job = newProblem->AddJob(job_data.ID, job_data.Description,
								job_data.ReleaseTime, job_data.DueDate, job_data.Weight);
	for(j=0; j<job_data.nOperations; j++){
	  read(hfin, &op_data, sizeof(DMOSP_OperationData));
	  DMOSP_Problem::SWC *pc = newProblem->WCbyID(op_data.WCID);
	  if(!pc || op_data.nAltMCs != pc->MCs.Count()){ // data integrality problem
		delete newProblem;
		Result = DATA_ERROR;
		return NULL;
	  }
	  DMOSP_Problem::SOperation *newop =
			  newProblem->AddOperation(op_data.ID, op_data.Description, job, pc, NULL);

	  // read processing time data
	  for(int m=0; m<op_data.nAltMCs; m++){
		read(hfin, &pt_data, sizeof(DMOSP_ProcessingTimeData));

		DMOSP_Problem::SMC *mc = newProblem->MCbyID(pt_data.MCID);
		if(mc->WC != pc){
		  delete newProblem;
		  Result = DATA_ERROR;
		  return NULL;
		}
		DMOSP_Problem::SOpMC *opmc = new DMOSP_Problem::SOpMC(mc, pt_data.ProcessingTime);
		newop->AltMCs.Add(opmc);
	  }
	}
  }

  if(newProblem->nOperations() != header.nOperations
	 || newProblem->nMachines() != header.nMCs){
	delete newProblem;
	Result = DATA_ERROR;
	return NULL;
  }

  close(hfin);
  Result = SUCCESS;
  return newProblem;
}

DMOSP_Solution *LoadDMOSPSolution(DMOSP_Problem *Problem, wchar_t *fileName, DMOSP_FIO_RESULT &Result)
{
  int hfin = _wrtl_open(fileName, O_RDONLY);
  if(hfin==-1){ Result = FAIL_TO_OPEN;   return NULL; }

  // Check first for the problem structure ------------------------------------
  //
  SList::SNode *nd, *mcnd, *opnd;
  int i;
  DMOSP_DataHeader header;
  DMOSP_WCData_s pc_data;
  DMOSP_MCData_s mc_data;
  DMOSP_JobData_s job_data;
  DMOSP_OperationData_s op_data;
  DMOSP_ProcessingTimeData_s pt_data;

  //--------------------- Verifying header information
  //
  read(hfin, &header, sizeof(DMOSP_DataHeader));
  for(i=0; i<8; i++)
	if(header.ID[i] != DMOSP_Solution_FileID[i]){
	  Result = UNKNOWN_FILE_TYPE;
	  return NULL;
	}
  if(header.nWCs != Problem->WCs.Count() ||
	 header.nMCs != Problem->nMachines() ||
	 header.nJobs != Problem->Jobs.Count() ||
	 header.nOperations != Problem->nOperations()){
	   Result = INCOMPATIBLE_PROBLEM;
	   return NULL;
  }

  //--------------------- Verifying Processing Centers' information
  //
  nd = Problem->WCs.head();
  while(nd){
	DMOSP_Problem::SWC *pc = (DMOSP_Problem::SWC *)nd->Data();
	read(hfin, &pc_data, sizeof(DMOSP_WCData_s));
	if(pc_data.nMCs != pc->MCs.Count()){
	   Result = INCOMPATIBLE_PROBLEM;
	   return NULL;
	}
	mcnd = pc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  read(hfin, &mc_data, sizeof(DMOSP_MCData_s));
	  if(mc_data.ReadyTime != mc->ReadyTime){
		Result = INCOMPATIBLE_PROBLEM;
		return NULL;
	  }
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }

  //--------------------- Verifying Jobs and operations data
  //
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *job = (DMOSP_Problem::SJob *)nd->Data();
	read(hfin, &job_data, sizeof(DMOSP_JobData_s));
	if( job_data.Weight != job->Weight ||
		job_data.ReleaseTime != job->ReleaseTime ||
		job_data.DueDate != job->DueDate ||
		job_data.nOperations != job->Operations.Count()){
		  Result = INCOMPATIBLE_PROBLEM;
		  return NULL;
	}
	opnd = job->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  read(hfin, &op_data, sizeof(DMOSP_OperationData_s));
	  if( op_data.WCIndex != Problem->WCs.IndexOf((void *)op->WC) ||
		  op_data.nAltMCs != op->AltMCs.Count()){
			Result = INCOMPATIBLE_PROBLEM;
			return NULL;
	  }
	  mcnd = op->AltMCs.head();
	  while(mcnd){
		DMOSP_Problem::SOpMC *altmc = (DMOSP_Problem::SOpMC *)mcnd->Data();
		read(hfin, &pt_data, sizeof(DMOSP_ProcessingTimeData_s));
		if( pt_data.MCIndex != op->WC->MCs.IndexOf((void *)altmc->MC) ||
			pt_data.ProcessingTime != altmc->pt){
			  Result = INCOMPATIBLE_PROBLEM;
			  return NULL;
		}
		mcnd = mcnd->Next();
	  }
	  opnd = opnd->Next();
	}
	nd = nd->Next();
  }

  // Load solution ------------------------------------------------------------
  //
  DMOSP_ScheduledTask schtsk;
  DMOSP_Solution *Sol = new DMOSP_Solution(Problem, false);
  for(i=0; i<header.nOperations; i++){
	read(hfin, &schtsk, sizeof(DMOSP_ScheduledTask));
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)(Problem->Jobs[schtsk.JobIndex]);
	DMOSP_Problem::SOperation *op =
			(DMOSP_Problem::SOperation *)(jb->Operations[schtsk.OperationIndex]);
	DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)(op->WC->MCs[schtsk.MCIndex]);
	Sol->ScheduledTasks.Add(new DMOSP_Solution::SchTask(op, mc, schtsk.Start, schtsk.End));
  }

  close(hfin);
  Result = SUCCESS;
  return Sol;
}

void SaveDMOSP_DAT(DMOSP_Problem *Problem, wchar_t *fName, DMOSP_FIO_RESULT &Result)
{
  SList::SNode *nd;
  wchar_t buf[128];

  FILE *DATFile = _wfopen(fName, L"w");
  if(DATFile == NULL){ Result = FAIL_TO_OPEN;   return; }

  fwprintf(DATFile, L"/*********************************************\n");
  fwprintf(DATFile, L" * OPL 12.8.0.0 Data\n");
  fwprintf(DATFile, L" * Author: Tamer Abdelmaguid\n");
  fwprintf(DATFile, L" * Input data from a specially developed application\n");
  fwprintf(DATFile, L" * for the DMOSP problem.\n");
  swprintf(buf, L" * Problem name: %s\n", fName);
  fwprintf(DATFile, buf);
  fwprintf(DATFile, L" *********************************************/\n\n");

  swprintf(buf, L" nj = %i;\n", Problem->Jobs.Count());
  fwprintf(DATFile, buf);

  int nm = 0;
  nd = Problem->WCs.head();
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	nm += wc->MCs.Count();
	nd = nd->Next();
  }
  swprintf(buf, L" nm = %i;\n", nm);
  fwprintf(DATFile, buf);

  nd = Problem->Jobs.head();
  int no = 0;
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	no += jb->Operations.Count();
	nd = nd->Next();
  }
  swprintf(buf, L" no = %i;\n\n", no);
  fwprintf(DATFile, buf);

  fwprintf(DATFile, L" // Jobs data:\n");
  fwprintf(DATFile, L"\t p = [ ");
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	swprintf(buf, L"%0.2f%s", jb->Weight, ((nd->Next()!= NULL)? L", " : L"];\n" ));
	fwprintf(DATFile, buf);
	nd = nd->Next();
  }
  fwprintf(DATFile, L"\t r = [ ");
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	swprintf(buf, L"%0.2f%s", jb->ReleaseTime, ((nd->Next()!= NULL)? L", " : L"];\n" ));
	fwprintf(DATFile, buf);
	nd = nd->Next();
  }

  fwprintf(DATFile, L"\n // Machines data:\n");
  fwprintf(DATFile, L"\tMW = [ ");
  nd = Problem->WCs.head();
  int w = 1;
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  swprintf(buf, L"%i%s", w,
					((mcnd->Next()!= NULL || nd->Next()!= NULL)? L", " : L"];\n" ));
	  fwprintf(DATFile, buf);
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
	w++;
  }
  fwprintf(DATFile, L"\t a = [ ");
  nd = Problem->WCs.head();
  while(nd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  swprintf(buf, L"%0.2f%s", mc->ReadyTime,
					((mcnd->Next()!= NULL || nd->Next()!= NULL)? L", " : L"];\n" ));
	  fwprintf(DATFile, buf);
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }

  fwprintf(DATFile, L"\n // Operations data:\n");
  fwprintf(DATFile, L"\tOJ = [ ");
  nd = Problem->Jobs.head();
  int j = 1;
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  swprintf(buf, L"%i%s", j,
					((opnd->Next()!= NULL || nd->Next()!= NULL)? L", " : L"];\n" ));
	  fwprintf(DATFile, buf);
	  opnd = opnd->Next();
	}
	nd = nd->Next();
	j++;
  }
  fwprintf(DATFile, L"\tOW = [ ");
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  swprintf(buf, L"%i%s", Problem->WCs.IndexOf((void *)(op->WC))+1,
					((opnd->Next()!= NULL || nd->Next()!= NULL)? L", " : L"];\n" ));
	  fwprintf(DATFile, buf);
	  opnd = opnd->Next();
	}
	nd = nd->Next();
  }

  double totalPT = 0;
  fwprintf(DATFile, L"\n // Ordered processing times:\n");
  fwprintf(DATFile, L"\td = [\n\t\t");
  nd = Problem->Jobs.head();
  while(nd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)nd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  SList::SNode *opmcnd = op->AltMCs.head();
	  while(opmcnd){
		DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)opmcnd->Data();
		swprintf(buf, L"%0.2f%s", opmc->pt,
					  ((opmcnd->Next()!= NULL)? L", " : ((opnd->Next()!=NULL || nd->Next()!=NULL)? L",\n\t\t" : L"\n") ));
		fwprintf(DATFile, buf);
		opmcnd = opmcnd->Next();
		totalPT += opmc->pt;
	  }
	  opnd = opnd->Next();
	}
	nd = nd->Next();
  }
  fwprintf(DATFile, L"\t    ];\n");

  double bigM = 1.0;
  while(totalPT > 0.999){
	totalPT /= 10.0;
	bigM *= 10.0;
  }
  swprintf(buf, L"\n bigM = %0.2f;\n\n", bigM);
  fwprintf(DATFile, buf);

  fwprintf(DATFile, L"/********************************* MOD file *************************************\n");

  fwprintf(DATFile, L" int nj = ...;         // number of jobs\n");
  fwprintf(DATFile, L" range J = 1..nj;      // set of jobs\n\n");
  fwprintf(DATFile, L" int nm = ...;         // total number of machines\n");
  fwprintf(DATFile, L" range M = 1..nm;      // set of all machines in the shop\n");
  fwprintf(DATFile, L" int MW[M] = ...;      // workstation # to which machine m belongs\n\n");
  fwprintf(DATFile, L" float a[M] = ...;     // non-negative ready time of a machine\n");
  fwprintf(DATFile, L" float r[J] = ...;     // non-negative release time of a job\n");
  fwprintf(DATFile, L" float p[J] = ...;     // priority of job j\n\n");
  fwprintf(DATFile, L" int no = ...;         // total number of operations\n");
  fwprintf(DATFile, L" range O = 1..no;      // set of all operations\n");
  fwprintf(DATFile, L" int OJ[O] = ...;      // index of the job to which the operation belongs\n");
  fwprintf(DATFile, L" int OW[O] = ...;      // index of the workstation to which the operation belongs\n\n");
  fwprintf(DATFile, L" tuple TOpMc {\n");
  fwprintf(DATFile, L"   int o;    // operation index\n");
  fwprintf(DATFile, L"   int m;    // machine index\n");
  fwprintf(DATFile, L" };\n");
  fwprintf(DATFile, L" setof(TOpMc) Process = { <o, m> | o in O, m in M : OW[o] == MW[m] };\n");
  fwprintf(DATFile, L" float d[Process] = ...;         // processing times of operations on machines\n\n");
  fwprintf(DATFile, L" tuple TJOpPair {\n");
  fwprintf(DATFile, L"   int w;    // index of the first operation\n");
  fwprintf(DATFile, L"   int q;    // index of the second operation\n");
  fwprintf(DATFile, L" };\n");
  fwprintf(DATFile, L" setof(TJOpPair) DefX = { <w, q> | w, q in O : OJ[w] == OJ[q] && q > w};\n");
  fwprintf(DATFile, L" dvar int x[DefX] in 0..1;\n\n");
  fwprintf(DATFile, L" dvar int y[Process] in 0..1;\n\n");
  fwprintf(DATFile, L" tuple TOpPairMc {\n");
  fwprintf(DATFile, L"   int j;   // index of the first operation\n");
  fwprintf(DATFile, L"   int k;   // index of the second operation\n");
  fwprintf(DATFile, L"   int m;   // machine index\n");
  fwprintf(DATFile, L" };\n");
  fwprintf(DATFile, L" setof(TOpPairMc) DefZ = { <j, k, m> | j, k in O, m in M : j != k && OW[j] == OW[k] && OW[j] == MW[m]};\n");
  fwprintf(DATFile, L" dvar int z[DefZ] in 0..1;\n\n");
  fwprintf(DATFile, L" dvar float+ s[O];\n");
  fwprintf(DATFile, L" dvar float+ c[J];\n");
  fwprintf(DATFile, L" dvar float Cmax;\n");
  fwprintf(DATFile, L" dvar float MWFT;\n\n");
  fwprintf(DATFile, L" float bigM = ...;\n\n");
  fwprintf(DATFile, L" //minimize Cmax;     // (1)\n\n");
  fwprintf(DATFile, L" minimize MWFT;     // (2)\n\n");
  fwprintf(DATFile, L" subject to {\n");
  fwprintf(DATFile, L"  MWFT_Definition:\n");
  fwprintf(DATFile, L"   MWFT == sum(j in J) p[j] * (c[j] - r[j]) / nj;\n\n");
  fwprintf(DATFile, L"  Cmax_epsilon_constraint:  // need to change the RHS value iteratively\n");
  fwprintf(DATFile, L"   Cmax <= bigM;  \n\n");
  fwprintf(DATFile, L"  Constraint1:\n");
  fwprintf(DATFile, L"   forall(j in J) Cmax >= c[j];           // (3)\n\n");
  fwprintf(DATFile, L"  Constraint2:\n");
  fwprintf(DATFile, L"   forall(j in J)\n");
  fwprintf(DATFile, L"     forall(o in O : OJ[o] == j)\n");
  fwprintf(DATFile, L"       c[j] >= s[o] + sum(m in M : OW[o] == MW[m]) d[<o, m>] * y[<o, m>];        // (4)\n\n");
  fwprintf(DATFile, L"  Constraint3:\n");
  fwprintf(DATFile, L"   forall(o in O)\n");
  fwprintf(DATFile, L"      sum(m in M : OW[o] == MW[m]) y[<o, m>] == 1;          // (5)\n\n");
  fwprintf(DATFile, L"  Constraint4:\n");
  fwprintf(DATFile, L"   forall(j in J)\n");
  fwprintf(DATFile, L"     forall(o in O : OJ[o] == j)\n");
  fwprintf(DATFile, L"       s[o] >= r[j];                                        // (6)\n\n");
  fwprintf(DATFile, L"  Constraint5:\n");
  fwprintf(DATFile, L"   forall(o in O)\n");
  fwprintf(DATFile, L"     s[o] >= sum(m in M : OW[o] == MW[m]) a[m] * y[<o, m>];        // (7)\n\n");
  fwprintf(DATFile, L"  Constraint6:\n");
  fwprintf(DATFile, L"   forall(j in J)\n");
  fwprintf(DATFile, L"     forall(w in O : OJ[w] == j)\n");
  fwprintf(DATFile, L"       forall(q in O : OJ[q] == j && q > w)\n");
  fwprintf(DATFile, L"         s[q] - s[w] >= sum(m in M : OW[w] == MW[m]) d[<w, m>] * y[<w, m>]\n");
  fwprintf(DATFile, L"                        - bigM * (1 - x[<w, q>]);                                  // (8)\n\n");
  fwprintf(DATFile, L"  Constraint7:\n");
  fwprintf(DATFile, L"   forall(j in J)\n");
  fwprintf(DATFile, L"     forall(w in O : OJ[w] == j)\n");
  fwprintf(DATFile, L"       forall(q in O : OJ[q] == j && q > w)\n");
  fwprintf(DATFile, L"         s[w] - s[q] >= sum(m in M : OW[q] == MW[m]) d[<q, m>] * y[<q, m>]\n");
  fwprintf(DATFile, L"                        - bigM * x[<w, q>];                                        // (9)\n\n");
  fwprintf(DATFile, L"  Constraint8:\n");
  fwprintf(DATFile, L"   forall(j in O)\n");
  fwprintf(DATFile, L"     forall(k in O : j < k)\n");
  fwprintf(DATFile, L"       forall(m in M : OW[j] == MW[m] && OW[k] == MW[m])\n");
  fwprintf(DATFile, L"         z[<j, k, m>] + z[<k, j, m>] <= (y[<j, m>] + y[<k, m>]) / 2;              // (10)\n\n");
  fwprintf(DATFile, L"  Constraint9:\n");
  fwprintf(DATFile, L"   forall(j in O)\n");
  fwprintf(DATFile, L"     forall(k in O : j < k)\n");
  fwprintf(DATFile, L"       forall(m in M : OW[j] == MW[m] && OW[k] == MW[m])\n");
  fwprintf(DATFile, L"         z[<j, k, m>] + z[<k, j, m>] >= y[<j, m>] + y[<k, m>] - 1;                // (11)\n\n");
  fwprintf(DATFile, L"  Constraint10:\n");
  fwprintf(DATFile, L"   forall(j in O)\n");
  fwprintf(DATFile, L"     forall(k in O : j != k && OW[k] == OW[j])\n");
  fwprintf(DATFile, L"        s[k] - s[j] >= sum(m in M : OW[j] == MW[m]) d[<j, m>] * y[<j, m>]\n");
  fwprintf(DATFile, L"                       - bigM * (1 - sum(m in M : OW[j] == MW[m]) z[<j, k, m>]);  // (12)\n");
  fwprintf(DATFile, L"}\n");

  fwprintf(DATFile, L" ************************************************************************************/\n\n");
  fwprintf(DATFile, L"/*main\n{\n thisOplModel.generate();\n");
  fwprintf(DATFile, L" cplex.exportModel(\"C:\\Work\\DMOSP\\Program\\CPLEX\\lpmodel.lp\");\n");
  fwprintf(DATFile, L"}/**/\n");

  fflush(DATFile);
  fclose(DATFile);
  Result = SUCCESS;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
