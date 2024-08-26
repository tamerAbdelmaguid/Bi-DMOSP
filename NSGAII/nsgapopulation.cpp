/***************************************************************
/* Single & Multi-Objective Real-Coded Genetic Algorithms Code */
/* Author: Kumara Sastry                                       */
/* Illinois Genetic Algorithms Laboratory (IlliGAL)            */
/* Deparment of General Engineering                            */
/* University of Illinois at Urbana-Champaign                  */
/* 104 S. Mathews Ave, Urbana, IL 61801                        */
/***************************************************************/

//---------------------------------------------------------------------------

#pragma hdrstop

#include "nsgapopulation.h"

Population::Population() {
  // Things to be done:
  // Initialization of member variables:
  //    1. Guys, newGuys -> Depending upon the type of GA
  //    2. mpool -> new int[popSize]
  //    3. bestInd -> new Individual
  //    4. selection = new kindOfSelection(this);
  //    5. crossover = new kindOfCrossover();
  // If (TypeOfGA == SGA)
  //    1. Initialize all statistics to new double;
  int ii;
  guys = new Individual*[globalSetup->populationSize];
  newGuys = new Individual*[globalSetup->populationSize];
  if (globalSetup->gaType==SGA) {
    bestInd = new Individual;
    freezeMask = new int[globalSetup->noOfDecisionVariables];
    for(ii = 0; ii < globalSetup->noOfDecisionVariables; ii++)
      freezeMask[ii] = OFF;

    for (ii=0; ii<globalSetup->populationSize; ii++) {
      guys[ii] = new Individual;
      newGuys[ii] = new Individual(guys[ii]);
    }
  }
  else {
    bestInd = new NsgaIndividual;
    for (ii=0; ii<globalSetup->populationSize; ii++) {
      guys[ii] = new NsgaIndividual;
      newGuys[ii] = new NsgaIndividual(guys[ii]);
    }
  }
  mpool = new int[globalSetup->populationSize];
  switch (globalSetup->selectionType) {
  case TournamentWOR: selection = new TournamentSelection(*((int *)globalSetup->selectionParameters), this);
    break;
  case SUS: selection = new StochasticUniversalSelection(this);
    break;
  case Truncation:
  	selection = new TruncationSelection(*((int *)globalSetup->selectionParameters), this);
    break;
  case TournamentWR:
    selection = new TournamentSelectionWithReplacement(*((int *)globalSetup->selectionParameters), this);
    break;
  case RouletteWheel:
    selection = new RouletteWheelSelection(this);
    break;
  default: exit(0);
  }
  switch (globalSetup->xOverType) {
  case OnePoint: crossover = new OneTwoPointCrossover(1);
    break;
  case TwoPoint: crossover = new OneTwoPointCrossover(2);
    break;
  case Uniform:
    if (globalSetup->xOverParameters==NULL)
      crossover = new UniformCrossover();
    else
      crossover = new UniformCrossover(((double *)globalSetup->xOverParameters)[0]);
    break;
  case SBX:
    if (globalSetup->xOverParameters==NULL)
      crossover = new SimulatedBinaryCrossover();
    else
      crossover = new SimulatedBinaryCrossover(((double *)globalSetup->xOverParameters)[0]);
    break;
  default: exit(0);
  }
  if(globalSetup->loadPopulation) {
    loadPopulationFromFile();
  }
  // Initialization of the other stuff
  if (globalSetup->gaType==SGA) {
    bestobj = new double;
    worstobj = new double;
    avgobj = new double;
    maxfit = new double;
    minfit = new double;
    avgfit = new double;
    varfit = new double;
    bestFitChange = new double;
    avgFitChange = new double;
    fitVarChange = new double;
    bestObjChange = new double;
    avgObjChange = new double;
    switch (globalSetup->localSearchMethod) {
    case NoLocalSearch: break;
    case SimplexSearch:
      localSearch = new Simplex;
      break;
      //    case Complex: localSearch = new Complex();
      //      break;
      //    case MOM: localSearch = new methodOfMultipliers();
      //      break;
    default: exit(0);
    }
    computeObjStatistics();
    mapObjectiveToFitness();
    computeFitnessStatistics();
    if((globalSetup->selectionType == SUS)||
       (globalSetup->selectionType == RouletteWheel)||
       (globalSetup->nichingType == Sharing)) {
      scaleFitness();
      if(globalSetup->nichingType == Sharing) shareFitness();
      computeFitnessStatistics();
    }
    replacePopulation();
    computeObjStatistics();
    mapObjectiveToFitness();
    computeFitnessStatistics();
  }
}

std::ostream &operator<< (std::ostream &out, const Population &pop )
{
  int ii, jj;
  if (globalSetup->gaType==SGA) {
    if(globalSetup->nichingType == NoNiching) {
      out << "Best Objective - "<<*(pop.bestobj)<< std::endl <<"Worst Objective - "<<*(pop.worstobj)<< std::endl<<"Average Objective - "<< *(pop.avgobj)<< std::endl;
      out << "Maximum Fitness - "<<*(pop.maxfit)<<std::endl<<"Average Fitness - "<<*(pop.avgfit)<<std::endl;
      out <<"The best individual"<<std::endl<<*(pop.bestInd)<<std::endl;
    }
    else {
      out << "% ";
      for(jj = 0; jj < globalSetup->noOfDecisionVariables; jj++)
	out << "var #" << jj << "\t";
      out << std::endl;
      for(ii = 0; ii < globalSetup->populationSize; ii++) {
	for(jj = 0; jj < globalSetup->noOfDecisionVariables; jj++)
	  out << (*pop[ii])[jj] << "\t";
	out << pop[ii]->getFitness() << std::endl;
      }
    }
  }
  else {
    out << "% ";
    for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++)
      out << "obj #" << jj << "\t";
    out << std::endl;
    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++)
	out << ((NsgaIndividual *)(pop[ii]))->getObjective(jj) << "\t";
      out << std::endl;
    }
  }
  return out;
}

