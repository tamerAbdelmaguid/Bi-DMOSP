//--- This code is Copyright 2014, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#include <math.h>
#include "Problem.h"
#include "random.h"

DMOSP_Problem::SWC::~SWC()
{
  SList::SNode *mcnd = MCs.head();
  while(mcnd){
	DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	delete mc;
	mcnd = mcnd->Next();
  }
  MCs.Clear();
}

DMOSP_Problem::SJob::~SJob()
{
  SList::SNode *nd = Operations.head();
  while(nd){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	delete op;
	nd = nd->Next();
  }
  Operations.Clear();
}

bool DMOSP_Problem::SJob::bUsedWC(DMOSP_Problem::SWC *wc)
{
  SList::SNode *opnd = Operations.head();
  while(opnd){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	if(op->WC == wc) return true;
	opnd = opnd->Next();
  }
  return false;
}

DMOSP_Problem::SOperation::SOperation(wchar_t *ID, wchar_t *Des, DMOSP_Problem::SJob *jb, DMOSP_Problem::SWC *wc, double *t)
   : SCH_Entity(ID, Des), Job(jb), WC(wc)
{
  int i=0;
  if(t != NULL){
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  DMOSP_Problem::SOpMC *opmc = new DMOSP_Problem::SOpMC(mc, t[i++]);
	  AltMCs.Add(opmc);
	  mcnd = mcnd->Next();
	}
  }
}

DMOSP_Problem::SOperation::SOperation(wchar_t *ID, wchar_t *Des, DMOSP_Problem::SJob *jb, DMOSP_Problem::SWC *wc, double t)
   : SCH_Entity(ID, Des), Job(jb), WC(wc)
{
  SList::SNode *mcnd = wc->MCs.head();
  while(mcnd){
	DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	DMOSP_Problem::SOpMC *opmc = new DMOSP_Problem::SOpMC(mc, t);
	AltMCs.Add(opmc);
	mcnd = mcnd->Next();
  }
}

DMOSP_Problem::SOperation::~SOperation()
{
  SList::SNode *opmcnd = AltMCs.head();
  while(opmcnd){
	DMOSP_Problem::SOpMC *opmc = (DMOSP_Problem::SOpMC *)opmcnd->Data();
    delete opmc;
	opmcnd = opmcnd->Next();
  }
  AltMCs.Clear();
}

double DMOSP_Problem::SOperation::pt(DMOSP_Problem::SMC *mc)
{
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->MC == mc) return opmc->pt;
	nd = nd->Next();
  }
  return -1;
}

double DMOSP_Problem::SOperation::minpt()
{
  double minpt = MAXDOUBLE;
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->pt < minpt) return minpt = opmc->pt;
	nd = nd->Next();
  }
  return minpt;
}

double DMOSP_Problem::SOperation::maxpt()
{
  double maxpt = 0;
  SList::SNode *nd = AltMCs.head();
  while(nd){
	SOpMC *opmc = (SOpMC *)nd->Data();
	if(opmc->pt > maxpt) return maxpt = opmc->pt;
	nd = nd->Next();
  }
  return maxpt;
}

//---------------------------------------------------------- DMOSP_Problem

DMOSP_Problem::DMOSP_Problem(wchar_t *ID, wchar_t *Des, DMOSP_Problem::TObjective O)
   : SCH_Entity (ID, Des), Objective(O)
{
  i = -1;     // for unsolved instance (the default value)
}

