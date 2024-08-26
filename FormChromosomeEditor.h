//---------------------------------------------------------------------------

#ifndef FormChromosomeEditorH
#define FormChromosomeEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include "Metaheuristic.h"
#include "VPBChromosome.h"

//---------------------------------------------------------------------------
class TfrmChromosomeEditor : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TPanel *pnlConstructionAlg;
	TLabel *Label2;
	TComboBox *cbxJSR;
	TComboBox *cbxMSR;
	TLabel *Label3;
	TSpeedButton *btnRunDSG;
	TPanel *pnlChromosome;
	TLabel *Label1;
	TSpeedButton *btnSave;
	TSpeedButton *btnLoad;
	TPageControl *pcChromosome;
	TTabSheet *tsOBChromosome;
	TTabSheet *tsRKChromosome;
	TStringGrid *grdOBChromosome;
	TStringGrid *grdRKChromosome;
	TLabel *Label4;
	TLabel *Label6;
	TLabel *lblCmax;
	TLabel *lblMWFT;
	TTabSheet *tsVPBChromosome;
	TStringGrid *grdVPBChromosome;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunDSGClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnLoadClick(TObject *Sender);
private:	// User declarations
	DMOSP_OBChromosome *OBChromosome;
	DMOSP_RKChromosome *RKChromosome;
	DMOSP_VPBChromosome *VPBChromosome;
	DMOSP_DSG *DSG;
	DMOSP_Metaheuristic *METAH;

	void __fastcall GenerateDSSol();
    void __fastcall CreateChromosome(DMOSP_Solution *Sol);
	void __fastcall DisplayChromosome();
	bool __fastcall ReadOBChromosome();
	void __fastcall InterpretOBChromosome();
	bool __fastcall ReadRKChromosome();
	void __fastcall InterpretRKChromosome();
	bool __fastcall ReadVPBChromosome();
	void __fastcall InterpretVPBChromosome();

	void __fastcall ClearGrids();

public:		// User declarations
	__fastcall TfrmChromosomeEditor(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmChromosomeEditor *frmChromosomeEditor;
//---------------------------------------------------------------------------
#endif
