//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------
USEFORM("FormSol.cpp", frmSolution);
USEFORM("FormCalcProgress.cpp", frmCalcProgress);
USEFORM("FormPSEditor.cpp", frmPSEditor);
USEFORM("Main.cpp", MainForm);
USEFORM("FormMILPReader.cpp", frmMILPSol);
USEFORM("FormMOGWO.cpp", frmMOGWO);
USEFORM("FormMOGWOCalcProgress.cpp", frmMOGWOCalcProgress);
USEFORM("FormGAGWDOERuns.cpp", frmGAGWDOERuns);
USEFORM("FormTuningMOSS2.cpp", frmTuningMOSS2);
USEFORM("FormMOSSOperators.cpp", frmMOSSOperators);
USEFORM("FormEvolutionaryOperators.cpp", frmEvolOperators);
USEFORM("FormAlgorithm.cpp", frmAlgorithm);
USEFORM("FormNSGAII.cpp", frmNSGAII);
USEFORM("FormMattaProblems.cpp", frmMattaProblems);
USEFORM("FormNSGA2CalcProgress.cpp", frmNSGA2CalcProgress);
USEFORM("About.cpp", AboutBox);
USEFORM("FormTuningNSGA2.cpp", frmTuningNSGA2);
USEFORM("FormChromosomeEditor.cpp", frmChromosomeEditor);
USEFORM("FormProblem.cpp", frmProblem);
USEFORM("FormGeneralDMOSPInstances.cpp", frmGeneralDMOSPInstances);
USEFORM("FormTuningMOSS.cpp", frmTuningMOSS);
USEFORM("FormTuningMOGWO.cpp", frmTuningMOGWO);
USEFORM("FormSolvedSetMultiRuns.cpp", frmSolvedSetMultiRuns);
USEFORM("FormGASSDOERuns.cpp", frmGASSDOERuns);
USEFORM("FormMOSS.cpp", frmMOSS);
USEFORM("FormMOSSCalcProgress.cpp", frmMOSSCalcProgress);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application->Initialize();
	Application->Title = "Multiprocessor Open Shop Scheduling Problem Solver";
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TfrmPSEditor), &frmPSEditor);
		Application->CreateForm(__classid(TfrmCalcProgress), &frmCalcProgress);
		Application->CreateForm(__classid(TfrmMattaProblems), &frmMattaProblems);
		Application->CreateForm(__classid(TfrmGeneralDMOSPInstances), &frmGeneralDMOSPInstances);
		Application->CreateForm(__classid(TfrmMILPSol), &frmMILPSol);
		Application->CreateForm(__classid(TfrmNSGAII), &frmNSGAII);
		Application->CreateForm(__classid(TfrmChromosomeEditor), &frmChromosomeEditor);
		Application->CreateForm(__classid(TfrmEvolOperators), &frmEvolOperators);
		Application->CreateForm(__classid(TfrmNSGA2CalcProgress), &frmNSGA2CalcProgress);
		Application->CreateForm(__classid(TfrmMOGWO), &frmMOGWO);
		Application->CreateForm(__classid(TfrmMOGWOCalcProgress), &frmMOGWOCalcProgress);
		Application->CreateForm(__classid(TfrmTuningNSGA2), &frmTuningNSGA2);
		Application->CreateForm(__classid(TfrmTuningMOGWO), &frmTuningMOGWO);
		Application->CreateForm(__classid(TfrmSolvedSetMultiRuns), &frmSolvedSetMultiRuns);
		Application->CreateForm(__classid(TfrmGAGWDOERuns), &frmGAGWDOERuns);
		Application->CreateForm(__classid(TfrmMOSS), &frmMOSS);
		Application->CreateForm(__classid(TfrmMOSSCalcProgress), &frmMOSSCalcProgress);
		Application->CreateForm(__classid(TfrmMOSSOperators), &frmMOSSOperators);
		Application->CreateForm(__classid(TfrmTuningMOSS), &frmTuningMOSS);
		Application->CreateForm(__classid(TfrmTuningMOSS2), &frmTuningMOSS2);
		Application->CreateForm(__classid(TfrmGASSDOERuns), &frmGASSDOERuns);
		Application->Run();

	return 0;
}
//---------------------------------------------------------------------
