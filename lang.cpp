#include "pub.h"
#include <shlwapi.h>
#include "af.h"

RETSTR *lang_file=NULL;
DWORD	lf_afc=0;
DWORD *msgs=NULL;
DWORD msgs_cnt;



char *get_msg(DWORD id)
{
	static char str[] = {"<string>"};


	if (msgs == NULL || msgs[id] == NULL)
		return str;

	return (char *)msgs[id];
}
void parse_msgs(char *buf,DWORD sz)
{
	RETSTR *crs;
	
	DWORD afc,id,ofs;
	AF *af;
	
	id=afc=ofs=0;
	af= make_af("{{%*}}",0,&afc);
	free(msgs);
	msgs=NULL;
	msgs_cnt=0;
	while( (crs=parse_string(buf+ofs,0,af,afc,1)) )
	{

		msgs = (DWORD *)realloc(msgs,(id+2)*sizeof(DWORD));
		msgs[id++]= (DWORD)crs[1].str;
		msgs[id]= 0;
		crs[1].str = NULL;
		msgs_cnt++;
		ofs+=crs[2].x+crs[2].dx;
		free_crs(crs,afc);
	}
	free_af(af,afc);
}

void accept_dlg_lang(HWND wnd,DWORD id)
{
	RETSTR *crs,*crr;
	char p_str[64];
	DWORD afc,afc2,ofs,sz,d;
	AF *af;
	char *buf;
	
	buf = lang_file[2].str;
	sz  = lang_file[2].dx;
	afc=ofs=0;
	sprintf(p_str,"[%i=%%*]%%*{%%*}",id);
	af= make_af(p_str,0,&afc);
	crs=parse_string(buf,sz,af,afc,1);
	if(!crs)
		return;
	free_af(af,afc);
	SetWindowText(wnd,crs[1].str);
	af = make_af("(%*)%*=%*\r",0,&afc2);
	ofs=0;
	while((crr=parse_string(crs[5].str+ofs,0/*crs[5].dx*/,af,afc2,1)))
	{
		d = StrToInt(crr[1].str);
		SetWindowText(GetDlgItem(wnd,d),crr[5].str);
		ofs+=crr[6].x+crr[6].dx;
		free_crs(crr,afc2);	
	}
	free_crs(crs,afc);
	free_af(af,afc2);
}

char *read_whole_file(char *pth,DWORD *r_sz)
{
	HANDLE hf;
	DWORD sz,r;
	char *buf;
	hf = CreateFile(pth,GENERIC_READ,FILE_SHARE_READ ,NULL,OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,NULL);
	if(hf==(HANDLE)-1)
		return NULL;
	sz = SetFilePointer(hf,0,0,FILE_END);
	if(!sz)
		return NULL;
	SetFilePointer(hf,0,0,FILE_BEGIN);
	buf = (char *)malloc(sz);
	r=0;
	ReadFile(hf,buf,sz,&r,NULL);
	CloseHandle(hf);

	if(r_sz)
		(*r_sz)=sz;
	return buf;

}	

void free_lang()
{
	free_crs(lang_file,lf_afc);
	for(int i=0;i<msgs_cnt;i++)
		free((void *)msgs[i]);
	free(msgs);
	msgs=NULL;
	lang_file=NULL;
	msgs_cnt=0;
}

char load_lang_file(char *pth)
{
	char *buf;
	DWORD sz=0;
	DWORD afc=0;
	AF *af;
	if(!(buf = read_whole_file(pth,&sz)))
		return 0;

	
	free_lang();//free_crs(lang_file,&lf_afc);
	af= make_af("%*[#(#)#]%*[#(#)#]%*",0,&lf_afc);
	lang_file=parse_string(buf,sz,af,lf_afc,1);
	

	load_menu_lang(lang_file[0].str,lang_file[0].dx);
	parse_msgs(lang_file[4].str,lang_file[4].dx);

	return 1;
}