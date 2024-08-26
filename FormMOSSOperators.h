//---------------------------------------------------------------------------

#ifndef FormMOSSOperatorsH
#define FormMOSSOperatorsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ComCtrls.hpp>
#include "MOSS.h"
//---------------------------------------------------------------------------
class TfrmMOSSOperators : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TComboBox *cbxPopSize;
	TPanel *pnlPopulation;
	TLabel *Label30;
	TListBox *lstPopulation;
	TPanel *pnlChromosome;
	TStringGrid *grdVPBChromosome;
	TPageControl *pcOperators;
	TTabSheet *tsInitialization;
	TStringGrid *sgInitialPop;
	TStringGrid *sgStats;
	TTabSheet *tsFronts;
	TChart *ParetoChart;
	TPointSeries *srsFront3;
	TPointSeries *srsRest;
	TPointSeries *srsFront2;
	TPointSeries *srsFront1;
	TPointSeries *srsFront0;
	TPointSeries *srsFront4;
	TPointSeries *srsFront5;
	TTabSheet *tsSolComb;
	TLabel *Label4;
	TLabel *Label7;
	TStringGrid *grdParents;
	TComboBox *cbxParent1;
	TComboBox *cbxParent2;
	TStringGrid *grdChild;
	TButton *btnApplyXOver;
	TButton *btnAppendxOverChild;
	TTabSheet *tsTS;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label16;
	TLabel *lblCmax;
	TLabel *Label18;
	TLabel *lblMWFT;
	TComboBox *cbxTSStartingSol;
	TButton *btnRunTS;
	TButton *btnRestartTS;
	TEdit *edtTSIterations;
	TProgressBar *pbTSIterations;
	TEdit *edtTabuSize;
	TCheckBox *cbxEnforceDominance;
	TLabel *Label17;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *lblnGenerated;
	TLabel *Label22;
	TLabel *lblnExcluded;
	TLabel *Label23;
	TEdit *edtDeltaThreshold;
	TTabSheet *tbDistance;
	TLabel *Label2;
	TComboBox *cbxDistSol1;
	TComboBox *cbxDistSol2;
	TStringGrid *grdDistSols;
	TButton *btnDistCalc;
	TLabel *Label3;
	TLabel *lblDeltaTau;
	TLabel *Label25;
	TLabel *lblDeltaWC;
	TLabel *Label6;
	TStringGrid *grdxOverPos;
	TLabel *Label8;
	TEdit *edtThetaX;
	TStringGrid *grdTSOriginalSol;
	TStringGrid *grdTSResultantSol;
	TLabel *Label9;
	TLabel *Label10;
	TButton *btnAppendTSResultantSol;
	TLabel *lblTSItr;
	TLabel *Label14;
	TLabel *lblBestCmax;
	TLabel *Label24;
	TLabel *lblBestMWFT;
	TLabel *Label15;
	TLabel *lblNCmax;
	TLabel *Label27;
	TLabel *lblNMWFT;
	TLabel *Label26;
	TLabel *lblnCimp;
	TLabel *Label29;
	TLabel *lblnFimp;
	TLabel *Label28;
	TLabel *Label31;
	TEdit *edtnBestMoves;
	TEdit *edtnRandMoves;
	TTabSheet *TabSheet1;
	TLabel *Label32;
	TComboBox *cbxMWFTNSStartingSol;
	TButton *btnMWFTNSRun;
	TLabel *Label33;
	TComboBox *cbxMWFTNSOperation;
	TStringGrid *grdMWFTNSResults;
	TButton *btnEnumrateAllMWFTNSMoves;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall lstPopulationClick(TObject *Sender);
	void __fastcall cbxDistSol1Change(TObject *Sender);
	void __fastcall cbxDistSol2Change(TObject *Sender);
	void __fastcall btnDistCalcClick(TObject *Sender);
	void __fastcall cbxParent1Change(TObject *Sender);
	void __fastcall cbxParent2Change(TObject *Sender);
	void __fastcall btnApplyXOverClick(TObject *Sender);
	void __fastcall btnAppendxOverChildClick(TObject *Sender);
	void __fastcall cbxTSStartingSolChange(TObject *Sender);
	void __fastcall btnRunTSClick(TObject *Sender);
	void __fastcall btnAppendTSResultantSolClick(TObject *Sender);
	void __fastcall btnRestartTSClick(TObject *Sender);
	void __fastcall cbxMWFTNSStartingSolChange(TObject *Sender);
	void __fastcall btnMWFTNSRunClick(TObject *Sender);
	void __fastcall cbxMWFTNSOperationChange(TObject *Sender);
	void __fastcall btnEnumrateAllMWFTNSMovesClick(TObject *Sender);

private:	// User declarations
	DMOSP_MOSS *moss;
	DMOSP_MOSS_Solution *SC;
	double *deltaMWFT;

	int nTSIterations, TabuSize, nBestMoves, nRandMoves;
	bool ReadTSParameters();

	int nCmaxNs, nMWFTNs;

public:		// User declarations
	bool bActive;
	SList xPos;   // crossover positions for jobs and workcenters
	bool bTSStarted;
	DMOSP_MOSS_Solution *TSSol;

	__fastcall TfrmMOSSOperators(TComponent* Owner);

	void __fastcall IncrementN1N2();
	void __fastcall IncrementN3N4();
    void __fastcall updateNimp(int nCimp, int nFimp);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMOSSOperators *frmMOSSOperators;
//---------------------------------------------------------------------------
#endif
