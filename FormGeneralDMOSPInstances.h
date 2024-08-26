//---------------------------------------------------------------------------

#ifndef FormGeneralDMOSPInstancesH
#define FormGeneralDMOSPInstancesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmGeneralDMOSPInstances : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *btnClose;
	TBitBtn *btnGenerate;
	TComboBox *cbxNoOfPCs;
	TLabel *Label2;
	TLabel *Label1;
	TComboBox *cbxnJobsTonWCs;
	TLabel *Label3;
	TComboBox *cbxqwLevel;
	TLabel *Label4;
	TComboBox *cbxLoadingLevel;
	TLabel *Label5;
	TComboBox *cbxPTF;
	TLabel *Label6;
	TEdit *edtnReplicates;
	TRadioGroup *rgProblemType;
	TLabel *Label7;
	TComboBox *cbxJRTF;
	TLabel *Label8;
	TComboBox *cbxMATF;
	TLabel *Label9;
	TComboBox *cbxPercentLateJobs;
	TLabel *Label10;
	TComboBox *cbxPercentBusyMachines;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnGenerateClick(TObject *Sender);
	void __fastcall rgProblemTypeClick(TObject *Sender);
private:	// User declarations
	void __fastcall UpdateProblemType();

public:		// User declarations
	__fastcall TfrmGeneralDMOSPInstances(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGeneralDMOSPInstances *frmGeneralDMOSPInstances;
//---------------------------------------------------------------------------
#endif
