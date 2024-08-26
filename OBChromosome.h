//---------------------------------------------------------------------------

#ifndef OBChromosomeH
#define OBChromosomeH

#include "DMOSPSolver.h"

// Operations-based solution representation

struct DMOSP_OBGene {
  DMOSP_Problem::SOperation *Operation;
  DMOSP_Problem::SMC *Machine;

  DMOSP_OBGene(DMOSP_Problem::SOperation *op, DMOSP_Problem::SMC *mc);
  DMOSP_OBGene(DMOSP_OBGene *g);
  DMOSP_OBGene();
  void Copy(DMOSP_OBGene *g);
  DMOSP_OBGene &operator = (DMOSP_OBGene &g);
};

class DMOSP_OBChromosome {
  private:
	DMOSP_Problem *Problem;
	int length;          // = number of operations
	DMOSP_OBGene *Genes;   // ordered list of genes (pairs of operation/machine)
	bool bConstructed;   // = true if the chromosome genes are assigned values

	class Decoder : public DMOSP_Algorithm {
	   private:
          DMOSP_OBChromosome *Chromosome;
		  double *mcrt,  // an array for the machine ready times
				 *jbrt;  // an array for the jobs release times which represent the
				         // minimum time at which the next unscheduled operation can start

	   public:
		  Decoder(DMOSP_OBChromosome *chrom);
		  ~Decoder();

		  virtual void AllocateMemory();
		  virtual void Initialize();
		  virtual bool Stop();
		  virtual void Finalize();
		  virtual void FreeMemory();
		  virtual void ComputeObjectiveValue();
	};

  public:
	DMOSP_OBChromosome(DMOSP_Problem *Prob);
	DMOSP_OBChromosome(DMOSP_OBChromosome *chrom);
	~DMOSP_OBChromosome();

	bool Copy(DMOSP_OBChromosome *chrom);
	DMOSP_OBChromosome &operator =(DMOSP_OBChromosome &chrom){
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
	DMOSP_OBGene *getGenes(){  return Genes; }
};


//---------------------------------------------------------------------------
#endif
