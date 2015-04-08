#include "pub.h"

#define DEF_SET_PATH ".\\settings\\settings.cfg"
SETTINGS def_set={{0,0,500,500},100,".\\settings\\language\\english.txt",0, false};
SETTINGS SETT={0};
HANDLE h_file=NULL;

void settings_ls(SETTINGS *set,char ls)// 0 - Read; 1- Write
{
//	WriteLog("settings_ls(SETTINGS *set->ShowNoghi=%d, char ls=%d)",set->ShowNoghi,ls);
	
	DWORD tmp;

	if(!h_file)
		h_file = CreateFile(DEF_SET_PATH,GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if(h_file==(HANDLE)-1)
	{
		char path[600];
		GetPodNogamiPath(path, 0);
		strcat(path, "settings\\settings.cfg");
		h_file = CreateFile(path,GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);

		if(h_file==(HANDLE)-1)
		{
		h_file = NULL;
		return;
		}
	}
	tmp =0;
	switch(ls)
	{
	case 0:
		memcpy(set,&def_set,sizeof(SETTINGS));
		SetFilePointer(h_file,0,0,FILE_BEGIN);
		ReadFile(h_file,set,sizeof(SETTINGS),&tmp,NULL);		
	//	WriteLog("set->shownoghi=%d");
		
		
		break;
	case 1:
		
		SetFilePointer(h_file,0,0,FILE_BEGIN);
		WriteFile(h_file,set,sizeof(SETTINGS),&tmp,NULL);
		CloseHandle(h_file);
		h_file= NULL;
		break;
	}
	
	
}