Population::~Population()
{
  int ii;

  for(ii = 0; ii < globalSetup->populationSize; ii++){
    delete guys[ii];
    delete newGuys[ii];
  }
  delete [] guys;
  delete [] newGuys;

  delete bestInd;
  delete []mpool;
  delete selection;
  delete crossover;
  if (globalSetup->gaType == SGA) {
    delete []freezeMask;
    delete bestobj;
    delete worstobj;
    delete avgobj;
    delete maxfit;
    delete minfit;
    delete avgfit;
    delete varfit;
    delete bestFitChange;
    delete avgFitChange;
    delete fitVarChange;
    delete bestObjChange;
    delete avgObjChange;
  }
}

/*============================================================
** Function Name: Population::doRTS()
** Function Task: This method performs Restricted Tournament Selection
**                For each new individual, random select W individuals and find the individual which closest to the new individual based on phenotypic distance. If the closest individual is better than the new individual, replace the new individual with the closest individual.
** Reference: Harik, G. (1994). "Finding Multiple Solutions In Problems of Bounded Difficulty", Illigal Report No. 94002. http://www-illigal.ge.uiuc.edu/techreps.php3. (ftp://ftp-illigal.ge.uiuc.edu/pub/papers/IlliGALs/94002.ps.Z).
** Output: None
** Functions Called:
**========================================================== */
void Population::doRTS(void)
{
  int windowSize = ((int *)globalSetup->nichingParameters)[0];
  int ii, jj, kk,closest, rndGuy;
  double minDist, distance, maxDist;

  Individual **tempGuys;
  tempGuys = new Individual*[globalSetup->populationSize];
  for(ii = 0; ii < globalSetup->populationSize; ii++)
    tempGuys[ii] = new Individual(guys[ii]);

  maxDist = 0;
  for(ii = 0; ii < globalSetup->noOfDecisionVariables; ii++)
    maxDist += ((globalSetup->variableRanges[ii][1] - globalSetup->variableRanges[ii][0])*(globalSetup->variableRanges[ii][1] - globalSetup->variableRanges[ii][0]));

  for(ii = 0; ii < globalSetup->populationSize; ii++) {
    minDist = maxDist;
    for(jj = 0; jj < windowSize; jj++) {
      distance = 0.0;
      rndGuy = myRandom.boundedIntegerRandom(0, globalSetup->populationSize);
      for(kk = 0; kk < globalSetup->noOfDecisionVariables; kk++)
	distance += ((*tempGuys[rndGuy])[kk]-(*newGuys[ii])[kk])*
	  ((*tempGuys[rndGuy])[kk]-(*newGuys[ii])[kk]);
      if(distance < minDist) {
	closest = rndGuy;
	minDist = distance;
      }
    }
    if(isBetter(newGuys[ii],tempGuys[closest]))
      *tempGuys[closest] = *newGuys[ii];
  }
  for(ii = 0; ii < globalSetup->populationSize; ii++) {
    *(newGuys[ii]) = *(tempGuys[ii]);
    delete tempGuys[ii];
  }
  delete []tempGuys;
}

/*============================================================
** Function Name: Population::shareFitness()
** Function Task: This method performs shared niching which consists
** of the following steps. Calculate the number of individuals sharing
** each niche using a phenotypic distance metric. An individual is
** considered to be in the same niche as another if phenotypic distance
** between is less than sigmaShare which is specified in th eglobal setup
** as a parameter. shareAlpha is another parameter used for calculating
** the sharing value which is used to update the niche count for each of
** the individuals. The shared fitness of each individaul is calculate by
** dividing is fitness by its niche count.
** Output: None
** Functions Called: Population::setFitness(double)
** Reference: Goldberg, D.E. & Richardson, J. (1987). "Genetic Algorithms with Sharing for Multimodal Function Optimization", Genetic Algorithms and their Applications: Proceedings of the Second International Conference on Genetic Algorithms, 41-49. (TCGA No. 01170).
**========================================================== */
void Population::shareFitness()
{
  double sigmaShare = ((double *)globalSetup->nichingParameters)[0];
  double shareAlpha = ((double *)globalSetup->nichingParameters)[1];
  double *sharingValue;
  int ii, jj, kk, ll, arraySize, transIndex;
  double tempDist, nicheCount;
  double phenoValue1, phenoValue2, sharedFitness;

  arraySize = ((globalSetup->populationSize)*(globalSetup->populationSize-1))/2;
  sharingValue = new double[arraySize];
  ll = 0;
  for(ii = 0; ii < globalSetup->populationSize-1; ii++) {
    nicheCount = 1.0;
    for(jj = ii + 1; jj < globalSetup->populationSize; jj++) {
      tempDist = 0.0;
      for(kk = 0; kk < globalSetup->noOfDecisionVariables; kk++) {
	phenoValue1 = (*newGuys[ii])[kk];
	phenoValue2 = (*newGuys[jj])[kk];
	tempDist += ((phenoValue1-phenoValue2)*(phenoValue1-phenoValue2));
      }
      tempDist = sqrt(tempDist);
      if(tempDist < sigmaShare)
	sharingValue[ll] = 1.0 - pow((tempDist/(sigmaShare)), shareAlpha);
      else
	sharingValue[ll] = 0.0;
      nicheCount += sharingValue[ll++];
    }
    for(jj = 0; jj < ii; jj++) {
      transIndex = (globalSetup->populationSize)*jj + (ii-jj-1) - (jj*(jj+1)/2);
      nicheCount += sharingValue[transIndex];
    }
    sharedFitness = (newGuys[ii]->getFitness())/nicheCount;
    newGuys[ii]->setFitness(sharedFitness);
  }
  delete []sharingValue;
}

