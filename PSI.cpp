//---------------------------------------------------------------------------


#pragma hdrstop

#include "PSI.h"
#include "FileSystem.h"

//------------------------ Definitions for processing sequence class functions

//-------------------- Processing sequence on a given machine

bool DMOSP_PS::AppendOp(DMOSP_Problem::SOperation *op)
{
  if(PS.IndexOf((void *)op) != -1) return false;
  PS.Add(op);
  return true;
}

bool DMOSP_PS::InsertOp(DMOSP_Problem::SOperation *newop, DMOSP_Problem::SOperation *op)
{
  if(PS.IndexOf((void *)newop) != -1) return false;
  SList::SNode *curNode = NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	DMOSP_Problem::SOperation *eop = (DMOSP_Problem::SOperation *)nd->Data();
	if(eop == op) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL) return false;

  PS.AddBefore(curNode, newop);
  return true;
}

bool DMOSP_PS::InsertOp(DMOSP_Problem::SOperation *newop, int pos)
{
  if(pos < 0 || pos > PS.Count()) return false;
  if(pos == PS.Count()){
	PS.Add(newop);
	return true;
  }
  SList::SNode *nd = PS.head();
  while(pos != 0){ nd = nd->Next();   pos--;  }
  if(nd){
	PS.AddBefore(nd, newop);
	return true;
  }
  return false;
}

bool DMOSP_PS::Swap(DMOSP_Problem::SOperation *op1, DMOSP_Problem::SOperation *op2)
{
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	if(op==op1) nd1 = nd;
	if(op==op2) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	nd = nd->Next();
  }
  if(nd1 == NULL || nd2 == NULL) return false;
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

bool DMOSP_PS::Swap(int pos1, int pos2)
{
  if(pos1<0 || pos1>PS.Count() || pos2<0 || pos2>PS.Count() || pos1==pos2)
	return false;
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = PS.head();
  while(nd){
	if(pos1 == 0) nd1 = nd;
	if(pos2 == 0) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	pos1--;    pos2--;
	nd = nd->Next();
  }
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

//-------------------- Vector of processing sequences on machines

DMOSP_VPS::DMOSP_VPS(DMOSP_Problem *P)
{
  Problem = P;
  SList::SNode *wcnd = Problem->WCs.head();
  while(wcnd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  PSs.Add(new DMOSP_PS(mc));
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
}

DMOSP_VPS::DMOSP_VPS(DMOSP_VPS *vps)
{
  Problem = vps->Problem;
  SList::SNode *wcnd = Problem->WCs.head();
  while(wcnd){
	DMOSP_Problem::SWC *wc = (DMOSP_Problem::SWC *)wcnd->Data();
	SList::SNode *mcnd = wc->MCs.head();
	while(mcnd){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  DMOSP_PS *newps = new DMOSP_PS(mc);
	  PSs.Add(newps);
	  newps->PS.Add(vps->PS(mc)->PS);
	  mcnd = mcnd->Next();
	}
	wcnd = wcnd->Next();
  }
}

DMOSP_PS *DMOSP_VPS::PS(DMOSP_Problem::SMC *mc)
{
  DMOSP_PS *ret = NULL;
  SList::SNode *nd = PSs.head();
  while(nd){
	DMOSP_PS *ps = (DMOSP_PS *)nd->Data();
	if(ps->MC == mc){ ret = ps;   break; }
	nd = nd->Next();
  }
  return ret;
}

DMOSP_VPS::~DMOSP_VPS()
{
  SList::SNode *nd = PSs.head();
  while(nd){
	DMOSP_PS *ps = (DMOSP_PS *)nd->Data();
	delete ps;
    nd = nd->Next();
  }
  PSs.Clear();
}

bool DMOSP_VPS::AppendOp(DMOSP_Problem::SMC *mc, DMOSP_Problem::SOperation *op)
{
  DMOSP_PS *ps=NULL;
  SList::SNode *psnd = PSs.head();
  while(psnd){
	ps = (DMOSP_PS *)psnd->Data();
	if(ps->MC == mc) break;
    psnd = psnd->Next();
  }
  if(!ps) return false;
  return ps->AppendOp(op);
}

bool DMOSP_VPS::isIncluded(DMOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC){
      psnd = psnd->Next();
	  continue;
	}
	if(ps->PS.IndexOf(op) != -1) return true;
	psnd = psnd->Next();
  }
  return false;
}

