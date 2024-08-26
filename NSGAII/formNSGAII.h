//---------------------------------------------------------------------------

#ifndef formNSGAIIH
#define formNSGAIIH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "Turning.h"
#include "GA.h"
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmNSGAII_turning : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label12;
	TLabel *lblGen;
	TButton *btnStop;
	TChart *Chart;
	TPointSeries *Series1;
	TPointSeries *Series2;
	TButton *btnRun;
	TProgressBar *ProgressBar;
	TPointSeries *Series3;
	TPointSeries *Series4;
	TPointSeries *Series5;
	TPointSeries *Series6;
	TPointSeries *Series7;
	TPointSeries *Series8;
	TPointSeries *Series9;
	TPointSeries *Series10;
	TPageControl *pgGAType;
	TTabSheet *TabSheet1;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TRadioGroup *rgNiching;
	TRadioGroup *rgGAType;
	TCheckBox *cbxNiching;
	TEdit *edtNichingRadius;
	TEdit *edtScalingFactor;
	TEdit *edtWindowSize;
	TTabSheet *TabSheet2;
	TRadioGroup *rgScaling;
	TRadioGroup *rgConstraintHandling;
	TTabSheet *TabSheet3;
	TLabel *Label10;
	TLabel *Label11;
	TRadioGroup *rgCrossover;
	TRadioGroup *rgMutation;
	TEdit *edtpMutation;
	TEdit *edtpCrossover;
	TCheckBox *chkLocalSearch;
	TTabSheet *TabSheet4;
	TLabel *Label8;
	TRadioGroup *rgSelection;
	TEdit *edtTournamentSize;
	TTabSheet *TabSheet5;
	TLabel *Label1;
	TCheckBox *chkOut;
	TCheckBox *chkGraphChange;
	TCheckBox *chkOutGraphs;
	TEdit *edtGOutStep;
	TEdit *edtMaxN;
	TGroupBox *gbxGeneralParameters;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *edtPopSize;
	TEdit *edtMaxGenerations;
	TEdit *edtReplaceProportion;
	TLabel *Label9;
	TLabel *lblN;
	TPointSeries *Series11;
	TPointSeries *Series12;
	TPointSeries *Series13;
	TPointSeries *Series14;
	TPointSeries *Series15;
	TPointSeries *Series16;
	TPointSeries *Series17;
	TPointSeries *Series18;
	TPointSeries *Series19;
	TPointSeries *Series20;
	TPopupMenu *mnuSaveImage;
	TMenuItem *SaveimageasWMV1;
	TSaveDialog *dlgSaveWMF;
	TMenuItem *Copyimagetoclipboard1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall rgConstraintHandlingClick(TObject *Sender);
	void __fastcall rgSelectionClick(TObject *Sender);
	void __fastcall cbxNichingClick(TObject *Sender);
	void __fastcall rgGATypeClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall chkOutGraphsClick(TObject *Sender);
	void __fastcall SaveimageasWMV1Click(TObject *Sender);
	void __fastcall Copyimagetoclipboard1Click(TObject *Sender);

private:	// User declarations
	bool bStop;
	int maxN;
	int MaxNoOfDecisionVariables;
	int GraphCount, GOutStep;

	double min_x, max_x, min_y, max_y;  // for controlling graph limits

	void __fastcall ReadSettings();
	void __fastcall FreeMemory();
	void __fastcall UpdateChart(GA *);

public:		// User declarations
	__fastcall TfrmNSGAII_turning(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmNSGAII_turning *frmNSGAII_turning;
//---------------------------------------------------------------------------
#endif