DMOSP_Problem::DMOSP_Problem(wchar_t *ID, wchar_t *Des, int nWCs, int nJobs, int *nMCs, double *pt)
   : SCH_Entity (ID, Des), Objective(MIN_MAKESPAN)    // A constructor for Matta's PMOSP instances
{
  wchar_t IDbuf[8];
  wchar_t Desbuf[65];

  // Construct WCs
  for(int i=0; i<nWCs; i++){
	swprintf(IDbuf, L"W%i", i+1);
	swprintf(Desbuf, L"Workstation #%i", i+1);
	SWC *newWC = new SWC(IDbuf, Desbuf);
	WCs.Add((void *)newWC);

	// Construct MCs
	for(int j=0; j<nMCs[i]; j++){
	  swprintf(IDbuf, L"M%i-%i", i+1, j+1);
	  swprintf(Desbuf, L"Machine #%i of workstation #%i", i+1, j+1);
	  SMC *newMC = new SMC(IDbuf, Desbuf, newWC, 0);
	  newWC->MCs.Add((void *)newMC);
	}
  }

  // Construct Jobs
  for(int i=0; i<nJobs; i++){
	swprintf(IDbuf, L"J%i", i+1);
	swprintf(Desbuf, L"Job order #%i", i+1);
	SJob *newjob = new SJob(IDbuf, Desbuf, 0, 0, 1);
	Jobs.Add((void *)newjob);

	// Construct Operations
	SList::SNode *wcnd = WCs.head();
	for(int j=0; j<nWCs; j++){
	  SWC *wc = (SWC *)wcnd->Data();
	  swprintf(IDbuf, L"%i-%i", i+1, j+1);
	  swprintf(Desbuf, L"Operation #%i of job J%i", j+1, i+1);
	  SOperation *newop = new SOperation(IDbuf, Desbuf, newjob, wc, pt[j]);
	  newjob->Operations.Add((void *)newop);
	  wcnd = wcnd->Next();
	}
  }

  i = -1;     // for unsolved instance (the default value)
}

DMOSP_Problem::DMOSP_Problem(wchar_t *ID, wchar_t *Des, int nWCs, int nJobs,
			 int qwLevel, double LL, int PTFLevel, int PLJLevel,
			 int JRTFLevel, int PBMCLevel, int MCATFLevel)
	 : SCH_Entity (ID, Des), Objective(MIN_MAKESPAN)   // Specially designed MOSP instances
{
  wchar_t IDbuf[8];
  wchar_t Desbuf[65];

  warmup_random(random(1000)/1000.);

  double *mainPT = new double[nWCs];

  // Construct WCs
  for(int w=0; w<nWCs; w++){
	swprintf(IDbuf, L"W%i", w+1);
	swprintf(Desbuf, L"Workstation #%i", w+1);
	SWC *newWC = new SWC(IDbuf, Desbuf);
	WCs.Add((void *)newWC);

	// Construct MCs
	int nMCs;
	switch(qwLevel){
	  case 1:  nMCs = rnd(1,2);  break;
	  case 2:  nMCs = rnd(1,3);  break;
	  default: nMCs = rnd(1,5);
	}
	for(int m=0; m<nMCs; m++){
	  swprintf(IDbuf, L"M%i-%i", w+1, m+1);
	  swprintf(Desbuf, L"Machine #%i of workstation #%i", m+1, w+1);
	  SMC *newMC = new SMC(IDbuf, Desbuf, newWC, 0);
	  newWC->MCs.Add((void *)newMC);
	}

	mainPT[w] = rndreal(30, 50) * double(nMCs);
  }

  // Construct Jobs
  for(int j=0; j<nJobs; j++){
	swprintf(IDbuf, L"J%i", j+1);
	swprintf(Desbuf, L"Job order #%i", j+1);
	SJob *newjob = new SJob(IDbuf, Desbuf, 0, 0, rnd(1,10));

	double SumPT=0.0;
	int nOps = 0;

	// Construct Operations
	SList::SNode *wcnd = WCs.head();
	for(int w=0; w<nWCs; w++){
	  if(flip(1.0-LL)){
		wcnd = wcnd->Next();
		continue;
	  }

	  SWC *wc = (SWC *)wcnd->Data();
	  swprintf(IDbuf, L"%i-%i", j+1, w+1);
	  swprintf(Desbuf, L"Operation job J%i on workstation #%i", j+1, w+1);

	  double ptf;
	  double *pt = new double[wc->MCs.Count()];
	  double avgpt = 0;

	  int m = 0;
	  for(SList::SNode *mcnd = wc->MCs.head(); mcnd != NULL; mcnd = mcnd->Next()){
		if(PTFLevel == 1) ptf = rndreal(0.8, 1.2);
		else ptf = rndreal(0.6, 1.4);
		pt[m] = ceil(ptf * mainPT[w] * rndreal(0.9, 1.0));
		avgpt += pt[m];
		m++;
	  }
	  avgpt /= double(m);
	  SumPT += avgpt;

	  SOperation *newop = new SOperation(IDbuf, Desbuf, newjob, wc, pt);
	  newjob->Operations.Add((void *)newop);
	  wcnd = wcnd->Next();

	  nOps++;

	  delete [] pt;
	}

	if(nOps == 0){
	  delete newjob;
	}
	else{
	  Jobs.Add((void *)newjob);
	  double prblj;
	  switch(PLJLevel){
		case 1:  prblj = 0.3;  break;
		default: prblj = 0.5;
	  }
	  if(flip(prblj)){
		double overallAvgPT = SumPT / double(nOps);
		double JRTF;
		switch(JRTFLevel){
		  case 1:   JRTF = rndreal(0.2, 2.0);   break;
		  default:  JRTF = rndreal(2.1, 4.0);
		}
		newjob->ReleaseTime = ceil(JRTF * overallAvgPT);
	  }
	}
  }

  // Now loop through all machines to generate machines available times
  int w = 0;
  for(SList::SNode *wcnd = WCs.head(); wcnd; wcnd = wcnd->Next()){
	SWC *wc = (SWC *)wcnd->Data();
	for(SList::SNode *mcnd = wc->MCs.head(); mcnd; mcnd = mcnd->Next()){
	  SMC *mc = (SMC *)mcnd->Data();
	  double prbmb;
	  switch(PBMCLevel){
		case 1:  prbmb = 0.3;  break;
		default: prbmb = 0.5;
	  }
	  if(flip(prbmb)){
		double ATF;
		switch(MCATFLevel){
		  case 1:   ATF = rndreal(0.2, 2.0);  break;
		  default:  ATF = rndreal(2.1, 4.0);
		}
		mc->ReadyTime = ceil(mainPT[w] * ATF);
	  }
	}
	w++;
  }

  delete [] mainPT;
  i = -1;     // for unsolved instance (the default value)
}

