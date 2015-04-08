/**********************************************************************
** Copyright (C) 2005-2015 Tesline-Service S.R.L. All rights reserved.
**
** Rohos Disk Browser, Rohos Mini Drive Portable.
** 
**
** This file may be distributed and/or modified under the terms of the
** GNU Affero General Public license version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. If not, see <http://www.gnu.org/licenses/>
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.rohos.com/ for GPL licensing information.
**
** Contact info@rohos.com http://rohos.com
**/


#define WINWER	0x5000
#ifndef PUB_H
#define PUB_H

#include "stdafx.h"
#include <stdio.h>
#include "parse.h"
#include "resource.h"
#include "settings.h"
#include "filesys\filesys.h"
#include "common1.h" // Rohos software library common code

typedef unsigned __int64 u64;

extern "C" {
#include "filesys\ntfs\config.h"
#include "filesys\ntfs\volume.h"
#include "filesys\ntfs\dir.h"
#include "filesys\ntfs\attrib.h"
#include "filesys\ntfs\inode.h"
#include "filesys\ntfs\layout.h"
#include "filesys\ntfs\index.h"
#include "filesys\ntfs\ntfstime.h"
#include "filesys\ntfs\misc.h"
}


// (:
#define str_len strlen
#define m_alloc malloc
#define m_free	free
#define re_alloc realloc
#define str_dup	strdup

#define SECTOR_SIZE	512

void free_brows_structs(FS_FF *ff);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define KEY_SIZE   64
#define IV_SIZE    64
#define DISK_MAX_PATH   256

#define DISK_FLAGS_READ_ONLY		0x1
#define DISK_FLAGS_VISIBLE_IMAGE	0x4
#define	DISK_FLAGS_FIXED_MEDIA		0x8

typedef struct
{
 DWORD  Version;
 BYTE   Key[KEY_SIZE];
 BYTE   IV[IV_SIZE];
 CHAR   FileName[DISK_MAX_PATH];
 CHAR   AlgorithmName[20]; 
 DWORD  DiskNumber;
 DWORD  DiskSizeLow;
 DWORD  DiskSizeHigh;
 DWORD  BlockSize;
 DWORD  DiskFlags;
 DWORD  KeySizeInBytes;
 DWORD  _unnamed3;
 DWORD  _unnamed4; 
 DWORD  Flags[30];		// for partition based disk - , [0], [1] - physicaldisk offset to rohos 1st blob (LoWord, HiWord), 
						
 TCHAR  Reserved1[200]; // [0] encryption mode	[5],[6] - disk data offset in MBs (LoWord, HiWord)
 TCHAR  Reserved2[200]; // 
 TCHAR  Reserved3[200];
 BYTE   Reserved4[1024];
} DISK_BLOB_INFO;



class CIStreamFile;

/*
Generic File system wrapper, encrypted partition mounter, 

*/
class CFsys
{
private:
	bool InitializeBoxedApp();	

	TCHAR _curr_folder[500];

	DWORD _recent_docs_policy;
	
	

public:
	bool CreateVirtualFile(const char *s_pth, const char *virtual_file_name = NULL, bool quick_create = false);
	bool CreateVirtualFolder(const char *folderPath);
	bool _isNTFS;	/// TRUE если данный диск отформ под NTFS 
	bool _readOnly;	// TRUE если диск открыт в readOnly 
	bool _NTFS_inited;// TRUE если инициализаци€ NTFS уже произошла

	bool _showHiddenSystemFiles;
	bool boxed_app;	
	TCHAR rohos_file_path[500];
	
	///> Handle to file container.
	HANDLE fatvolume_handle, ntfsvolume_handle;       // FAT
	PARTITION *partition_;  //FAT
	BOOL partition_container;

	ntfs_volume *ntfs_volume; // NTFS volume 
	FILETIME _modification_time; // container modification time.
	//CIStreamFile* i_file;

	PVOID _pDiskPartitionClass;	///< if not NULL - read from %physicaldrive1%  device by pDiskPartitionClass
	BOOL _encrypted_disk; ///< TRUE this is encrypted Rohos partition.	

	LARGE_INTEGER _disk_free_space;
	LARGE_INTEGER _disk_total_space;

	 int  isNTFS();
	 int  OpenVolume(const char *path, bool encrypted_partition = true, int disk_data_offset_mb=0);
	char  BrowsePath(const char *path, FS_FF *flfd, bool sort = true);     
	char  isDirectory(const char* pth);
	int  CreateDirectory(const char *path, const char *folder_name);
	 int  Delete(const char *name);
	 u64  GetFileSize(const char *pth);
    char  SaveFile(const char *s_pth,  const char *d_pth, u64 sz,DWORD addr, DLGPROC callback_function = NULL	);
	char  LoadFile(const char *source, const char *destination, DLGPROC callback_function = NULL);
	char *ReadWholeFile(char *s_pth,DWORD *sz);
	char WriteWholeFile(LPBYTE pbData, DWORD data_size, const char *destination , DLGPROC callback_function);
	bool RenameItem(char *s_pth, char* new_path);
	void  CloseVolume(bool leave_handle=false);
	char  ShellExecute(const char *s_pth, bool openas=false, bool isolate=false, bool create_tree=false);
	int   GetDiskSizeMB();
	int   GetDiskFreeSpaceMB();
	int	  ReadDiskData( LPBYTE buff, DWORD size, LARGE_INTEGER offset);
	void  Enable_VirtualIsolation();
	void  Disable_VirtualIsolation();
	