bool DMOSP_VPS::operator ==(DMOSP_VPS &vps)
{
  if(Problem != vps.Problem) return false;

  SList::SNode *psnd1, *psnd2;
  psnd1 = PSs.head();
  psnd2 = vps.PSs.head();
  while(psnd1 && psnd2){
	DMOSP_PS *ps1 = (DMOSP_PS *)psnd1->Data();
	DMOSP_PS *ps2 = (DMOSP_PS *)psnd2->Data();
	if(ps1->PS.Count() != ps2->PS.Count() || ps1->MC != ps2->MC) return false;
    SList::SNode *opnd1, *opnd2;
    opnd1 = ps1->PS.head();
    opnd2 = ps2->PS.head();
	for(int j=0; j<ps1->PS.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return false;
	  opnd1 = opnd1->Next();
      opnd2 = opnd2->Next();
    }
    psnd1 = psnd1->Next();
    psnd2 = psnd2->Next();
  }
  return true;
}

bool DMOSP_VPS::operator !=(DMOSP_VPS &vps)
{
  if(Problem != vps.Problem) return true;

  SList::SNode *psnd1, *psnd2;
  psnd1 = PSs.head();
  psnd2 = vps.PSs.head();
  while(psnd1 && psnd2){
	DMOSP_PS *ps1 = (DMOSP_PS *)psnd1->Data();
	DMOSP_PS *ps2 = (DMOSP_PS *)psnd2->Data();
	if(ps1->PS.Count() != ps2->PS.Count() || ps1->MC != ps2->MC) return true;
	SList::SNode *opnd1, *opnd2;
	opnd1 = ps1->PS.head();
	opnd2 = ps2->PS.head();
	for(int j=0; j<ps1->PS.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return true;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	psnd1 = psnd1->Next();
	psnd2 = psnd2->Next();
  }
  return false;
}

bool DMOSP_VPS::Copy(DMOSP_VPS *vps)
{
  if(vps->Problem != Problem) return false;
  SList::SNode *psnd = PSs.head(), *psndg = vps->PSs.head();
  while(psnd){
    DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
    DMOSP_PS *psg = (DMOSP_PS *)psndg->Data();
    ps->PS.Clear();
	ps->MC = psg->MC;
    ps->PS.Add(psg->PS);
    psnd = psnd->Next();
    psndg = psndg->Next();
  }
  return true;
}

DMOSP_VPS& DMOSP_VPS::operator =(DMOSP_VPS &vps)
{
  if(vps.Problem != Problem) return *this;
  SList::SNode *psnd = PSs.head(), *psndg = vps.PSs.head();
  while(psnd){
    DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
    DMOSP_PS *psg = (DMOSP_PS *)psndg->Data();
    ps->PS.Clear();
	ps->MC = psg->MC;
    ps->PS.Add(psg->PS);
    psnd = psnd->Next();
	psndg = psndg->Next();
  }
  return *this;
}

bool DMOSP_VPS::isComplete()
{
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  if(!isIncluded(op)) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }
  return true;
}

DMOSP_Problem::SMC *DMOSP_VPS::MCAssignedToOP(DMOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC) continue;
	if(ps->PS.IndexOf(op) != -1) return ps->MC;
	psnd = psnd->Next();
  }
  return NULL;
}

DMOSP_PS *DMOSP_VPS::PSContainingOP(DMOSP_Problem::SOperation *op)
{
  SList::SNode *psnd = PSs.head();
  while(psnd){
	DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
	if(ps->MC->WC != op->WC) continue;
	if(ps->PS.IndexOf(op) != -1) return ps;
	psnd = psnd->Next();
  }
  return NULL;
}