/*============================================================
** Function Name: Population::freeze(int)
** Function Task: Given a locus this method sets the freezemask
** for the corresponding location on the chromosome.
** It then calls Individual::freeze(int,) for each individual
** to set value of the frozen gene in all individuals to the
** value of the corresponding gene in the best individual.
** This feature is diabled when Niching is on.
** Output:None
** Functions called: Individual::freeze(int,)
**========================================================*/
void Population::freeze(int locus)
{
  int ii;
  if((globalSetup->gaType == SGA)&&
     (globalSetup->nichingType == NoNiching)&&
     (freezeMask[locus] == OFF)) {
    freezeMask[locus] = ON;
    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      guys[ii]->freeze(locus, (*bestInd)[locus]);
      newGuys[ii]->freeze(locus, (*bestInd)[locus]);
    }
  }
}

/*============================================================
** Function Name: Population::flood(int)
** Function Task: Given a locus this method sets the freezemask
** of the corresponding location on the chromosome.
** It then calls Individual::flood(int,) for each individual
** to set value of the flooded genes to some random value.
** This feature is diabled when Niching is on.
** Output:None
** Functions called: Individual::flood(int)
**========================================================*/
void Population::flood(int locus)
{
  int ii;
  freezeMask[locus]=OFF;
  if((globalSetup->gaType == SGA)&&
     (globalSetup->nichingType == NoNiching)) {
    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      guys[ii]->flood(locus);
      newGuys[ii]->flood(locus);
    }
  }
}

/*============================================================
** Function Name: Population::computeObjStatistics(void)
** Function Task: this method generates statistics according to
** objective function values. It finds out the average objective
** value for the population and the best and worst objective
** values. This depends on whether the type of optimization in the
**  problem is minimization or maximization.
** Output:None
** Functions called: Individual::getObjective()
**========================================================*/
void Population::computeObjStatistics(void)
{
  int ii, bestGuy, worstGuy;
  double oneOverPopulationSize;
  double oldBestObj, oldAvgObj;

  oneOverPopulationSize = 1.0/(1.0*(globalSetup->populationSize));

  oldBestObj = *bestobj;
  oldAvgObj = *avgobj;

  bestGuy = worstGuy = 0;
  *avgobj = oneOverPopulationSize*(newGuys[0]->getObjective());
  for(ii = 1; ii < globalSetup->populationSize; ii++) {
    *avgobj += oneOverPopulationSize*(newGuys[ii]->getObjective());
    if(isBetter(newGuys[ii], newGuys[bestGuy])) bestGuy = ii;
    if(isBetter(newGuys[worstGuy], newGuys[ii])) worstGuy = ii;
  }
  *bestobj = newGuys[bestGuy]->getObjective();
  *worstobj = newGuys[worstGuy]->getObjective();

  *bestObjChange = fabs(oldBestObj - *bestobj);
  *avgObjChange = fabs(oldAvgObj - *avgobj);
}

/*============================================================
** Function Name: Population::computeFitnessStatistics(void)
** Function Task: this method generates statistics according to
** fitness function values. It finds out the average fitness
** value for the population and the maximum and minimum fitness
** values as well as the fitness variance.
** Output:None
** Functions called: Individual::getFitness()
**========================================================*/
void Population::computeFitnessStatistics(void)
{
  int ii, bestIndvID, worstIndvID;
  double oneOverPopulationSize;
  double oldMaxFit, oldAvgFit, oldFitVar;

  // have to compute maxobj, minobj, maxfit, minfit, avgfit, bestInd,
  // fitnessVariance, objectiveVariance
  oneOverPopulationSize = 1.0/(1.0*(globalSetup->populationSize));

  oldMaxFit = *maxfit;
  oldFitVar = *varfit;
  oldAvgFit = *avgfit;

  *varfit = oneOverPopulationSize*(newGuys[0]->getFitness())*(newGuys[0]->getFitness());
  *avgfit = oneOverPopulationSize*(newGuys[0]->getFitness());
  bestIndvID = worstIndvID = 0;

  for(ii = 1; ii < globalSetup->populationSize; ii++) {
    *avgfit += oneOverPopulationSize*(newGuys[ii]->getFitness());
    *varfit += oneOverPopulationSize*(newGuys[ii]->getFitness())*(newGuys[ii]->getFitness());
    if(selection->betterIndividual(newGuys[ii], newGuys[bestIndvID])) bestIndvID = ii;
    if(selection->betterIndividual(newGuys[worstIndvID], newGuys[ii])) worstIndvID = ii;
  }
  *varfit -= (*avgfit)*(*avgfit);
  *maxfit = newGuys[bestIndvID]->getFitness();
  *minfit = newGuys[worstIndvID]->getFitness();

  *(bestInd) = *(newGuys[bestIndvID]);
  *bestFitChange = fabs(oldMaxFit - *maxfit);
  *avgFitChange = fabs(oldAvgFit - *avgfit);
  *fitVarChange = fabs(oldFitVar - *varfit);
}