DMOSP_Problem::~DMOSP_Problem()
{
  SList::SNode *nd;

  nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	delete wc;
	nd = nd->Next();
  }
  WCs.Clear();

  nd = Jobs.head();
  while(nd){
    SJob *jb = (SJob *)nd->Data();
    delete jb;
    nd = nd->Next();
  }
  Jobs.Clear();
}

DMOSP_Problem::SWC *DMOSP_Problem::AddWC(wchar_t *ID, wchar_t *Des)
{
  if(UsedID(ID)) return NULL;
  SWC *newWC = new SWC(ID, Des);
  WCs.Add(newWC);
  return newWC;
}

DMOSP_Problem::SWC *DMOSP_Problem::InsertWC(SWC *curWC, wchar_t *ID, wchar_t *Des)
{
  if(UsedID(ID)) return NULL;
  SWC *newWC = new SWC(ID, Des);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *ewc = (SWC *)nd->Data();
	if(ewc == curWC){
	  curNode = nd;
	  break;
	}
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newWC;
	return NULL;
  }

  WCs.AddBefore(curNode, newWC);

  return newWC;
}

DMOSP_Problem::SWC *DMOSP_Problem::InsertWC(int icurWC, wchar_t *ID, wchar_t *Des)
{
  if(icurWC < 0 || icurWC >= WCs.Count()) return NULL;
  return InsertWC((SWC *)(WCs[icurWC]), ID, Des);
}

