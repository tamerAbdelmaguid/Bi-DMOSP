//---------------------------------------------------------------------------

#ifndef FormMOSSCalcProgressH
#define FormMOSSCalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "FormCalcProgress.h"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "FormMOSS.h"
#include "MOSS.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include "Main.h"
//---------------------------------------------------------------------------
class TfrmMOSSCalcProgress : public TfrmCalcProgress
{
__published:	// IDE-managed Components
	TLabel *lblCmaxLB;
	TLabel *Label5;
	TLabel *Label10;
	TLabel *lblbestCmax;
	TLabel *Label15;
	TPanel *pnlTabuSearch;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *lblTSCurrentCmax;
	TLabel *lblTSBestCmax;
	TLabel *lblTSIter;
	TLabel *Label4;
	TLabel *lblTSInitialCmax;
	TLabel *Label6;
	TLabel *lblTSImpIter;
	TLabel *Label7;
	TLabel *Label16;
	TLabel *Label19;
	TLabel *lblTSCurrentMWFT;
	TLabel *lblTSBestMWFT;
	TLabel *Label22;
	TLabel *lblTSInitialMWFT;
	TProgressBar *pbTabuSearch;
	TCheckBox *cbxTabuSearch;
	TPanel *pnlConstruction;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label11;
	TStringGrid *grdInitialSolutions;
	TProgressBar *pbConstruction;
	TCheckBox *cbxConstruction;
	TPanel *Panel1;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *lblSCIter;
	TStringGrid *sgSCIterations;
	TProgressBar *pbSCProgress;
	TCheckBox *cbxSolutionCombination;
	TProgressBar *pbSSPRIterations;
	TLabel *lblMWFTLB;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *lblbestMWFT;
	TChart *ParetoChart;
	TPointSeries *psParetoFront;
	TLineSeries *lsCmaxLB;
	TLineSeries *lsMWFTLB;
	TPointSeries *psOptimalParetoFront;
	TPointSeries *psReferenceSet;
	TSaveDialog *dlgSaveWMF;
	TPopupMenu *mnuWMF;
	TMenuItem *ExportWMF1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ExportWMF1Click(TObject *Sender);
private:	// User declarations
	float CLB, FLB;

public:		// User declarations
	__fastcall TfrmMOSSCalcProgress(TComponent* Owner);

	virtual void ClearForm();
	void DrawLBLines(double CmaxLB, double MWFTLB);
	void DrawParetoFront(SList &P);
	void DrawOnlyParetoFront(SList &P);
	void DrawOptimalParetoFront(int i);
    void DrawReferenceSet(SList &R);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMOSSCalcProgress *frmMOSSCalcProgress;
//---------------------------------------------------------------------------
#endif