/*============================================================
** Function Name: Population::loadPopulationFromFile(void)
** Function Task: this method calls Individual::evaluateFitness().
** unless the Niching type is Detereministic Crowding because
** the fitness evaluation be required soon after offsprings are
** generated during crossover.
** Output:None
** Functions called: Individual::evaluateFitness();
**========================================================*/
void Population::loadPopulationFromFile(void)
{
  int ii, jj, numGlobalEvals;
  int numSolutions, numToLoad;
  double *variableValues;
  double *objValues, *constViolValues;
  double dummy;
  double penaltyValue;

  variableValues = new double[globalSetup->noOfDecisionVariables];
  objValues = new double[globalSetup->finalNoOfObjectives];
  constViolValues = new double[globalSetup->finalNoOfConstraints];

  std::ifstream infile(globalSetup->populationFileName);
  infile >> numSolutions;
  if(numSolutions > globalSetup->populationSize)
	numToLoad = globalSetup->populationSize;
  else
	numToLoad = numSolutions;
  printf("Loading initial population of size %d\n", numSolutions);
  for(ii = 0; ii < numToLoad; ii++) {
	for(jj = 0; jj < globalSetup->noOfDecisionVariables; jj++) {
	  infile >> variableValues[jj];
	  //      printf("%14.10f ", variableValues[jj]);
    }
    for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
      infile >> objValues[jj];
    }
    if(globalSetup->finalNoOfConstraints) {
      for(jj = 0; jj < globalSetup->finalNoOfConstraints; jj++) {
	infile >> constViolValues[jj];
      }
      infile >> penaltyValue;
    }
    //    printf("%f %f\n", objValues[0], objValues[1]);
    guys[ii]->loadIndividual(variableValues,objValues,constViolValues,penaltyValue);
    newGuys[ii]->loadIndividual(variableValues,objValues,constViolValues,penaltyValue);
  }
  if(globalSetup->evaluateAgain) {
    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      guys[ii]->evaluateFitness();
      if(globalSetup->gaType == NSGA) {
	for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
          ((NsgaIndividual *)(newGuys[ii]))->setObjective(jj, ((NsgaIndividual *)(guys[ii]))->getObjective(jj));
	}
	for(jj = 0; jj < globalSetup->finalNoOfConstraints; jj++) {
          ((NsgaIndividual *)(newGuys[ii]))->setViolation(jj, ((NsgaIndividual *)(guys[ii]))->getViolation(jj));
	}
      }
      else {
	newGuys[ii]->setObjective(guys[ii]->getObjective());
	for(jj = 0; jj < globalSetup->finalNoOfConstraints; jj++) {
          newGuys[ii]->setViolation(jj, guys[ii]->getViolation(jj));
	}
      }
      newGuys[ii]->setPenalty(guys[ii]->getPenalty());
      newGuys[ii]->setNoOfViolations(guys[ii]->getNoOfViolations());
    }
  }
  else if(numSolutions < globalSetup->populationSize) {
    for(ii = numSolutions; ii < globalSetup->populationSize; ii++) {
      guys[ii]->evaluateFitness();
      if(globalSetup->gaType == NSGA) {
	for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
          ((NsgaIndividual *)(newGuys[ii]))->setObjective(jj, ((NsgaIndividual *)(guys[ii]))->getObjective(jj));
	}
	for(jj = 0; jj < globalSetup->finalNoOfConstraints; jj++) {
	  ((NsgaIndividual *)(newGuys[ii]))->setViolation(jj, ((NsgaIndividual *)(guys[ii]))->getViolation(jj));
	}
      }
      else {
        newGuys[ii]->setObjective(guys[ii]->getObjective());
	for(jj = 0; jj < globalSetup->finalNoOfConstraints; jj++) {
          newGuys[ii]->setViolation(jj, guys[ii]->getViolation(jj));
	}
      }
      newGuys[ii]->setPenalty(guys[ii]->getPenalty());
      newGuys[ii]->setNoOfViolations(guys[ii]->getNoOfViolations());
    }
  }
  delete []variableValues;
  delete []objValues;
  delete []constViolValues;
  infile.close();
}


/*============================================================
** Function Name: Population::doEvaluate(void)
** Function Task: this method calls Individual::evaluateFitness().
** unless the Niching type is Detereministic Crowding because
** the fitness evaluation be required soon after offsprings are
** generated during crossover.
** Output:None
** Functions called: Individual::evaluateFitness();
**========================================================*/
int Population::doEvaluate(void)
{
  int ii, numGlobalEvals;

  if(globalSetup->nichingType != DeterministicCrowding)
    for(ii = 0; ii < globalSetup->populationSize; ii++)
      newGuys[ii]->evaluateFitness();
  numGlobalEvals = globalSetup->populationSize;
  return numGlobalEvals;
}

/*============================================================
** Function Name: Population::doLocalSearch(void)
** Function Task: This method calls local searcher for each of
** the individuals in the population with probability
** localSearchProbability.
** A Lamarckian (backsubstituting point obtained from local search)
** or Baldwinian (taking only the fitness value of the point from
** local searcher and not its structure) strategy is used based on
** the lamarckianProbability.
** Output:None
** Functions called: LocalSearch::localSearch()
**========================================================*/
int Population::doLocalSearch(void)
{
  int ii, jj, numLocalEvals = 0, numEvals;
  Individual *localGuy;

  localGuy = new Individual;

  if((globalSetup->gaType == SGA) &&
     (globalSetup->localSearchMethod != NoLocalSearch)) {

    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      if(myRandom.flip(globalSetup->localSearchProbability)) {
	numEvals = localSearch->localSearcher(newGuys[ii],localGuy,freezeMask);
	numLocalEvals += numEvals;
	if(myRandom.flip(globalSetup->lamarckianProbability))
	  *newGuys[ii] = *localGuy;
	else {
	  for(jj = 0; jj < globalSetup->noOfDecisionVariables; jj++)
	    localGuy->setValue(jj,(*newGuys[ii])[jj]);
	  *newGuys[ii] = *localGuy;
	}
      }
    }
  }
  delete localGuy;
  return numLocalEvals;
}