DMOSP_Problem::SMC *DMOSP_Problem::AddMC(wchar_t *ID, wchar_t *Des, DMOSP_Problem::SWC *wc, double rt)
{
  if(UsedID(ID)) return NULL;
  SMC *newmc = new SMC(ID, Des, wc, rt);
  wc->MCs.Add(newmc);

  // loop through all operations to add the new machine to the list of alternative
  // machines whenever an operation uses wc
  //
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == wc){
		SOpMC *newopmc = new SOpMC(newmc);
		op->AltMCs.Add(newopmc);
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  return newmc;
}

DMOSP_Problem::SMC *DMOSP_Problem::InsertMC(DMOSP_Problem::SMC *curMC, wchar_t *ID, wchar_t *Des, DMOSP_Problem::SWC *wc, double rt)
{
  if(UsedID(ID)) return NULL;
  SMC *newmc = new SMC(ID, Des, wc, rt);

  SList::SNode *curnd = NULL;
  SList::SNode *mcnd = wc->MCs.head();
  while(mcnd){
	SMC *emc = (SMC *)mcnd->Data();
	if(emc == curMC){
	  curnd = mcnd;
	  break;
	}
	mcnd = mcnd->Next();
  }
  if(curnd == NULL){
	delete newmc;
	return NULL;
  }

  wc->MCs.AddBefore(curnd, newmc);

  // loop through all operations to add the new machine to the list of alternative
  // machines whenever an operation uses wc
  //
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == wc){
		SOpMC *newopmc = new SOpMC(newmc);
		curnd = NULL;
		mcnd = op->AltMCs.head();
		while(mcnd){
		  SOpMC *eopmc = (SOpMC *)mcnd->Data();
		  if(eopmc->MC == curMC){
			curnd = mcnd;
			break;
		  }
		  mcnd = mcnd->Next();
		}
		if(curnd) op->AltMCs.AddBefore(curnd, newopmc);
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  return newmc;
}

DMOSP_Problem::SMC *DMOSP_Problem::InsertMC(int icurMC, wchar_t *ID, wchar_t *Des, DMOSP_Problem::SWC *wc, double rt)
{
  if(icurMC < 0 || icurMC >= wc->MCs.Count()) return NULL;
  return InsertMC((SMC *)(wc->MCs[icurMC]), ID, Des, wc, rt);
}

DMOSP_Problem::SJob *DMOSP_Problem::AddJob(wchar_t *ID, wchar_t *Des, double rt,
									 double dd, double w)
{
  if(UsedID(ID)) return NULL;
  SJob *newJob = new SJob(ID, Des, rt, dd, w);
  Jobs.Add(newJob);

  return newJob;
}

DMOSP_Problem::SJob *DMOSP_Problem::InsertJob(DMOSP_Problem::SJob *curJob,
	   wchar_t *ID, wchar_t *Des, double rt, double dd, double w)
{
  if(UsedID(ID)) return NULL;
  SJob *newJob = new SJob(ID, Des, rt, dd, w);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = Jobs.head();
  while(nd){
	SJob *ejb = (SJob *)nd->Data();
	if(ejb == curJob) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newJob;
	return NULL;
  }

  Jobs.AddBefore(curNode, newJob);

  return newJob;
}

DMOSP_Problem::SJob *DMOSP_Problem::InsertJob(int icurJob, wchar_t *ID, wchar_t *Des,
						   double rt, double dd, double w)
{
  if(icurJob < 0 || icurJob >= Jobs.Count()) return NULL;
  return InsertJob((SJob *)(Jobs[icurJob]), ID, Des, rt, dd, w);
}

DMOSP_Problem::SOperation *DMOSP_Problem::AddOperation(wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t)
{
  if(UsedID(ID)) return NULL;
  SOperation *newOP;
  newOP = new SOperation(ID, Des, j, p, t);
  j->Operations.Add(newOP);
  return newOP;
}

