//---------------------------------------------------------------------------

#ifndef FormNSGA2CalcProgressH
#define FormNSGA2CalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormCalcProgress.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include "NSGAII.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include "Main.h"
//---------------------------------------------------------------------------
class TfrmNSGA2CalcProgress : public TfrmCalcProgress
{
	friend class DMOSP_NSGAII;

__published:	// IDE-managed Components
	TLabel *lblCmaxLB;
	TLabel *Label5;
	TLabel *Label10;
	TLabel *lblbestCmax;
	TPanel *pnlTabuSearch;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *lblSACurrentCmax;
	TLabel *lblSABestCmax;
	TLabel *lblSAIter;
	TLabel *Label4;
	TLabel *lblSAInitialCmax;
	TLabel *Label7;
	TLabel *Label16;
	TLabel *Label19;
	TLabel *lblSACurrentMWFT;
	TLabel *lblSABestMWFT;
	TLabel *Label22;
	TLabel *lblSAInitialMWFT;
	TProgressBar *pbSA;
	TPanel *pnlConstruction;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label11;
	TStringGrid *grdInitialSolutions;
	TProgressBar *pbConstruction;
	TPanel *Panel1;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *lblGeneration;
	TStringGrid *sgGenerations;
	TProgressBar *pbGeneration;
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
	TLabel *Label6;
	TLabel *lblSATemperature;
	TCheckBox *cbxConstruction;
	TCheckBox *cbxGenerations;
	TCheckBox *cbxSA;
	TSaveDialog *dlgSaveWMF;
	TPopupMenu *mnuWMF;
	TMenuItem *ExportWMF1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ExportWMF1Click(TObject *Sender);
private:	// User declarations
	float CLB, FLB;

public:		// User declarations
	virtual void ClearForm();

	__fastcall TfrmNSGA2CalcProgress(TComponent* Owner);

	void DrawLBLines(double CmaxLB, double MWFTLB);
	void DrawFronts();
	void DrawNonDominatedFront();
	void DrawOptimalParetoFront(int i);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmNSGA2CalcProgress *frmNSGA2CalcProgress;
//---------------------------------------------------------------------------
#endif
