
/////////////////////////////////////////////////////////
//Работа с реестром   reg_e.cpp
/////////////////////////////////////////////////////////
#include "pub.h"


void reg_ss(HKEY hsk,LPSTR rpth,LPSTR value,char *dbuf) 
{
	HKEY hk; 
	RegOpenKey(hsk,rpth,&hk);
	RegSetValueEx(hk,value,0,REG_EXPAND_SZ,(LPBYTE)dbuf,strlen(dbuf)); 
	RegCloseKey(hk); 
}

void reg_gs(HKEY hsk,LPSTR rpth,LPSTR value,char *dbuf)
{
    HKEY hk=NULL; 
	DWORD dwb=256,rt=REG_EXPAND_SZ;
	RegOpenKey(hsk,rpth,&hk);

	RegQueryValueEx(hk,value,0,&rt,(LPBYTE)dbuf,&dwb); 
	RegCloseKey(hk); 
}
 
void reg_sw(HKEY hsk,LPSTR rpth,LPSTR value,DWORD dbuf)
{
	HKEY hk;  
	RegOpenKey(hsk,rpth,&hk);
	RegSetValueEx(hk,value,0,REG_DWORD,(LPBYTE)&dbuf,sizeof(dbuf)); 
	RegCloseKey(hk);
}

void reg_gw(HKEY hsk,LPSTR rpth,LPSTR value,DWORD* dbuf)
{
	HKEY hk; 
	DWORD dwb=sizeof(dbuf),rt=REG_DWORD;

	RegOpenKey(hsk,rpth,&hk);
	RegQueryValueEx(hk,value,0,&rt,(LPBYTE)dbuf,&dwb); 
	
	RegCloseKey(hk);
}
void reg_ck(HKEY hsk,LPSTR rpth)
{
	HKEY hk;

	 if(RegCreateKeyEx(hsk,rpth,0,NULL,0,KEY_ALL_ACCESS,NULL,&hk,NULL)
		 !=ERROR_SUCCESS) MessageBox(0,"err","ERROR",MB_OK); 

	 RegCloseKey(hk); 
}

void reg_dv(HKEY hks,LPSTR rpth,LPSTR valuename)
{ 
    HKEY hk;
	RegOpenKey(hks,rpth,&hk);
	RegDeleteValue(hk,valuename);
	RegCloseKey(hk);
}

