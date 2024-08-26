//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormChromosomeEditor.h"
#include "FormSol.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmChromosomeEditor *frmChromosomeEditor;
//---------------------------------------------------------------------------
__fastcall TfrmChromosomeEditor::TfrmChromosomeEditor(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  OBChromosome = NULL;
  RKChromosome = NULL;
  VPBChromosome = NULL;
  DSG = NULL;
  METAH = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);

  frmSolution->btnLoadSol->Visible = false;
  frmSolution->btnSaveSol->Visible = false;

  grdOBChromosome->ColCount = Problem->nOperations();
  grdRKChromosome->ColCount = 2 * Problem->nOperations();
  grdVPBChromosome->ColCount = Problem->nOperations() + 2;
  grdVPBChromosome->RowCount = Problem->WCs.Count() + 2;

  SList::SNode *wsnd = Problem->WCs.head();
  for(int w=0; w<Problem->WCs.Count(); w++){
	DMOSP_Problem::SWC *ws = (DMOSP_Problem::SWC *)wsnd->Data();
	grdVPBChromosome->Cells[0][w+2] = ws->ID();
	wsnd = wsnd->Next();
  }

  cbxJSR->ItemIndex = 8;
  cbxMSR->ItemIndex = 2;

  GenerateDSSol();
  CreateChromosome(DSG->Solution);
  DisplayChromosome();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::GenerateDSSol()
{
  if(DSG) delete DSG;
  DMOSP_DSG::T_JobDispatchingRule jdr;
  DMOSP_DSG::T_MCSelectionRule msr;

  jdr = (DMOSP_DSG::T_JobDispatchingRule)cbxJSR->ItemIndex;
  msr = (DMOSP_DSG::T_MCSelectionRule)cbxMSR->ItemIndex;

  DSG = new DMOSP_DSG(Problem, jdr, msr);
  DSG->Run();
  frmSolution->SetCurrentSolution(DSG->Solution);

  double Cmax = DSG->Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
  double MWFT = DSG->Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);
  String s;
  s = FloatToStr(Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(MWFT);
  lblMWFT->Caption = s;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::btnRunDSGClick(TObject *)
{
  GenerateDSSol();
  CreateChromosome(DSG->Solution);
  DisplayChromosome();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::FormClose(TObject *Sender, TCloseAction &Action)

{
  TfrmAlgorithm::FormClose(Sender, Action);

  if(OBChromosome) delete OBChromosome;   OBChromosome = NULL;
  if(RKChromosome) delete RKChromosome;   RKChromosome = NULL;
  if(VPBChromosome) delete VPBChromosome;   VPBChromosome = NULL;
  if(DSG) delete DSG;   DSG = NULL;
  if(METAH) delete METAH;   METAH = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::CreateChromosome(DMOSP_Solution *Sol)
{
  if(OBChromosome) delete OBChromosome;
  OBChromosome = new DMOSP_OBChromosome(Problem);
  OBChromosome->Encode(Sol);

  if(RKChromosome) delete RKChromosome;
  RKChromosome = new DMOSP_RKChromosome(Problem);
  RKChromosome->Encode(Sol);

  if(VPBChromosome) delete VPBChromosome;
  VPBChromosome = new DMOSP_VPBChromosome(Problem);
  VPBChromosome->Encode(Sol);
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::DisplayChromosome()
{
  int i;

  ClearGrids();

  // Display the operations based chromosome
  DMOSP_OBGene *obgenes = OBChromosome->getGenes();
  for(i=0; i<OBChromosome->getLength(); i++){
	grdOBChromosome->Cells[i][0] = obgenes[i].Operation->ID();
	grdOBChromosome->Cells[i][1] = obgenes[i].Machine->ID();
  }

  // Display the random keys chromosome
  double *rkgenes = RKChromosome->getGenes();
  char buf[32];
  i=0;
  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	for(SList::SNode *ond = jb->Operations.head(); ond; ond = ond->Next()){
	  DMOSP_Problem::SOperation *op = (DMOSP_Problem::SOperation *)ond->Data();
	  grdRKChromosome->Cells[i][0] = op->ID();
	  sprintf(buf, "%.3f", rkgenes[i]);
	  grdRKChromosome->Cells[i][1] = buf;
	  ++i;
	  sprintf(buf, "%.3f", rkgenes[i]);
	  grdRKChromosome->Cells[i][1] = buf;
	  ++i;
	}
	jnd = jnd->Next();
  }

  // Display the Vector of Permutations-based chromosome
  DMOSP_VPBString *jbString = VPBChromosome->getJobsString();
  DMOSP_VPBString *wsString = VPBChromosome->getWorkstationsString();
  for(i=0; i<jbString->length; i++){
	grdVPBChromosome->Cells[i+1][0] = jbString->Gene[i]->ID();
  }
  for(i=0; i<wsString->length; i++){
	grdVPBChromosome->Cells[i+1][1] = wsString->Gene[i]->ID();
  }
  for(int s=0; s<wsString->nSubstrings; s++){
	DMOSP_VPBSubString &substr = wsString->Substring[s];
	for(int p=0; p < substr.nSeparators; p++){
	  int c = substr.SeparatorPos[p];
	  String tmp = grdVPBChromosome->Cells[c+1][2+s];
	  tmp = tmp + "^";
	  grdVPBChromosome->Cells[c+1][2+s] = tmp;
	}
  }
}
//---------------------------------------------------------------------------
bool __fastcall TfrmChromosomeEditor::ReadOBChromosome()
{
  DMOSP_OBGene *genes = OBChromosome->getGenes();

  // Read the given chromosome
  String s;
  for(int i=0; i<OBChromosome->getLength(); i++){
	s = grdOBChromosome->Cells[i][0];
	DMOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	if(op) genes[i].Operation = op;
	else{
	  s = "Unrecognized operation " + s;
	  Application->MessageBox(s.c_str(),L"Error reading operation",MB_OK);
	  return false;
	}
	for(int j=0; j<i; j++){
	  if(op == genes[j].Operation){
		s = "Operation " + s + " is included more than once!";
		Application->MessageBox(s.c_str(),L"duplicate operation",MB_OK);
		return false;
	  }
	}

	s = grdOBChromosome->Cells[i][1];
	DMOSP_Problem::SMC *mc = Problem->MCbyID(s.c_str());
	if(mc){
	  if(op->pt(mc) == -1){
		s = "Machine " + s + " cannot process operation " + op->ID();
		Application->MessageBox(s.c_str(),L"Machine cannot be used",MB_OK);
		return false;
	  }
	  genes[i].Machine = mc;
	}
	else{
	  s = "Unrecognized machine " + s;
	  Application->MessageBox(s.c_str(),L"Error reading machine",MB_OK);
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::InterpretOBChromosome()
{
  double Cmax, MWFT;
  if(METAH) delete METAH;
  METAH = new DMOSP_Metaheuristic(Problem);
  METAH->AllocateMemory();
  METAH->DecodeOBChromosome(OBChromosome, DSG->Solution, Cmax, MWFT);
  METAH->ConvertOBtoRK(OBChromosome, RKChromosome);
  METAH->FreeMemory();
  delete METAH;    METAH = NULL;

  //OBChromosome->Decode(DSG->Solution);
  VPBChromosome->Encode(DSG->Solution);
  frmSolution->SetCurrentSolution(DSG->Solution);

  String s;
  s = FloatToStr(Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(MWFT);
  lblMWFT->Caption = s;
}
//---------------------------------------------------------------------------

bool __fastcall TfrmChromosomeEditor::ReadRKChromosome()
{
  double *rkgenes = RKChromosome->getGenes();

  int i=0;
  String s;
  double d;

  SList::SNode *jnd = Problem->Jobs.head();
  while(jnd){
	DMOSP_Problem::SJob *jb = (DMOSP_Problem::SJob *)jnd->Data();
	for(SList::SNode *ond = jb->Operations.head(); ond; ond = ond->Next()){
	  s = grdRKChromosome->Cells[i][1];
	  try{
		d = StrToFloat(s);
	  } catch(EConvertError&){
		  Application->MessageBox(L"Use appropriate numerical values for RK genes",
								   L"Error reading numbers",
									MB_OK);
		  return false;
		}

	  if(d<0.0 || d > 1.0){
		  Application->MessageBox(L"values for RK genes must be between 0.0 and 1.0",
								   L"Inappropriate numbers",
									MB_OK);
		  return false;
	  }
	  rkgenes[i] = d;
	  ++i;

	  s = grdRKChromosome->Cells[i][1];
	  try{
		d = StrToFloat(s);
	  } catch(EConvertError&){
		  Application->MessageBox(L"Use appropriate numerical values for RK genes",
								   L"Error reading numbers",
									MB_OK);
		  return false;
		}

	  if(d<0.0 || d > 1.0){
		  Application->MessageBox(L"values for RK genes must be between 0.0 and 1.0",
								   L"Inappropriate numbers",
									MB_OK);
		  return false;
	  }
	  rkgenes[i] = d;
	  ++i;
	}
	jnd = jnd->Next();
  }
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::InterpretRKChromosome()
{
  double Cmax, MWFT;
  if(METAH) delete METAH;
  METAH = new DMOSP_Metaheuristic(Problem);
  METAH->AllocateMemory();
  METAH->DecodeRKChromosome(RKChromosome, DSG->Solution, Cmax, MWFT);
  METAH->ConvertRKtoOB(RKChromosome, OBChromosome);
  METAH->FreeMemory();
  delete METAH;    METAH = NULL;

  //RKChromosome->Decode(DSG->Solution);
  VPBChromosome->Encode(DSG->Solution);
  frmSolution->SetCurrentSolution(DSG->Solution);

  String s;
  s = FloatToStr(Cmax);
  lblCmax->Caption = s;
  s = FloatToStr(MWFT);
  lblMWFT->Caption = s;
}
//---------------------------------------------------------------------------
bool __fastcall TfrmChromosomeEditor::ReadVPBChromosome()
{
  // Read the given VPB chromosome and check its validity
  DMOSP_VPBString *jbString = VPBChromosome->getJobsString();
  DMOSP_VPBString *wsString = VPBChromosome->getWorkstationsString();
  String s;

  // Read jobs' strings
  for(int j=0; j<jbString->nSubstrings; j++){
	for(int i=jbString->Substring[j].start; i<=jbString->Substring[j].end; i++){
	  s = grdVPBChromosome->Cells[i+1][0];
	  DMOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(op){
		if(VPBChromosome->getProblem()->Jobs.IndexOf(op->Job) != j){
		  s = "Operation " + s + " is wrongly places in job "
				  + ((DMOSP_Problem::SJob *)VPBChromosome->getProblem()->Jobs[j])->ID()
				  + "\'s substring!";
		  Application->MessageBox(s.c_str(),L"Error: misplacing operation",MB_OK);
		  return false;
		}
		else jbString->Gene[i] = op;
	  }
	  else{
		s = "Unrecognized operation " + s;
		Application->MessageBox(s.c_str(),L"Error reading operation",MB_OK);
		return false;
	  }
	  for(int l=jbString->Substring[j].start; l<i; l++){
		if(op == jbString->Gene[l]){
		  s = "Operation " + s + " is included more than once in jobs' string!";
		  Application->MessageBox(s.c_str(),L"duplicate operation",MB_OK);
		  return false;
		}
	  }
	}
  }

  // Read workstations strings
  for(int w=0; w<wsString->nSubstrings; w++){
	for(int i=wsString->Substring[w].start; i<=wsString->Substring[w].end; i++){
	  s = grdVPBChromosome->Cells[i+1][1];
	  DMOSP_Problem::SOperation *op = Problem->OpByID(s.c_str());
	  if(op){
		if(VPBChromosome->getProblem()->WCs.IndexOf(op->WC) != w){
		  s = "Operation " + s + " is wrongly places in workstation "
				  + ((DMOSP_Problem::SWC *)VPBChromosome->getProblem()->WCs[w])->ID()
				  + "\'s substring!";
		  Application->MessageBox(s.c_str(),L"Error: misplacing operation",MB_OK);
		  return false;
		}
		else wsString->Gene[i] = op;
	  }
	  else{
		s = "Unrecognized operation " + s;
		Application->MessageBox(s.c_str(),L"Error reading operation",MB_OK);
		return false;
	  }
	  for(int l=wsString->Substring[w].start; l<i; l++){
		if(op == wsString->Gene[l]){
		  s = "Operation " + s + " is included more than once in workstations' string!";
		  Application->MessageBox(s.c_str(),L"duplicate operation",MB_OK);
		  return false;
		}
	  }
	}
  }

  // Read the separators positions
  int **nSeparators = new int *[wsString->nSubstrings];
  for(int ws=0; ws<wsString->nSubstrings; ws++)
	 nSeparators[ws] = new int[grdVPBChromosome->ColCount - 1];
  for(int ws=0; ws<wsString->nSubstrings; ws++){
	for(int col=1; col<grdVPBChromosome->ColCount; col++){
	  s = grdVPBChromosome->Cells[col][ws + 2];
	  nSeparators[ws][col-1] = s.Length();
	}
  }
  for(int w=0; w<wsString->nSubstrings; w++){
	int nsep = wsString->Substring[w].nSeparators;
	if(nsep > 0){
	  int i, sep = 0;
	  for(i=wsString->Substring[w].start; i<=wsString->Substring[w].end; i++){
		for(; nSeparators[w][i]>0; nSeparators[w][i]--)
		  wsString->Substring[w].SeparatorPos[sep++] = i;
		if(sep > nsep){
		  DMOSP_Problem::SWC *WS = (DMOSP_Problem::SWC *)Problem->WCs[w];
		  s = WS->ID();
		  s = "Number of separators for workstation " + s + " exceed number of machines - 1";
		  Application->MessageBox(s.c_str(),L"wrong number of separators",MB_OK);
		  for(int ws=0; ws < wsString->nSubstrings; ws++) delete [] nSeparators[ws];
		  delete [] nSeparators;
		  return false;
		}
	  }
	  if(sep < nsep){
		for(; nSeparators[w][i]>0; nSeparators[w][i]--)
		  wsString->Substring[w].SeparatorPos[sep++] = i;
		if(sep > nsep){
		  DMOSP_Problem::SWC *WS = (DMOSP_Problem::SWC *)Problem->WCs[w];
		  s = WS->ID();
		  s = "Number of separators for workstation " + s + " exceed number of machines - 1";
		  Application->MessageBox(s.c_str(),L"wrong number of separators",MB_OK);
		  for(int ws=0; ws<wsString->nSubstrings; ws++) delete [] nSeparators[ws];
		  delete [] nSeparators;
		  return false;
		}
		if(sep < nsep){
		  DMOSP_Problem::SWC *WS = (DMOSP_Problem::SWC *)Problem->WCs[w];
		  s = WS->ID();
		  s = "Number of separators for workstation " + s + " is less than number of machines - 1";
		  Application->MessageBox(s.c_str(),L"wrong number of separators",MB_OK);
		  for(int ws=0; ws<wsString->nSubstrings; ws++) delete [] nSeparators[ws];
		  delete [] nSeparators;
		  return false;
		}
	  }
	}
  }

  for(int ws=0; ws<wsString->nSubstrings; ws++) delete [] nSeparators[ws];
  delete [] nSeparators;
  return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChromosomeEditor::InterpretVPBChromosome()
{
  double Cmax, MWFT;

  if(METAH) delete METAH;
  METAH = new DMOSP_Metaheuristic(Problem);
  METAH->AllocateMemory();
  if(METAH->DecodeVPBChromosome(VPBChromosome, DSG->Solution, Cmax, MWFT)){
	String s;
	s = FloatToStr(Cmax);
	lblCmax->Caption = s;
	s = FloatToStr(MWFT);
	lblMWFT->Caption = s;

	OBChromosome->Encode(DSG->Solution);
	RKChromosome->Encode(DSG->Solution);
	frmSolution->SetCurrentSolution(DSG->Solution);
  }
  else {
	Application->MessageBox(L"Infeasible solution",
					 L"VPB chromosome results in an infeasible solution",MB_OK);
  }
  METAH->FreeMemory();
  delete METAH;    METAH = NULL;

  /*if(VPBChromosome->Decode(DSG->Solution)){
	Cmax = DSG->Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MAKESPAN);
	MWFT = DSG->Solution->ComputeObjectiveValue(DMOSP_Problem::MIN_MEAN_W_FLOWTIME);

	String s;
	s = FloatToStr(Cmax);
	lblCmax->Caption = s;
	s = FloatToStr(MWFT);
	lblMWFT->Caption = s;

	OBChromosome->Encode(DSG->Solution);
	RKChromosome->Encode(DSG->Solution);
	frmSolution->SetCurrentSolution(DSG->Solution);
  }
  else {
	Application->MessageBox(L"Infeasible solution",
					 L"VPB chromosome results in an infeasible solution",MB_OK);
  }*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmChromosomeEditor::btnRunClick(TObject *)
{
  if(pcChromosome->ActivePage == tsOBChromosome){
	if(ReadOBChromosome()){
	  InterpretOBChromosome();
	  DisplayChromosome();
	}
  }
  else if(pcChromosome->ActivePage == tsRKChromosome){
	if(ReadRKChromosome()){
	  InterpretRKChromosome();
	  DisplayChromosome();
	}
  }
  else if(pcChromosome->ActivePage == tsVPBChromosome){
	if(ReadVPBChromosome()){
	  InterpretVPBChromosome();
	  DisplayChromosome();
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChromosomeEditor::btnSaveClick(TObject *)
{
  frmSolution->btnSaveSolClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmChromosomeEditor::btnLoadClick(TObject *)
{
  frmSolution->btnLoadSolClick(this);
  CreateChromosome(frmSolution->CurrentSol);
  DisplayChromosome();
}
//---------------------------------------------------------------------------

void __fastcall TfrmChromosomeEditor::ClearGrids()
{
  int i;

  for(i=0; i<grdOBChromosome->ColCount; i++){
	grdOBChromosome->Cells[i][0] = "";
	grdOBChromosome->Cells[i][1] = "";
  }
  for(i=0; i<grdRKChromosome->ColCount; i++){
	grdRKChromosome->Cells[i][0] = "";
	grdRKChromosome->Cells[i][1] = "";
  }
  for(i=1; i<grdVPBChromosome->ColCount; i++){
	for(int j=0; j<grdVPBChromosome->RowCount; j++){
	  grdVPBChromosome->Cells[i][j] = "";
	}
  }
}
//---------------------------------------------------------------------------