DMOSP_Problem::SOperation *DMOSP_Problem::
  InsertOperation(DMOSP_Problem::SOperation *curOp, wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t)
{
  if(UsedID(ID)) return NULL;
  SOperation *newOp = new SOperation(ID, Des, j, p, t);

  SList::SNode *curNode = NULL;
  SList::SNode *nd = j->Operations.head();
  while(nd){
	DMOSP_Problem::SOperation *eop = (DMOSP_Problem::SOperation *)nd->Data();
	if(eop == curOp) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL){
	delete newOp;
	return NULL;
  }

  j->Operations.AddBefore(curNode, newOp);
  return newOp;
}

DMOSP_Problem::SOperation *DMOSP_Problem::
  InsertOperation(int icurOp, wchar_t *ID, wchar_t *Des, SJob *j, SWC *p, double *t)
{
  if(icurOp < 0 || icurOp >= j->Operations.Count()) return NULL;
  return InsertOperation((SOperation *)(j->Operations[icurOp]), ID, Des, j, p, t);
}

bool DMOSP_Problem::RemoveMC(SMC *mc)
{
  // first loop through all operations to remove mc from their lists
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *ond = jb->Operations.head();
	while(ond){
	  SOperation *op = (SOperation *)ond->Data();
	  if(op->WC == mc->WC){
		SOpMC *opmc;
		SList::SNode *opmcnd = op->AltMCs.head();
		while(opmcnd){
		  opmc = (SOpMC *)opmcnd->Data();
		  if(opmc->MC == mc){
			op->AltMCs.Remove(opmcnd);
			delete opmc;
			break;
		  }
		  opmcnd = opmcnd->Next();
		}
	  }
	  ond = ond->Next();
	}
	jnd = jnd->Next();
  }

  mc->WC->MCs.Remove((void *)mc);
  delete mc;
  return true;
}

bool DMOSP_Problem::RemoveWC(SWC *wc)
{
  // Check if it is currently used by any operation
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
	SJob *jb = (SJob *)jbnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  SOperation *sop = (SOperation *)opnd->Data();
	  if(sop->WC == wc) return false;
	  opnd = opnd->Next();
	}
	jbnd = jbnd->Next();
  }

  WCs.Remove((void *)wc);
  delete wc;
  return true;
}

bool DMOSP_Problem::RemoveJob(SJob *jb)
{
  // first delete all operations related to this job
  SList::SNode *opnd = jb->Operations.head();
  while(opnd){
	SOperation *sop = (SOperation *)opnd->Data();
	delete sop;
    opnd = opnd->Next();
  }
  jb->Operations.Clear();

  Jobs.Remove((void *)jb);
  delete jb;

  return true;
}

bool DMOSP_Problem::RemoveOperation(SOperation *op)
{
  SJob *j = op->Job;
  j->Operations.Remove((void *) op);
  delete op;
  return true;
}

bool DMOSP_Problem::ModifyWC(SWC *wc, wchar_t *id, wchar_t *des)
{
  if(UsedIDinWCs(id, wc) || UsedIDinMCs(id) || UsedIDinJOs(id) || UsedIDinOps(id)) return false;
  wcsncpy(wc->ID(), id, 8);
  wcsncpy(wc->Description(), des, 64);
  return true;
}

bool DMOSP_Problem::ModifyMC(SMC *mc, wchar_t *id, wchar_t *des, double rt)
{
  if(UsedIDinWCs(id) || UsedIDinMCs(id, mc) || UsedIDinJOs(id) || UsedIDinOps(id)) return false;
  wcsncpy(mc->ID(), id, 8);
  wcsncpy(mc->Description(), des, 64);
  mc->ReadyTime = rt;
  return true;
}

bool DMOSP_Problem::ModifyJob(SJob *jb, wchar_t *id, wchar_t *des, double rt,
										  double dd, double w, double)
{
  if(UsedIDinJOs(id, jb) || UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinOps(id)) return false;
  wcsncpy(jb->ID(), id, 8);
  wcsncpy(jb->Description(), des, 64);
  jb->ReleaseTime = rt;
  jb->DueDate = dd;
  jb->Weight = w;
  return true;
}

