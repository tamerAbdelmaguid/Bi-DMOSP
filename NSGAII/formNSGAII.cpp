//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "formNSGAII.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TfrmNSGAII_turning *frmNSGAII_turning;

//---------------------------------------------------------------------------
__fastcall TfrmNSGAII_turning::TfrmNSGAII_turning(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::FormShow(TObject *Sender)
{
  globalSetup = new GlobalSetup;

  rgGAType->ItemIndex = 1;
  edtPopSize->Text = "100";
  edtMaxGenerations->Text = "5000";
  edtReplaceProportion->Text = "0.9";

  chkLocalSearch->Checked = true;

  // Niching and related data
  //
  cbxNiching->Checked = false;
  rgNiching->Enabled = false;
  edtNichingRadius->Text = "4.24";
  edtScalingFactor->Text = "1.0";
  edtWindowSize->Text = "4";
  edtNichingRadius->Enabled = false;
  edtScalingFactor->Enabled = false;
  edtWindowSize->Enabled = false;

  rgScaling->ItemIndex = 0;
  rgConstraintHandling->ItemIndex = 0;
  rgMutation->ItemIndex = 2;
  rgCrossover->ItemIndex = 2;

  rgSelection->ItemIndex = 0;
  edtTournamentSize->Text = "10";

  edtpCrossover->Text = "0.9";
  edtpMutation->Text = "0.1";

  edtGOutStep->Text = "10";
  edtGOutStep->Enabled = false;

  edtMaxN->Text = "10";
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::FormClose(TObject *Sender, TCloseAction &Action)
{
  delete globalSetup;
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::btnRunClick(TObject *Sender)
{
  bStop = false;
  btnRun->Visible = false;

  ReadSettings();  // Read NSGA-II settings and allocate memory

  Series1->Clear();
  Series2->Clear();
  Series3->Clear();
  Series4->Clear();
  Series5->Clear();
  Series6->Clear();
  Series7->Clear();
  Series8->Clear();
  Series9->Clear();
  Series10->Clear();
  Series11->Clear();
  Series12->Clear();
  Series13->Clear();
  Series14->Clear();
  Series15->Clear();
  Series16->Clear();
  Series17->Clear();
  Series18->Clear();
  Series19->Clear();
  Series20->Clear();

  ProgressBar->Min = 0;
  ProgressBar->Max = globalSetup->maxGenerations * maxN;
  ProgressBar->Position = 0;

  min_x = 10000;
  max_x = 0;
  min_y = 1000;
  max_y = 0;

  GA *ga;

  for(curN=1; curN<=maxN && !bStop; curN++){
	globalSetup->noOfDecisionVariables = 2+curN;  // V, f and tool wear compensations

	//--------------------------------------------------------------- Constraints
	//
	globalSetup->noOfRawConstraints = 9*curN;  // number of constraints
	globalSetup->noOfLinearConstraintCombinations = 0;
	globalSetup->finalNoOfConstraints = globalSetup->noOfRawConstraints;

	// penalty weights
	globalSetup->penaltyWeights = new double[globalSetup->finalNoOfConstraints];
	for(int ii = 0; ii < globalSetup->finalNoOfConstraints; ii++) {
	  globalSetup->penaltyWeights[ii] = 1;
	}

	ga = new GA();

	if (globalSetup->gaType == SGA) {
		while(ga->generate() && !bStop){
		  ProgressBar->Position = ProgressBar->Position + 1;
		  lblGen->Caption = IntToStr(ProgressBar->Position % globalSetup->maxGenerations);
		  lblN->Caption = IntToStr(curN);
		  Application->ProcessMessages();
		}
	}
	else {
		while(ga->nsgaGenerate() && !bStop){
		  ProgressBar->Position = ProgressBar->Position + 1;
		  lblGen->Caption = IntToStr(ProgressBar->Position % globalSetup->maxGenerations);
		  lblN->Caption = IntToStr(curN);
		  UpdateChart(ga);
		  Application->ProcessMessages();
		  UpdateChart(ga);
		}
	}

	// Save the latest population results
	//
	if(globalSetup->savePopulation){
		int ii;
		FILE *outEvals = fopen(globalSetup->saveEvalSolutions, "a");
		fprintf(outEvals, "\n\nAll solutions at N = %i\n", curN);
		NsgaPopulation *pop = (NsgaPopulation *)ga->getPopulation();
		for(int i=0; i < globalSetup->populationSize; i++){
		  NsgaIndividual *guy = (NsgaIndividual *)((*pop)[i]);
		  fprintf(outEvals, "%i\t", curN);
		  for(ii = 0; ii < globalSetup->noOfDecisionVariables; ii++) {
			fprintf(outEvals, "%f\t", (*guy)[ii]);
		  }
		  for(ii = 0; ii < globalSetup->finalNoOfObjectives; ii++) {
			fprintf(outEvals, "%f\t", guy->getObjective(ii));
		  }
		  if(globalSetup->finalNoOfConstraints > 0) {
			for(ii = 0; ii < globalSetup->finalNoOfConstraints; ii++) {
			  fprintf(outEvals, "%f\t", guy->getViolation(ii));
			}
			fprintf(outEvals, "%f", guy->getPenalty());
		  }
		  fprintf(outEvals, "\n");
		}

		// Save the pareto front
		fprintf(outEvals, "\n\nPareto front at N=%i\n", curN);
		for(int i=0; i<globalSetup->populationSize; i++){
		  NsgaIndividual *guy = (NsgaIndividual *)((*pop)[i]);
		  fprintf(outEvals, "%i\t", curN);
		  bool bDominated = false;
		  double y = guy->getObjective(0);
		  double x = guy->getObjective(1);

		  for(int j=0; j<globalSetup->populationSize; j++){
			if(j==i) continue;
			NsgaIndividual *guy2 = (NsgaIndividual *)((*pop)[j]);
			if(x < guy2->getObjective(1) && y > guy2->getObjective(0)){
			bDominated = true;
			break;
			}
		  }
		  if(!bDominated){
			for(ii = 0; ii < globalSetup->noOfDecisionVariables; ii++) {
			  fprintf(outEvals, "%f\t", (*guy)[ii]);
			}
			for(ii = 0; ii < globalSetup->finalNoOfObjectives; ii++) {
			  fprintf(outEvals, "%f\t", guy->getObjective(ii));
			}
			if(globalSetup->finalNoOfConstraints > 0) {
			  for(ii = 0; ii < globalSetup->finalNoOfConstraints; ii++) {
				fprintf(outEvals, "%f\t", guy->getViolation(ii));
			  }
			  fprintf(outEvals, "%f", guy->getPenalty());
			}
			fprintf(outEvals, "\n");
		  }
		}

		fflush(outEvals);
		fclose(outEvals);
	}

	delete ga;
	delete [] globalSetup->penaltyWeights;
  }

  FreeMemory();

  bStop = false;
  btnRun->Visible = true;

  GraphCount = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::ReadSettings()
{
	////////////////////////////////////////////////////////////////////
	// Here, the NSGA-II parameters and problem structure is defined  //
	////////////////////////////////////////////////////////////////////

  AnsiString s;
  int ii;
  FILE *fOutput;

  s = edtMaxN->Text;
  maxN = StrToInt(s);    // maximum number of parts in the search
  if(maxN > MAX_N_PARTS) maxN = MAX_N_PARTS;

  switch(rgGAType->ItemIndex){
	case 0:  globalSetup->gaType = SGA;  break;
	case 1:  globalSetup->gaType = NSGA;
  }

  //------------------------------------------------------ Decision variables
  // Here, the memory for decision variables is allocated so that there are
  // a maximum of 10 parts. However, the noOfDecisionVariables will change
  // based on the current number of parts.
  //
  MaxNoOfDecisionVariables = 2 + maxN;  // V, f and maxN tool wear compensations

  globalSetup->variableTypes = new VariableType[MaxNoOfDecisionVariables];
  globalSetup->variableRanges = new double*[MaxNoOfDecisionVariables];

  // all variables are real
  for(ii = 0; ii < MaxNoOfDecisionVariables; ii++) {
	globalSetup->variableTypes[ii] = REAL;
  }

  //-------------------------- Upper and lower bounds of the decision variables
  //
  for(ii = 0; ii < MaxNoOfDecisionVariables; ii++) {
	globalSetup->variableRanges[ii] = new double[2];
  }

  // speed of cut (V)
  globalSetup->variableRanges[0][0] = Problem.V_min;  // lower bound
  globalSetup->variableRanges[0][1] = Problem.V_max;  // upper bound

  // feed (f)
  globalSetup->variableRanges[1][0] = Problem.f_min;  // lower bound
  globalSetup->variableRanges[1][1] = Problem.f_max;  // upper bound

  // Tool wear compensations
  for(ii=2; ii<MaxNoOfDecisionVariables; ii++){
	globalSetup->variableRanges[ii][0] = 0;    // lower bound
	globalSetup->variableRanges[ii][1] = Problem.W_max * Problem.k_g;
											   // upper bound
  }

  //------------------------------------------------------- Objective functions
  //
  globalSetup->noOfRawObjectives = 2;  // two objective functions
  globalSetup->finalNoOfObjectives = globalSetup->noOfRawObjectives;
  globalSetup->noOfLinearObjectiveCombinations = 0;

  globalSetup->typeOfOptimizations = new OptimType[globalSetup->finalNoOfObjectives];
  globalSetup->typeOfOptimizations[0] = Maximization;  // maximize throughput
  globalSetup->typeOfOptimizations[1] = Minimization;  // minimize max. deviation from target
													   // diameter

  //----------------------------------------------- General parameters for MOGA
  //
  s = edtPopSize->Text;
  globalSetup->populationSize = StrToInt(s);
  if((globalSetup->populationSize)%10) globalSetup->populationSize += (globalSetup->populationSize)%10;

  s = edtMaxGenerations->Text;
  globalSetup->maxGenerations = StrToInt(s);
  if((globalSetup->maxGenerations)%10) globalSetup->maxGenerations += 10-(globalSetup->maxGenerations)%10;

  s = edtReplaceProportion->Text;
  globalSetup->replaceProportion = StrToFloat(s);

  if(!cbxNiching->Checked){
	globalSetup->nichingType = NoNiching;
	globalSetup->nichingParameters = NULL;
  }
  else{
	switch(rgNiching->ItemIndex){
	  case 0:  // Fitness sharing
			   globalSetup->nichingType = Sharing;
			   break;

	  case 1:  // Deterministic crowding
			   globalSetup->nichingType = DeterministicCrowding;
			   break;

	  case 2:  // Restricted tournament selection
			   globalSetup->nichingType = RTS;
	}
  }

  switch(globalSetup->nichingType) {
	case NoNiching:
	case DeterministicCrowding:
		 // no extra parameters
		 break;

	case Sharing:
		 globalSetup->nichingParameters = new double[2];
		 s = edtNichingRadius->Text;
		 ((double *)(globalSetup->nichingParameters))[0] = StrToFloat(s);
		 s = edtScalingFactor->Text;
		 ((double*)(globalSetup->nichingParameters))[1] = StrToFloat(s);
		 break;

	case RTS:
		 globalSetup->nichingParameters = new int[1];
		 s = edtWindowSize->Text;
         ((int*)(globalSetup->nichingParameters))[0] = StrToInt(s);

	     // the window size should be in (0, populationSize]
		 if ((((int*)globalSetup->nichingParameters)[0] <= 0) || (((int*)globalSetup->nichingParameters)[0] > globalSetup->populationSize)) {
		   Application->MessageBoxA(L"window size should be at least as population size / 20", L"incorrect window size", mrOk);
		   delete [](globalSetup->penaltyWeights);
		   delete [](globalSetup->variableTypes);
		   for(ii = 0; ii < globalSetup->noOfDecisionVariables; ii++) {
			 delete [](globalSetup->variableRanges[ii]);
		   }
		   delete [](globalSetup->variableRanges);
		   delete [](globalSetup->typeOfOptimizations);
           delete [](int*)(globalSetup->nichingParameters);
		   return;
		 }
		 break;
  }

  globalSetup->scalingMethod = (ScalingMethod)rgScaling->ItemIndex;
  if(globalSetup->scalingMethod == SigmaScaling){
	globalSetup->scalingParameters = new double[1];
	((double*)globalSetup->scalingParameters)[0] = 1.0; // Use the default value
  }

  if(rgConstraintHandling->ItemIndex == 0) globalSetup->constraintMethod = Tournament;
  else globalSetup->constraintMethod = Penalty;
  if(rgConstraintHandling->ItemIndex == 1) globalSetup->penaltyFunction = Linear;
  else globalSetup->penaltyFunction = Quadratic;

  globalSetup->xOverType = (XOverType)rgCrossover->ItemIndex;
  switch(globalSetup->xOverType) {
	case OnePoint:
	case TwoPoint:
	  //no extra parameters
	  break;

	case Uniform:
	  globalSetup->xOverParameters = new double[1];
	  ((double*)globalSetup->xOverParameters)[0] = 0.5; // genewise probability
	  break;

	case SBX:
	  globalSetup->xOverParameters = new double[2];
	  ((double*)globalSetup->xOverParameters)[0] = 0.5; // genewise probability
	  ((double*)globalSetup->xOverParameters)[1] = 10; // polynomial order
	  break;
  }
  s = edtpCrossover->Text;
  globalSetup->xOverProbability = StrToFloat(s);

  globalSetup->mutationType = (MutationType)rgMutation->ItemIndex;
  s = edtpMutation->Text;
  globalSetup->mutationProbability = StrToFloat(s);
  switch(globalSetup->mutationType) {
	case Selective:
	  // no extra parameters
	  break;

	case Polynomial:
	  globalSetup->mutationParameters = new int[1];
	  ((int*)globalSetup->mutationParameters)[0] = 20;
	  break;

	case Genewise:
	  globalSetup->mutationParameters = new double[globalSetup->noOfDecisionVariables];
	  for (ii = 0 ; ii < globalSetup->noOfDecisionVariables ; ii++) {
		((double*)globalSetup->mutationParameters)[ii] = 0.1*(globalSetup->variableRanges[ii][1] - globalSetup->variableRanges[ii][0]);
	  }
	  break;
  }

  if(chkLocalSearch->Checked) globalSetup->localSearchMethod = SimplexSearch;
  else globalSetup->localSearchMethod = NoLocalSearch;
  globalSetup->maxLocalTolerance = 1.0E-3;
  globalSetup->maxLocalEvaluations = 20;
  globalSetup->initialLocalPenaltyParameter = 1.0;
  globalSetup->localUpdateParameter = 2.0;
  globalSetup->lamarckianProbability = 0.15;
  globalSetup->localSearchProbability = 0.5;

  globalSetup->selectionType = (SelectionType)rgSelection->ItemIndex;
  switch(globalSetup->selectionType) {
	case TournamentWOR:
	case TournamentWR:
	case Truncation:
	  globalSetup->selectionParameters = new int[1];
	  s = edtTournamentSize->Text;
	  ((int*)globalSetup->selectionParameters)[0] = StrToInt(s);
	  break;

	case SUS:
	case RouletteWheel:
	  // no extra parameters
	  break;
  }

  //----------------------------------------------------------- File output
  //
  globalSetup->noOfStoppingCriterias = 0;
  globalSetup->loadPopulation = false;
  globalSetup->evaluateAgain = true;

  globalSetup->savePopulation = chkOut->Checked;
  globalSetup->saveEvalSolutions = new char[256];
  globalSetup->saveEvalSolutions = "Solutions.txt";

  s = edtGOutStep->Text;
  GOutStep = StrToInt(s);
}

void __fastcall TfrmNSGAII_turning::FreeMemory()
{
  int ii;

  // free all allocated memory
  //
  delete [](globalSetup->penaltyWeights);
  delete [](globalSetup->variableTypes);
  for(ii = 0; ii < MaxNoOfDecisionVariables; ii++) {
	delete [](globalSetup->variableRanges[ii]);
  }
  delete [](globalSetup->variableRanges);
  delete [](globalSetup->typeOfOptimizations);
  if((globalSetup->selectionType == TournamentWR)||(globalSetup->selectionType == TournamentWOR)||(globalSetup->selectionType == Truncation)) {
	delete [](int*)(globalSetup->selectionParameters);
  }
  if((globalSetup->xOverType==Uniform)||(globalSetup->xOverType == SBX))
	delete [](double*)(globalSetup->xOverParameters);
  if(globalSetup->mutationType == Polynomial)
	delete [](int*)(globalSetup->mutationParameters);
  else if(globalSetup->mutationType == Genewise)
	delete [](double*)(globalSetup->mutationParameters);
  if(globalSetup->nichingType == RTS)
	delete [](int*)(globalSetup->nichingParameters);
  if(globalSetup->nichingType == Sharing)
	delete [](double*)(globalSetup->nichingParameters);
  if(globalSetup->scalingMethod == SigmaScaling)
	delete [](double*)(globalSetup->scalingParameters);
  if(globalSetup->noOfStoppingCriterias > 0) {
	delete [] globalSetup->otherStoppingCriteria;
	delete [](double*)(globalSetup->stoppingParameter);
  }
  if(globalSetup->loadPopulation)
	delete [](globalSetup->populationFileName);
  if(globalSetup->savePopulation)
	delete [](globalSetup->saveEvalSolutions);
}

void __fastcall TfrmNSGAII_turning::rgConstraintHandlingClick(TObject *)
{
  if(rgGAType->ItemIndex == 1 && rgConstraintHandling->ItemIndex != 0){
	rgConstraintHandling->ItemIndex = 0;
	Application->MessageBoxA(L"Penalty based constraint handling method cannot be used with NSGA", L"Error", mrOk);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::rgSelectionClick(TObject *)
{
  if(rgGAType->ItemIndex == 1 && (rgSelection->ItemIndex == 3 || rgSelection->ItemIndex == 4)){
	rgSelection->ItemIndex = 0;
	Application->MessageBoxA(L"With NSGA, valid selection methods are: TournamentWOR, TournamentWR, and Truncation", L"Error", mrOk);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::cbxNichingClick(TObject *)
{
  if(rgGAType->ItemIndex == 1){
	rgSelection->ItemIndex = 0;
	Application->MessageBoxA(L"With NSGA, it must be no niching", L"Error", mrOk);
  }
  if(!cbxNiching->Checked){
	rgNiching->Enabled = false;
	edtNichingRadius->Enabled = false;
	edtScalingFactor->Enabled = false;
	edtWindowSize->Enabled = false;
  }
  else{
	rgNiching->Enabled = true;
	edtNichingRadius->Enabled = true;
	edtScalingFactor->Enabled = true;
	edtWindowSize->Enabled = true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::rgGATypeClick(TObject *)
{
  if(rgGAType->ItemIndex == 1){
	cbxNiching->Checked = false;
	cbxNiching->Enabled = false;
	rgNiching->Enabled = false;
	edtNichingRadius->Enabled = false;
	edtScalingFactor->Enabled = false;
	edtWindowSize->Enabled = false;

	rgConstraintHandling->ItemIndex = 0;
	rgSelection->ItemIndex = 0;
	rgSelection->ItemIndex = 0;
  }
  else{
	cbxNiching->Enabled = true;
	rgNiching->Enabled = true;
	edtNichingRadius->Enabled = true;
	edtScalingFactor->Enabled = true;
	edtWindowSize->Enabled = true;

	if(!cbxNiching->Checked){
	  rgNiching->Enabled = false;
	  edtNichingRadius->Enabled = false;
	  edtScalingFactor->Enabled = false;
	  edtWindowSize->Enabled = false;
	}
	else{
	  rgNiching->Enabled = true;
	  edtNichingRadius->Enabled = true;
	  edtScalingFactor->Enabled = true;
	  edtWindowSize->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::UpdateChart(GA *ga)
{
  NsgaPopulation *pop = (NsgaPopulation *)ga->getPopulation();

  TPointSeries *Points[10];        // all points
  Points[0] = dynamic_cast<TPointSeries *>(Series1);
  Points[1] = dynamic_cast<TPointSeries *>(Series3);
  Points[2] = dynamic_cast<TPointSeries *>(Series5);
  Points[3] = dynamic_cast<TPointSeries *>(Series7);
  Points[4] = dynamic_cast<TPointSeries *>(Series9);
  Points[5] = dynamic_cast<TPointSeries *>(Series11);
  Points[6] = dynamic_cast<TPointSeries *>(Series13);
  Points[7] = dynamic_cast<TPointSeries *>(Series15);
  Points[8] = dynamic_cast<TPointSeries *>(Series17);
  Points[9] = dynamic_cast<TPointSeries *>(Series19);

  TPointSeries *DomPoints[10];     // Dominating points
  DomPoints[0] = dynamic_cast<TPointSeries *>(Series2);
  DomPoints[1] = dynamic_cast<TPointSeries *>(Series4);
  DomPoints[2] = dynamic_cast<TPointSeries *>(Series6);
  DomPoints[3] = dynamic_cast<TPointSeries *>(Series8);
  DomPoints[4] = dynamic_cast<TPointSeries *>(Series10);
  DomPoints[5] = dynamic_cast<TPointSeries *>(Series12);
  DomPoints[6] = dynamic_cast<TPointSeries *>(Series14);
  DomPoints[7] = dynamic_cast<TPointSeries *>(Series16);
  DomPoints[8] = dynamic_cast<TPointSeries *>(Series18);
  DomPoints[9] = dynamic_cast<TPointSeries *>(Series20);

	Points[curN-1]->Clear();
	DomPoints[curN-1]->Clear();

	// draw all points in the current population
	//
	for(int i=0; i < globalSetup->populationSize; i++){
		NsgaIndividual *guy = (NsgaIndividual *)((*pop)[i]);
		if(guy->getNoOfViolations() == 0){
		  double x = guy->getObjective(1); // quality deviation
		  double y = guy->getObjective(0); // throughput
		  Points[curN-1]->AddXY(x, y, "");
		  if(x>max_x) max_x = x;
		  if(x<min_x) min_x = x;
		  if(y>max_y) max_y = y;
		  if(y<min_y) min_y = y;
		}
	}

	if(chkGraphChange->Checked){
		if(max_y > min_y){
		  Chart->LeftAxis->Minimum = 0;
		  Chart->LeftAxis->Maximum = max_y + 2 * (max_y - min_y);
		  Chart->LeftAxis->Minimum = min_y - 2 * (max_y - min_y);
		}
		if(max_x > min_x){
		  Chart->BottomAxis->Minimum = 0;
		  Chart->BottomAxis->Maximum = max_x + 2 * (max_x - min_x);
		  Chart->BottomAxis->Minimum = min_x - 2 * (max_x - min_x);
		}
		chkGraphChange->Checked = false;
	}

	// draw all points in the pareto front
	//
	for(int i=0; i<globalSetup->populationSize; i++){
		NsgaIndividual *guy = (NsgaIndividual *)((*pop)[i]);

		if(guy->getNoOfViolations() != 0) continue;

		bool bDominated = false;
		double y = guy->getObjective(0);  // throughput
		double x = guy->getObjective(1);  // quality deviation

		for(int j=0; j<globalSetup->populationSize; j++){
		  if(j==i) continue;
		  NsgaIndividual *guy2 = (NsgaIndividual *)((*pop)[j]);
		  if(x < guy2->getObjective(1) && y > guy2->getObjective(0)){
			bDominated = true;
			break;
		  }
		}
		if(!bDominated) DomPoints[curN-1]->AddXY(x, y, "");
	}

	// output graphs to files
	if(chkOutGraphs->Checked){
		int g = GraphCount % GOutStep;
		if(g == 0){
		  String fn = "graph"+IntToStr(GraphCount/GOutStep)+".wmf";
		  Chart->SaveToMetafile(fn);
		}
		GraphCount++;
	}

}

void __fastcall TfrmNSGAII_turning::btnStopClick(TObject *)
{
  bStop = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::chkOutGraphsClick(TObject *)
{
  if(chkOutGraphs->Checked) edtGOutStep->Enabled = true;
  else edtGOutStep->Enabled = false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmNSGAII_turning::SaveimageasWMV1Click(TObject *Sender)
{
  AnsiString s = "Results.wmf";
  dlgSaveWMF->FileName = s;
  if(dlgSaveWMF->Execute()){
	s = dlgSaveWMF->FileName;
	Chart->SaveToMetafileEnh(s);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmNSGAII_turning::Copyimagetoclipboard1Click(TObject *Sender)
{
  Chart->CopyToClipboardMetafile(true);
}
//---------------------------------------------------------------------------

