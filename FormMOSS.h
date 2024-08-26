//---------------------------------------------------------------------------

#ifndef FormMOSSH
#define FormMOSSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "MOSS.h"
#include "FormMOSSCalcProgress.h"
#include "FormSol.h"
//---------------------------------------------------------------------------
class TfrmMOSS : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TPanel *pnlPareto;
	TLabel *Label30;
	TListBox *lstPareto;
	TLabel *lblGD;
	TLabel *Label32;
	TLabel *lblIGD;
	TLabel *Label34;
	TLabel *lblHV;
	TLabel *Label35;
	TLabel *lblHV0;
	TLabel *Label36;
	TLabel *lblHVpr;
	TLabel *Label33;
	TLabel *lblHV0pr;
	TLabel *Label37;
	TPageControl *pgctrlSSPR;
	TTabSheet *tsSSPRNbrTest;
	TLabel *Label15;
	TLabel *Label7;
	TLabel *Label6;
	TLabel *Label1;
	TLabel *Label10;
	TLabel *Label9;
	TLabel *Label2;
	TComboBox *cbxOps;
	TBitBtn *btnNSParameters;
	TEdit *edtMCx2;
	TEdit *edtMCx1;
	TComboBox *cbxFromMC;
	TEdit *edtJobx2;
	TEdit *edtJobx1;
	TComboBox *cbxJobs;
	TBitBtn *btnB1Parameters;
	TBitBtn *btnB2Parameters;
	TBitBtn *btnEnumerate;
	TTabSheet *tsSSPRLB;
	TSpeedButton *btnLB;
	TLabel *Label8;
	TLabel *Label11;
	TLabel *lblLB1;
	TLabel *lblLB2;
	TLabel *Label12;
	TLabel *lblLB;
	TLabel *lblLB3;
	TLabel *Label14;
	TLabel *Label28;
	TLabel *Label29;
	TTabSheet *tsSSPRRun;
	TLabel *Label13;
	TLabel *Label16;
	TLabel *lblStatus;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label31;
	TEdit *edtRSize;
	TEdit *edtDeltaThreshold;
	TEdit *edtTabuSize;
	TEdit *edtnTSIterations;
	TEdit *edtnImpIterations;
	TEdit *edtTSnBestMoves;
	TEdit *edtTSnRandomMoves;
	TEdit *edtnSSPRIterations;
	TEdit *edtTheta_X;
	TEdit *edtTimeLimit;
	TLabel *lblTGD;
	TLabel *Label4;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnLBClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall lstParetoClick(TObject *Sender);
private:	// User declarations
	DMOSP_MOSS *mossAlg;
	DMOSP_MOSS *tmp_mossAlg;

public:		// User declarations
	__fastcall TfrmMOSS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMOSS *frmMOSS;
//---------------------------------------------------------------------------
#endif
