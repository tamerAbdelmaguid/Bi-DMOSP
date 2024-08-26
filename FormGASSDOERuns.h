//---------------------------------------------------------------------------

#ifndef FormGASSDOERunsH
#define FormGASSDOERunsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include "NSGAII.h"
#include "MOSS.h"
//---------------------------------------------------------------------------
class TfrmGASSDOERuns : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label5;
	TLabel *lblTCReadResult;
	TLabel *Label2;
	TSpeedButton *btnTerminate;
	TLabel *lblRunNo;
	TLabel *Label4;
	TLabel *lblAlg;
	TLabel *Label6;
	TLabel *lblInstName;
	TMemo *memoTCs;
	TButton *btnReadRuns;
	TProgressBar *pbRun;
	TBitBtn *btnRun;
	TBitBtn *btnClose;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnReadRunsClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnTerminateClick(TObject *Sender);
private:	// User declarations
	DMOSP_Problem *Instance;
	DMOSP_NSGAII *nsga2Alg;
	DMOSP_MOSS *mossAlg;

	struct DMOSP_TreatmentCombination {
	  int W, J;
	  double LL;
	  int PLJ, PBM;
	  int ReplicateNo;
	};

	int nTCs;
	DMOSP_TreatmentCombination *TCs;
	bool bTerminated;

public:		// User declarations
	__fastcall TfrmGASSDOERuns(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGASSDOERuns *frmGASSDOERuns;
//---------------------------------------------------------------------------
#endif
