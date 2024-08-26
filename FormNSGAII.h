//---------------------------------------------------------------------------

#ifndef FormNSGAIIH
#define FormNSGAIIH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include ".\NSGAII\GA.h"
#include <ComCtrls.hpp>
#include "NSGAII.h"

//---------------------------------------------------------------------------
class TfrmNSGAII : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TPageControl *pgGAType;
	TTabSheet *TabSheet3;
	TLabel *Label10;
	TLabel *Label11;
	TRadioGroup *rgCrossover;
	TRadioGroup *rgMutation;
	TEdit *edtpMutation;
	TEdit *edtpCrossover;
	TGroupBox *gbxGeneralParameters;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtPopSize;
	TEdit *edtMaxGenerations;
	TPanel *pnlPareto;
	TLabel *Label30;
	TListBox *lstPareto;
	TLabel *lblGD;
	TLabel *Label32;
	TLabel *lblIGD;
	TLabel *Label34;
	TLabel *Label1;
	TEdit *edtpMutationRLB;
	TEdit *edtMaxTimeLimit;
	TLabel *Label6;
	TTabSheet *tsSA;
	TLabel *Label4;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *edtSAprop;
	TEdit *edtSAT0;
	TEdit *edtSAMaxIter;
	TEdit *edtSACoolingRate;
	TLabel *lblHV;
	TLabel *Label35;
	TLabel *lblHV0;
	TLabel *Label13;
	TLabel *lblHVpr;
	TLabel *Label16;
	TLabel *lblHV0pr;
	TLabel *Label18;
	TLabel *lblTGD;
	TLabel *Label14;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall lstParetoClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);

private:	// User declarations
	int ps, maxgen;
	double timelimit;
	double xPrb, mPrb, rlsMutProb;
	DMOSP_NSGAII::DMOSP_XOVER_OPERATOR xOpr;
	DMOSP_NSGAII::DMOSP_MUTATION_OPERATOR mOpr;
	double SAProp, SAT0, SACoolingRate;
	int SAMaxIter;

	int GOutStep;

	int maxN;
	bool __fastcall ReadSettings();

	DMOSP_NSGAII *nsga2Alg;

public:		// User declarations
	__fastcall TfrmNSGAII(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmNSGAII *frmNSGAII;
//---------------------------------------------------------------------------
#endif