/***
Replace the old population with the new population. Incorporates elitism using
 the parameter globalSetup::replaceProportion.
 If the value of replaceProportion is less than 1.0 then the bottom
 (100*replaceProportion)% individuals from the current population are replaced
 by the top (100*replaceProportion)% individuals from the new population.
*/
void Population::replacePopulation(void)
{
  int ii, startIndex, *randomArray1, *randomArray2;

  if(fabs(globalSetup->replaceProportion - 1.0) <= ZERO) {
    Individual **temp = newGuys;
    newGuys = guys;
    guys = temp;
  }
  else {
    Individual **eliteGuys;
    randomArray1 = new int[globalSetup->populationSize];
    randomArray2 = new int[globalSetup->populationSize];

    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      randomArray1[ii] = ii;
      randomArray2[ii] = ii;
    }
    rankingQuickSort(randomArray1, 0, globalSetup->populationSize);

    Individual **temp = newGuys;
    newGuys = guys;
    guys = temp;

    rankingQuickSort(randomArray2, 0, globalSetup->populationSize);

    eliteGuys = new Individual*[globalSetup->populationSize];

    startIndex = (int)((1.0-globalSetup->replaceProportion)*(globalSetup->populationSize));

    for(ii = 0; ii < startIndex; ii++)
      eliteGuys[ii] = new Individual(newGuys[randomArray2[globalSetup->populationSize-1-ii]]);
    for(ii = startIndex; ii < globalSetup->populationSize; ii++)
      eliteGuys[ii] = new Individual(guys[randomArray1[globalSetup->populationSize - 1 - ii + startIndex]]);

    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      *(newGuys[ii]) = *(eliteGuys[ii]);
      delete eliteGuys[ii];
    }
    delete []eliteGuys;
  }
}

///Perfrom selection
void Population::doSelect(void) { selection->select(mpool); }

///Perform crossover
void Population::doCrossover(void)
{
  int ii;
  for(ii = 0; ii < globalSetup->populationSize; ii += 2)
    if(myRandom.flip(globalSetup->xOverProbability))
      crossover->crossover(newGuys[ii], newGuys[ii+1]);
}

///Perform mutation
void Population::doMutate(void)
{
  int ii;
  for(ii = 0; ii < globalSetup->populationSize; ii++)
    newGuys[ii]->mutate(freezeMask);
}

/***
Map objective to fitness depending on minimization or maximzation and the constraint handling method.
If the problem is a maximization and the constraint handling method is either a tournament method or if there are no constraints, then the fitness is set equal to the objective value. If the constraint handling method is penalty then the fitness is equal to the objective value minus the penalty due to constraint violation. On the other hand, if the problem is a minimization one and the constraint handling method is either a tournament method or if there are no constraints, then set the fitness equal to the negative of the objective value. If the constraint handling method is the penalty method then the fitness is equal to the negative of the objective value minus the penalty due to constraint violation.
*/
void Population::mapObjectiveToFitness(void)
{
  int ii;
  if(*(globalSetup->typeOfOptimizations) == Maximization) {
    switch(globalSetup->constraintMethod) {
    case NoConstraints: case Tournament:
      for(ii = 0; ii < globalSetup->populationSize; ii++)
	newGuys[ii]->setFitness(newGuys[ii]->getObjective());
      break;
    case Penalty:
      for(ii = 0; ii < globalSetup->populationSize; ii++)
	newGuys[ii]->setFitness(newGuys[ii]->getObjective() - newGuys[ii]->getPenalty());
      break;
    default: exit(0);
    }
  }
  else {
    switch(globalSetup->constraintMethod) {
    case NoConstraints: case Tournament:
      for (ii=0;ii<globalSetup->populationSize; ii++)
	newGuys[ii]->setFitness(-newGuys[ii]->getObjective());
      break;
    case Penalty:
      for(ii = 0; ii < globalSetup->populationSize; ii++)
	newGuys[ii]->setFitness(-newGuys[ii]->getObjective() - newGuys[ii]->getPenalty());
      break;
    default: exit(0);
    }
  }
}

/***
 Procedures for scaling fitness used with proportionate selection schemes and with fitness sharing. Two scaling methods (1) Sigma scaling (truncation), and (2) ranking are provided.
Ranking Method: Sort the individuals according to their fitness and constraint violation and assign a linear ranking as the new fitness. Here the best individual has a fitness of N and the worst individual has a fitness value of 1.
Reference:  Baker, J.E. (1985). "Adaptive Selection Methods for Genetic Algorithms", In Grefenstte, J. (Ed.), Proceedings of the International Conference on Genetic Algorithms and Their Applications (pp. 101-111). Hillsdale, NJ:Lawrence Erlbaum Associates (TCGA No. 00460).
Sigma Scaling (Truncation): Scale the fitness using as
fscaled = 1 + (f - favg)/(sigmaParameter*fstd),
where f is the fitness of the individual, favg is the average fitness, and fstd is the standard deviation of fitness and sigmaParameter is usually taken as 2.0.
If the fscaled is less than 0.0 then it is arbitrarily set a low value of 0.1.
Reference: Forrest, S. (1985). "Documentation for PRISONERS DILEMMA and NORMS programs that use the genetic algorithm. Unpublished manuscript, University of Michigan, Ann Arbor. (TCGA No. 00614).
If none of the scaling method is used and the minimum fitness is negative then an absolute value of the minimum fitness is added to the fitness of each individual.
*/
void Population::scaleFitness(void)
{
  double sigmaParam;
  int *randomArray, ii;
  double fitStdDev, scaledFitness, sigTerm;

  randomArray = new int[globalSetup->populationSize];
  switch(globalSetup->scalingMethod) {
  case Ranking:
    for(ii = 0; ii < globalSetup->populationSize; ii++)
      randomArray[ii] = ii;
    rankingQuickSort(randomArray, 0, globalSetup->populationSize);
    scaledFitness = 1.0*(globalSetup->populationSize);
    newGuys[randomArray[globalSetup->populationSize - 1]]->setFitness(scaledFitness);

    for(ii = globalSetup->populationSize-2; ii >= 0; ii--) {
      if(selection->betterIndividual(newGuys[randomArray[ii+1]],newGuys[randomArray[ii]]))
	scaledFitness = newGuys[randomArray[ii+1]]->getFitness() - 1.0;
      newGuys[randomArray[ii]]->setFitness(scaledFitness);
    }
    break;
  case SigmaScaling:
    sigmaParam = ((double *)globalSetup->scalingParameters)[0];
    fitStdDev = sqrt(*varfit);
    for(ii = 0; ii < globalSetup->populationSize; ii++) {
      if(fitStdDev >= 0.0001) {
	sigTerm = sigmaParam * fitStdDev;
	scaledFitness = (newGuys[ii]->getFitness() -
			 *avgfit + sigTerm)/sigTerm;
      }
      else
	scaledFitness = (newGuys[ii]->getFitness() + *avgfit)/(2.0*(*avgfit));
      if(scaledFitness < 0.0) scaledFitness = 0.1;
      newGuys[ii]->setFitness(scaledFitness);
    }
    break;
  default:
    if(*minfit < 0.0) {
      for(ii = 0; ii < globalSetup->populationSize; ii++) {
	scaledFitness = newGuys[ii]->getFitness() - *minfit;
	newGuys[ii]->setFitness(scaledFitness);
      }
    }
  }
  delete []randomArray;
}

