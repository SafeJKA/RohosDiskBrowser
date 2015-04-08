typedef struct 
{
	unsigned  __int64		inode;
	WCHAR	*uni_name;
	char	*ans_name; // file name + ext
	char	*ans_ext; // file ext
	DWORD	len; // nam len
	DWORD	sz;
	__int64 file_data_size; // file size
	__int64 file_last_change_date; // file size
}FS_FILE;

typedef struct
{
	FS_FILE *fle;
	DWORD	fl_c;
	FS_FILE *fld;
	DWORD	fd_c;
}FS_FF;

typedef struct
{
	char		*pth;	// полнуть путь данного токена 
	FS_FILE		*token; // массив структур папок (с:\temp\dir) 
	DWORD		t_cnt; // количество елементов в масиве token

}PATH_TOKEN;
