//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "About.h"
#include "Problem.h"
#include "FileSystem.h"
#include <io.h>
#include "FormPSEditor.h"
#include "FormMattaProblems.h"
#include "FormGeneralDMOSPInstances.h"
#include "FormNSGAII.h"
#include "FormChromosomeEditor.h"
#include "FormEvolutionaryOperators.h"
#include "FormMOGWO.h"
#include "FormTuningNSGA2.h"
#include "FormTuningMOGWO.h"
#include "FormSolvedSetMultiRuns.h"
#include "FormGAGWDOERuns.h"
#include "FormTuningMOSS.h"
#include "FormTuningMOSS2.h"
#include "FormGASSDOERuns.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent *Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CreateMDIChild(String Name)
{
  TfrmProblem *Child;

  String s = IntToStr(MDIChildCount);
  DMOSP_Problem *newProblem = new DMOSP_Problem(s.c_str(), Name.c_str());

  //--- create a new MDI child window ----
  Child = new TfrmProblem(Application, newProblem, "");
  Child->Caption = Name;
  Child->tbSolvedSetParetoFront->TabVisible = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileNew1Execute(TObject *)
{
  CreateMDIChild("MOSP" + IntToStr(MDIChildCount + 1));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileOpen1Execute(TObject *)
{
  if (OpenDialog->Execute()){
	String msg;
	String s = OpenDialog->FileName;
	DMOSP_FIO_RESULT Result;
	DMOSP_Problem *newProblem = LoadDMOSP(s.c_str(), Result);
	if(Result != SUCCESS){
	  switch(Result){
		case FAIL_TO_OPEN:
		  msg = "failed to open file";
		  break;
		case UNKNOWN_FILE_TYPE:
		  msg = "unknown file type!";
		  break;
		case DATA_ERROR:
		  msg = "Error in data";
		  break;
	  }
	  Application->MessageBox(msg.c_str(), L"Error reading file", MB_OK);
	  return;
	}
	TfrmProblem *Child = new TfrmProblem(Application, newProblem, s);
	String ProblemName = ExtractFileName(s);
	Child->Caption = ProblemName;
	UpdateWorkingDirectory(ExtractFilePath(s));

	//Check if this problem belongs to the solved set or not
	wchar_t *prbname = ProblemName.c_str();
	int p;

    // Check if the problem is the SampleDMOSP or not
	bool bSampleDMOSP = true;
	wchar_t *SampleDMOSPFileName = L"SampleDMOSP.dmosp";
	for(p=0; p < (signed) wcslen(SampleDMOSPFileName); p++){
	  if(prbname[p] != SampleDMOSPFileName[p]){
		bSampleDMOSP = false;
		break;
	  }
	}
	if(bSampleDMOSP){
	  prbname = L"DMOSP-5-10-1-0.7-1-2-1-2-1-31.dmosp";
      ProblemName = "DMOSP-5-10-1-0.7-1-2-1-2-1-31.dmosp";
	}

	wchar_t *slvdprbname = L"DMOSP-5-10-1-0.7-1-2-1-2-1-";
	bool bSolvedProblem = true;
	for(p=0; p < (signed) wcslen(slvdprbname); p++){
	  if(prbname[p] != slvdprbname[p]){
		bSolvedProblem = false;
		break;
	  }
	}
	if(bSolvedProblem){
	  Child->tbSolvedSetParetoFront->TabVisible = true;
	  Child->sgParetoFront->Cells[0][0] = "Sol. #";
	  Child->sgParetoFront->Cells[1][0] = "Makespan";
	  Child->sgParetoFront->Cells[2][0] = "MWFT";

	  String Sprobno = ProblemName.SubString(p+1,2);
	  int probno;
	  try{
	    probno = StrToFloat(Sprobno) - 1;
	  } catch(EConvertError&){
          probno = -1;
		}

	  if(probno < 0 || probno > 30){
		newProblem->i = -1;
	    Child->tbSolvedSetParetoFront->TabVisible = false;
	  }
	  else{
		newProblem->i = probno;
		Child->sgParetoFront->RowCount = SolvedDMOSPs[probno].nSol + 1;
		for(p = 0; p < SolvedDMOSPs[probno].nSol; p++){
		  Child->sgParetoFront->Cells[0][p+1] = IntToStr(p+1);
		  Child->sgParetoFront->Cells[1][p+1] = FloatToStr(SolvedDMOSPs[probno].Cmax[p]);
		  Child->sgParetoFront->Cells[2][p+1] = FloatToStr(SolvedDMOSPs[probno].MWFT[p]);
		}
	  }
	}
	else{
      newProblem->i = -1;
      Child->tbSolvedSetParetoFront->TabVisible = false;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::HelpAbout1Execute(TObject *)
{
  AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileExit1Execute(TObject *)
{
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::FormShow(TObject *)
{
  UpdateWorkingDirectory(ExtractFilePath(Application->ExeName));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *)
{
  CurrentChild = NULL;
  btnSave->Enabled = false;
  FileSaveItem->Enabled = false;
  FileSaveAsItem->Enabled = false;
  Exportto->Enabled = false;
  mnuSolve->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSaveItemClick(TObject *Sender)
{
  if(!CurrentChild) return;
  if(!CurrentChild->bFileNameGiven) FileSaveAsItemClick(Sender);
  else{
    String s;
    DMOSP_FIO_RESULT Result;
	SaveDMOSP(CurrentChild->Problem, CurrentChild->FileName.c_str(), Result);
    if(Result != SUCCESS){
      switch(Result){
        case FAIL_TO_OPEN:
          s = "failed to save file";
          break;
        case DATA_ERROR:
          s = "Error in data";
          break;
      }
	  Application->MessageBox(s.c_str(), L"Error saving file", MB_OK);
      return;
    }
    CurrentChild->NeedToBeSaved = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSaveAsItemClick(TObject *)
{
  if(!CurrentChild) return;
  SaveDialog->FileName = CurrentChild->Caption;
  if(SaveDialog->Execute()){
	AnsiString s = SaveDialog->FileName;
	if(access(s.c_str(), 0) == 0){
	  String msg = "The file "+s+" already exists, would you like to replace it?";
	  int r = Application->MessageBox(msg.c_str(), L"File exists", MB_YESNO);
	  if(r == IDNO) return;
	}
	DMOSP_FIO_RESULT Result;
	String fname = s;
	SaveDMOSP(CurrentChild->Problem, fname.c_str(), Result);
    CurrentChild->bFileNameGiven = true;
    CurrentChild->FileName = fname;
    CurrentChild->Caption = ExtractFileName(s);
    CurrentChild->NeedToBeSaved = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::LNGfile1Click(TObject *)
{
  if(!CurrentChild) return;
  AnsiString s = CurrentChild->Caption;
  int i = s.AnsiPos(".dmosp")-1;
  if(i != -1) s = s.SubString(1, i) + ".dat";
  else s = s + ".dat";
  SaveDATDialog->FileName = s;
  if(SaveDATDialog->Execute()){
	s = SaveDATDialog->FileName;
	if(access(s.c_str(), 0) == 0){
	  String msg = "The file "+s+" already exists, would you like to replace it?";
	  int r = Application->MessageBox(msg.c_str(), L"File exists", MB_YESNO);
	  if(r == IDNO) return;
	}
	String str = s;
	DMOSP_FIO_RESULT Result;
	SaveDMOSP_DAT(CurrentChild->Problem, str.c_str(), Result);
	if(Result != SUCCESS){
	  Application->MessageBox(str.c_str(), L"Error writing file", MB_OK);
	  return;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UpdateWorkingDirectory(String newDir)
{
  WorkingDirectory = newDir;
  OpenDialog->InitialDir = WorkingDirectory;
  OpenDialogTestProblem->InitialDir = WorkingDirectory;
  SaveDialog->InitialDir = WorkingDirectory;
  SaveDATDialog->InitialDir = WorkingDirectory;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuPSEditorClick(TObject *)
{
  if(!CurrentChild) return;
  frmPSEditor->Problem = CurrentChild->Problem;
  frmPSEditor->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GenerateMattasProblems1Click(TObject *)
{
  frmMattaProblems->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GenerategeneralMOSPinstances1Click(TObject *)
{
  frmGeneralDMOSPInstances->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NSGAIIforCmaxandMWFT1Click(TObject *)
{
  if(!CurrentChild) return;
  frmNSGAII->Problem = CurrentChild->Problem;
  frmNSGAII->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvolutionarySolutionRepresentationEditor1Click(TObject *)

{
  if(!CurrentChild) return;
  frmChromosomeEditor->Problem = CurrentChild->Problem;
  frmChromosomeEditor->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EvolutionaryOperators1Click(TObject *)
{
  if(!CurrentChild) return;
  frmEvolOperators->Problem = CurrentChild->Problem;
  frmEvolOperators->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MOGWOforCmaxandMWFT1Click(TObject *)
{
  if(!CurrentChild) return;
  frmMOGWO->Problem = CurrentChild->Problem;
  frmMOGWO->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ConducttuningrunsforNSGAII1Click(TObject *)
{
  frmTuningNSGA2->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ConducttuningrunsforMOGWO1Click(TObject *)
{
  frmTuningMOGWO->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Conductmultiplerunsforsmallbenchmarkinstances1Click(TObject *)

{
  frmSolvedSetMultiRuns->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DOErunsforlargeinstances1Click(TObject *)
{
  frmGAGWDOERuns->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MOSSforCmaxandMWFT1Click(TObject *)
{
  if(!CurrentChild) return;
  frmMOSS->Problem = CurrentChild->Problem;
  frmMOSS->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MOSSOoperators1Click(TObject *)
{
  if(!CurrentChild) return;
  frmMOSSOperators->Problem = CurrentChild->Problem;
  frmMOSSOperators->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ConducttuningrunsforMOSS1Click(TObject *)
{
  frmTuningMOSS->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ConductadditionaltuningrunsforMOSS1Click(TObject *)

{
  frmTuningMOSS2->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DOENSGAIIxMOSSrunsforlargeinstances1Click(TObject *)
{
  frmGASSDOERuns->ShowModal();
}
//---------------------------------------------------------------------------

