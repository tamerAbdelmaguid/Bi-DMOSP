//---------------------------------------------------------------------------

#ifndef FormAlgorithmH
#define FormAlgorithmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include "Problem.h"
#include "DMOSPSolver.h"
//---------------------------------------------------------------------------
class TfrmAlgorithm : public TForm
{
__published:	// IDE-managed Components
        TPanel *pnlSolution;
        TPanel *pnlGTAParameters;
        TLabel *Label5;
        TLabel *lblTime;
        TBitBtn *btnRun;
        TBitBtn *btnClose;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall btnCloseClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
		DMOSP_Problem *Problem;

		__fastcall TfrmAlgorithm(TComponent* Owner);

        virtual void __fastcall DisplaySolution(DMOSP_Solution *Sol);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAlgorithm *frmAlgorithm;
//---------------------------------------------------------------------------
#endif
