/* * * * * * * * * * * * * * * * * * * 
*  (C)   j3d1 aka Егор                 *
* ICQ:  8370005						  *
* mail: aux.in.out@gmail.com		  *
 * * * * * * * * * * * * * * * * * * */

#include <windows.h>
#include <malloc.h>
#include "af.h"


///////////
//формат
//
// % - символ экранирования
// %* - любой символ
// %% - символ процента


AF * add_af(AF **aaf,DWORD afc,char *rgl_equ,DWORD ofst,DWORD sx)
{

	AF *af=(*aaf);

	af = (AF *)realloc(af,sizeof(AF)*(afc+1));
	memset(&af[afc],0,sizeof(AF));
			
	if(rgl_equ)
	{
		af[afc].str = (char *)malloc(sx);
		memcpy(af[afc].str,rgl_equ+ofst,sx-1);
		af[afc].str[sx-1]=0;
		af[afc].dx = sx-1;
	}
	(*aaf)=af;
	return af;
}



void free_af(AF *af,DWORD afc)
{
	while(afc--)
		free(af[afc].str);
	free(af);
}

void free_crs(RETSTR *crs,DWORD afc)
{
	while(afc--)
		free(crs[afc].str);
	free(crs);
}


AF *make_af(char *rgl_equ,DWORD rsz,DWORD *af_cnt)
{
	DWORD i,ri,lri,x;
	AF *af;
	DWORD afc;
	
	lri = afc = ri=i=0;
	af = NULL;

	if(!rgl_equ)
		return NULL;

	if(!rsz)
		rsz = strlen(rgl_equ);


	while(ri<rsz)
	{
		lri=ri;
		while(rgl_equ[ri++]!='%' && ri<(rsz+1));
		//ri++;
		if(ri>=rsz)
			break;
		switch(rgl_equ[ri])
		{
			case '*':
				x = (ri-lri-1);
				if(x>0)
				{
					add_af(&af,afc,rgl_equ,lri,x+1);
					af[afc].cmd = AF_TYPE_STR;
					afc++;
				}
			
				add_af(&af,afc,NULL,0,0);
				af[afc].cmd = AF_TYPE_ANY;

				afc++;
				ri++;
				if(ri==rsz)
					lri=ri;
			break;

			case '%':
			default:
			x = (ri-lri);
			if(x)
			{
				add_af(&af,afc,rgl_equ,lri,x);
				af[afc].cmd = AF_TYPE_STR;
				afc++;
			}
			break;
		}

	}
	x = (ri-lri);
	if(x>0)
	{
		add_af(&af,afc,rgl_equ,lri,x);
		af[afc].cmd = AF_TYPE_STR;
		afc++;
	}

	(*af_cnt) = afc;
	return af;
}





void add_retstr(RETSTR *crs,DWORD ai,DWORD si,DWORD dx,char * str)
{
	if(crs[ai].str)
		free(crs[ai].str);
	crs[ai].str =(char *)malloc(dx+1);
	memcpy(crs[ai].str,str +si,dx);
	crs[ai].str[dx] =0;
	crs[ai].dx = dx;
	crs[ai].x = si;
}
/*
char *remove_from_str(char *str,char *del_str)
{
	char *tmp;
	DWORD i;
	while(
}
*/

void exec_af(AF *af,DWORD &ai,DWORD &si,DWORD &lsi,DWORD &llsi,RETSTR *crs)
{
	switch(af[ai].cmd)
	{
		case AF_TYPE_ANY:
			ai++;
		break;
	}
}


RETSTR *parse_string(char *str,DWORD sz,AF *af,DWORD afc,char flag)
{
	DWORD si,ri,ai,lai,dx,lsi,llsi,cmp;
	RETSTR *crs;
//Проверка на правельность входных данных	
	if(!str || !af)
		return NULL;

	if(!sz)
		sz= strlen(str);
	
	if(!sz || !afc)
		return NULL;
//////////////////////////////////////////
	
	crs = NULL;
	llsi = lsi = lai=ai=ri=si=0;
	
//Подсчитываем кол-во нужных буферов для возврата
	crs = (RETSTR *)malloc(sizeof(RETSTR)*afc);
	memset(crs,0,sizeof(RETSTR)*afc);
////////////

	exec_af(af,ai,si,lsi,llsi,crs);

	while(si<sz)
	{

		//обработка флагов
		switch(flag)
		{
			case PARSER_IGNORE_SPACE:
				if(str[si]==' ' || str[si]=='\t')
				{
					llsi=si++ +1;
					
					continue;
				}
			break;
		}
		///////


		dx = af[ai].dx;
		//
		if(dx+si<=sz)
			cmp=CompareString(LOCALE_USER_DEFAULT,0,str+si,dx,af[ai].str,dx);

			if(cmp==CSTR_EQUAL)
			{
				cmp = 4;
				///Если правило совпало
				if(!ai)
					llsi=lsi=si;
				
			//сохраняем результат!
				if((si-llsi)>0)
					add_retstr(crs,ai-1,llsi,si-llsi,str);
				add_retstr(crs,ai,si,dx,str);
				llsi = si+dx;
			/////////////////
				si+=dx-1;
				ai++;
				if(ai>=afc)
					break;
				exec_af(af,ai,si,lsi,llsi,crs);
				///
			}
		si++;
		//если массив закончился, а правила еще остались
		if(si>=sz && lsi<sz && ai)
		{
			if(ai==afc)
			{
				dx = crs[ai-2].x+crs[ai-2].dx;
				add_retstr(crs,ai-1,dx,si-dx,str);
				break;
			}
			ai=0;
			llsi=(si=lsi+1)+1;
		}

	}
	if(ai!=afc)
	{
		free(crs);
		return NULL;
		
	}
	return crs;
}



///////////////////////////////
/*
int main()
{
	AF *af;
	DWORD afc;
	RETSTR *crs;
	af= make_af("[%*][%*]",0,&afc);
	crs = parse_string(" 0 00   02  3 21 sadf qer aaq][d] aaa [a][b",0,af,afc,1);
	///
	af = make_af("%*=%*",0,&afc);
	crs = parse_string("absgd = 123",0,af,afc,1);

	free_af(af,afc);

	return 1;
}*/