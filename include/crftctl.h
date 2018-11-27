#ifndef __CRFT_CTL_H__
#define __CRFT_CTL_H__
#include <stdio.h>
#define SINGLEWORDLEN 256

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int genCRFSample(const char *path);
int genCRFSampleCtl(const char* fileName,int isDir,void * user);

void setCRFTrainFile(FILE *fp);
void setCRFTestFile(FILE *fp);
FILE * getCRFTrainFile();
FILE * getCRFTestFile();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif //__CRFT_CTL_H__