bool DMOSP_Problem::ModifyOperation(DMOSP_Problem::SOperation *op, wchar_t *id, wchar_t *des, SWC* wc, double *t)
{
  if(UsedIDinJOs(id) || UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinOps(id, op)) return false;

  wcsncpy(op->ID(), id, 8);
  wcsncpy(op->Description(), des, 64);

  if(op->WC == wc){
	int i=0;
	for(SList::SNode *opmcnd = op->AltMCs.head(); opmcnd; opmcnd = opmcnd->Next()){
	  SOpMC *opmc = (SOpMC *)opmcnd->Data();
	  opmc->pt = t[i++];
	}
  }
  else{
	for(SList::SNode *opmcnd = op->AltMCs.head(); opmcnd; opmcnd = opmcnd->Next()){
	  SOpMC *opmc = (SOpMC *)opmcnd->Data();
	  delete opmc;
	}
	op->AltMCs.Clear();
	int i=0;
	for(SList::SNode *mcnd = wc->MCs.head(); mcnd; mcnd = mcnd->Next()){
      SMC *mc = (SMC *)mcnd->Data();
	  SOpMC *newopmc = new SOpMC(mc, t[i++]);
	  op->AltMCs.Add(newopmc);
	}
	op->WC = wc;
  }

  return true;
}

bool DMOSP_Problem::UsedIDinWCs(wchar_t *id, SWC *except)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	if(wcscmp(id, wc->ID()) == 0 && wc != except) return true;
	nd = nd->Next();
  }
  return false;
}

bool DMOSP_Problem::UsedIDinMCs(wchar_t *id, SMC *except)
{
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  SMC *mc = (SMC *)mcnd->Data();
	  if(wcscmp(id, mc->ID()) == 0 && mc != except) return true;
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
  return false;
}

bool DMOSP_Problem::UsedIDinJOs(wchar_t *id, SJob *except)
{
  SList::SNode *nd = Jobs.head();
  while(nd){
	SJob *jb = (SJob *)nd->Data();
	if(wcscmp(id, jb->ID()) == 0 && jb != except) return true;
	nd = nd->Next();
  }
  return false;
}

bool DMOSP_Problem::UsedIDinOps(wchar_t *id, SOperation *except)
{
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
	SJob *jb = (SJob *)jbnd->Data();
	SList::SNode *nd = jb->Operations.head();
	while(nd){
	  SOperation *op = (SOperation *)nd->Data();
	  if(wcscmp(id, op->ID()) == 0 && op != except) return true;
	  nd = nd->Next();
	}
	jbnd = jbnd->Next();
  }
  return false;
}

bool DMOSP_Problem::UsedID(wchar_t *id)
{
  return UsedIDinWCs(id) || UsedIDinMCs(id) || UsedIDinJOs(id) || UsedIDinOps(id);
}

DMOSP_Problem::SWC *DMOSP_Problem::WCbyID(wchar_t *id)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	if(wcscmp(id, wc->ID()) == 0) return wc;
	nd = nd->Next();
  }
  return NULL;
}

DMOSP_Problem::SMC *DMOSP_Problem::MCbyID(wchar_t *id)
{
  SList::SNode *nd = WCs.head();
  while(nd){
	SWC *wc = (SWC *)nd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  SMC *mc = (SMC *)mcnd->Data();
	  if(wcscmp(id, mc->ID()) == 0) return mc;
	  mcnd = mcnd->Next();
	}
	nd = nd->Next();
  }
  return NULL;
}

DMOSP_Problem::SJob *DMOSP_Problem::JObyID(wchar_t *id)
{
  SList::SNode *nd = Jobs.head();
  while(nd){
	SJob *jb = (SJob *)nd->Data();
	if(wcscmp(id, jb->ID()) == 0) return jb;
	nd = nd->Next();
  }
  return NULL;
}

DMOSP_Problem::SOperation *DMOSP_Problem::OpByID(wchar_t *id)
{
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
	SJob *jb = (SJob *)jbnd->Data();
	SList::SNode *nd = jb->Operations.head();
	while(nd){
	  SOperation *op = (SOperation *)nd->Data();
	  if(wcscmp(id, op->ID()) == 0) return op;
	  nd = nd->Next();
	}
	jbnd = jbnd->Next();
  }
  return NULL;
}

