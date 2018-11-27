#ifndef __EFTCTL_H__
#define __EFTCTL_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
int genEndSampleCtl(const char* fileName,int isDir);


void openLogFile();
void closeLogFile();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EFTCTL_H__
