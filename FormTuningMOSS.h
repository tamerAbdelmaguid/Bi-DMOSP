//---------------------------------------------------------------------------

#ifndef FormTuningMOSSH
#define FormTuningMOSSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include "MOSS.h"
#include "FileSystem.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TfrmTuningMOSS : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *grdLoadedInstances;
	TButton *btnLoadProblems;
	TLabel *Label3;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label9;
	TLabel *Label15;
	TLabel *Label4;
	TSpeedButton *btnTerminate;
	TMemo *memoTime;
	TMemo *memoIGD;
	TProgressBar *pbRun;
	TProgressBar *pbConstruction;
	TProgressBar *pbIterations;
	TMemo *memoGD;
	TBitBtn *btnRun;
	TBitBtn *btnClose;
	TMemo *memoTCs;
	TLabel *Label5;
	TButton *btnReadRuns;
	TLabel *lblTCReadResult;
	TLabel *Label6;
	TMemo *memoTGD;
	TLabel *Label7;
	TMemo *memoDHVpr;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnLoadProblemsClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnReadRunsClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnTerminateClick(TObject *Sender);
private:	// User declarations
	DMOSP_Problem **Instance;

	struct DMOSP_TreatmentCombination {
	  int RefSetSize;
	  double Theta_X;
	  int TabuSize;
	  int n_TS_maxitr;
	  int n_noimp_maxitr;
	  int n_bestmoves;
	  int n_randmoves;
	  int PSize;
	};

    int nTCs;
	DMOSP_TreatmentCombination *TCs;

	DMOSP_MOSS *mossAlg;

public:		// User declarations
    bool bActive;

	__fastcall TfrmTuningMOSS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTuningMOSS *frmTuningMOSS;
//---------------------------------------------------------------------------
#endif
