//---------------------------------------------------------------------------

#ifndef FormEvolutionaryOperatorsH
#define FormEvolutionaryOperatorsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "NSGAII.h"
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"

//---------------------------------------------------------------------------
class TfrmEvolOperators : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TComboBox *cbxPopSize;
	TPanel *pnlPopulation;
	TLabel *Label30;
	TListBox *lstPopulation;
	TPageControl *pcEvolOperators;
	TTabSheet *tsSelection;
	TTabSheet *tsCrossover;
	TTabSheet *tsMutation;
	TPanel *pnlChromosome;
	TStringGrid *grdOBChromosome;
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
	TButton *btnSelect;
	TLabel *Label2;
	TStringGrid *sgMatingPool;
	TLabel *Label3;
	TEdit *edtTournamentSize;
	TStringGrid *grdParents;
	TLabel *Label4;
	TComboBox *cbxParent1;
	TComboBox *cbxParent2;
	TStringGrid *grdChildren;
	TComboBox *cbxXOverOperator;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *lblxOverPoints;
	TButton *btnApplyXOver;
	TButton *btnRestart;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *lblMutationPoints;
	TStringGrid *grdOriginalChromosome;
	TComboBox *cbxOriginalChromosome;
	TStringGrid *grdMutatedChromosome;
	TComboBox *cbxMutationOperator;
	TButton *btnApplyMutation;
	TButton *btnRestartMutation;
	TTabSheet *tsSA;
	TLabel *Label11;
	TStringGrid *sgSAOriginalChromosome;
	TComboBox *cbxSAOriginalChromosome;
	TStringGrid *sgSAFinalChromosome;
	TButton *btnRunSA;
	TButton *btnRestartSA;
	TLabel *Label12;
	TEdit *edtnoSAIterations;
	TProgressBar *pbSAIterations;
	TLabel *Label13;
	TEdit *edtInitTemp;
	TLabel *Label14;
	TEdit *edtCoolingFactor;
	TLabel *Label15;
	TLabel *lblTemperature;
	TButton *btnStepSA;
	TLabel *Label16;
	TLabel *lblCmax;
	TLabel *Label18;
	TLabel *lblMWFT;
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall lstPopulationClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnSelectClick(TObject *Sender);
	void __fastcall cbxParent1Change(TObject *Sender);
	void __fastcall cbxParent2Change(TObject *Sender);
	void __fastcall btnApplyXOverClick(TObject *Sender);
	void __fastcall btnRestartClick(TObject *Sender);
	void __fastcall btnRestartMutationClick(TObject *Sender);
	void __fastcall cbxOriginalChromosomeChange(TObject *Sender);
	void __fastcall btnApplyMutationClick(TObject *Sender);
	void __fastcall cbxSAOriginalChromosomeChange(TObject *Sender);
	void __fastcall btnRestartSAClick(TObject *Sender);
	void __fastcall btnRunSAClick(TObject *Sender);
	void __fastcall btnStepSAClick(TObject *Sender);
private:	// User declarations
	DMOSP_NSGAII *nsga2;
	int iParent1, iParent2, iOriginalChromosome, iSAOriginalChromosome;

	int nSAIterations, SAIter;
	double SAInitialTemp, SACoolingFactor;
	bool __fastcall ReadSAParameters();
	DMOSP_Individual *OriginalSAIndividual;

public:		// User declarations
	bool bActive;

	__fastcall TfrmEvolOperators(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEvolOperators *frmEvolOperators;
//---------------------------------------------------------------------------
#endif