int DMOSP_Problem::nOperations()
{
  int nops = 0;
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    nops += (jb->Operations.Count());
    jbnd = jbnd->Next();
  }
  return nops;
}

int DMOSP_Problem::nOperationsByWC(SWC *wc)
{
  int nops = 0;
  SList::SNode *jbnd = Jobs.head();
  while(jbnd){
    SJob *jb = (SJob *)jbnd->Data();
    SList::SNode *opnd = jb->Operations.head();
    while(opnd){
      SOperation *op = (SOperation *)opnd->Data();
	  if(op->WC == wc) nops++;
      opnd = opnd->Next();
    }
    jbnd = jbnd->Next();
  }
  return nops;
}

int DMOSP_Problem::nMachines()
{
  int nmcs = 0;
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	nmcs += (wc->MCs.Count());
	wcnd = wcnd->Next();
  }
  return nmcs;
}

bool DMOSP_Problem::isProportionate()
{
  // A proportionate MOSP means that all jobs needs processing on all workcenters
  // and the processing times on all parallel machines of any workcenter are equal
  //

  // 1. Check if all jobs need processing on all workcenters
  int nWCs = WCs.Count();
  SList::SNode *jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	if(jb->Operations.Count() != nWCs) return false;
	jnd = jnd->Next();
  }

  // 2. Check if each job will have the same processing time on all prallel machines
  //    for all workcenters
  jnd = Jobs.head();
  while(jnd){
	SJob *jb = (SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  SOperation *op = (SOperation *)opnd->Data();
	  if(op->minpt() != op->maxpt()) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }

  // 3. Check if all jobs will have the same processing time on the same workcenter
  SList::SNode *wcnd = WCs.head();
  while(wcnd){
	SWC *wc = (SWC *)wcnd->Data();
	float pt = -1;
	jnd = Jobs.head();
	while(jnd){
	  SJob *jb = (SJob *)jnd->Data();
	  SList::SNode *opnd = jb->Operations.head();
	  SOperation *gop = NULL;
	  while(opnd){
		SOperation *op = (SOperation *)opnd->Data();
		if(op->WC == wc){
		  gop = op;
		  break;
		}
		opnd = opnd->Next();
	  }
	  if(gop == NULL) return false;
	  if(pt == -1) pt = gop->minpt();
	  if(pt != gop->minpt()) return false;
	  jnd = jnd->Next();
	}
	wcnd = wcnd->Next();
  }

  return true;
}

//----------- Information about the set of solved instances -------------------

