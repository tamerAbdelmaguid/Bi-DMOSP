//---------------------------------------------------------------------------

#ifndef VPBChromosomeH
#define VPBChromosomeH

#include "DMOSPSolver.h"

// Vectors of permutations-based (VPB) solution representation

struct DMOSP_VPBSubString {
  int start;    // starting position (inclusive)
  int end;      // ending position (inclusive)

  int nSeparators;     // number of ordered separators
  int *SeparatorPos;   // ordered positions of the separators

  DMOSP_VPBSubString(int s, int e, int n=0);
  DMOSP_VPBSubString();
  ~DMOSP_VPBSubString();

  void Copy(DMOSP_VPBSubString *substr);
  void Reset();

  void SetRange(int s, int n);
  void SetnSeparators(int ns);

  bool operator == (DMOSP_VPBSubString &ss);
  bool operator != (DMOSP_VPBSubString &ss){  return !(*this == ss); };
};

struct DMOSP_VPBString {
  int length;          	     		  // = number of operations
  DMOSP_Problem::SOperation **Gene;   // Genes or list of operations

  int nSubstrings;                    // number of sub-strings
  DMOSP_VPBSubString *Substring;      // list of substrings

  DMOSP_VPBString(int l, int ns);
  ~DMOSP_VPBString();

  bool Copy(DMOSP_VPBString *str);
  void Reset();

  bool operator == (DMOSP_VPBString &s);
  bool operator != (DMOSP_VPBString &s){  return !(*this == s); };
};

class DMOSP_VPBChromosome {
  private:
	class Decoder : public DMOSP_Algorithm {
	   private:
		  DMOSP_VPBChromosome *Chromosome;

		  SList *jbList; // Lists for operations in the order of processing for the jobs
		  SList *mcList; // lists for operations in the order of processing on the machines

	      int *nxt;      // Next schedulable operation index
		  double *mcrt,  // an array for the machine ready times
				 *jbrt;  // an array for the jobs release times which represent the
						 // minimum time at which the next unscheduled operation can start

	   public:
		  Decoder(DMOSP_VPBChromosome *chrom);
		  ~Decoder();

		  virtual void AllocateMemory();
		  virtual void Initialize();
		  virtual bool Stop();
		  virtual void Finalize();
		  virtual void FreeMemory();
		  virtual void ComputeObjectiveValue();
	};

    friend class Decoder;

	DMOSP_Problem *Problem;
	DMOSP_VPBString *jbString;          // Upper string for job routing
	DMOSP_VPBString *wsString;          // Lower string for workstations' orders
	bool bConstructed;   // = true if the chromosome genes are assigned values

	Decoder *decoder;

  public:
	DMOSP_VPBChromosome(DMOSP_Problem *Prob);
	DMOSP_VPBChromosome(DMOSP_VPBChromosome *chrom);
	~DMOSP_VPBChromosome();

	bool Copy(DMOSP_VPBChromosome *chrom);
	DMOSP_VPBChromosome &operator =(DMOSP_VPBChromosome &chrom){
				   Copy(&chrom);
				   return *this;
				};

	bool Encode(DMOSP_Solution *Sol);   // Converts a given solution to a
										//   chromosome representation
	bool Decode(DMOSP_Solution *Sol);   // Convers the VPB chromosome
										//   representation to solution space
										//   which is copied to the Sol reference

	bool isConstructed(){  return bConstructed; }
	void setConstructed(){ bConstructed = true; }
	DMOSP_Problem *getProblem(){  return Problem; }
	DMOSP_VPBString *getJobsString(){  return jbString; }
	DMOSP_VPBString *getWorkstationsString(){  return wsString; }
};

//---------------------------------------------------------------------------
#endif
