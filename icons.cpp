#include "pub.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

char *get_ext(char *pth)
{
	int i;
	i = strlen(pth);
	while(pth[i]!='.' && i--);
	
	if(i==-1)
		return NULL;
	return pth+i;
	
}

int check_ext(char *ext)
{
	const int cnt = 2;
	char *ex[cnt] = {".exe"};
	int	 as[cnt] = {2},i,ret;

	ret = 0;
	for(i=0;i<cnt;i++)
		if(CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT,NORM_IGNORECASE,ext,-1,ex[i],-1))
		{
			ret = as[i];
			break;
		}

	return ret;
}

HICON extract_ext_icon(char *ext) //должнобыть с точкой впереди ".txt" - вот так
{
	HKEY hk=NULL;
	char buf[1024]={0};
	char str[4096];
	char *bf;
	UINT ndx;

	if(!ext)
		return NULL;
	
	reg_gs(HKEY_CLASSES_ROOT,ext,NULL,buf);
	
	if(!strlen(buf))
	{
		return ExtractIcon(GetModuleHandle(NULL),"shell32.dll",0);
		//return NULL;
	}
	
	sprintf(str,"%s\\DefaultIcon",buf);
	reg_gs(HKEY_CLASSES_ROOT,str,NULL,buf);
	
	ndx =0;
	bf = strstr(buf,",");
	
	if(bf)
	{
		(*bf) = 0;
		bf++;
		ndx = StrToInt(bf);
	}else
	{
		ndx = check_ext(ext);
		sprintf(buf,"shell32.dll");
	}
	return ExtractIcon(GetModuleHandle(NULL),buf,ndx);


}