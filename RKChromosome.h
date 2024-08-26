//---------------------------------------------------------------------------

#ifndef RKChromosomeH
#define RKChromosomeH
#include "DMOSPSolver.h"

// Random keys solution representation
extern void RKQSort(DMOSP_Problem::SOperation **Ops, DMOSP_Problem::SMC **MCs,
                    double *key, int l, int r);

class DMOSP_RKChromosome {
  private:
	DMOSP_Problem *Problem;
	int length;          // = number of operations x 2
	double *Genes;       // ordered list of genes (pairs of operation/machine)
	bool bConstructed;   // = true if the chromosome genes are assigned values

	class Decoder : public DMOSP_Algorithm {
	   private:
		  DMOSP_RKChromosome *Chromosome;
		  double *mcrt,  // an array for the machine ready times
				 *jbrt;  // an array for the jobs release times which represent the
				         // minimum time at which the next unscheduled operation can start

	   public:
		  Decoder(DMOSP_RKChromosome *chrom);
		  ~Decoder();

		  virtual void AllocateMemory();
		  virtual void Initialize();
		  virtual bool Stop();
		  virtual void Finalize();
		  virtual void FreeMemory();
		  virtual void ComputeObjectiveValue();

		  DMOSP_Problem::SMC *SelectedMachine(int opIndex, double key);
	};

  public:
	DMOSP_RKChromosome(DMOSP_Problem *Prob);
	DMOSP_RKChromosome(DMOSP_RKChromosome *chrom);
	~DMOSP_RKChromosome();

	bool Copy(DMOSP_RKChromosome *chrom);
	DMOSP_RKChromosome &operator =(DMOSP_RKChromosome &chrom){
				   Copy(&chrom);
				   return *this;
				};

	bool Encode(DMOSP_Solution *Sol);   // Converts a given solution to a
										//   chromosome representation
	bool Decode(DMOSP_Solution *Sol);   // Convers the operations-based chromosome
										//   representation to solution space
										//   which is copied to the Sol reference

	bool isConstructed(){  return bConstructed; }
	void setConstructed(){ bConstructed = true; }
    DMOSP_Problem *getProblem(){  return Problem; }
	int getLength(){  return length;  }
	double *getGenes(){  return Genes; }
};

//---------------------------------------------------------------------------
#endif