/// Quick sort procedure used for ranking selection
void Population::rankingQuickSort(int *output, int left, int right)
{
  int ii, xx;
  Individual *target;

  if(right > left) {
    target = newGuys[output[right-1]];
    ii = left-1;
    xx = right-1;
    for(;;) {
      while(!(selection->betterIndividual(newGuys[output[++ii]],target)))
	if(ii >= right-1) break;
      if(ii >= xx) break;
      while(selection->betterIndividual(newGuys[output[--xx]],target))
	if(xx <= 0) break;
      if(ii >= xx) break;
      swap(output[ii],output[xx]);
    }
    swap(output[ii],output[right-1]);
    rankingQuickSort(output, left, ii);
    rankingQuickSort(output, ii+1, right);
  }
}

/// swap procedure used by quick sort
void Population::swap(int& ii, int& jj)
{
  int temp;
  temp = jj; jj = ii; ii = temp;
}


NsgaPopulation::NsgaPopulation(void)
{
  int ii, combinedPopSize = 2*(globalSetup->populationSize);
  numFronts = 0;
  numFrontChange = 0;
  combinedGuys = new NsgaIndividual*[combinedPopSize];
  numIndsFront = new int[combinedPopSize];
  paretoFront = new int*[combinedPopSize];
  for(ii = 0; ii < combinedPopSize; ii++) {
    combinedGuys[ii] = new NsgaIndividual;
    paretoFront[ii] = new int[combinedPopSize];
  }
  bestobj = new double[globalSetup->finalNoOfObjectives];
  worstobj = new double[globalSetup->finalNoOfObjectives];
  avgobj = new double[globalSetup->finalNoOfObjectives];
  maxfit = new double[globalSetup->finalNoOfObjectives];
  minfit = new double[globalSetup->finalNoOfObjectives];
  avgfit = new double[globalSetup->finalNoOfObjectives];
  varfit = new double[globalSetup->finalNoOfObjectives];
  computeObjStatistics(GUYS);
  mapObjectiveToFitness(GUYS);
  computeFitnessStatistics(GUYS);
}

NsgaPopulation::~NsgaPopulation(void)
{
  int ii, combinedPopSize = 2*(globalSetup->populationSize);
  for(ii = 0; ii < combinedPopSize; ii++) {
    delete combinedGuys[ii];
    delete paretoFront[ii];
  }
  delete []combinedGuys;
  delete []numIndsFront;
  delete []paretoFront;
  delete []bestobj;
  delete []worstobj;
  delete []avgobj;
  delete []minfit;
  delete []maxfit;
  delete []avgfit;
  delete []varfit;
}


/***
Peforms a nondominated sort of individuals to create a set of pareto fronts.
Details:
  *  The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
  *  domCount[i]: (domination count) Number of the individuals that dominate individual i
  *  numIndDom[i]: Number of individuals that individual i dominates
  *  indDomByMe[i]: Array of indices of individuals that individual i dominates
  *  paretoFront[i]: Array of indices of individuals that are members of pareto front i.
  *  numIndsFront[i]: Number of individuals in the ith front.
  *  For every unique pair of individuals (i,j)
     --Check if i dominates j, If yes then add j to indDomByMe[i], increment numIndDom[i] by 1 and increment domCount[j] by 1.
     --On the other hand if j dominates i, then add i to indDomByMe[j], increment numIndDom[j] by 1 and increment domCount[i] by 1.
  * For every individual i
     --If domCount[i] equals 0, then add individual i to paretoFront[0], increment numIndsFront[0], and set the rank of individual i to 0
  * Set frontID = 0. While number of individuals in the pareto Front is greater than zero, do
     --For every individual i in the pareto front frontID
         *  For every individual j that individual i dominates
	    --Decrement the domination count of j by i
            --If the domination count of j is equal to zero, then add j to the paretoFron[frontID+1], increment numIndsFront[frontID+1], and set the rank of individual j to frontID+1.
     --Increment the frontID by one
 * Set total number of pareto fronts to frontID.
*/
void NsgaPopulation::doNonDominatedSort(int whichGuys)
{
  int ii, jj, popSize, fid, idomj, nif;
  int *domCount, **indDomByMe, *numIndDom;
  int oldNoOfFronts;
  NsgaIndividual **theGuys;

  oldNoOfFronts = numFronts;

  if(whichGuys == GUYS) {
    popSize = globalSetup->populationSize;
    theGuys = (NsgaIndividual**)guys;
  }
  else if(whichGuys == COMBINEDGUYS) {
    popSize = 2*(globalSetup->populationSize);
    theGuys = combinedGuys;
  }
  domCount = new int[popSize];
  numIndDom = new int[popSize];
  indDomByMe = new int*[popSize];
  for(ii = 0; ii < popSize; ii++) {
    indDomByMe[ii] = new int[popSize];
    domCount[ii] = numIndDom[ii] = 0;
  }
  for(ii = 0; ii < popSize-1; ii++) {
    for(jj = ii+1; jj < popSize; jj++) {
      idomj = dominates(theGuys[ii],theGuys[jj]);
      if(idomj == IDOMJ) {
	indDomByMe[ii][numIndDom[ii]] = jj;
	numIndDom[ii] += 1;
	domCount[jj] += 1;
      }
      else if(idomj == JDOMI) {
	indDomByMe[jj][numIndDom[jj]] = ii;
	numIndDom[jj] += 1;
	domCount[ii] += 1;
      }
    }
  }
  nif = 0;

  for(ii = 0; ii < popSize; ii++) {
    if(domCount[ii] == 0) {
      paretoFront[0][nif++] = ii;
      theGuys[ii]->setRank(0);
    }
  }
  numIndsFront[0] = nif;

  fid = 0;
  while(numIndsFront[fid] != 0) {
    nif = 0;
    for(ii = 0; ii < numIndsFront[fid]; ii++) {
      for(jj = 0; jj < numIndDom[paretoFront[fid][ii]]; jj++) {
	domCount[indDomByMe[paretoFront[fid][ii]][jj]] -= 1;
	if(domCount[indDomByMe[paretoFront[fid][ii]][jj]] == 0) {
	  paretoFront[fid+1][nif++] = indDomByMe[paretoFront[fid][ii]][jj];
	  theGuys[indDomByMe[paretoFront[fid][ii]][jj]]->setRank(fid+1);
	}
      }
    }
    fid += 1;
    numIndsFront[fid] = nif;
  }
  numFronts = fid;
  numFrontChange = abs(oldNoOfFronts - numFronts);
  if(whichGuys == GUYS)
    for(ii = 0; ii < popSize; ii++) guys[ii] = theGuys[ii];
  else if(whichGuys == COMBINEDGUYS)
    for(ii = 0; ii < popSize; ii++) combinedGuys[ii] = theGuys[ii];

  for(ii = 0; ii < popSize; ii++)
    delete indDomByMe[ii];
  delete []domCount;
  delete []numIndDom;
  delete []indDomByMe;
}