//------------ Definitions of operations order within jobs management functions

//-------------------- Order of operations within a given job

bool DMOSP_OO::AppendOp(DMOSP_Problem::SOperation *op)
{
  if(OO.IndexOf((void *)op) != -1) return false;
  OO.Add(op);
  return true;
}

bool DMOSP_OO::InsertOp(DMOSP_Problem::SOperation *newop, DMOSP_Problem::SOperation *op)
{
  if(OO.IndexOf((void *)newop) != -1) return false;
  SList::SNode *curNode = NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	DMOSP_Problem::SOperation *eop = (DMOSP_Problem::SOperation *)nd->Data();
	if(eop == op) curNode = nd;
	nd = nd->Next();
  }
  if(curNode == NULL) return false;

  OO.AddBefore(curNode, newop);
  return true;
}

bool DMOSP_OO::InsertOp(DMOSP_Problem::SOperation *newop, int pos)
{
  if(pos < 0 || pos > OO.Count()) return false;
  if(pos == OO.Count()){
	OO.Add(newop);
	return true;
  }
  SList::SNode *nd = OO.head();
  while(pos != 0){ nd = nd->Next();   pos--;  }
  if(nd){
	OO.AddBefore(nd, newop);
	return true;
  }
  return false;
}

bool DMOSP_OO::Swap(DMOSP_Problem::SOperation *op1, DMOSP_Problem::SOperation *op2)
{
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)nd->Data();
	if(op==op1) nd1 = nd;
	if(op==op2) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	nd = nd->Next();
  }
  if(nd1 == NULL || nd2 == NULL) return false;
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

bool DMOSP_OO::Swap(int pos1, int pos2)
{
  if(pos1<0 || pos1>OO.Count() || pos2<0 || pos2>OO.Count() || pos1==pos2)
	return false;
  SList::SNode *nd1=NULL, *nd2=NULL;
  SList::SNode *nd = OO.head();
  while(nd){
	if(pos1 == 0) nd1 = nd;
	if(pos2 == 0) nd2 = nd;
	if(nd1 != NULL && nd2 != NULL) break;
	pos1--;    pos2--;
	nd = nd->Next();
  }
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
  return true;
}

//-------------------- Vector of operations orders within jobs

DMOSP_VOO::DMOSP_VOO(DMOSP_Problem *P)
{
  Problem = P;
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jbnd->Data();
	OOs.Add(new DMOSP_OO(jb));
	jbnd = jbnd->Next();
  }
}

DMOSP_VOO::DMOSP_VOO(DMOSP_VOO *voo)
{
  Problem = voo->Problem;
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jbnd->Data();
	DMOSP_OO *newoo = new DMOSP_OO(jb);
	OOs.Add(newoo);
	newoo->OO.Add(voo->OO(jb)->OO);
	jbnd = jbnd->Next();
  }
}

DMOSP_OO *DMOSP_VOO::OO(DMOSP_Problem::SJob *jb)
{
  DMOSP_OO *ret = NULL;
  SList::SNode *nd = OOs.head();
  while(nd){
	DMOSP_OO *oo = (DMOSP_OO *)nd->Data();
	if(oo->job == jb){ ret = oo;   break; }
	nd = nd->Next();
  }
  return ret;
}

DMOSP_VOO::~DMOSP_VOO()
{
  SList::SNode *nd = OOs.head();
  while(nd){
	DMOSP_OO *oo = (DMOSP_OO *)nd->Data();
	delete oo;
	nd = nd->Next();
  }
  OOs.Clear();
}

bool DMOSP_VOO::AppendOp(DMOSP_Problem::SOperation *op)
{
  DMOSP_OO *oo=NULL;
  SList::SNode *oond = OOs.head();
  while(oond){
	oo = (DMOSP_OO *)oond->Data();
	if(oo->job == op->Job) break;
	oond = oond->Next();
  }
  if(!oo) return false;
  return oo->AppendOp(op);
}

