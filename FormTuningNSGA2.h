//---------------------------------------------------------------------------

#ifndef FormTuningNSGA2H
#define FormTuningNSGA2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include "NSGAII.h"
//---------------------------------------------------------------------------
class TfrmTuningNSGA2 : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label3;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label9;
	TLabel *Label15;
	TLabel *Label4;
	TSpeedButton *btnTerminate;
	TLabel *Label5;
	TLabel *lblTCReadResult;
	TStringGrid *grdLoadedInstances;
	TButton *btnLoadProblems;
	TMemo *memoTime;
	TMemo *memoIGD;
	TProgressBar *pbRun;
	TProgressBar *pbConstruction;
	TProgressBar *pbIterations;
	TMemo *memoGD;
	TBitBtn *btnRun;
	TBitBtn *btnClose;
	TMemo *memoTCs;
	TButton *btnReadRuns;
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
	  DMOSP_NSGAII::DMOSP_XOVER_OPERATOR XOp;
	  DMOSP_NSGAII::DMOSP_MUTATION_OPERATOR MOp;
	  int popsize;
	  double prob_xover;
	  double prob_mut;
	  double prob_RLS;
	  double p_SA;
	  int PSize;
	};

    int nTCs;
	DMOSP_TreatmentCombination *TCs;

	DMOSP_NSGAII *nsga2Alg;

public:		// User declarations
    bool bActive;

	__fastcall TfrmTuningNSGA2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmTuningNSGA2 *frmTuningNSGA2;
//---------------------------------------------------------------------------
#endif
