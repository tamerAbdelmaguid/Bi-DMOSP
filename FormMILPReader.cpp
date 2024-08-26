//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMILPReader.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMILPSol *frmMILPSol;
//---------------------------------------------------------------------------
__fastcall TfrmMILPSol::TfrmMILPSol(TComponent* Owner)
	: TForm(Owner)
{
  MILPSol = NULL;
}
//---------------------------------------------------------------------------
void TfrmMILPSol::ShowMILP(DMOSP_Solution *Sol)
{
  if(MILPSol) delete MILPSol;
  MILPSol = new DMOSP_MILPSolution(Sol);

  WriteCurrentMILPSol();

  this->ShowModal();
}
//---------------------------------------------------------------------------
void TfrmMILPSol::WriteCurrentMILPSol()
{
  int j, o;
  unsigned int x, y, z;
  char str[82], buf[80];
  int ln = 0;  // current line idnex
  int nchars = 0, buflen;

  memset(str, 0, 82);
  memoMILPSolution->Lines->Clear();

  // output of jobs' completion times
  //
  sprintf(buf, "c = [");
  buflen = strlen(buf);
  memcpy(&str[nchars], buf, sizeof(char) * buflen);
  nchars = buflen;

  for(j=0; j<MILPSol->nj; j++){
	sprintf(buf, "%1.0f ", MILPSol->c[j]);
	buflen = strlen(buf);
	if(nchars + buflen < 80){
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
	else{
	  str[nchars++] = '\r';
	  str[nchars] = '\n';
	  memoMILPSolution->Lines->Strings[ln++] =  str;
	  memset(str, 0, 82);
	  nchars = 0;
	  sprintf(buf, "     ");
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	  sprintf(buf, "%1.0f ", MILPSol->c[j]);
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
  }
  sprintf(buf, "];\r\n");
  buflen = strlen(buf);
  if(nchars + buflen < 80){
	memcpy(&str[nchars], buf, sizeof(char) * buflen);
	memoMILPSolution->Lines->Strings[ln++] =  str;
  }
  else{
	str[nchars++] = '\r';
	str[nchars] = '\n';
	memoMILPSolution->Lines->Strings[ln++] =  str;
	sprintf(buf, "    ];\r\n");
	memoMILPSolution->Lines->Strings[ln++] =  buf;
  }
  memset(str, 0, 82);
  nchars = 0;

  // output of operations' start times
  //
  sprintf(buf, "s = [");
  buflen = strlen(buf);
  memcpy(&str[nchars], buf, sizeof(char) * buflen);
  nchars = buflen;

  for(o=0; o<MILPSol->no; o++){
	sprintf(buf, "%1.0f ", MILPSol->s[o]);
	buflen = strlen(buf);
	if(nchars + buflen < 80){
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
	else{
	  str[nchars++] = '\r';
	  str[nchars] = '\n';
	  memoMILPSolution->Lines->Strings[ln++] =  str;
	  memset(str, 0, 82);
	  nchars = 0;
	  sprintf(buf, "     ");
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	  sprintf(buf, "%1.0f ", MILPSol->s[o]);
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
  }
  sprintf(buf, "];\r\n");
  buflen = strlen(buf);
  if(nchars + buflen < 80){
	memcpy(&str[nchars], buf, sizeof(char) * buflen);
	memoMILPSolution->Lines->Strings[ln++] =  str;
  }
  else{
	str[nchars++] = '\r';
	str[nchars] = '\n';
	memoMILPSolution->Lines->Strings[ln++] =  str;
	sprintf(buf, "    ];\r\n");
	memoMILPSolution->Lines->Strings[ln++] =  buf;
  }
  memset(str, 0, 82);
  nchars = 0;;

  // output of y decision variables
  //
  sprintf(buf, "y = [");
  buflen = strlen(buf);
  memcpy(&str[nchars], buf, sizeof(char) * buflen);
  nchars = buflen;

  for(y=0; y<MILPSol->ysz; y++){
	sprintf(buf, "%i ", MILPSol->y[y]);
	buflen = strlen(buf);
	if(nchars + buflen < 80){
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
	else{
	  str[nchars++] = '\r';
	  str[nchars] = '\n';
	  memoMILPSolution->Lines->Strings[ln++] =  str;
	  memset(str, 0, 82);
	  nchars = 0;
	  sprintf(buf, "     ");
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	  sprintf(buf, "%i ", MILPSol->y[y]);
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
  }
  sprintf(buf, "];\r\n");
  buflen = strlen(buf);
  if(nchars + buflen < 80){
	memcpy(&str[nchars], buf, sizeof(char) * buflen);
	memoMILPSolution->Lines->Strings[ln++] =  str;
  }
  else{
	str[nchars++] = '\r';
	str[nchars] = '\n';
	memoMILPSolution->Lines->Strings[ln++] =  str;
	sprintf(buf, "    ];\r\n");
	memoMILPSolution->Lines->Strings[ln++] =  buf;
  }
  memset(str, 0, 82);
  nchars = 0;;

  // output of x decision variables
  //
  sprintf(buf, "x = [");
  buflen = strlen(buf);
  memcpy(&str[nchars], buf, sizeof(char) * buflen);
  nchars = buflen;

  for(x=0; x<MILPSol->xsz; x++){
	sprintf(buf, "%i ", MILPSol->x[x]);
	buflen = strlen(buf);
	if(nchars + buflen < 80){
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
	else{
	  str[nchars++] = '\r';
	  str[nchars] = '\n';
	  memoMILPSolution->Lines->Strings[ln++] =  str;
	  memset(str, 0, 82);
	  nchars = 0;
	  sprintf(buf, "     ");
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	  sprintf(buf, "%i ", MILPSol->x[x]);
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
  }
  sprintf(buf, "];\r\n");
  buflen = strlen(buf);
  if(nchars + buflen < 80){
	memcpy(&str[nchars], buf, sizeof(char) * buflen);
	memoMILPSolution->Lines->Strings[ln++] =  str;
  }
  else{
	str[nchars++] = '\r';
	str[nchars] = '\n';
	memoMILPSolution->Lines->Strings[ln++] =  str;
	sprintf(buf, "    ];\r\n");
	memoMILPSolution->Lines->Strings[ln++] =  buf;
  }
  memset(str, 0, 82);
  nchars = 0;;

  // output of z decision variables
  //
  sprintf(buf, "z = [");
  buflen = strlen(buf);
  memcpy(&str[nchars], buf, sizeof(char) * buflen);
  nchars = buflen;

  for(z=0; z<MILPSol->zsz; z++){
	sprintf(buf, "%i ", MILPSol->z[z]);
	buflen = strlen(buf);
	if(nchars + buflen < 80){
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
	else{
	  str[nchars++] = '\r';
	  str[nchars] = '\n';
	  memoMILPSolution->Lines->Strings[ln++] =  str;
	  memset(str, 0, 82);
	  nchars = 0;
	  sprintf(buf, "     ");
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	  sprintf(buf, "%i ", MILPSol->z[z]);
	  buflen = strlen(buf);
	  memcpy(&str[nchars], buf, sizeof(char) * buflen);
	  nchars += buflen;
	}
  }
  sprintf(buf, "];\r\n");
  buflen = strlen(buf);
  if(nchars + buflen < 80){
	memcpy(&str[nchars], buf, sizeof(char) * buflen);
	memoMILPSolution->Lines->Strings[ln++] =  str;
  }
  else{
	str[nchars++] = '\r';
	str[nchars] = '\n';
	memoMILPSolution->Lines->Strings[ln++] =  str;
	sprintf(buf, "    ];\r\n");
	memoMILPSolution->Lines->Strings[ln++] =  buf;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMILPSol::btnRunClick(TObject *)
{
  // Read the CPLEX MILP solution 
  char buf[80];
  unsigned int strpos, bufpos;
  char *str;
  int ln = 0;
  while(ln < memoMILPSolution->Lines->Count){
	strpos = 0;
	str = memoMILPSolution->Lines->Strings[ln].t_str();
	switch(str[0]){
	  case 'c': while(str[strpos] != '[') strpos++;
				strpos++;
				for(int j=0; j<MILPSol->nj; j++){
				  while(str[strpos] < '0' || str[strpos] > '9') strpos++;
				  if(strpos >= strlen(str)){
					str = memoMILPSolution->Lines->Strings[++ln].t_str();
					strpos = 0;
				    while(str[strpos] < '0' || str[strpos] > '9') strpos++;
				  }
				  memset(buf, 0, 80*sizeof(char));
				  bufpos = 0;
				  while((str[strpos] >= '0' && str[strpos] <= '9') || str[strpos] == '.'){
					buf[bufpos] = str[strpos];
					bufpos++;
					strpos++;
				  }
				  MILPSol->c[j] = atof(buf);
				}
				ln++;
				break;

	  case 's': while(str[strpos] != '[') strpos++;
				strpos++;
				for(int o=0; o<MILPSol->no; o++){
				  while(str[strpos] < '0' ||  str[strpos] > '9') strpos++;
				  if(strpos >= strlen(str)){
					str = memoMILPSolution->Lines->Strings[++ln].t_str();
					strpos = 0;
				    while(str[strpos] < '0' || str[strpos] > '9') strpos++;
				  }
				  memset(buf, 0, 80*sizeof(char));
				  bufpos = 0;
				  while((str[strpos] >= '0' && str[strpos] <= '9') || str[strpos] == '.'){
					buf[bufpos] = str[strpos];
					bufpos++;
					strpos++;
				  }
				  MILPSol->s[o] = atof(buf);
				}
				ln++;
				break;

	  case 'y': while(str[strpos] != '[') strpos++;
				strpos++;
				for(unsigned int yi=0; yi<MILPSol->ysz; yi++){
				  while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  if(strpos >= strlen(str)){
					str = memoMILPSolution->Lines->Strings[++ln].t_str();
					strpos = 0;
				    while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  }
				  if(str[strpos] == '1') MILPSol->y[yi] = 1;
				  else MILPSol->y[yi] = 0;
				  strpos++;
				}
				ln++;
				break;

	  case 'x': while(str[strpos] != '[') strpos++;
				strpos++;
				for(unsigned int xi=0; xi<MILPSol->xsz; xi++){
				  while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  if(strpos >= strlen(str)){
					str = memoMILPSolution->Lines->Strings[++ln].t_str();
					strpos = 0;
				    while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  }
				  if(str[strpos] == '1') MILPSol->x[xi] = 1;
				  else MILPSol->x[xi] = 0;
				  strpos++;
				}
				ln++;
				break;

	  case 'z': while(str[strpos] != '[') strpos++;
				strpos++;
				for(unsigned int zi=0; zi<MILPSol->zsz; zi++){
				  while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  if(strpos >= strlen(str)){
					str = memoMILPSolution->Lines->Strings[++ln].t_str();
					strpos = 0;
				    while(str[strpos] != '0' && str[strpos] != '1') strpos++;
				  }
				  if(str[strpos] == '1') MILPSol->z[zi] = 1;
				  else MILPSol->z[zi] = 0;
				  strpos++;
				}
				ln++;
				break;

	  default:  ln++;
				break;
	}
  }

  // Modify tasks accordingly
  MILPSol->ModifyTasks();

  if(MILPSol->bTasksModified){
    ModalResult = mrOk;
  }
  else{
	Application->MessageBox(L"The given MILP solution was not interpretted correctly",
							L"Error reading MILP solution", MB_OK);
  }
}
//---------------------------------------------------------------------------