/***
Computes the crowding distance of each individual.
Details:
  * The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
  * For every front i
    -- For every objective j
       * Sort the individuals in front i in ascending order of objective j
       * Set the crowding distance of the first and the last individual to infinity
       * For every individual k in the front i except the first and the last individuals
         --Normalize the fitness j by dividing the fitness j of individual k-1  and individual k+1 by maximum jth fitness.
	 --Add absolute value of (Normalized jth fitness of individual k+1 - Normalized jth fitness of individual k-1) to the crowding distance of kth individual.
*/
void NsgaPopulation::computeCrowdingDistance(int whichGuys)
{
  int ii, jj, kk, firstInd, lastInd;
  int indId1, indId2, indId, popSize;
  int *sortListByObj;
  double normFit1, normFit2, *crowdDist;
  NsgaIndividual **theGuys;

  if(whichGuys == GUYS) {
    popSize = globalSetup->populationSize;
    theGuys = (NsgaIndividual**)guys;
  }
  else if(whichGuys == COMBINEDGUYS) {
    popSize = 2*(globalSetup->populationSize);
    theGuys = combinedGuys;
  }

  crowdDist = new double[popSize];
  for(ii = 0; ii < popSize; ii++) crowdDist[ii] = 0.0;

  for(ii = 0; ii < numFronts; ii++) {
    sortListByObj = new int[numIndsFront[ii]];
    for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
      for(kk = 0; kk < numIndsFront[ii]; kk++)
	sortListByObj[kk] = paretoFront[ii][kk];
      quickSort(theGuys, sortListByObj, 0, numIndsFront[ii],jj);
      firstInd = sortListByObj[0];
      lastInd = sortListByObj[numIndsFront[ii]-1];
      crowdDist[firstInd] = crowdDist[lastInd] = INFTY;
      for(kk = 1; kk < numIndsFront[ii]-1; kk++) {
	indId = sortListByObj[kk];
	indId1 = sortListByObj[kk+1];
	indId2 = sortListByObj[kk-1];
	normFit1 = theGuys[indId1]->getFitness(jj)/(1.0+maxfit[jj]);
	normFit2 = theGuys[indId2]->getFitness(jj)/(1.0+maxfit[jj]);
	crowdDist[indId] += fabs(normFit1 - normFit2);
      }
    }
    delete []sortListByObj;
  }

  if(whichGuys == GUYS)
    for(ii = 0; ii < popSize; ii++)
      ((NsgaIndividual**)guys)[ii]->setCrowdingDistance(crowdDist[ii]);
  else if (whichGuys == COMBINEDGUYS)
    for(ii = 0; ii < popSize; ii++)
      combinedGuys[ii]->setCrowdingDistance(crowdDist[ii]);
  delete [] crowdDist;
}

/// Quick sort routine used for crowding distance computation. Sorts the individuals in a given pareto front in ascending order of objId th objective value.
void NsgaPopulation::quickSort(NsgaIndividual **theGuys, int *output,
			       int left, int right, int objId)
{
  int ii, xx;
  double target;

  if(right > left) {
    target = theGuys[output[right-1]]->getFitness(objId);
    ii = left-1;
    xx = right-1;
    for(;;) {
	  while(theGuys[output[++ii]]->getFitness(objId) < target)
	if(ii >= right-1) break;
      if(ii >= xx) break;
      while(theGuys[output[--xx]]->getFitness(objId) > target)
	if(xx <= 0) break;
      if(ii >= xx) break;
      swap(output[ii],output[xx]);
    }
    swap(output[ii],output[right-1]);
    quickSort(theGuys, output, left, ii, objId);
    quickSort(theGuys, output, ii+1, right, objId);
  }
}

///Swap used by the sort function
void NsgaPopulation::swap(int& ii, int& jj)
{
  int temp;
  temp = jj; jj = ii; ii = temp;
}