bool DMOSP_VOO::isIncluded(DMOSP_Problem::SOperation *op)
{
  SList::SNode *oond = OOs.head();
  while(oond){
	DMOSP_OO *oo = (DMOSP_OO *)oond->Data();
	if(oo->job != op->Job){
	  oond = oond->Next();
	  continue;
	}
	if(oo->OO.IndexOf(op) != -1) return true;
	oond = oond->Next();
  }
  return false;
}

bool DMOSP_VOO::operator ==(DMOSP_VOO &voo)
{
  if(Problem != voo.Problem) return false;

  SList::SNode *oond1, *oond2;
  oond1 = OOs.head();
  oond2 = voo.OOs.head();
  while(oond1 && oond2){
	DMOSP_OO *oo1 = (DMOSP_OO *)oond1->Data();
	DMOSP_OO *oo2 = (DMOSP_OO *)oond2->Data();
	if(oo1->OO.Count() != oo2->OO.Count() || oo1->job != oo2->job) return false;
	SList::SNode *opnd1, *opnd2;
	opnd1 = oo1->OO.head();
	opnd2 = oo2->OO.head();
	for(int j=0; j<oo1->OO.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return false;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	oond1 = oond1->Next();
	oond2 = oond2->Next();
  }
  return true;
}

bool DMOSP_VOO::operator !=(DMOSP_VOO &voo)
{
  if(Problem != voo.Problem) return true;

  SList::SNode *oond1, *oond2;
  oond1 = OOs.head();
  oond2 = voo.OOs.head();
  while(oond1 && oond2){
	DMOSP_OO *oo1 = (DMOSP_OO *)oond1->Data();
	DMOSP_OO *oo2 = (DMOSP_OO *)oond2->Data();
	if(oo1->OO.Count() != oo2->OO.Count() || oo1->job != oo2->job) return true;
	SList::SNode *opnd1, *opnd2;
	opnd1 = oo1->OO.head();
	opnd2 = oo2->OO.head();
	for(int j=0; j<oo1->OO.Count(); j++){
	  if(opnd1->Data() != opnd2->Data()) return true;
	  opnd1 = opnd1->Next();
	  opnd2 = opnd2->Next();
	}
	oond1 = oond1->Next();
	oond2 = oond2->Next();
  }
  return false;
}

bool DMOSP_VOO::Copy(DMOSP_VOO *voo)
{
  if(voo->Problem != Problem) return false;
  SList::SNode *oond = OOs.head(), *oondg = voo->OOs.head();
  while(oond){
	DMOSP_OO *oo = (DMOSP_OO *)oond->Data();
	DMOSP_OO *oog = (DMOSP_OO *)oondg->Data();
	oo->OO.Clear();
	oo->job = oog->job;
	oo->OO.Add(oog->OO);
	oond = oond->Next();
	oondg = oondg->Next();
  }
  return true;
}

DMOSP_VOO& DMOSP_VOO::operator =(DMOSP_VOO &voo)
{
  if(voo.Problem != Problem) return *this;
  SList::SNode *oond = OOs.head(), *oondg = voo.OOs.head();
  while(oond){
	DMOSP_OO *oo = (DMOSP_OO *)oond->Data();
	DMOSP_OO *oog = (DMOSP_OO *)oondg->Data();
	oo->OO.Clear();
	oo->job = oog->job;
	oo->OO.Add(oog->OO);
	oond = oond->Next();
	oondg = oondg->Next();
  }
  return *this;
}

bool DMOSP_VOO::isComplete()
{
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	SList::SNode *opnd = jb->Operations.head();
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  if(!isIncluded(op)) return false;
	  opnd = opnd->Next();
	}
	jnd = jnd->Next();
  }
  return true;
}