DMOSP_SolvedInstance SolvedDMOSPs[31] =
{
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-1*/ 8,
  391, 366, 362, 348, 340, 339, 337, 334, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  818.7, 819.9, 828.1, 830.8, 839.1, 839.6, 840.1, 853.9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-2*/ 4,
  391, 371, 369, 366, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  746.2, 746.3, 752.6, 765.8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-3*/ 4,
  305, 303, 301, 299, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  800, 801.9, 802.4, 830, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-4*/ 14,
  399, 380, 376, 364, 342, 334, 333, 328, 327, 320, 318, 314, 313, 312, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1018.4, 1018.9, 1020.4, 1022, 1024.7, 1025.3, 1032.6, 1033.2, 1039.5, 1050.2, 1051.2, 1064.7, 1173.4, 1178.4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-5*/ 1,
  392, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  616.5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-6*/ 4,
  412, 395, 390, 389, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1354.2, 1363.8, 1364.2, 1367.8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-7*/ 2,
  376, 375, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1279.5555, 1295.2222, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-8*/ 7,
  403, 402, 401, 395, 394, 393, 387, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  884.2, 885.7, 890.8, 891.7, 892.7, 901.5, 902, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-9*/ 7,
  407, 389, 387, 384, 381, 376, 375, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1267.5, 1280.7, 1282.7, 1280, 1290.6, 1292.7, 1305.8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-10*/ 4,
  407, 388, 377, 362, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1510.7, 1515.8, 1521.2, 1538, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-11*/ 10,
  379, 377, 370, 369, 363, 361, 359, 352, 343, 342, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  624.9, 626.2, 627.2, 627.8, 628.1, 628.3, 628.7, 629.4, 630.5, 631.3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-12*/ 7,
  413, 412, 411, 403, 402, 401, 400, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1197, 1199.8, 1200, 1204.9, 1209.2, 1213.3, 1219, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-13*/ 16,
  430, 428, 426, 404, 379, 378, 377, 376, 375, 374, 372, 370, 369, 368, 367, 365, -1, -1, -1, -1, -1, -1, -1,
  1591.8, 1592.2, 1594.2, 1595.8, 1613.4, 1622.4, 1626.8, 1627.6, 1628.2, 1632.4, 1640, 1641.6, 1652.1, 1660.3, 1664.2, 1684.1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-14*/ 8,
  352, 349, 346, 342, 336, 332, 330, 328, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  952.3, 954.8, 957.1, 957.9, 959.9, 969.7, 971.3, 975.6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-15*/ 17,
  376, 375, 365, 364, 347, 346, 345, 340, 339, 339, 335, 334, 332, 330, 328, 326, 324, -1, -1, -1, -1, -1, -1,
  842, 847.4, 848, 852.9, 853.5, 854.7, 862.1, 863.7, 865.9, 869.2, 873.3, 884.3, 884.9, 887.9, 905.7, 909.9, 912, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-16*/ 3,
  390, 387, 386, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1185.2, 1188, 1191.7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-17*/ 11,
  490, 478, 475, 460, 448, 446, 437, 431, 426, 425, 423, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1225.2, 1226.6, 1230, 1231.4, 1235.4, 1242.2, 1246, 1250.8, 1253.3, 1260.4, 1372.1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-18*/ 6,
  339, 328, 326, 316, 314, 309, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  698.1, 700.3, 702.5, 702.7, 703, 714.2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-19*/ 3,
  363, 356, 354, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1084.1, 1093.6, 1112.6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-20*/ 2,
  468, 467, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  914.5, 916.8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-21*/ 11,
  311, 307, 302, 301, 297, 286, 285, 279, 276, 275, 274, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  826.9, 833.1, 835.6, 836.5, 837.1, 847.3, 847.9, 850.1, 854.6, 870, 879.3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-22*/ 23,
  434, 417, 414, 413, 394, 391, 390, 372, 363, 359, 349, 347, 341, 339, 338, 332, 331, 329, 328, 324, 316, 310, 308,
  1129.9, 1132.8, 1134.4, 1135.6, 1136.5, 1137.4, 1141.9, 1142.4, 1145.6, 1147.3, 1150.1, 1151.2, 1154.8, 1156.8, 1158.6, 1163.4, 1166.8, 1171.6, 1172.6, 1173.4, 1173.8, 1193.1, 1196.3,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-23*/ 1,
  412, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1194.9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-24*/ 8,
  436, 433, 431, 428, 423, 422, 421, 420, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  874.8, 881.3, 882.1, 887.8, 888, 891.7, 892.3, 893.5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-25*/ 5,
  389, 384, 376, 369, 364, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  815.5, 818.6, 820.1, 824.3, 839, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-26*/ 5,
  333, 332, 326, 325, 324, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  861.9, 868.3, 868.7, 869.2, 871.1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-27*/ 2,
  347, 314, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1112.3, 1114.9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-28*/ 2,
  389, 363, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  997, 1001.4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-29*/ 2,
  435, 434, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  738.222, 740.444, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*DMOSP-5-10-1-0.7-1-2-1-2-1-30*/ 1,
  424, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1580.6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*SampleDMOSP*/ 6,
  327, 316, 310, 301, 298, 296, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  1134, 1134.67, 1158.167, 1160.167, 1184.5, 1246, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
