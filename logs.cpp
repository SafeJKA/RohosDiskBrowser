#include "pub.h"



char *LOG_PATH=NULL;
HANDLE LOG_FILE=NULL;
char CUR_PATH[] = ".";


DWORD write_to_file(HANDLE hFile,void *buf,DWORD sz)
{
    DWORD bw;
    if(hFile==INVALID_HANDLE_VALUE)
        return 0;

    WriteFile(hFile,buf,sz,&bw,NULL);
    return bw;
}


void start_log(void)
{
	SYSTEMTIME lt;
	char *pth; //.\logs\00.00.00_00:00.log
	GetLocalTime(&lt);
	pth = (char *)m_alloc(str_len(CUR_PATH)+45);
	sprintf(pth,"%s%s(%0.2i.%0.2i.%0.4i)_(%0.2i.%0.2i.%0.2i).log",CUR_PATH,"\\logs\\",
						lt.wDay,lt.wMonth,lt.wYear,
						lt.wHour,lt.wMinute,lt.wSecond);
	LOG_FILE = CreateFile(pth,GENERIC_WRITE | GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,NULL); //open_for_write(pth);
	m_free(pth);

}

void log_put_raw(char *buf,DWORD sz)
{
	write_to_file(LOG_FILE,buf,sz);
}

//пока понимает только строку и лонг
void log_put(char *fmt,...)
{
	char	 *lbuf,*tmp,t,sl[35],pz;
	long	 sz, p, lp,b;
	DWORD	 dlt;
	

	va_list vl;
	
	if(!fmt || !str_len(fmt))
		return;

	va_start(vl, fmt);
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	sprintf(sl,"\n%0.2i.%0.2i.%0.2i %0.2i:%0.2i:%0.2i::%0.3i - ",
						lt.wDay,lt.wMonth,lt.wYear,
						lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);

	write_to_file(LOG_FILE,sl,str_len(sl));
	lp=p=0;
	sz = str_len(fmt);

	while(p<sz)
	{
		if(fmt[p]=='%')
		{
			if(p-lp)
			{
				lbuf=(char *)m_alloc(p-lp +1);
				CopyMemory(lbuf,fmt+lp,p-lp);
				lbuf[p-lp]=0;
				write_to_file(LOG_FILE,lbuf,p-lp);
				m_free(lbuf);
			}

			switch(fmt[p+1])
			{
				case 's':
					tmp = va_arg(vl,char *);
					write_to_file(LOG_FILE,tmp,str_len(tmp));
					p++;
				break;
				case 'i':
					b = va_arg(vl,long);
					if(b<0)
					{
						b=(~b)+1;
						t = '-';
						write_to_file(LOG_FILE,&t,1);
					}
					if(!b)
					{
						t='0';
						write_to_file(LOG_FILE,&t,1);
					}else
						dlt=1000000000;
						pz=0;
						while(b && dlt)
						{
							t = (b/dlt);
							b=b%dlt;
							dlt/=10;
							if(!t && !pz)
								continue;
							else
								pz=1;
							
							t+='0';
							write_to_file(LOG_FILE,&t,1);
						}
					p++;
				break;
			}

			lp=p+1;
		}
		p++;

	}
	
	if(sz-lp)
	{
		lbuf=(char *)m_alloc(sz-lp+1);
		CopyMemory(lbuf,fmt+lp,sz-lp);
		lbuf[sz-lp]=0;
		write_to_file(LOG_FILE,lbuf,sz-lp);
		m_free(lbuf);
	}
	va_end(vl);
}


void stop_log(void)
{
	CloseHandle(LOG_FILE);
	LOG_FILE = NULL;

}