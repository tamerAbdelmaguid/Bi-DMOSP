//---------------------------------------------------------------------------

#ifndef FormMOGWOH
#define FormMOGWOH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "MOGWO.h"

//---------------------------------------------------------------------------
class TfrmMOGWO : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TPanel *pnlPareto;
	TLabel *Label30;
	TListBox *lstPareto;
	TLabel *lblGD;
	TLabel *Label32;
	TLabel *lblIGD;
	TLabel *Label34;
	TPageControl *pgMOGWOParameters;
	TTabSheet *TabSheet3;
	TTabSheet *tsSA;
	TLabel *Label4;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *edtSAprop;
	TEdit *edtSAT0;
	TEdit *edtSAMaxIter;
	TEdit *edtSACoolingRate;
	TRadioGroup *rgMutation;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label6;
	TLabel *Label1;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TEdit *edtnGreyWolves;
	TEdit *edtMaxIterations;
	TEdit *edtMaxTimeLimit;
	TEdit *edtnGrids;
	TEdit *edtArchiveSize;
	TEdit *edtAlpha;
	TEdit *edtBeta;
	TEdit *edtGamma;
	TLabel *lblHV;
	TLabel *Label35;
	TLabel *lblHV0;
	TLabel *Label10;
	TLabel *lblHVpr;
	TLabel *Label16;
	TLabel *lblHV0pr;
	TLabel *Label18;
	TLabel *lblTGD;
	TLabel *Label17;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall lstParetoClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
private:	// User declarations
	DMOSP_MOGWO *mogwo;

	// MOGWO parameters
	int nGWs, nIter;
	double timelimit;
	int szArch, ngrd;
	double a, b, g;
	DMOSP_MOGWO::DMOSP_MUTATION_OPERATOR mOpr;
	double SAProp, SAT0, SACoolingRate;
	int SAMaxIter;

	int GOutStep;

	int maxN;
	bool __fastcall ReadSettings();

public:		// User declarations
	__fastcall TfrmMOGWO(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMOGWO *frmMOGWO;
//---------------------------------------------------------------------------
#endif
