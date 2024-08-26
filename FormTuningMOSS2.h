//---------------------------------------------------------------------------

#ifndef FormTuningMOSS2H
#define FormTuningMOSS2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include "MOSS.h"
#include "FileSystem.h"

//---------------------------------------------------------------------------
class TfrmTuningMOSS2 : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label2;
	TLabel *Label9;
	TLabel *Label15;
	TSpeedButton *btnTerminate;
	TLabel *Label5;
	TLabel *lblTCReadResult;
	TLabel *Label6;
	TLabel *Label7;
	TStringGrid *grdLoadedInstances;
	TButton *btnLoadProblems;
	TProgressBar *pbRun;
	TProgressBar *pbConstruction;
	TProgressBar *pbIterations;
	TBitBtn *btnRun;
	TBitBtn *btnClose;
	TMemo *memoTCs;
	TButton *btnReadRuns;
	TMemo *memoTGD;
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
	  int n_TS_maxitr;
	  int PSize;
	};

    int nTCs;
	DMOSP_TreatmentCombination *TCs;

	DMOSP_MOSS *mossAlg;

public:		// User declarations
    bool bActive;

	__fastcall TfrmTuningMOSS2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTuningMOSS2 *frmTuningMOSS2;
//---------------------------------------------------------------------------
#endif
