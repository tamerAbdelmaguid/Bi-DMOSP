//----------------------------------------------------------------------------
#ifndef MainH
#define MainH
//----------------------------------------------------------------------------
#include "FormProblem.h"
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Messages.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <ToolWin.hpp>
#include "FormMOSS.h"
#include "FormMOSSOperators.h"
//----------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *FileNewItem;
	TMenuItem *FileOpenItem;
	TMenuItem *FileCloseItem;
	TMenuItem *Window1;
	TMenuItem *Help1;
	TMenuItem *N1;
	TMenuItem *FileExitItem;
	TMenuItem *WindowCascadeItem;
	TMenuItem *WindowTileItem;
	TMenuItem *WindowArrangeItem;
	TMenuItem *HelpAboutItem;
	TOpenDialog *OpenDialog;
	TMenuItem *FileSaveItem;
	TMenuItem *FileSaveAsItem;
	TMenuItem *Edit1;
	TMenuItem *CutItem;
	TMenuItem *CopyItem;
	TMenuItem *PasteItem;
	TMenuItem *WindowMinimizeItem;
        TStatusBar *StatusBar;
        TActionList *ActionList1;
        TEditCut *EditCut1;
        TEditCopy *EditCopy1;
        TEditPaste *EditPaste1;
        TAction *FileNew1;
        TAction *FileSave1;
        TAction *FileExit1;
        TAction *FileOpen1;
        TAction *FileSaveAs1;
        TWindowCascade *WindowCascade1;
        TWindowTileHorizontal *WindowTileHorizontal1;
        TWindowArrange *WindowArrangeAll1;
        TWindowMinimizeAll *WindowMinimizeAll1;
        TAction *HelpAbout1;
        TWindowClose *FileClose1;
        TWindowTileVertical *WindowTileVertical1;
        TMenuItem *WindowTileItem2;
        TToolBar *ToolBar2;
        TToolButton *btnOpen;
        TToolButton *btnSave;
        TToolButton *ToolButton3;
        TToolButton *btnCut;
        TToolButton *btnCopy;
        TToolButton *btnPaste;
        TToolButton *ToolButton7;
        TToolButton *ToolButton8;
        TToolButton *btnNew;
        TToolButton *ToolButton10;
        TToolButton *ToolButton11;
        TImageList *ImageList1;
        TSaveDialog *SaveDialog;
	TSaveDialog *SaveDATDialog;
        TMenuItem *Exportto;
        TMenuItem *LNGfile1;
        TMenuItem *mnuSolve;
        TOpenDialog *OpenDialogTestProblem;
        TMenuItem *mnuPSEditor;
	TMenuItem *GenerateMattasProblems1;
	TMenuItem *GenerategeneralMOSPinstances1;
	TMenuItem *mSSPRMultiplerunsforCmaxandMWFT1;
	TMenuItem *NSGAIIforCmaxandMWFT1;
	TMenuItem *N3;
	TMenuItem *EvolutionarySolutionRepresentationEditor1;
	TMenuItem *EvolutionaryOperators1;
	TMenuItem *N5;
	TMenuItem *MOGWOforCmaxandMWFT1;
	TMenuItem *ConducttuningrunsforNSGAII1;
	TMenuItem *ConducttuningrunsforMOGWO1;
	TMenuItem *N6;
	TMenuItem *N7;
	TMenuItem *Conductmultiplerunsforsmallbenchmarkinstances1;
	TMenuItem *DOErunsforlargeinstances1;
	TMenuItem *MOSSforCmaxandMWFT1;
	TMenuItem *MOSSOoperators1;
	TMenuItem *ConducttuningrunsforMOSS1;
	TMenuItem *ConductadditionaltuningrunsforMOSS1;
	TMenuItem *DOENSGAIIxMOSSrunsforlargeinstances1;
        void __fastcall FileNew1Execute(TObject *Sender);
        void __fastcall FileOpen1Execute(TObject *Sender);
        void __fastcall HelpAbout1Execute(TObject *Sender);
        void __fastcall FileExit1Execute(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FileSaveItemClick(TObject *Sender);
        void __fastcall FileSaveAsItemClick(TObject *Sender);
        void __fastcall LNGfile1Click(TObject *Sender);
        void __fastcall mnuPSEditorClick(TObject *Sender);
	void __fastcall GenerateMattasProblems1Click(TObject *Sender);
	void __fastcall GenerategeneralMOSPinstances1Click(TObject *Sender);
	void __fastcall NSGAIIforCmaxandMWFT1Click(TObject *Sender);
	void __fastcall EvolutionarySolutionRepresentationEditor1Click(TObject *Sender);
	void __fastcall EvolutionaryOperators1Click(TObject *Sender);
	void __fastcall MOGWOforCmaxandMWFT1Click(TObject *Sender);
	void __fastcall ConducttuningrunsforNSGAII1Click(TObject *Sender);
	void __fastcall ConducttuningrunsforMOGWO1Click(TObject *Sender);
	void __fastcall Conductmultiplerunsforsmallbenchmarkinstances1Click(TObject *Sender);
	void __fastcall DOErunsforlargeinstances1Click(TObject *Sender);
	void __fastcall MOSSforCmaxandMWFT1Click(TObject *Sender);
	void __fastcall MOSSOoperators1Click(TObject *Sender);
	void __fastcall ConducttuningrunsforMOSS1Click(TObject *Sender);
	void __fastcall ConductadditionaltuningrunsforMOSS1Click(TObject *Sender);
	void __fastcall DOENSGAIIxMOSSrunsforlargeinstances1Click(TObject *Sender);




private:
	void __fastcall CreateMDIChild(const String Name);
public:
	virtual __fastcall TMainForm(TComponent *Owner);
        void __fastcall UpdateWorkingDirectory(String newDir);

        String WorkingDirectory;
        TfrmProblem *CurrentChild;
};
//----------------------------------------------------------------------------
extern TMainForm *MainForm;
extern TfrmProblem *__fastcall MDIChildCreate(void);
//----------------------------------------------------------------------------
#endif    
