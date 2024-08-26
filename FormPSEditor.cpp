//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormPSEditor.h"
#include "SList.h"
#include "Problem.h"
#include "FormSol.h"
#include "Main.h"
#include <io.h>
#include "FileSystem.h"
#include "FormMILPReader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmPSEditor *frmPSEditor;
//---------------------------------------------------------------------------
__fastcall TfrmPSEditor::TfrmPSEditor(TComponent* Owner)
		: TfrmAlgorithm(Owner)
{
  NSTester = NULL;
  moss = NULL;
  PSI = NULL;
  DSG = NULL;
  VPS = NULL;
  VOO = NULL;
  LoadedSol = NULL;

  Vec = NULL;
  Vec_y = NULL;
  Vec_rho = NULL;
  Net = NULL;
  Net_y = NULL;
  Net_rho = NULL;

  MILPSol = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPSEditor::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);

  btnInitialize->Enabled = true;
  btnInsert->Enabled = false;
  btnRemoveOp->Enabled = false;
  cbxOps->Enabled = false;
  rgRemoveFrom->Enabled = false;
  cbxMachines->Enabled = false;
  edtInPos->Text = "";
  edtInPos->Enabled = false;

  frmSolution->btnLoadSol->Visible = false;
  frmSolution->btnSaveSol->Visible = false;

  cbxJSR->ItemIndex = 8;
  cbxMSR->ItemIndex = 2;

  grdPS->RowCount = Problem->nMachines()+1;
  grdPS->ColCount = 2;

  grdOO->RowCount = Problem->Jobs.Count()+1;
  grdOO->ColCount = 2;

  cbxFromMC->Enabled = true;
  cbxFromMC->Clear();

  int maxops = 1;
  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	DMOSP_Problem::SWC *pc = (DMOSP_Problem::SWC *)pcnd->Data();
	if(maxops < Problem->nOperationsByWC(pc)) maxops = Problem->nOperationsByWC(pc);
	for(SList::SNode *mcnd = pc->MCs.head(); mcnd; mcnd = mcnd->Next()){
	  DMOSP_Problem::SMC *mc = (DMOSP_Problem::SMC *)mcnd->Data();
	  cbxFromMC->Items->Add(mc->ID());
	}
	pcnd = pcnd->Next();
  }
  grdPS->ColCount = maxops + 1;

  edtMCx1->Enabled = true;
  edtMCx2->Enabled = true;
  btnMCRemove->Enabled = true;
  edtMCx1->Text = "";
  edtMCx2->Text = "";
  edtMCInsertX->Text = "";
  cbxToMC->Enabled = false;
  edtMCInsertX->Enabled = false;
  btnMCInsert->Enabled = false;

  cbxJobs->Enabled = true;
  cbxJobs->Clear();

  maxops = 1;
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	cbxJobs->Items->Add(jb->ID());
	if(maxops < jb->Operations.Count()) maxops = jb->Operations.Count();
	jnd = jnd->Next();
  }
  grdOO->ColCount = maxops + 1;

  edtJobx1->Enabled = true;
  edtJobx2->Enabled = true;
  btnJobRemove->Enabled = true;
  edtJobx1->Text = "";
  edtJobx2->Text = "";
  edtJobInsertX->Text = "";
  edtJobInsertX->Enabled = false;
  btnJobInsertX->Enabled = false;

  GenerateDSSol();

  OriginalSol = new DMOSP_Solution(Problem);

  MILPSol = new DMOSP_Solution(Problem);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::GenerateDSSol()
{
  if(DSG) delete DSG;
  DMOSP_DSG::T_JobDispatchingRule jdr;
  DMOSP_DSG::T_MCSelectionRule msr;

  jdr = (DMOSP_DSG::T_JobDispatchingRule)cbxJSR->ItemIndex;
  msr = (DMOSP_DSG::T_MCSelectionRule)cbxMSR->ItemIndex;

  DSG = new DMOSP_DSG(Problem, jdr, msr);
  DSG->Run();
  UpdateVPS(DSG->Solution);
  UpdateVOO(DSG->Solution);
  frmSolution->SetCurrentSolution(DSG->Solution);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnRunDSGClick(TObject *)
{
  GenerateDSSol();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
  TfrmAlgorithm::FormClose(Sender, Action);

  if(PSI) delete PSI;   PSI = NULL;
  if(NSTester){ NSTester->FreeMemory(); delete NSTester; }    NSTester = NULL;
  if(moss){ moss->FreeNSTestMemory(); delete moss; }  moss = NULL;
  if(DSG) delete DSG;   DSG = NULL;
  if(VPS) delete VPS;   VPS = NULL;
  if(VOO) delete VOO;   VOO = NULL;
  if(LoadedSol) delete LoadedSol;       LoadedSol = NULL;

  if(Vec) delete Vec;   Vec = NULL;
  if(Vec_y) delete Vec_y; Vec_y = NULL;
  if(Vec_rho) delete Vec_rho; Vec_rho = NULL;
  if(Net) delete Net;   Net = NULL;
  if(Net_y) delete Net_y;   Net_y = NULL;
  if(Net_rho) delete Net_rho;  Net_rho = NULL;

  delete OriginalSol;

  delete MILPSol;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnRunClick(TObject *)
{
  if(ReadVPS() && ReadVOO()){
	if(InterpretVectors()){
	  DisplayVPS(VPS);
	  DisplayVOO(VOO);
	  frmSolution->SetCurrentSolution(PSI->Solution);
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnSaveClick(TObject *)
{
  frmSolution->btnSaveSolClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnLoadClick(TObject *)
{
  frmSolution->btnLoadSolClick(this);
  UpdateVPS(frmSolution->CurrentSol);
  UpdateVOO(frmSolution->CurrentSol);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::DisplayVPS(DMOSP_VPS *vps)
{
  int r=1, c;

  for(c=1; c<=grdPS->ColCount; c++) grdPS->Cells[c][0] = IntToStr(c);

  SList::SNode *psnd = vps->PSs.tail();
  while(psnd){
	DMOSP_PS *ps = (DMOSP_PS *)psnd->Data();
	SList::SNode *opnd = ps->PS.head();
	grdPS->Cells[0][r] = ps->MC->ID();
	c = 1;
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  grdPS->Cells[c][r] = op->ID();
	  opnd = opnd->Next();
	  c++;
	}
	while(c < grdPS->ColCount){
	  grdPS->Cells[c][r] = "";
	  c++;
	}
	psnd = psnd->Prev();
	r++;
  }
  grdPS->Row = r-1;
  grdPS->Col = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::UpdateVPS(DMOSP_Solution *Sol)
{
  if(VPS) delete VPS;
  VPS = new DMOSP_VPS(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VPS->AppendOp(tsk->SelectedMC, tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVPS(VPS);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmPSEditor::ReadVPS()
{
  String s;
  int r, c;

  DMOSP_VPS *vps = new DMOSP_VPS(Problem);

  for(r=1; r<grdPS->RowCount; r++){
	s = grdPS->Cells[0][r];
	DMOSP_Problem::SMC *mc = Problem->MCbyID(s.c_str());
	if(!mc){
	  String msg = "Unknown machine " + s;
	  Application->MessageBox(msg.c_str(), L"Error reading machine", MB_OK);
	  delete vps;
	  return false;
	}

	for(c=1; c<grdPS->ColCount; c++){
	  s = grdPS->Cells[c][r];
	  if(s == "") break;
	  DMOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(!op){
		String msg = "Unknown operation "+s+" in machines processing sequences";
		Application->MessageBox(msg.c_str(), L"Error reading operation", MB_OK);
		delete vps;
		return false;
	  }
	  vps->AppendOp(mc, op);
	}
  }

  if(!vps->isComplete()){
	Application->MessageBox(L"Incomplete vector of processing sequences",
							L"Invalid processing sequences",
							MB_OK);
	delete vps;
	return false;
  }

  if(VPS) delete VPS;
  VPS = vps;
  return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::DisplayVOO(DMOSP_VOO *voo)
{
  int r=1, c;

  for(c=1; c<=grdOO->ColCount; c++) grdOO->Cells[c][0] = IntToStr(c);

  SList::SNode *oond = voo->OOs.head();
  while(oond){
	DMOSP_OO *oo = (DMOSP_OO *)oond->Data();
	SList::SNode *opnd = oo->OO.head();
	grdOO->Cells[0][r] = oo->job->ID();
	c = 1;
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  grdOO->Cells[c][r] = op->ID();
	  opnd = opnd->Next();
	  c++;
	}
	while(c < grdOO->ColCount){
	  grdOO->Cells[c][r] = "";
	  c++;
	}
	oond = oond->Next();
	r++;
  }
  grdOO->Row = 1;
  grdOO->Col = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::UpdateVOO(DMOSP_Solution *Sol)
{
  if(VOO) delete VOO;
  VOO = new DMOSP_VOO(Problem);

  // arrange scheduled tasks in an increasing order of their start times
  Sol->ScheduledTasks.QSort(tskComp);

  SList::SNode *tsknd = Sol->ScheduledTasks.head();
  while(tsknd){
	DMOSP_Solution::SchTask *tsk = (DMOSP_Solution::SchTask *)tsknd->Data();
	if(tsk->SelectedMC != NULL) VOO->AppendOp(tsk->Operation);
	tsknd = tsknd->Next();
  }

  DisplayVOO(VOO);
}
//---------------------------------------------------------------------------

bool __fastcall TfrmPSEditor::ReadVOO()
{
  String s;
  int r, c;

  DMOSP_VOO *voo = new DMOSP_VOO(Problem);

  for(r=1; r<grdOO->RowCount; r++){
	s = grdOO->Cells[0][r];
	DMOSP_Problem::SJob *jb = Problem->JObyID(s.c_str());
	if(!jb){
	  String msg = "Unknown job "+s;
	  Application->MessageBox(msg.c_str(), L"Error reading job", MB_OK);
	  delete voo;
	  return false;
	}

	for(c=1; c<grdOO->ColCount; c++){
	  s = grdOO->Cells[c][r];
      if(s == "") break;
	  DMOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(!op){
		String msg = "Unknown operation "+s+" in operation\'s job order";
		Application->MessageBox(msg.c_str(), L"Error reading operation", MB_OK);
		delete voo;
		return false;
	  }
	  voo->AppendOp(op);
	}
  }

  if(!voo->isComplete()){
	Application->MessageBox(L"Incomplete operations\' job orders",
							L"Invalid job orders",
							MB_OK);
	delete voo;
	return false;
  }

  if(VOO) delete VOO;
  VOO = voo;
  return true;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmPSEditor::InterpretVectors()
{
  if(PSI) delete PSI;
  PSI = new DMOSP_PSI(Problem, VPS, VOO);
  PSI->Run();
  if(PSI->SolStatus == DMOSP_Algorithm::INFEASIBLE){
	Application->MessageBox(L"The represented processing sequences and operations\' orders results in "\
							"an infeasible solution",
							L"Infeasible solution!!",
							MB_OK);
	return false;
  }
  return true;
}

//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnRemoveOpClick(TObject *)
{
  if(cbxOps->ItemIndex == -1) return;
  String opID;
  opID = cbxOps->Items->operator [](cbxOps->ItemIndex);
  v_y = Problem->OpByID(opID.c_str());
  if(v_y){
	if(Vec_y) delete Vec_y;
	Vec_y = new DMOSP_VectorSol(NSTester);
	Vec_y->Copy(Vec);

	if(Net_y) delete Net_y;
	Net_y = new DMOSP_Network(NSTester);

	cbxOps->Enabled = false;
	btnRemoveOp->Enabled = false;
	rgRemoveFrom->Enabled = false;
	btnInsert->Enabled = true;
	edtInPos->Enabled = true;
	x = (rgRemoveFrom->ItemIndex == 0)? Vec->TauPosition(v_y) : Vec->PiPosition(v_y);
	edtInPos->Text = IntToStr(x);

	if(rgRemoveFrom->ItemIndex == 1){
	  cbxMachines->Enabled = true;
	  from_mc = Vec->MCofOp(v_y);
	  int curMC = 0, mcindx;
	  SList::SNode *mcnd = from_mc->WC->MCs.head();
	  while(mcnd){
		DMOSP_Problem::SMC *curmc = (DMOSP_Problem::SMC *)mcnd->Data();
		cbxMachines->Items->Add(curmc->ID());
		if(curmc == from_mc) mcindx = curMC;
		curMC++;
		mcnd = mcnd->Next();
	  }
	  cbxMachines->ItemIndex = mcindx;

	  // remove the operation from pi
	  Vec_y->RemovePiOperation(v_y);
	}
	else{
	  // remove the operation from tau
	  Vec_y->RemoveTauOperation(v_y);
	}

	Net_y->InterpretVectors(Vec_y, false);

	if(Net_y->ConvertIncompleteNetToTasks()){
	  frmSolution->SetCurrentSolution(NSTester->Solution, Net_y);
	  DisplayVector(Vec_y);
	  if(rgRemoveFrom->ItemIndex == 1){
		DisplayN2Tables();
		DisplayN4Tables();
	  }
	  else{
		DisplayN1Tables();
		DisplayN3Tables();
	  }
	}
	else{
	  Application->MessageBox(L"Infeasible solution is encountered",
							  L"Infeasible solution!!",
							  MB_OK);
	}
  }
  else{
	  Application->MessageBox(L"Unknown operation",
							L"Please select an operation from the drop down box",
							MB_OK);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnInitializeClick(TObject *)
{
  cbxJSR->Enabled = false;
  cbxMSR->Enabled = false;
  btnRunDSG->Enabled = false;
  btnLoad->Enabled = false;
  btnSave->Enabled = false;
  btnRun->Enabled = false;
  tbMCPermutation->Enabled = false;
  tbJobPermutation->Enabled = false;

  btnInitialize->Enabled = false;
  btnRemoveOp->Enabled = true;
  rgRemoveFrom->Enabled = true;

  ClearNTables();

  // include all operations in the operations' drop down box
  cbxOps->Enabled = true;
  pgAlgPars->TabIndex = 0;

  Ops.Clear();
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	Ops.Add(jb->Operations);
	jnd = jnd->Next();
  }
  cbxOps->Clear();
  SList::SNode *ond = Ops.head();
  while(ond){
	DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)ond->Data();
	cbxOps->AddItem(op->ID(), NULL);
	ond = ond->Next();
  }

  cbxMachines->Clear();
  edtInPos->Text = "";

  if(NSTester){ NSTester->FreeMemory(); delete NSTester; }
  NSTester = new DMOSP_NSTester(Problem, frmSolution->CurrentSol);
  NSTester->AllocateMemory();
  NSTester->Initialize();

  if(moss){  moss->FreeNSTestMemory();   delete moss; }
  moss = new DMOSP_MOSS(Problem, frmSolution->CurrentSol);
  moss->AllocateMemory();
  moss->Initialize4NSTests();

  if(Vec) delete Vec;
  Vec = new DMOSP_VectorSol(NSTester);
  Vec->ConvertTasksToVect();

  if(Net) delete Net;
  Net = new DMOSP_Network(NSTester);
  Net->InterpretVectors(Vec, false);

  bool bF, bC;
  Net->InterpretNetwork(bF, bC);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnInsertClick(TObject *)
{
  try{
	x = StrToInt(edtInPos->Text);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate integer values for the insertion position",
							  L"Error reading numbers",
							  MB_OK);
	  return;
	}

  if(x < 1){
	Application->MessageBox(L"Insertion position must be greater than 0",
							L"Error in selecting positions",
							MB_OK);
	return;
  }

  DMOSP_VectorSol *tmpVec = NULL;

  // Conduct the requested move
  if(rgRemoveFrom->ItemIndex == 1){   // Machine movement
	String MCID;
	MCID = cbxMachines->Items->operator [](cbxMachines->ItemIndex);
	to_mc = Problem->MCbyID(MCID.c_str());
	if(!to_mc){
	  Application->MessageBox(L"Unknown machine",
							  L"Please select a machine from the drop down box",
							  MB_OK);
	  return;
	}

	DMOSP_PS *mcps;
	SList::SNode *psnd = VPS->PSs.head();
	while(psnd){
	  DMOSP_PS *cur = (DMOSP_PS *)psnd->Data();
	  if(cur->MC == to_mc){
		mcps = cur;
		break;
	  }
	  psnd = psnd->Next();
	}

	int psl = mcps->PS.Count();
	if((from_mc == to_mc && x > psl) || (from_mc != to_mc && x > psl+1) || x < 1){
	  Application->MessageBox(L"Insertion position is outside allowed range",
							  L"Error in selecting positions",
							  MB_OK);
	  return;
	}

	// Now do the insertion and check the feasibility of the new solution
	//
    tmpVec = new DMOSP_VectorSol(Vec_y);
	Vec_y->UnremoveAll();
	Vec_y->MoveOpToPiPos(v_y, to_mc->i, x, false);

	if(Net_y) delete Net_y;
	Net_y = new DMOSP_Network(NSTester);
	Net_y->InterpretVectors(Vec_y, false);
  }
  else{ // job movement
	DMOSP_OO *jboo;
	SList::SNode *oond = VOO->OOs.head();
	while(oond){
	  DMOSP_OO *cur = (DMOSP_OO *)oond->Data();
	  if(cur->job == v_y->Job){
		jboo = cur;
		break;
	  }
	  oond = oond->Next();
	}
	int ool = jboo->OO.Count();
	if(x < 1 || x > ool){
	  Application->MessageBox(L"Insertion position is outside allowed range",
							  L"Error in selecting positions",
							  MB_OK);
	  return;
	}

	// Now do the insertion and check the feasibility of the new solution
	//
    tmpVec = new DMOSP_VectorSol(Vec_y);
	Vec_y->UnremoveAll();
	Vec_y->MoveOpToTauPos(v_y, x, false);

	if(Net_y) delete Net_y;
	Net_y = new DMOSP_Network(NSTester);
	Net_y->InterpretVectors(Vec_y, false);
  }

  if(Net_y->ConvertNetToTasks()){
	UpdateVPS(NSTester->Solution);
	UpdateVOO(NSTester->Solution);
	InterpretVectors();  // to use PSI->Solution instead of NSTester->Solution
	frmSolution->SetCurrentSolution(PSI->Solution);
  }
  else{
	Application->MessageBox(L"Infeasible solution is encountered",
							L"Infeasible solution!!",
							MB_OK);
	Vec_y->Copy(tmpVec);
	delete tmpVec;
	return;
  }

  if(Vec) delete Vec;      Vec = NULL;
  if(Net) delete Net;      Net = NULL;
  if(Vec_y) delete Vec_y;  Vec_y = NULL;
  if(Net_y) delete Net_y;  Net_y = NULL;
  if(tmpVec) delete tmpVec;

  cbxJSR->Enabled = true;
  cbxMSR->Enabled = true;
  btnRunDSG->Enabled = true;
  btnLoad->Enabled = true;
  btnSave->Enabled = true;
  btnRun->Enabled = true;
  tbMCPermutation->Enabled = true;
  tbJobPermutation->Enabled = true;

  btnInitialize->Enabled = true;
  btnRemoveOp->Enabled = false;
  btnInsert->Enabled = false;

  cbxOps->Clear();
  cbxOps->Enabled = false;
  cbxMachines->Clear();
  cbxMachines->Enabled = false;
  edtInPos->Text = "";
  edtInPos->Enabled = false;

  ClearNTables();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPSEditor::btnCloseClick(TObject *)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnMCRemoveClick(TObject *)
{
  // First read the selected machine and positions to check their validity
  String MCID;
  MCID = cbxFromMC->Items->operator [](cbxFromMC->ItemIndex);
  from_mc = Problem->MCbyID(MCID.c_str());
  if(!from_mc){
	Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
	return;
  }

  try{
	x1 = StrToInt(edtMCx1->Text);
	x2 = StrToInt(edtMCx2->Text);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate integer values for the stard and"\
							  " end positions",
							  L"Error reading numbers",
							  MB_OK);
	  return;
	}

  if(x1 > x2 || x1 < 1 || x2 < 1){
	Application->MessageBox(L"Start position must be less than end position"\
							 " and they both must be greater than 0",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  DMOSP_PS *mcps;
  SList::SNode *psnd = VPS->PSs.head();
  while(psnd){
	DMOSP_PS *cur = (DMOSP_PS *)psnd->Data();
	if(cur->MC == from_mc){
	  mcps = cur;
	  break;
	}
	psnd = psnd->Next();
  }

  int psl = mcps->PS.Count();
  if(x1 > psl || x2 > psl){
	Application->MessageBox(L"start/end position is out of range",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  cbxJSR->Enabled = false;
  cbxMSR->Enabled = false;
  btnRunDSG->Enabled = false;
  btnLoad->Enabled = false;
  btnSave->Enabled = false;
  btnRun->Enabled = false;
  tbOpRemover->Enabled = false;
  tbJobPermutation->Enabled = false;

  cbxFromMC->Enabled = false;
  edtMCx1->Enabled = false;
  edtMCx2->Enabled = false;
  btnMCRemove->Enabled = false;
  cbxToMC->Enabled = true;
  edtMCInsertX->Enabled = true;
  btnMCInsert->Enabled = true;

  pgAlgPars->TabIndex = 1;

  cbxToMC->Clear();
  SList::SNode *mcnd = from_mc->WC->MCs.head();
  while(mcnd){
	DMOSP_Problem::SMC *curmc = (DMOSP_Problem::SMC *)mcnd->Data();
	cbxToMC->Items->Add(curmc->ID());
	mcnd = mcnd->Next();
  }

  grdNbSets->RowCount = 2;
  for(int i=0; i<grdNbSets->ColCount; i++)
	grdNbSets->Cells[i][1] = "";
  edtSetS->Text = "";
  edtSetI->Text = "";

  // Now apply the removal operation
  OriginalSol->Copy(frmSolution->CurrentSol);
  if(NSTester){ NSTester->FreeMemory(); delete NSTester; }
  NSTester = new DMOSP_NSTester(Problem, frmSolution->CurrentSol);
  NSTester->AllocateMemory();
  NSTester->Initialize();

  if(Vec) delete Vec;
  Vec = new DMOSP_VectorSol(NSTester);
  Vec->ConvertTasksToVect();
  if(Vec_rho) delete Vec_rho;
  Vec_rho = new DMOSP_VectorSol(Vec);
  Vec_rho->RemovePiSubpermutation(from_mc->i, x1, x2);

  if(Net) delete Net;
  Net = new DMOSP_Network(NSTester);
  Net->InterpretVectors(Vec, false);
  bool bF, bC;
  Net->InterpretNetwork(bF, bC);

  if(Net_rho) delete Net_rho;
  Net_rho = new DMOSP_Network(NSTester);
  Net_rho->InterpretVectors(Vec_rho, false);

  if(Net_rho->ConvertIncompleteNetToTasks()){
	frmSolution->SetCurrentSolution(NSTester->Solution, Net_rho);
	DisplayVector(Vec_rho);
  }
  else{
	Application->MessageBox(L"Infeasible solution is encountered",
							L"Infeasible solution!!",
							MB_OK);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnMCInsertClick(TObject *)
{
  String MCID;
  MCID = cbxToMC->Items->operator [](cbxToMC->ItemIndex);
  to_mc = Problem->MCbyID(MCID.c_str());
  if(!to_mc){
	Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
	return;
  }

  try{
	x = StrToInt(edtMCInsertX->Text);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate integer values for the insertion position",
							  L"Error reading numbers",
							  MB_OK);
	  return;
	}

  if(x < 1){
	Application->MessageBox(L"Insertion position must be greater than 0",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  DMOSP_PS *mcps;
  SList::SNode *psnd = VPS->PSs.head();
  while(psnd){
	DMOSP_PS *cur = (DMOSP_PS *)psnd->Data();
	if(cur->MC == to_mc){
	  mcps = cur;
	  break;
	}
	psnd = psnd->Next();
  }

  int psl = mcps->PS.Count();
  if((from_mc == to_mc && x > psl - (x2-x1)) || (from_mc != to_mc && x > psl+1)){
	Application->MessageBox(L"Insertion position is outside allowed range",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  // Now do the insertion and check the feasibility of the new solution
  //
  Vec->UnremoveAll();
  Vec->MovePiSubpermutation(from_mc->i, x1, x2, to_mc->i, x, false);

  if(Net) delete Net;
  Net = new DMOSP_Network(NSTester);
  Net->InterpretVectors(Vec, false);

  if(Net->ConvertNetToTasks()){
	UpdateVPS(NSTester->Solution);
	UpdateVOO(NSTester->Solution);
	InterpretVectors();  // to use PSI->Solution instead of NSTester->Solution
	frmSolution->SetCurrentSolution(PSI->Solution);
  }
  else{
	Application->MessageBox(L"Infeasible solution is encountered",
							L"Infeasible solution!!",
							MB_OK);
	UpdateVPS(OriginalSol);
	UpdateVOO(OriginalSol);
	InterpretVectors();  // to use PSI->Solution instead of NSTester->Solution
	frmSolution->SetCurrentSolution(PSI->Solution);
  }

  cbxJSR->Enabled = true;
  cbxMSR->Enabled = true;
  btnRunDSG->Enabled = true;
  btnLoad->Enabled = true;
  btnSave->Enabled = true;
  btnRun->Enabled = true;
  tbOpRemover->Enabled = true;
  tbJobPermutation->Enabled = true;

  cbxFromMC->Enabled = true;
  edtMCx1->Enabled = true;
  edtMCx2->Enabled = true;
  btnMCRemove->Enabled = true;
  cbxToMC->Enabled = false;
  edtMCInsertX->Enabled = false;
  btnMCInsert->Enabled = false;

  grdNbSets->RowCount = 2;
  for(int i=0; i<grdNbSets->ColCount; i++)
	grdNbSets->Cells[i][1] = "";
  edtSetS->Text = "";
  edtSetI->Text = "";

  for(int i=0; i<sgB1AlphaOmega->RowCount; i++){
	sgB1AlphaOmega->Cells[0][i] = "";
	sgB1AlphaOmega->Cells[1][i] = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnJobRemoveClick(TObject *)
{
  // First read the selected job and positions to check their validity
  String JobID;
  JobID = cbxJobs->Items->operator [](cbxJobs->ItemIndex);
  fromto_job = Problem->JObyID(JobID.c_str());
  if(!fromto_job){
	Application->MessageBox(L"Unknown job",
							L"Please select a job from the drop down box",
							MB_OK);
	return;
  }

  try{
	x1 = StrToInt(edtJobx1->Text);
	x2 = StrToInt(edtJobx2->Text);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate integer values for the stard and"\
							  " end positions",
							  L"Error reading numbers",
							  MB_OK);
	  return;
	}

  if(x1 > x2 || x1 < 1 || x2 < 1){
	Application->MessageBox(L"Start position must be less than end position"\
							 " and they both must be greater than 0",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  DMOSP_OO *jboo;
  SList::SNode *oond = VOO->OOs.head();
  while(oond){
	DMOSP_OO *cur = (DMOSP_OO *)oond->Data();
	if(cur->job == fromto_job){
	  jboo = cur;
	  break;
	}
	oond = oond->Next();
  }

  int ool = jboo->OO.Count();
  if(x1 > ool || x2 > ool){
	Application->MessageBox(L"start/end position is out of range",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  cbxJSR->Enabled = false;
  cbxMSR->Enabled = false;
  btnRunDSG->Enabled = false;
  btnLoad->Enabled = false;
  btnSave->Enabled = false;
  btnRun->Enabled = false;
  tbOpRemover->Enabled = false;
  tbMCPermutation->Enabled = false;

  cbxJobs->Enabled = false;
  edtJobx1->Enabled = false;
  edtJobx2->Enabled = false;
  btnJobRemove->Enabled = false;
  edtJobInsertX->Enabled = true;
  btnJobInsertX->Enabled = true;

  pgAlgPars->TabIndex = 1;

  // Now apply the removal operation
  OriginalSol->Copy(frmSolution->CurrentSol);
  if(NSTester){ NSTester->FreeMemory(); delete NSTester; }
  NSTester = new DMOSP_NSTester(Problem, frmSolution->CurrentSol);
  NSTester->AllocateMemory();
  NSTester->Initialize();

  if(Vec) delete Vec;
  Vec = new DMOSP_VectorSol(NSTester);
  Vec->ConvertTasksToVect();
  if(Vec_rho) delete Vec_rho;
  Vec_rho = new DMOSP_VectorSol(Vec);
  Vec_rho->RemoveTauSubpermutation(fromto_job->i, x1, x2);

  if(Net) delete Net;
  Net = new DMOSP_Network(NSTester);
  Net->InterpretVectors(Vec, false);
  bool bF, bC;
  Net->InterpretNetwork(bF, bC);

  if(Net_rho) delete Net_rho;
  Net_rho = new DMOSP_Network(NSTester);
  Net_rho->InterpretVectors(Vec_rho, false);

  if(Net_rho->ConvertIncompleteNetToTasks()){
	frmSolution->SetCurrentSolution(NSTester->Solution, Net_rho);
	DisplayVector(Vec_rho);

	int alpha, omega, apr_alpha, apr_omega;
	Net->alpha_omega_B1(fromto_job->i, x1, x2, alpha, omega, apr_alpha, apr_omega);
	sgB1AlphaOmega->Cells[0][0] = "alpha";
	sgB1AlphaOmega->Cells[0][1] = "~alpha";
	sgB1AlphaOmega->Cells[0][2] = "omega";
	sgB1AlphaOmega->Cells[0][3] = "~omega";
	sgB1AlphaOmega->Cells[1][0] = IntToStr(alpha);
	sgB1AlphaOmega->Cells[1][1] = IntToStr(apr_alpha);
	sgB1AlphaOmega->Cells[1][2] = IntToStr(omega);
	sgB1AlphaOmega->Cells[1][3] = IntToStr(apr_omega);
  }
  else{
	Application->MessageBox(L"Infeasible solution is encountered",
							L"Infeasible solution!!",
							MB_OK);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnJobInsertXClick(TObject *)
{
  try{
	x = StrToInt(edtJobInsertX->Text);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate integer values for the insertion position",
							  L"Error reading numbers",
							  MB_OK);
	  return;
	}

  if(x < 1){
	Application->MessageBox(L"Insertion position must be greater than 0",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  DMOSP_OO *jboo;
  SList::SNode *oond = VOO->OOs.head();
  while(oond){
	DMOSP_OO *cur = (DMOSP_OO *)oond->Data();
	if(cur->job == fromto_job){
	  jboo = cur;
	  break;
	}
	oond = oond->Next();
  }

  int ool = jboo->OO.Count();
  if(x > ool - (x2 - x1)){
	Application->MessageBox(L"Insertion position is not in range",
							  L"Error in selecting positions",
							  MB_OK);
	return;
  }

  // Now do the insertion and check the feasibility of the new solution
  //
  Vec->MoveTauSubpermutation(fromto_job->i, x1, x2, x, false);

  if(Net) delete Net;
  Net = new DMOSP_Network(NSTester);
  Net->InterpretVectors(Vec, false);

  if(Net->ConvertNetToTasks()){
	UpdateVPS(NSTester->Solution);
	UpdateVOO(NSTester->Solution);
	InterpretVectors();  // to use PSI->Solution instead of NSTester->Solution
	frmSolution->SetCurrentSolution(PSI->Solution);
  }
  else{
	Application->MessageBox(L"Infeasible solution is encountered",
							L"Infeasible solution!!",
							MB_OK);
	UpdateVPS(OriginalSol);
	UpdateVOO(OriginalSol);
	InterpretVectors();  // to use PSI->Solution instead of NSTester->Solution
	frmSolution->SetCurrentSolution(PSI->Solution);
  }

  cbxJSR->Enabled = true;
  cbxMSR->Enabled = true;
  btnRunDSG->Enabled = true;
  btnLoad->Enabled = true;
  btnSave->Enabled = true;
  btnRun->Enabled = true;
  tbOpRemover->Enabled = true;
  tbMCPermutation->Enabled = true;

  cbxJobs->Enabled = true;
  edtJobx1->Enabled = true;
  edtJobx2->Enabled = true;
  btnJobRemove->Enabled = true;
  edtJobInsertX->Enabled = false;
  btnJobInsertX->Enabled = false;

  grdNbSets->RowCount = 2;
  for(int i=0; i<grdNbSets->ColCount; i++)
	grdNbSets->Cells[i][1] = "";
  edtSetS->Text = "";
  edtSetI->Text = "";

  for(int i=0; i<sgB1AlphaOmega->RowCount; i++){
	sgB1AlphaOmega->Cells[0][i] = "";
	sgB1AlphaOmega->Cells[1][i] = "";
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::DisplayVector(DMOSP_VectorSol *V)
{
  int r=1, c, m, j;

  for(c=1; c<=grdPS->ColCount; c++) grdPS->Cells[c][0] = IntToStr(c);

  for(m=NSTester->nMCs-1; m>=0; m--){
	SList::SNode *opnd = V->headOfPi(m);
	grdPS->Cells[0][r] = NSTester->M[m]->ID();
	c = 1;
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  if(V->u[op->i] & 2){
		grdPS->Cells[c][r] = op->ID();
		c++;
	  }
	  opnd = opnd->Next();
	}
	while(c < grdPS->ColCount){
	  grdPS->Cells[c][r] = "";
	  c++;
	}
	r++;
  }
  grdPS->Row = r-1;
  grdPS->Col = 1;

  r=1;

  for(c=1; c<=grdOO->ColCount; c++) grdOO->Cells[c][0] = IntToStr(c);

  for(j=0; j<NSTester->nJobs; j++){
	SList::SNode *opnd = V->headOfTau(j);
	grdOO->Cells[0][r] = NSTester->J[j]->ID();
	c = 1;
	while(opnd){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)opnd->Data();
	  if(V->u[op->i] & 1){
		grdOO->Cells[c][r] = op->ID();
		c++;
	  }
	  opnd = opnd->Next();
	}
	while(c < grdOO->ColCount){
	  grdOO->Cells[c][r] = "";
	  c++;
	}
	r++;
  }
  grdOO->Row = 1;
  grdOO->Col = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::cbxToMCChange(TObject *)
{
  String MCID;
  MCID = cbxToMC->Items->operator [](cbxToMC->ItemIndex);
  to_mc = Problem->MCbyID(MCID.c_str());
  if(!to_mc){
	Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
	return;
  }

  // Display the vectors of N2 or N3
  int alpha, omega, apr_alpha, apr_omega;
  Net->alpha_omega_B2(from_mc->i, to_mc->i, x1, x2, alpha, omega, apr_alpha,
					  apr_omega);
  sgB1AlphaOmega->Cells[0][0] = "alpha";
  sgB1AlphaOmega->Cells[0][1] = "~alpha";
  sgB1AlphaOmega->Cells[0][2] = "omega";
  sgB1AlphaOmega->Cells[0][3] = "~omega";
  sgB1AlphaOmega->Cells[1][0] = IntToStr(alpha);
  sgB1AlphaOmega->Cells[1][1] = IntToStr(apr_alpha);
  sgB1AlphaOmega->Cells[1][2] = IntToStr(omega);
  sgB1AlphaOmega->Cells[1][3] = IntToStr(apr_omega);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::ClearNTables()
{
  sgRemovalValues->Cells[0][0] = "phi";
  sgRemovalValues->Cells[0][1] = "alpha";
  sgRemovalValues->Cells[0][2] = "~alpha";
  sgRemovalValues->Cells[0][3] = "omega";
  sgRemovalValues->Cells[0][4] = "~omega";
  for(int i=0; i<5; i++) sgRemovalValues->Cells[1][i] = "";

  sgAprVs->RowCount = 2;
  sgAprVs->Cells[0][0] = "x";
  sgAprVs->Cells[1][0] = "/\\delta";
  sgAprVs->Cells[2][0] = "\\/delta";
  sgAprVs->Cells[3][0] = "delta";
  sgAprVs->Cells[4][0] = "~Cmax";
  for(int i=0; i<sgAprVs->ColCount; i++)
	sgAprVs->Cells[i][1] = "";

  sgAprMWFT->RowCount = 2;
  sgAprMWFT->Cells[0][0] = "x";
  sgAprMWFT->Cells[1][0] = "D_MWFT";
  sgAprMWFT->Cells[2][0] = "MWFT";
  for(int i=0; i<sgAprMWFT->ColCount; i++)
	sgAprMWFT->Cells[i][1] = "";

  sgEstVs->RowCount = 2;
  sgEstVs->Cells[0][0] = "x";
  sgEstVs->Cells[1][0] = "/\\delta";
  sgEstVs->Cells[2][0] = "\\/delta";
  sgEstVs->Cells[3][0] = "delta";
  sgEstVs->Cells[4][0] = "~Cmax";
  for(int i=0; i<sgEstVs->ColCount; i++)
	sgEstVs->Cells[i][1] = "";

  sgEstMWFT->RowCount = 2;
  sgEstMWFT->Cells[0][0] = "x";
  sgEstMWFT->Cells[1][0] = "D_MWFT";
  sgEstMWFT->Cells[2][0] = "MWFT";
  for(int i=0; i<sgEstMWFT->ColCount; i++)
	sgEstMWFT->Cells[i][1] = "";

  sgActVs->RowCount = 2;
  sgActVs->Cells[0][0] = "x";
  sgActVs->Cells[1][0] = "/\\delta";
  sgActVs->Cells[2][0] = "\\/delta";
  sgActVs->Cells[3][0] = "delta";
  sgActVs->Cells[4][0] = "~Cmax";
  for(int i=0; i<sgAprVs->ColCount; i++)
	sgActVs->Cells[i][1] = "";

  sgActMWFT->RowCount = 2;
  sgActMWFT->Cells[0][0] = "x";
  sgActMWFT->Cells[1][0] = "D_MWFT";
  sgActMWFT->Cells[2][0] = "MWFT";
  for(int i=0; i<sgAprMWFT->ColCount; i++)
	sgActMWFT->Cells[i][1] = "";
}

void __fastcall TfrmPSEditor::DisplayN1Tables()
{
  int alpha, omega, apr_alpha, apr_omega;
  double phi;

  phi = Net->phi_N1(v_y);
  Net->alpha_omega_N1(v_y, alpha, omega, apr_alpha, apr_omega);

  sgRemovalValues->Cells[1][0] = FloatToStr(phi);
  sgRemovalValues->Cells[1][1] = IntToStr(alpha);
  sgRemovalValues->Cells[1][2] = IntToStr(apr_alpha);
  sgRemovalValues->Cells[1][3] = IntToStr(omega);
  sgRemovalValues->Cells[1][4] = IntToStr(apr_omega);

  int nx = Net->sizeof_tau(v_y->Job->i);
  sgAprVs->RowCount = nx + 1;
  sgEstVs->RowCount = nx + 1;
  sgActVs->RowCount = nx + 1;
  for(int curx = 1; curx <= nx; curx++){
	sgAprVs->Cells[0][curx] = IntToStr(curx);
	sgEstVs->Cells[0][curx] = IntToStr(curx);
	sgActVs->Cells[0][curx] = IntToStr(curx);

	if(curx < alpha || curx > omega){
	  sgAprVs->Cells[1][curx] = "---";
	  sgAprVs->Cells[2][curx] = "---";
	  sgAprVs->Cells[3][curx] = "---";

	  sgEstVs->Cells[1][curx] = "---";
	  sgEstVs->Cells[2][curx] = "---";
	  sgEstVs->Cells[3][curx] = "---";

	  sgActVs->Cells[1][curx] = "---";
	  sgActVs->Cells[2][curx] = "---";
	  sgActVs->Cells[3][curx] = "---";
	}
	else{
	  double hatDelta, checkDelta, delta, _Cmax;
	  Net->ApproxDeltas_N1(v_y, curx, hatDelta, checkDelta);
	  sgAprVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgAprVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0)? 0 : hatDelta) + ((checkDelta < 0)? 0 : checkDelta);
	  sgAprVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgAprVs->Cells[4][curx] = FloatToStr(_Cmax);

	  Net->EstDeltas_N1(v_y, curx, hatDelta, checkDelta);
	  sgEstVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgEstVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0)? 0 : hatDelta) + ((checkDelta < 0)? 0 : checkDelta);
	  sgEstVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgEstVs->Cells[4][curx] = FloatToStr(_Cmax);

	  Net->ExactDeltas_N1(Net_y, v_y, curx, hatDelta, checkDelta);
	  sgActVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgActVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0)? 0 : hatDelta) + ((checkDelta < 0)? 0 : checkDelta);
	  sgActVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgActVs->Cells[4][curx] = FloatToStr(_Cmax);
	}
  }
}

void __fastcall TfrmPSEditor::DisplayN2Tables()
{
  String MCID;
  MCID = cbxMachines->Items->operator [](cbxMachines->ItemIndex);
  DMOSP_Problem::SMC *SelMC = Problem->MCbyID(MCID.c_str());
  if(!SelMC){
	Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
	return;
  }

  int alpha, omega, apr_alpha, apr_omega;
  double phi;

  phi = Net->phi_N2(v_y, SelMC);
  Net->alpha_omega_N2(v_y, SelMC, alpha, omega, apr_alpha, apr_omega);

  sgRemovalValues->Cells[1][0] = FloatToStr(phi);
  sgRemovalValues->Cells[1][1] = IntToStr(alpha);
  sgRemovalValues->Cells[1][2] = IntToStr(apr_alpha);
  sgRemovalValues->Cells[1][3] = IntToStr(omega);
  sgRemovalValues->Cells[1][4] = IntToStr(apr_omega);

  int nx = Net->sizeof_pi(SelMC->i) + ((SelMC->i != Net->Nodes[v_y->i].mc)? 1 : 0);
  sgAprVs->RowCount = nx + 1;
  sgEstVs->RowCount = nx + 1;
  sgActVs->RowCount = nx + 1;
  for(int curx = 1; curx <= nx; curx++){
	sgAprVs->Cells[0][curx] = IntToStr(curx);
	sgEstVs->Cells[0][curx] = IntToStr(curx);
	sgActVs->Cells[0][curx] = IntToStr(curx);

	if(curx < alpha || curx > omega){
	  sgAprVs->Cells[1][curx] = "---";
	  sgAprVs->Cells[2][curx] = "---";
	  sgAprVs->Cells[3][curx] = "---";

	  sgEstVs->Cells[1][curx] = "---";
	  sgEstVs->Cells[2][curx] = "---";
	  sgEstVs->Cells[3][curx] = "---";

	  sgActVs->Cells[1][curx] = "---";
	  sgActVs->Cells[2][curx] = "---";
	  sgActVs->Cells[3][curx] = "---";
	}
	else{
	  double hatDelta, checkDelta, delta, _Cmax;
	  Net->ApproxDeltas_N2(v_y, SelMC, curx, hatDelta, checkDelta);
	  sgAprVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgAprVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0.0)? 0.0 : hatDelta) + ((checkDelta < 0.0)? 0.0 : checkDelta);
	  sgAprVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgAprVs->Cells[4][curx] = FloatToStr(_Cmax);

	  Net->EstDeltas_N2(v_y, SelMC, curx, hatDelta, checkDelta);
	  sgEstVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgEstVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0.0)? 0.0 : hatDelta) + ((checkDelta < 0.0)? 0.0 : checkDelta);
	  sgEstVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgEstVs->Cells[4][curx] = FloatToStr(_Cmax);

	  Net->ExactDeltas_N2(Net_y, v_y, SelMC, curx, hatDelta, checkDelta);
	  sgActVs->Cells[1][curx] = FloatToStr(hatDelta);
	  sgActVs->Cells[2][curx] = FloatToStr(checkDelta);
	  delta = ((hatDelta < 0.0)? 0.0 : hatDelta) + ((checkDelta < 0.0)? 0.0 : checkDelta);
	  sgActVs->Cells[3][curx] = FloatToStr(delta);
	  _Cmax = phi + delta;
	  sgActVs->Cells[4][curx] = FloatToStr(_Cmax);
	}
  }
}

void __fastcall TfrmPSEditor::DisplayN3Tables()
{
  double curMWFT = moss->NSTestSol->MWFTValue();

  int alpha, omega, minX;
  int Est_alpha, Est_omega, Est_minX;
  double *deltaMWFT = new double[moss->nWCs+1];
  double *Est_deltaMWFT = new double[moss->nWCs+1];

  moss->N3ExactResults(v_y->i, alpha, omega, deltaMWFT, minX);
  moss->N3Results(v_y->i, Est_alpha, Est_omega, Est_deltaMWFT, Est_minX);

  String s;
  s = sgRemovalValues->Cells[1][2];
  s = s + " (" + IntToStr(alpha) + ", " + IntToStr(Est_alpha) + ")";
  sgRemovalValues->Cells[1][2] = s;
  s = sgRemovalValues->Cells[1][4];
  s = s + " (" + IntToStr(omega) + ", " + IntToStr(Est_omega) + ")";
  sgRemovalValues->Cells[1][4] = s;

  int nx = Net->sizeof_tau(v_y->Job->i);
  sgEstMWFT->RowCount = nx + 1;
  sgActMWFT->RowCount = nx + 1;
  for(int curx = 1; curx <= nx; curx++){
	sgEstMWFT->Cells[0][curx] = IntToStr(curx);
	sgActMWFT->Cells[0][curx] = IntToStr(curx);

	if(curx < alpha || curx > omega){
	  sgEstMWFT->Cells[1][curx] = "---";
	  sgEstMWFT->Cells[2][curx] = "---";
	  sgActMWFT->Cells[1][curx] = "---";
	  sgActMWFT->Cells[2][curx] = "---";
	}
	else{
	  sgEstMWFT->Cells[1][curx] = FloatToStr(Est_deltaMWFT[curx]);
	  sgActMWFT->Cells[1][curx] = FloatToStr(deltaMWFT[curx]);
	  sgEstMWFT->Cells[2][curx] = FloatToStr(curMWFT+Est_deltaMWFT[curx]);
	  sgActMWFT->Cells[2][curx] = FloatToStr(curMWFT+deltaMWFT[curx]);
	}
  }

  delete [] deltaMWFT;
  delete [] Est_deltaMWFT;
}

void __fastcall TfrmPSEditor::DisplayN4Tables()
{
  double curMWFT = moss->NSTestSol->MWFTValue();

  String MCID;
  MCID = cbxMachines->Items->operator [](cbxMachines->ItemIndex);
  DMOSP_Problem::SMC *SelMC = Problem->MCbyID(MCID.c_str());
  if(!SelMC){
	Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
	return;
  }

  int alpha, omega, minX;
  int Est_alpha, Est_omega, Est_minX;
  double *deltaMWFT = new double[moss->nJobs+1];
  double *Est_deltaMWFT = new double[moss->nJobs+1];

  moss->N4ExactResults(v_y->i, SelMC->i, alpha, omega, deltaMWFT, minX);
  moss->N4Results(v_y->i, SelMC->i, Est_alpha, Est_omega, Est_deltaMWFT, Est_minX);

  String s;
  s = sgRemovalValues->Cells[1][2];
  s = s + " (" + IntToStr(alpha) + ", " + IntToStr(Est_alpha) + ")";
  sgRemovalValues->Cells[1][2] = s;
  s = sgRemovalValues->Cells[1][4];
  s = s + " (" + IntToStr(omega) + ", " + IntToStr(Est_omega) + ")";
  sgRemovalValues->Cells[1][4] = s;

  int nx = Net->sizeof_pi(SelMC->i) + ((SelMC->i != Net->Nodes[v_y->i].mc)? 1 : 0);
  sgEstMWFT->RowCount = nx + 1;
  sgActMWFT->RowCount = nx + 1;
  for(int curx = 1; curx <= nx; curx++){
	sgEstMWFT->Cells[0][curx] = IntToStr(curx);
	sgActMWFT->Cells[0][curx] = IntToStr(curx);

	if(curx < alpha || curx > omega){
	  sgEstMWFT->Cells[1][curx] = "---";
	  sgEstMWFT->Cells[2][curx] = "---";
	  sgActMWFT->Cells[1][curx] = "---";
	  sgActMWFT->Cells[2][curx] = "---";
	}
	else{
	  sgEstMWFT->Cells[1][curx] = FloatToStr(Est_deltaMWFT[curx]);
	  sgEstMWFT->Cells[2][curx] = FloatToStr(curMWFT+Est_deltaMWFT[curx]);
	  sgActMWFT->Cells[1][curx] = FloatToStr(deltaMWFT[curx]);
	  sgActMWFT->Cells[2][curx] = FloatToStr(curMWFT+deltaMWFT[curx]);
	}
  }
}

void __fastcall TfrmPSEditor::cbxMachinesChange(TObject *)
{
  DisplayN2Tables();
  DisplayN4Tables();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPSEditor::btnMILPSolClick(TObject *)
{
  MILPSol->Copy(frmSolution->CurrentSol);
  frmMILPSol->ShowMILP(MILPSol);
  if(frmMILPSol->ModalResult == mrOk){
	frmSolution->SetCurrentSolution(MILPSol);
	UpdateVPS(MILPSol);
	UpdateVOO(MILPSol);
  }
}
//---------------------------------------------------------------------------