	int OpenFAT(const char *path);
	   
		CFsys();
		virtual ~CFsys();	

};
extern        CFsys _FSys;// ƒл€ работы с файловой системой


void GetPodNogamiPath_Temp(LPTSTR path);


//wnd_fnx.cpp
void ctrls_init(CTreeCtrl &f_tree,CListCtrl &f_list,CImageList &f_img,CToolBar &f_tb,CWnd *wnd);
void load_list_files(char *path,CTreeCtrl &f_tree,CListCtrl &f_list,CImageList &f_img);
void make_folder_tree(PATH_TOKEN *pt,CTreeCtrl &f_tree,  LPCTSTR in3="");
void select_folder_tree(PATH_TOKEN *pt,CTreeCtrl &f_tree);
HICON extract_ext_icon(char *ext);


///reg_e.cpp

void reg_ck(HKEY,LPSTR);
void reg_ss(HKEY,LPSTR,LPSTR,char *);
void reg_sw(HKEY,LPSTR,LPSTR,DWORD);  
void reg_gs(HKEY,LPSTR,LPSTR,char*);
void reg_gw(HKEY,LPSTR,LPSTR,DWORD*);
void reg_dv(HKEY,LPSTR,LPSTR);



//crypt.cpp

void decrypt(char *buf,u64 pos,DWORD sz);
void encrypt(char *buf,u64 pos,DWORD sz);
char init_crypto(char *key,char *iv);

#define RH_KEY_SIZE			64
#define RH_KEY_SIZE2		32
#define RH_IV_SIZE			64
#define RH_BLOCK_SIZE		16


bool DeriveKeyFromPassw(LPCTSTR passw, BYTE Key[RH_KEY_SIZE], LPTSTR strKey = NULL);
bool DeriveKeyFromPassw_v2(LPCTSTR passw, BYTE Key[RH_KEY_SIZE], LPTSTR strKey = NULL);
bool EncryptBuffer( BYTE *Buffer, DWORD *BufferLen, DWORD BufferSize, const BYTE Key[RH_KEY_SIZE], const BYTE IV[RH_IV_SIZE] ) ;
bool DecryptBuffer( BYTE *Buffer, DWORD *BufferLen, const BYTE Key[RH_KEY_SIZE], const BYTE IV[RH_IV_SIZE] );



//icons.cpp
char *get_ext(char *pth);
HICON extract_ext_icon(char *ext); //должнобыть с точкой впереди ".txt" - вот так

//parse.cpp
void add_token(PATH_TOKEN *pt,char *pth_elt);
void strpath_to_token(char *path,PATH_TOKEN *pt);
char path_down(PATH_TOKEN *pt,char *sub);
char path_up(PATH_TOKEN *pt);
void free_fs_file(FS_FILE *ff);
void free_path_token(PATH_TOKEN *pt);
char make_right_path(char *path,char *obuf);

//TL_NTFSREADERDlg.cpp
#define TB_BT_UP		2000
#define TB_BT_LOAD		2001
#define TB_BT_SAVE		2002
#define TB_BT_VIEW1		2003
#define TB_BT_BACK		2004
#define TB_BT_FORWARD	2005
#define TB_BT_UP2		2006
extern CWnd *m_wnd;
extern HACCEL h_acl;
void process_msg();



//resizer.cpp
int CALLBACK RESIZER(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp);
void init_resizer(HWND hwnd,int pos);
void set_lr_wnd(HWND hl,HWND hr);
void resize_wnd(HWND hwnd);


//dlgs.h
char * file_dlg(char *filter, char* title ,char save_open,char *def_ext,char *def_path);
void show_prop_dlg(HWND h_prt,CListCtrl *f_lst,PATH_TOKEN *pt_tok);
void show_search_dlg(HWND h_prt,PATH_TOKEN *pt_tok);
void get_folder(char *fld);
void show_hex_dlg(HWND h_prt,PATH_TOKEN *pt_tok);

DWORD WINAPI show_copy_dlg(void *h);
void exit_copy_dlg(void);
void make_cpy_progress(int prg,char f,char *f_nme);
typedef void (*cpy_prg)(int ,char ,char *);

//settings.cpp
extern SETTINGS SETT;
void settings_ls(SETTINGS *set,char ls);

#endif