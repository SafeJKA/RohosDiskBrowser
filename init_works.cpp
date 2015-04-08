#include "pub.h"

#define LIB_PATH "libntfs5.dll"
HMODULE libntfs = NULL;
#define LFNX(y,x,z)	x =(y)GetProcAddress(libntfs,z)

 _ntfs_mount ntfs_mount;
 _ntfs_umount ntfs_umount;
 _ntfs_inode_open ntfs_inode_open;
 _ntfs_inode_close ntfs_inode_close;
 _ntfs_mbstoucs ntfs_mbstoucs;
 _ntfs_ucstombs ntfs_ucstombs;
 _ntfs_inode_lookup_by_name ntfs_inode_lookup_by_name;
 _ntfs_readdir ntfs_readdir;
 _ntfs_attr_get_search_ctx ntfs_attr_get_search_ctx;
 _ntfs_attr_lookup ntfs_attr_lookup;
 _ntfs_get_attribute_value_length ntfs_get_attribute_value_length;
 _ntfs_attr_put_search_ctx ntfs_attr_put_search_ctx;
 _ntfs_attr_open ntfs_attr_open;
 _ntfs_attr_close ntfs_attr_close;
 _ntfs_attr_pread ntfs_attr_pread;
 _ntfs_attr_pwrite ntfs_attr_pwrite;


char init_ntfs()
{
	ntfs_volume *vol=NULL;
	
	inj_code("kernel32.dll","ReadFile",(long)&crypt_ReadFile);//	ReadFile(0,0,0,0,0);
	libntfs = LoadLibrary(LIB_PATH);

	// Если не получилось загрузить библиотеку - libntfs5.dll
	if(!libntfs)
	{
		// Вывод сообщения
		AfxMessageBox("Failed to load file: \"libntfs5.dll\" ",MB_ICONERROR	);
		PostQuitMessage(0);// И выходы из программы		
		return 0;
	}

	//ntfs_mount=(_ntfs_mount)GetProcAddress(libntfs,"ntfs_mount");
	//LFNX(ntfs_mount,_ntfs_mount,"ntfs_mount");
	
	LFNX(_ntfs_mount,ntfs_mount,"ntfs_mount");
	LFNX(_ntfs_umount, ntfs_umount,"ntfs_umount");
	LFNX(_ntfs_inode_open, ntfs_inode_open,"ntfs_inode_open");
	LFNX(_ntfs_inode_close, ntfs_inode_close,"ntfs_inode_close");
	LFNX(_ntfs_mbstoucs,ntfs_mbstoucs,"ntfs_mbstoucs");
	LFNX(_ntfs_ucstombs,ntfs_ucstombs,"ntfs_ucstombs");
	LFNX(_ntfs_inode_lookup_by_name, ntfs_inode_lookup_by_name,"ntfs_inode_lookup_by_name");
	LFNX(_ntfs_readdir,ntfs_readdir,"ntfs_readdir");
	LFNX(_ntfs_attr_get_search_ctx,ntfs_attr_get_search_ctx,"ntfs_attr_get_search_ctx");
	LFNX(_ntfs_attr_put_search_ctx, ntfs_attr_put_search_ctx,"ntfs_attr_put_search_ctx");
	LFNX(_ntfs_attr_lookup,ntfs_attr_lookup,"ntfs_attr_lookup");
	LFNX(_ntfs_get_attribute_value_length, ntfs_get_attribute_value_length,"ntfs_get_attribute_value_length");
	
	LFNX(_ntfs_attr_open ,ntfs_attr_open,"ntfs_attr_open");
	LFNX(_ntfs_attr_close, ntfs_attr_close,"ntfs_attr_close");
	LFNX(_ntfs_attr_pread, ntfs_attr_pread	,"ntfs_attr_pread");
	LFNX(_ntfs_attr_pwrite, ntfs_attr_pwrite,"ntfs_attr_pwrite");

    //vol = (mntfs)("s_i",TRUE);
	//vol = ntfs_mount("s_i",TRUE);
	return 1;
}


