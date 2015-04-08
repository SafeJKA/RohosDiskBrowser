#include "pub.h"

#define PATH_SEP	'/'
#define PATH_SEPP	"/"
#define MAX_PATH	1024


//------------------------------------------------------------------------------------
void add_token(PATH_TOKEN *pt,char *pth_elt)
{
	pt->token  = (FS_FILE *)realloc(pt->token,(pt->t_cnt+1)*sizeof(FS_FILE));
	/*
	pt->hti	= (HTREEITEM *)realloc(pt->hti,(pt->t_cnt+1)*sizeof(HTREEITEM));
	memset(&pt->hti[pt->t_cnt],0,sizeof(HTREEITEM));
	*/
	memset(&pt->token[pt->t_cnt],0,sizeof(FS_FILE));


	pt->token[pt->t_cnt].ans_name = strdup(pth_elt);
	pt->t_cnt++;
}
//------------------------------------------------------------------------------------

char make_right_path(char *path,char *obuf)
{
	int i,sz,k;
	char ls;


	if(!path || !obuf)
		return 0;
	sz = strlen(path)+1;
	k=0;
	ls = '/';//path[0];
	for(i=0;i<sz;i++)
	{
		if(ls==path[i] && ls==PATH_SEP)
			continue;
		ls = obuf[k++] = path[i];

	}
	if(obuf[k-2]==PATH_SEP)
		obuf[k-2]=0;
	return 1;
}
//------------------------------------------------------------------------------------

// создать токен используя полный путь
//путь _НЕ_ должен быть со слэшем на конце
void strpath_to_token(char *upath, PATH_TOKEN *pt)
{
	char *buf,*pbuf;
	char *ptmp;
	DWORD sz;
	char path[MAX_PATH]={0};

	if(!path)
		return;
	if(pt->token)
		free(pt->token);
	pt->token=NULL;
	pt->t_cnt = 0;
	
	make_right_path(upath,path);
	sz = strlen(path);
	if(!sz)
		return;

	pt->pth = strdup(path);
	ptmp = strdup(path);

	pbuf=ptmp;
	while((buf=strstr(pbuf,PATH_SEPP)))
	{
		(*buf)=0;
		add_token(pt,pbuf);
		pbuf=buf+1;
	}
	add_token(pt,pbuf);
	free(ptmp);
}
//------------------------------------------------------------------------------------
// дополнить токен на директорию которая находиться на уровень ниже в иерархии папок
char path_down(PATH_TOKEN *pt,char *sub)
{

	DWORD sz1,sz2;

	if(!pt || !sub)
		return 0;

	sz1=sz2=0;
	sz1 = strlen(sub);
	if(!sz1)
		return 1;
	if(pt->pth)
		sz2 = strlen(pt->pth);
	pt->pth = (char *)realloc(pt->pth,sz1+sz2+4);
	if(pt->t_cnt)
		sprintf(pt->pth+sz2,"%c%s",PATH_SEP,sub);
	else
		sprintf(pt->pth+sz2,"%s",sub);

	add_token(pt,sub);

	return 1;
}
//------------------------------------------------------------------------------------
// убрать из токена самую последнюю директорию
char path_up(PATH_TOKEN *pt)
{
	int i;
	if(!pt)
		return 0;
	if(!pt->pth)
		return 0;

	i = strlen(pt->pth);
	if(!i)
		return 1;

	while(pt->pth[i]!=PATH_SEP && i--);

	if(i>=0)
		pt->pth[i]=0;
	else
		pt->pth[0]=0;

	if(pt->t_cnt)
		pt->t_cnt--;

	return 1;
}
//------------------------------------------------------------------------------------
//////освобождаем память
void free_fs_file(FS_FILE *ff)
{
	if(ff->ans_name)
		free(ff->ans_name);
	if(ff->uni_name)
		free(ff->uni_name);
}
//------------------------------------------------------------------------------------
void free_path_token(PATH_TOKEN *pt)
{
	for(int i=0;i<pt->t_cnt;i++)
		free_fs_file(&pt->token[i]);
	free(pt->pth);
	pt->token = NULL;
	pt->pth = NULL;
	pt->t_cnt = 0;
//	free(pt);
}