//--------------- A Simple processing sequence and job order interpretter

DMOSP_PSI::DMOSP_PSI(DMOSP_Problem *P, DMOSP_VPS *vps, DMOSP_VOO *voo, wchar_t *N)
  : DMOSP_Algorithm(P, N), VPS(vps), VOO(voo)
{
}

DMOSP_PSI::DMOSP_PSI(DMOSP_Problem *P, DMOSP_VPS *vps, DMOSP_VOO *voo)
  : DMOSP_Algorithm(P, L"Simple Processing Sequnce Interpretter (PSI)"),
	VPS(vps), VOO(voo)
{
}

DMOSP_PSI::~DMOSP_PSI()
{
}

void DMOSP_PSI::AllocateMemory()
{
  DMOSP_Algorithm::AllocateMemory();
  tmpVPS = new DMOSP_VPS(&VPS);
  tmpVOO = new DMOSP_VOO(&VOO);
  nxt = new int[Problem->Jobs.Count()];
  mcrt = new double[Problem->nMachines()];
  jbrt = new double[Problem->Jobs.Count()];
}

void DMOSP_PSI::Initialize()
{
  SList::SNode *nd, *wcnd, *mcnd, *jbnd, *opnd;
  int i, j, m;

  for(m=0; m<nMCs; m++){
	mcrt[m] = M[m]->ReadyTime;
  }
  for(j=0; j<nJobs; j++){
	jbrt[j] = J[j]->ReleaseTime;
	opnd = tmpVOO->OO(J[j])->OO.head();
	if(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  nxt[j] = op->i;   // index of the next schedulable operation based on OOs
	}
	else nxt[j] = -1;
  }

  bool bContinue;
  do{
	bContinue = false;
	nd = tmpVPS->PSs.head();
	while(nd){
	  DMOSP_PS *ps = (DMOSP_PS *)nd->Data();
	  if(ps->PS.Count() != 0){
		DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)(ps->PS[0]);
		j = op->Job->i;  // job index
		i = op->i; // operation index
		m = ps->MC->i;  // machine index
		if(nxt[j] == i){
		  DMOSP_Solution::SchTask *task = tsk[i];
		  task->SelectedMC = ps->MC;
		  task->StartTime = max(jbrt[j], mcrt[m]);
		  task->EndTime = task->StartTime + pt[j][m];
		  task->bScheduled = true;
		  jbrt[j] = task->EndTime;
		  mcrt[m] = task->EndTime;

		  DMOSP_OO *joo = tmpVOO->OO(J[j]);
		  joo->OO.Remove(joo->OO.head());
		  opnd = joo->OO.head();
		  if(opnd){
			DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
			nxt[j] = op->i;   // index of the next schedulable operation based on OOs
		  }
		  else nxt[j] = -1;

		  ps->PS.Remove(ps->PS.head());
		  bContinue = true;
		  break;
		}
	  }
	  nd = nd->Next();
	}
  }while(bContinue);

  bool bComplete = true;          // check for infeasible processing sequence
  nd = tmpVPS->PSs.head();
  while(nd){
	DMOSP_PS *ps = (DMOSP_PS *)nd->Data();
	if(ps->PS.Count() != 0){
	  bComplete = false;
	  break;
	}
	nd = nd->Next();
  }
  if(!bComplete) SolStatus = INFEASIBLE;
  else SolStatus = FEASIBLE;
}

bool DMOSP_PSI::Stop()
{
  return true;
}

void DMOSP_PSI::Finalize()
{
  DMOSP_Algorithm::Finalize();
}

void DMOSP_PSI::FreeMemory()
{
  DMOSP_Algorithm::FreeMemory();

  delete [] nxt;
  delete [] mcrt;
  delete [] jbrt;
  delete tmpVPS;
  delete tmpVOO;
}

void DMOSP_PSI::ComputeObjectiveValue()
{
  ObjectiveValue = Solution->ComputeObjectiveValue(Problem->Objective);
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
