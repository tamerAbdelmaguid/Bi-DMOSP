/***************************************************************
/* Single & Multi-Objective Real-Coded Genetic Algorithms Code */
/* Author: Kumara Sastry                                       */
/* Illinois Genetic Algorithms Laboratory (IlliGAL)            */
/* Deparment of General Engineering                            */
/* University of Illinois at Urbana-Champaign                  */
/* 104 S. Mathews Ave, Urbana, IL 61801                        */
/***************************************************************/

//---------------------------------------------------------------------------

#ifndef LocalSearchH
#define LocalSearchH

#include "globalSetup.h"
#include "MOGARandom.h"
#include "individual.h"
#include <iostream.h>

class LocalSearch
{
protected:
  int *localFreezeMask;
  int maxLocalEvaluations;
  double maxLocalTolerance;

public:
  LocalSearch();
  virtual int localSearcher( Individual *theGuy,  Individual *localHero, int *freezeMask) = 0;
};


class Simplex: public LocalSearch
{
private:
  int noOfVariables;
  int simplexPoints;
  int best, worst, nextWorst;
  Individual **localGuys;
  double *xSum;

  inline void getSum(void)
  {
    int ii, jj, kk;
    double sum;
    for(kk = 0, jj = 0; jj < globalSetup->noOfDecisionVariables; jj++) {
      if(localFreezeMask[jj] == OFF) {
	for(sum = 0.0, ii = 0; ii < simplexPoints; ii++)
	  sum += (*localGuys[ii])[jj];
	xSum[kk++] = sum;
      }
    }
  }
  void findGoodBadUgly();
  double simplexOperator(double thisOperation, Individual *trialGuy);

public:

  Simplex(void);
  int localSearcher( Individual *theGuy,  Individual *localHero, int *freezeMask);
  //initLocalSearchParams(const int *freezeMask);
  ~Simplex(void);

};


//---------------------------------------------------------------------------
#endif
