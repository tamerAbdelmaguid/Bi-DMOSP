//---------------------------------------------------------------------------

#ifndef FormSolvedSetMultiRunsH
#define FormSolvedSetMultiRunsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>

#include "NSGAII.h"
#include "MOSS.h"
#include "MOGWO.h"
//---------------------------------------------------------------------------
class TfrmSolvedSetMultiRuns : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label2;
	TSpeedButton *btnTerminate;
	TProgressBar *pbRun;
	TBitBtn *btnRun;
	TBitBtn *btnClose;
	TLabel *lblRunNo;
	TListBox *lstGroups;
	TLabel *Label1;
	TListBox *lstInstances;
	TLabel *Label3;
	TCheckBox *cbxNSGA2;
	TCheckBox *cbxMOGWO;
	TLabel *Label4;
	TLabel *lblAlg;
	TLabel *Label6;
	TLabel *lblInstName;
	TCheckBox *cbxMOSS;
	void __fastcall lstGroupsClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnTerminateClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	DMOSP_Problem *Instance;
	DMOSP_NSGAII *nsga2Alg;
	DMOSP_MOSS *mossAlg;
	DMOSP_MOGWO *mogwoAlg;

public:		// User declarations
	__fastcall TfrmSolvedSetMultiRuns(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSolvedSetMultiRuns *frmSolvedSetMultiRuns;
//---------------------------------------------------------------------------
#endif
