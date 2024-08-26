//---------------------------------------------------------------------------

#ifndef FormPSEditorH
#define FormPSEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include "DMOSPSolver.h"
#include "PSI.h"
#include <ComCtrls.hpp>
#include "MOSS.h"
//---------------------------------------------------------------------------
class TfrmPSEditor : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TStringGrid *grdPS;
	TSpeedButton *btnRunDSG;
	TSpeedButton *btnLoad;
	TSpeedButton *btnSave;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *cbxJSR;
	TComboBox *cbxMSR;
	TStringGrid *grdOO;
	TLabel *Label4;
	TPageControl *pgEditor;
	TTabSheet *tbOpRemover;
	TTabSheet *tbMCPermutation;
	TSpeedButton *btnRemoveOp;
	TComboBox *cbxOps;
	TButton *btnInitialize;
	TButton *btnInsert;
	TTabSheet *tbJobPermutation;
	TComboBox *cbxFromMC;
	TComboBox *cbxJobs;
	TEdit *edtMCx1;
	TEdit *edtMCx2;
	TLabel *Label6;
	TLabel *Label7;
	TButton *btnMCRemove;
	TEdit *edtMCInsertX;
	TLabel *Label8;
	TButton *btnMCInsert;
	TEdit *edtJobx1;
	TEdit *edtJobx2;
	TLabel *Label9;
	TLabel *Label10;
	TButton *btnJobRemove;
	TEdit *edtJobInsertX;
	TLabel *Label11;
	TButton *btnJobInsertX;
	TComboBox *cbxToMC;
	TPanel *pnlAlgParamaeters;
	TPageControl *pgAlgPars;
	TTabSheet *tbNfn;
	TTabSheet *tsBfn;
	TStringGrid *grdNbSets;
	TEdit *edtSetI;
	TEdit *edtSetS;
	TLabel *Label14;
	TLabel *Label13;
	TLabel *Label12;
	TLabel *Label15;
	TRadioGroup *rgRemoveFrom;
	TComboBox *cbxMachines;
	TLabel *Label16;
	TEdit *edtInPos;
	TLabel *Label17;
	TPageControl *PageControl1;
	TTabSheet *tsAprVs;
	TTabSheet *tsEstVs;
	TTabSheet *tsActVs;
	TStringGrid *sgActVs;
	TStringGrid *sgRemovalValues;
	TStringGrid *sgEstVs;
	TStringGrid *sgAprVs;
	TStringGrid *sgB1AlphaOmega;
	TBitBtn *btnMILPSol;
	TStringGrid *sgAprMWFT;
	TStringGrid *sgEstMWFT;
	TStringGrid *sgActMWFT;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunDSGClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnLoadClick(TObject *Sender);
	void __fastcall btnRemoveOpClick(TObject *Sender);
	void __fastcall btnInitializeClick(TObject *Sender);
	void __fastcall btnInsertClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnMCRemoveClick(TObject *Sender);
	void __fastcall btnMCInsertClick(TObject *Sender);
	void __fastcall btnJobRemoveClick(TObject *Sender);
	void __fastcall btnJobInsertXClick(TObject *Sender);
	void __fastcall cbxToMCChange(TObject *Sender);
	void __fastcall cbxMachinesChange(TObject *Sender);
	void __fastcall btnMILPSolClick(TObject *Sender);
private:	// User declarations
	DMOSP_DSG *DSG;
	DMOSP_Solution *LoadedSol;
	DMOSP_VPS *VPS;
    DMOSP_VOO *VOO;
	DMOSP_PSI *PSI;
	DMOSP_NSTester *NSTester;
	DMOSP_MOSS *moss;

	SList Ops;  // a list of currently included operations in the drop down box

    DMOSP_Solution *OriginalSol;
	DMOSP_VectorSol *Vec;
	DMOSP_VectorSol *Vec_y;
	DMOSP_VectorSol *Vec_rho;
	DMOSP_Network *Net;  // Network Gamma associated with current solution
	DMOSP_Network *Net_y; // Network Gamma^-y where y in {1,2} for N1 and N2
    DMOSP_Network *Net_rho; // Network Gamma^-rho for B1 and B2

	int x, x1, x2;
    DMOSP_Problem::SOperation *v_y;
	DMOSP_Problem::SMC *from_mc, *to_mc;
	DMOSP_Problem::SJob *fromto_job;

	DMOSP_Solution *MILPSol;

public:		// User declarations
	__fastcall TfrmPSEditor(TComponent* Owner);

	void __fastcall GenerateDSSol();

	void __fastcall DisplayVPS(DMOSP_VPS *vps);
	void __fastcall UpdateVPS(DMOSP_Solution *Sol); // this also updates VPS
	bool __fastcall ReadVPS();

	void __fastcall DisplayVOO(DMOSP_VOO *voo);
	void __fastcall UpdateVOO(DMOSP_Solution *Sol); // this also updates VPS
	bool __fastcall ReadVOO();

	bool __fastcall InterpretVectors();

	void __fastcall DisplayVector(DMOSP_VectorSol *V); // Display current vector
	                                                  // solution (Vec) in tables

    void __fastcall ClearNTables();
	void __fastcall DisplayN1Tables();
	void __fastcall DisplayN2Tables();
	void __fastcall DisplayN3Tables();
	void __fastcall DisplayN4Tables();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPSEditor *frmPSEditor;
//---------------------------------------------------------------------------
#endif
