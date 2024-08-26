//---------------------------------------------------------------------------

#ifndef FormMILPReaderH
#define FormMILPReaderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "PSI.h"
//---------------------------------------------------------------------------
class TfrmMILPSol : public TForm
{
__published:	// IDE-managed Components
	TMemo *memoMILPSolution;
	TBitBtn *btnClose;
	TLabel *Label1;
	TLabel *Label2;
	TBitBtn *btnRun;
	void __fastcall btnRunClick(TObject *Sender);
private:	// User declarations
    DMOSP_MILPSolution *MILPSol;

public:		// User declarations
	__fastcall TfrmMILPSol(TComponent* Owner);
	void TfrmMILPSol::ShowMILP(DMOSP_Solution *Sol);
	void TfrmMILPSol::WriteCurrentMILPSol();

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMILPSol *frmMILPSol;
//---------------------------------------------------------------------------
#endif