///Quick sort function to sort individuals in a pareto front according to the crowding distance.
void NsgaPopulation::regQSort(double *crowdDist, int *output, int left, int right)
{
  int ii, xx;
  double target;
  if(right > left) {
    target = crowdDist[output[right-1]];
    ii = left-1;
    xx = right-1;
    for(;;) {
      while(crowdDist[output[++ii]] < target)
	if(ii >= right-1) break;
      if(ii >= xx) break;
      while(crowdDist[output[--xx]] > target)
	if(xx <= 0) break;
      if(ii >= xx) break;
      swap(output[ii],output[xx]);
    }
    swap(output[ii],output[right-1]);
    regQSort(crowdDist, output, left, ii);
    regQSort(crowdDist, output, ii+1, right);
  }
}

/*============================================================
** Function Name: NsgaPopulation::computeObjStatistics(int whichGuys)
** The flag whichGuys refer to either the current population (for the
** initial generation) or the combined population.
** Function Task: this method generates statistics according to
** each objective function values. It finds out the average objective
** value for the population and the best and worst objective
** values for each objective function. This depends on whether the
** type of optimization in the problem of each objective function
** is minimization or maximization.
** Output:None
** Functions called: Individual::getObjective(int jj)
**========================================================*/
void NsgaPopulation::computeObjStatistics(int whichGuys)
{
  int ii, jj, popSize;
  double oneOverPopulationSize;
  NsgaIndividual **theGuys;

  if(whichGuys == GUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual**)guys;
  }
  else if(whichGuys == NEWGUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual**)newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	popSize = 2*(globalSetup->populationSize);
	theGuys = combinedGuys;
  }

  oneOverPopulationSize = 1.0/(1.0*popSize);

  for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
	bestobj[jj] = worstobj[jj] = theGuys[0]->getObjective(jj);
	avgobj[jj] = oneOverPopulationSize*(theGuys[0]->getObjective(jj));
	for(ii = 1; ii < popSize; ii++) {
	  avgobj[jj] += oneOverPopulationSize*(theGuys[ii]->getObjective(jj));
	  if(globalSetup->typeOfOptimizations[jj] == Maximization) {
	if(theGuys[ii]->getObjective(jj) > bestobj[jj]) bestobj[jj] = theGuys[ii]->getObjective(jj);
	if(theGuys[ii]->getObjective(jj) < worstobj[jj]) worstobj[jj] = theGuys[ii]->getObjective(jj);
	  }
	  if(globalSetup->typeOfOptimizations[jj]==Minimization) {
	if(theGuys[ii]->getObjective(jj) < bestobj[jj]) bestobj[jj] = theGuys[ii]->getObjective(jj);
	if(theGuys[ii]->getObjective(jj) > worstobj[jj]) worstobj[jj] = theGuys[ii]->getObjective(jj);
	  }
	}
  }
}

/*============================================================
** Function Name: NsgaPopulation::computeFitnessStatistics(int whichGuys)
** The flag whichGuys refer to either the current population
** (for the initial generation) or the combined population.
** Function Task: this method generates statistics according to
** every fitness function values. It finds out the average fitness
** value for the population and the maximum and minimum fitness
** values as well as the fitness variance for each fitness function.
** Output:None
** Functions called: Individual::getFitness(int jj)
**========================================================*/
void NsgaPopulation::computeFitnessStatistics(int whichGuys)
{
  int ii, jj, popSize;
  double oneOverPopulationSize;
  NsgaIndividual **theGuys;

  if(whichGuys == GUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual**)guys;
  }
  else if(whichGuys == NEWGUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual**)newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	popSize = 2*(globalSetup->populationSize);
	theGuys = combinedGuys;
  }

  oneOverPopulationSize = 1.0/(1.0*popSize);

  for(jj = 0; jj < globalSetup->finalNoOfObjectives; jj++) {
	maxfit[jj] = minfit[jj] = theGuys[0]->getFitness(jj);
	avgfit[jj] = oneOverPopulationSize*(theGuys[0]->getFitness(jj));
	varfit[jj] = oneOverPopulationSize*(theGuys[0]->getFitness(jj))*(theGuys[0]->getFitness(jj));

	for(ii = 1; ii < popSize; ii++) {
	  avgfit[jj] += oneOverPopulationSize*(theGuys[ii]->getFitness(jj));
	  varfit[jj] += oneOverPopulationSize*(theGuys[ii]->getFitness(jj))*(theGuys[ii]->getFitness(jj));
	  if(theGuys[ii]->getFitness(jj) > maxfit[jj]) maxfit[jj] = theGuys[ii]->getFitness(jj);
	  if(theGuys[ii]->getFitness(jj) < minfit[jj]) minfit[jj] = theGuys[ii]->getFitness(jj);
	}
	varfit[jj] -= avgfit[jj]*avgfit[jj];
  }
}

/***
Map each objective to corresponding fitness value depending on minimization or maximization.
The flag whichGuys refer to either the current population (for the initial generation) or the combined population.
If the jth objective value is a maximization, then the fitness is set equal to the objective value. On the other hand, if the problem is a minimization one, then set the fitness equal to the negative of the objective value.
*/
void NsgaPopulation::mapObjectiveToFitness(int whichGuys)
{
  int ii,jj, popSize;
  NsgaIndividual **theGuys;

  if(whichGuys == GUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual **)guys;
  }
  else if(whichGuys == NEWGUYS) {
	popSize = globalSetup->populationSize;
	theGuys = (NsgaIndividual **)newGuys;
  }
  else if(whichGuys == COMBINEDGUYS) {
	popSize = 2*(globalSetup->populationSize);
	theGuys = combinedGuys;
  }

  for (jj=0; jj<globalSetup->finalNoOfObjectives; jj++) {
	if (globalSetup->typeOfOptimizations[jj] == Maximization)
	  for(ii = 0; ii < popSize; ii++)
		theGuys[ii]->setFitness(jj, theGuys[ii]->getObjective(jj));
	else
	  for (ii=0; ii< popSize; ii++)
	theGuys[ii]->setFitness(jj, -theGuys[ii]->getObjective(jj));
  }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
