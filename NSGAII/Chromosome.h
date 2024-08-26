/***************************************************************
/* Single & Multi-Objective Real-Coded Genetic Algorithms Code */
/* Author: Kumara Sastry                                       */
/* Illinois Genetic Algorithms Laboratory (IlliGAL)            */
/* Deparment of General Engineering                            */
/* University of Illinois at Urbana-Champaign                  */
/* 104 S. Mathews Ave, Urbana, IL 61801                        */
/***************************************************************/

//---------------------------------------------------------------------------

#ifndef ChromosomeH
#define ChromosomeH

#include "GlobalSetup.h"
#include "MOGARandom.h"
#include <iostream.h>

extern GlobalSetup *globalSetup;
extern MOGA_Random myRandom;

class Chromosome
{
  private:
    double *genes;
  public:
  Chromosome();
  Chromosome(const Chromosome &chrom);
  ~Chromosome();
  inline void setValue(const int &locus, const double &value) { genes[locus] = value; }
  inline double &operator[](const int &locus) const { return genes[locus]; }
  void mutateMinMax(const int *freezeMask);
  void mutateNormal(const int *freezeMask);
  void mutatePolynomial(const int *freezeMask);
  void mutatePolynomial(const int index);
  Chromosome &operator= (const Chromosome &sourceChrom);
  friend std::ostream &operator<< (std::ostream &out, Chromosome &chrom);
};


//---------------------------------------------------------------------------
#endif
