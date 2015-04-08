// Fsys.cpp: implementation of the CFsys class.


//
//////////////////////////////////////////////////////////////////////
#include "pub.h"
#include "io.h"
#include <locale.h>

HANDLE hLogWrite_Mutex; 
time_t FILETIME_to_time_t(const FILETIME *ft);

extern PATH_TOKEN  PT_TOK;

#include "CDiskRepartition.h"
CDiskRepartition *theDiskPartition; // if not null - read from %physicaldrive1%  device by theDiskPartition 
LARGE_INTEGER filePos;				// set by crypt_SetFilePointer()
LARGE_INTEGER diskDataOffset;		// set by CFsys::OpenVolume();

BOOL is_encrypted_partition; // TRUE this is encrypted rohos disk. (False, if rohos is opening 2nd partition from usb drive)


#define O_RDONLY       0x0000  /* open for reading only */
#define O_WRONLY       0x0001  /* open for writing only */
#define O_RDWR         0x0002  /* open for reading and writing */
#define O_APPEND       0x0008  /* writes done at eof */
#define O_CREAT        0x0100  /* create and open file */
#define O_TRUNC        0x0200  /* open and truncate */
#define O_EXCL         0x0400  /* open only if file doesn't already exist */


typedef void ( WINAPI	* P_SHAddToRecentDocs) (UINT uFlags,    LPCVOID pv);
void My_SHAddToRecentDocs(          UINT uFlags,   LPCVOID pv)
{
	return;
}
HANDLE hHook__SHAddToRecentDocs; 

#ifdef _USE_BOXEDAPP_SDK

// file/registry virtualization SDK 

#include ".\\BoxedAppSDK\\include\\BoxedAppSDK.h"
#pragma comment(lib, ".\\BoxedAppSDK\\lib\\static_lib\\BoxedAppSDK.lib")

#endif  // _BOXEDAPP_USE


typedef void (_stdcall *PFUNCTION_ENTRYPOINT)(
										   HWND hwndStub,
										   HINSTANCE hAppInstance,
										   LPSTR lpCmdLine,
										   int    nCmdShow
										   );


CPtrArray arrayIFiles; // array with Isteam files. 

/**
On-the-fly encryption for virtual files.

CIStreamFile Required for  BoxedAppSDK_CreateVirtualFileBasedOnIStream()
see CFSys::CreateVirtualFile()

*/
class CIStreamFile : public IStream  
 {  

// Istream 
 public:
	 LONG m_nRefCount;  
     TCHAR fileName[500];
	 TCHAR virtualFileName[500];
     LARGE_INTEGER m_dwPosition;  	 
	 bool m_setsize_zero; // TRUE if file were zeroed
	 CFsys* fsys; 
	 time_t ctime, mtime, atime;

	 _reent *_fat_entry;
	 int	_fat_fd; 
	 ntfs_attr	*_ntfs_attr;
	 ntfs_inode *_ntfs_inode;
	 LARGE_INTEGER m_dwSize; // file size;
	 bool _isNTFS;
	 bool was_read, was_write;
	 

  protected:  
     // IUnknown  
     virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) ;
	 virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release(); 
	virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead) ;
	virtual HRESULT STDMETHODCALLTYPE Write(const void* pv, ULONG cb, ULONG* pcbWritten) ;
	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition) ;

	 virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)     
	 {     
		 was_write =1;

		 if (libNewSize.QuadPart == 0)
		 {
			 //WriteLog("ISetSize0 ");
			 m_setsize_zero = 1;
			 return S_OK;     

		 }

		 m_dwSize.QuadPart = libNewSize.QuadPart;
		 int ret;

		 //WriteLog("ISetSize %X", libNewSize.LowPart );

		 if (_isNTFS)
			 ret = ntfs_attr_truncate(_ntfs_attr, m_dwSize.QuadPart);
		 else 
			 ret = _FAT_ftruncate_r(_fat_entry, _fat_fd, m_dwSize.LowPart);
		
		 return S_OK;     

	 }  
  
    virtual HRESULT STDMETHODCALLTYPE Clone(IStream** ppstm)  
    {  
		CIStreamFile *file = new CIStreamFile(fsys);
		file->_fat_entry = _fat_entry;
		file->_fat_fd = _fat_fd;		
		file->_ntfs_attr = _ntfs_attr;
		file->_ntfs_inode = _ntfs_inode;
		file->m_dwSize = m_dwSize;
		file->_isNTFS = _isNTFS;
		file->m_setsize_zero = m_setsize_zero;
		_tcscpy(file->fileName, fileName);
        *ppstm = file;  
        return S_OK;  
    }  
      
    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten){
		return E_NOTIMPL;
	}      
    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags)
	{		
		return E_NOTIMPL;
	}  
    virtual HRESULT STDMETHODCALLTYPE Revert()
	{
		return E_NOTIMPL;
	}      
    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}      
    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	// 
    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pstatstg, DWORD grfStatFlag)
	{
		return E_NOTIMPL;
/*
 // Appling file times failed...  :( 
		FILETIME ftCreated, tfLastAccess, ftLastWrite;
		HANDLE m_hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		GetFileTime(m_hFile, &ftCreated, &tfLastAccess, &ftLastWrite);
		CloseHandle(m_hFile);

		WriteLog("IStream::Stat %d", grfStatFlag);

			pstatstg->ctime = ftCreated;
			pstatstg->atime = tfLastAccess; 
			pstatstg->mtime = ftLastWrite; 
			pstatstg->cbSize.QuadPart = m_dwSize.QuadPart;

			//pstatstg->

		
		return S_OK;*/
	}  

public:
	virtual ~CIStreamFile()  {
		 //m_pFile->Release();  		
		int i= -1;
	}

	 CIStreamFile(CFsys* pFsys) :   
         m_nRefCount(1),            
		 fsys(pFsys)
     {  
		 m_dwPosition.QuadPart = 0;
		 m_dwSize.QuadPart = 0xFFFFFFFF;
		 _isNTFS = pFsys->_isNTFS; 
		 was_read = was_write = 0;

		 _fat_entry = NULL;
		 _ntfs_attr = NULL;
		 _ntfs_inode = NULL;
		 m_setsize_zero = 0;

		          
     }  

		 /** if the FS attribute is not open still, oen it (for files that was creates in quick mode)
		 */
	 void openFileAttr()
	 {
		  if (_isNTFS)
		 {
			 if (_ntfs_attr == NULL) 
			 {
				 if (_ntfs_inode == NULL)
					 _ntfs_inode = ntfs_pathname_to_inode(fsys->ntfs_volume, NULL, fileName);

				 _ntfs_attr = ntfs_attr_open (_ntfs_inode, AT_DATA, NULL, 0);
			 }

		  }
		  else {

		  }

	 }

		 /**  return current file size (real)
		 */
	 LARGE_INTEGER getFileSize()
	 {
		 LARGE_INTEGER li;
		 li.QuadPart =0;
		 if (_isNTFS)
		 {
			 if (_ntfs_attr == NULL) 
			 {
				 if (_ntfs_inode)
					 li.QuadPart =  _ntfs_inode->data_size;
				 else 
					 li.QuadPart = 1;

			 }
			 li.QuadPart = _ntfs_attr->data_size;
			 //WriteLog("%X", li.LowPart);
			 return li;
		 }
		 else 
		 {			
			 FILE_STRUCT* file = (FILE_STRUCT*)_fat_fd;
			 li.QuadPart = file->filesize;
			 return li;
		 }
		 return li;

	 }

};

  // IUnknown  
 HRESULT STDMETHODCALLTYPE CIStreamFile::QueryInterface(REFIID riid, void** ppvObject)  
     {  
         *ppvObject = NULL;  
   
         if (IsEqualIID(IID_IUnknown, riid))  
             *ppvObject = this;  
         else if (IsEqualIID(IID_IStream, riid))  
             *ppvObject = this;  
         else if (IsEqualIID(IID_ISequentialStream, riid))  
             *ppvObject = this;  
   
         if (NULL != *ppvObject)  
         {  
             AddRef();  
             return S_OK;  
         }  
         else  
             return E_NOINTERFACE;  
     }  
       
      ULONG STDMETHODCALLTYPE CIStreamFile::AddRef()  
     {  
         InterlockedIncrement(&m_nRefCount);  
         return m_nRefCount;  
     }  
       
      ULONG STDMETHODCALLTYPE CIStreamFile::Release()  
     {  
         InterlockedDecrement(&m_nRefCount);  

		  
         LONG nRefCount = m_nRefCount;  
   
         if (0 == m_nRefCount)  
             delete this;  
   
         return nRefCount;  
     }  
   
     // ISequentialStream  
      HRESULT STDMETHODCALLTYPE CIStreamFile::Read(void* pv, ULONG cb, ULONG* pcbRead)  
     {  
		 was_read = 1;
		 DWORD dwWaitResult = WaitForSingleObject( hLogWrite_Mutex, 100000L);   
		 if ( dwWaitResult!=WAIT_OBJECT_0)
		 {
			 return S_OK;
		 }
         
         DWORD dwBytesToRead;  
		 openFileAttr();

		 //WriteLog("%X Read %X %X", this, m_dwPosition, cb);

		
		if (cb==0)
		{
			if (NULL != pcbRead)  
				*pcbRead = 0; 

			ReleaseMutex(hLogWrite_Mutex);
			return S_OK;
		}

		m_dwSize = 	 getFileSize();

			 
   
		if (m_dwPosition.QuadPart < 0 || m_dwPosition.QuadPart > m_dwSize.QuadPart)  
             dwBytesToRead = 0;  
         else  
             dwBytesToRead = cb < m_dwSize.QuadPart - m_dwPosition.QuadPart ?  cb : m_dwSize.QuadPart - m_dwPosition.QuadPart;  
   
    
		int ret =0;

		if (_isNTFS)
		{
			int offset=0;

			while (dwBytesToRead) {				
				ret = ntfs_attr_pread( _ntfs_attr, m_dwPosition.QuadPart+offset, dwBytesToRead, (char*)pv+offset);
				//WriteLog("Read %X %X %X (%X)", ret, offset, dwBytesToRead, cb);
				if (ret<=0)
					break;			
				dwBytesToRead   -= ret;
				offset += ret;				
			}

			if (ret)
				ret=cb;

		}
		else
		{
			FILE_STRUCT* file = (FILE_STRUCT*)_fat_fd;

			if ( file->currentPosition != m_dwPosition.LowPart)
				_FAT_seek_r(_fat_entry, _fat_fd, m_dwPosition.LowPart, SEEK_SET);

			ret = _FAT_read_r(_fat_entry, _fat_fd, (char*)pv, dwBytesToRead);
		}


		if (ret != cb )
		 {
			 int i = ret;
			 WriteLog("Read err %X (cb:%X) (size:%X  pos:%X)", ret, cb, m_dwSize, m_dwPosition.QuadPart);
			 //ret = cb;

			 //if (ret > cb)
			//	 return S_FALSE;
		 }
   
		m_dwPosition.QuadPart += ret;  
   
         if (NULL != pcbRead)  
             *pcbRead = ret; 
   
		 ReleaseMutex(hLogWrite_Mutex);
         return S_OK;  
     }  
       
      HRESULT STDMETHODCALLTYPE CIStreamFile::Write(const void* pv, ULONG cb, ULONG* pcbWritten)  
     {  
		 was_write = 1;

 
		  openFileAttr();
   
         DWORD dwBytesToWrite;  

		 if (cb==0)
		{
			if (NULL != pcbWritten)  
				*pcbWritten = 0; 

			return S_OK;
		}
		 
           
		 if (m_dwPosition.QuadPart < 0 /*|| m_dwPosition > m_dwSize*/)  
             dwBytesToWrite = 0;  
         else  
            dwBytesToWrite = cb; // < m_dwSize - m_dwPosition ?  cb : m_dwSize - m_dwPosition;  
   
         //CopyMemory(m_pFile->m_p + m_dwPosition, pv, dwBytesToWrite);  

		 int ret;

		 if (_isNTFS)
		 {
			 ret = ntfs_attr_pwrite( _ntfs_attr, m_dwPosition.QuadPart, dwBytesToWrite, pv);
			 
		 }
		 else{
			 _FAT_seek_r(_fat_entry, _fat_fd, m_dwPosition.LowPart, SEEK_SET);
			  ret = _FAT_write_r(_fat_entry, _fat_fd, (char*)pv, dwBytesToWrite);
			}

		 if (m_setsize_zero)  // after a Write() do setsize if it SetSize(0) were called before.
		 {			 
			 if (_isNTFS)
				 ret = ntfs_attr_truncate(_ntfs_attr, cb);
			 else 
				 ret = _FAT_ftruncate_r(_fat_entry, _fat_fd, cb);

			 m_setsize_zero = 0;
		 }

		 LARGE_INTEGER liFileSize = getFileSize();

		 if ( liFileSize.QuadPart != m_dwSize.QuadPart)
		 {			 
			 m_dwSize.QuadPart = liFileSize.QuadPart;			 
		 }

		 

		 /*if ( _ntfs_inode->data_size != m_dwSize)
			 m_dwSize = _ntfs_inode->data_size;*/

		 if (ret < cb)
		 {
			 ret = cb;
		 }
   
		 m_dwPosition.QuadPart += ret;  
   
         if (NULL != pcbWritten)  
             *pcbWritten = ret;  
   
         return S_OK;  
     }  
   
     // IStream  
      HRESULT STDMETHODCALLTYPE CIStreamFile::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)  
     {  
         //CMemoryFileLock lock(m_pFile);  
   
          //Note: new position can be more than m_dwSize  

		 //WriteLog("%X Seek %d %X", this, dwOrigin, dlibMove.QuadPart);
   
         switch (dwOrigin)  
         {  
             case STREAM_SEEK_CUR:  
             {  
				 if (dlibMove.QuadPart == 0)
					 break;

				 /*if ( !_isNTFS )
				 {					 
					 m_dwPosition = _FAT_seek_r(_fat_entry, _fat_fd, dlibMove.QuadPart, SEEK_CUR);
				 }else */
				 m_dwPosition.QuadPart += dlibMove.QuadPart;  
                 break;  
             }  
             case STREAM_SEEK_END:  
             {  				 

				 /*if ( !_isNTFS )
				 {
					 if ( m_dwPosition != getFileSize())
						m_dwPosition = _FAT_seek_r(_fat_entry, _fat_fd, dlibMove.QuadPart, SEEK_END);
				 }else */
				 LARGE_INTEGER liFileSize; 
				 liFileSize = getFileSize();
				 m_dwPosition.QuadPart = liFileSize.QuadPart + dlibMove.QuadPart;  
                 break;  
             }  
             case STREAM_SEEK_SET:  
             {  
				  /*if ( !_isNTFS )
				 {
					 if ( m_dwPosition != dlibMove.QuadPart)
						m_dwPosition = _FAT_seek_r(_fat_entry, _fat_fd, dlibMove.QuadPart, SEEK_SET);
				 }else */
				 m_dwPosition.QuadPart = dlibMove.QuadPart;  
                 break;  
             }  
             default:  
             {  
                 return E_FAIL;  
             }  
         }  
		 
   
         if (NULL != plibNewPosition)  
			 plibNewPosition->QuadPart = m_dwPosition.QuadPart;  
   
         return S_OK;  
     }  

// end of CIStreamFile
// ---------------------------------------------------------------



void decrypt(char *buf, u64 sectorNo , DWORD sz);
void encrypt(char *buf, u64 sectorNo , DWORD sz);
char init_crypto(char *key,char *iv);

extern char IV[64];
extern char Key[32];



extern "C" {

// this is Win32 API ReadFile() WriteFile()... replacement with Rohos encryption layer
// 
// called from :
// filesys\fat\disc.cpp(43)
// filesys\ntfs\win32_io.c

BOOL crypt_ReadFile(HANDLE hFile, LPVOID buf, DWORD psz, LPDWORD sz, LPOVERLAPPED lpOverlapped)
{
	BOOL ret;
	DWORD n_pos,n_sz,nr_sz,n__sz;
	DWORD *zn,tmp;
	DWORD pl,ph,pph,ppl;
	u64	  pos;
	char *n_buf;
	ret = 1;
	
	{
		
		n__sz=n_sz = psz;

		if (theDiskPartition)
		{			
			ret = theDiskPartition->ReadData( (LPBYTE)buf,n_sz,&theDiskPartition->partOffset);
			pos = theDiskPartition->partOffset.QuadPart;
			nr_sz = n_sz;
			//return ret;
		}  else
		{		
			
			pos = filePos.QuadPart;

			// позиция файла должна быть кратная 512 			
			nr_sz=0;	

			ret = ReadFile(hFile, buf,n_sz,&nr_sz,lpOverlapped);
		}

		if (is_encrypted_partition == false)
			return ret;						

		// encryption key was created here - FindBlob_CheckPassword()
		decrypt((char *)buf, (pos >> 9), nr_sz);  // (>>9 - conver to sector number / 512)

		//WriteLog("decrypt %X, %s", LOWORD(pos), (char *)buf);
		(*sz)=nr_sz;		

	}
	return ret;
}

BOOL crypt_WriteFile(HANDLE hFile, LPVOID buf, DWORD psz, LPDWORD sz, LPOVERLAPPED lpOverlapped)
{
	BOOL ret;
	DWORD n_pos,n_sz,nr_sz,n__sz;
	DWORD *zn,tmp;
	DWORD pl,ph,pph,ppl;
	u64	  pos;
	char *n_buf;

		

	ret = 1;

	
	{
		if (theDiskPartition)
		{
			pos = theDiskPartition->partOffset.QuadPart;

		} else
		{
			pos = filePos.QuadPart;
		}		

		n_buf = (char *)malloc(psz+32);
		memcpy(n_buf, buf, psz);

		if (is_encrypted_partition)
		{
			// encryption key was created here - FindBlob_CheckPassword()
			encrypt((char *)n_buf, (pos >>9), psz); // >>9 - to convert position to sector number (/512)
		}

		if (theDiskPartition)
		{			
			ret = theDiskPartition->WriteData( (LPBYTE)n_buf,psz, &theDiskPartition->partOffset);						
			*sz = psz;

			free(n_buf);

			return ret;
		} else
		{			
			nr_sz=0;
			ret = WriteFile(hFile, n_buf,psz, sz, lpOverlapped);
			*sz = psz;
			
		}
		
		free(n_buf);
		
		
	}
	return ret;
}

DWORD crypt_SetFilePointer(  HANDLE hFile,   LONG lDistanceToMove,  PLONG lpDistanceToMoveHigh,  DWORD dwMoveMethod )
{
	DWORD ret;

	if (theDiskPartition)
	{
		if ( dwMoveMethod == FILE_CURRENT && lDistanceToMove == 0)
		{
			*lpDistanceToMoveHigh = theDiskPartition->partOffset.HighPart;
			return theDiskPartition->partOffset.LowPart;
		}
		LARGE_INTEGER offs2;
		offs2.QuadPart=0;
		offs2.LowPart = lDistanceToMove;
		if (lpDistanceToMoveHigh)
			offs2.HighPart = *lpDistanceToMoveHigh;
		
		theDiskPartition->SetFilePointer2(&offs2);
		return lDistanceToMove;
	}

	filePos.QuadPart =0;
	filePos.LowPart = lDistanceToMove;
	if (lpDistanceToMoveHigh)
		filePos.HighPart = *lpDistanceToMoveHigh;

	filePos.QuadPart += diskDataOffset.QuadPart;
	ret = SetFilePointerEx( hFile,   filePos,  NULL,  dwMoveMethod );

	filePos.QuadPart -= diskDataOffset.QuadPart; // return to initial position becase of 'position' should be as real offset in disk image
	return ret;
}


BOOL crypt_SetFilePointerEx(  HANDLE hFile,  LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer,  DWORD dwMoveMethod)
{
	DWORD ret;
	LONG moveHight = liDistanceToMove.HighPart;
	ret = crypt_SetFilePointer( hFile,   liDistanceToMove.LowPart,  &liDistanceToMove.HighPart,  dwMoveMethod );

	// lpNewFilePointer - in our case it is NULL

	if (liDistanceToMove.LowPart == INVALID_SET_FILE_POINTER &&
			GetLastError() != NO_ERROR) {		
		return FALSE;
	}

	return true;
}

} // extern "C" 





//*////////////////////////////////////////////////////////////////////
// Construction/Destruction //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
CFsys::CFsys()
{
	_isNTFS = false;// Временно. Для тестирования FAT образов
	_NTFS_inited = false;
	_showHiddenSystemFiles = boxed_app = partition_container = false; 
	//i_file = NULL;
	theDiskPartition = NULL;	
	partition_ = NULL;
	ntfs_volume = NULL;
}
CFsys::~CFsys()
{

}




void  CFsys::CloseVolume(bool leave_handle /* = false do not close FileImage handles */)
{
	CIStreamFile* i_file =0;	

	if (partition_ == NULL && ntfs_volume == NULL )
		return;	

	if ( _isNTFS )
	{
		if (!ntfs_volume)
		{
			if (leave_handle == false)
			{
				if (_modification_time.dwHighDateTime)
					SetFileTime(ntfsvolume_handle, NULL, NULL, &_modification_time);
				CloseHandle(ntfsvolume_handle);
			}

			return;
		}

		WriteLog("Close n Volume %X", _modification_time.dwHighDateTime);
		
		
#ifdef _USE_BOXEDAPP_SDK
		while ( arrayIFiles.GetCount() )
		{
			i_file = (CIStreamFile*)arrayIFiles.GetAt(0);
			BoxedAppSDK_DeleteFileFromVirtualFileSystem(i_file->virtualFileName);

			if (i_file->_ntfs_attr ){
				ntfs_time_update_flags tuf1=(ntfs_time_update_flags)0;
				if (i_file->was_read)
					tuf1 = NTFS_UPDATE_ATIME;
				if (i_file->was_write)
					tuf1 = (ntfs_time_update_flags) (NTFS_UPDATE_ATIME | NTFS_UPDATE_MTIME);

				ntfs_inode_update_times(i_file->_ntfs_attr->ni, tuf1);				
				ntfs_attr_close(i_file->_ntfs_attr);
			}
			if (i_file->_ntfs_inode);
				ntfs_inode_close(i_file->_ntfs_inode);	
			arrayIFiles.RemoveAt(0);
		}
#endif

		ntfsvolume_handle = (HANDLE)ntfs_volume->dev->d_handle;

		if ( ntfs_umount(ntfs_volume, (BOOL1)1) )
		{
			WriteLog("error CloseVolume");
		}

		if (leave_handle == false)
		{
			//if (_modification_time.dwHighDateTime)			
			BOOL b = 	SetFileTime(ntfsvolume_handle, NULL, NULL, &_modification_time);
			WriteLog("tm %i %X", b, GetLastError());
			CloseHandle(ntfsvolume_handle);
		}

		ntfs_volume = NULL;
	}
	else
	{
		if (!partition_)
		{
			if (leave_handle == false)
			{
				if (_modification_time.dwHighDateTime)
					SetFileTime(fatvolume_handle, NULL, NULL, &_modification_time);

				CloseHandle(fatvolume_handle);
			}

			return;
		}

		WriteLog("Close f Volume");

#ifdef _USE_BOXEDAPP_SDK
		while ( arrayIFiles.GetCount() )
		{
			i_file = (CIStreamFile*)arrayIFiles.GetAt(0);
			BoxedAppSDK_DeleteFileFromVirtualFileSystem(i_file->virtualFileName);			
			
			_FAT_close_r(i_file->_fat_entry,i_file->_fat_fd);
			arrayIFiles.RemoveAt(0);
		}
#endif

		if (partition_)
			_FAT_partition_destructor(partition_);

		if (leave_handle == false)
		{
			if (_modification_time.dwHighDateTime)
					SetFileTime(fatvolume_handle, NULL, NULL, &_modification_time);

			CloseHandle(fatvolume_handle);
		}

		partition_ = NULL;
	}

#ifdef _USE_BOXEDAPP_SDK
	if (boxed_app)
	{
		//BoxedAppSDK_UnhookFunction(hHook__SHAddToRecentDocs);  

		//if (_recent_docs_policy)
		//AfxMessageBox("1");
		WriteLog("Boxed App close %d", _recent_docs_policy);


		BoxedAppSDK_Exit();


		WriteReg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoRecentDocsHistory", (DWORD)_recent_docs_policy);
		
		boxed_app = false;

		TCHAR virtFileName[500];
		strcpy(virtFileName, "c:\\");
		virtFileName[0] = _curr_folder[0];
		strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));

		::RemoveDirectory(virtFileName);		
	}
#endif 

	if (theDiskPartition)
	{
		if (leave_handle == false)
			theDiskPartition->CloseDisk();

		_pDiskPartitionClass = NULL;

		TCHAR rohos_file_path[MAX_PATH];

		GetPodNogamiPath(rohos_file_path, false);
		_tcscat(rohos_file_path, "0x111.dat");

		DeleteFile(rohos_file_path);
	}

	

}
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Проверяет файловую систему образа и маунтит его ///////////////////
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
int CFsys::OpenVolume(const char *path, bool encrypted_partition/* = true*/, int disk_data_offset_mb/*=0*/)
{
	HANDLE hFile; 

	hLogWrite_Mutex =  CreateMutex(  NULL,  FALSE,   "RohosDiskBrowserMutex"); 
	
	_tcscpy(rohos_file_path, path);
	_encrypted_disk = is_encrypted_partition = encrypted_partition;

	GetPodNogamiPath(_curr_folder, false );
	_pDiskPartitionClass = NULL;
	
	
	// offset of Disk Data inside of disk blob
	diskDataOffset.QuadPart = disk_data_offset_mb;
	diskDataOffset.QuadPart *= 1048576; /*1Mb*/

	if ( _tcslen(path) <=4 /*just drive letter - partition based encryption */)	
	{
		// open partition 

		if (theDiskPartition == NULL)
		{
			theDiskPartition =  new CDiskRepartition();			
		}
		else 
			theDiskPartition->CloseDisk();

		_pDiskPartitionClass = theDiskPartition;

		
		theDiskPartition->SetDriveLetter(path[0]);
		if ( theDiskPartition->CheckDisk() && theDiskPartition->GetUnpartitionedSpace().QuadPart )
		{
			
			// create fake file			
			GetPodNogamiPath(rohos_file_path, false);
			_tcscat(rohos_file_path, "0x111.dat");
			HANDLE h = CreateFile(rohos_file_path,  GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			CloseHandle(h);

		}
		else
		{
			theDiskPartition->CloseDisk();			
			_pDiskPartitionClass = NULL;
			return false;
		}

		theDiskPartition->partOffset.QuadPart = 0; 
		LARGE_INTEGER liOffset;
		liOffset.QuadPart = disk_data_offset_mb;
		liOffset.QuadPart *= 1048576; /*1Mb*/

		theDiskPartition->SetDataOffset(&liOffset);

		WriteLog("Opening partition container");

	}
	
	// открыть файл и дешифровать первые 512 байт
	hFile = CreateFile(rohos_file_path,    // file to open
		GENERIC_READ,          // open for reading and writing
		FILE_SHARE_READ | FILE_SHARE_WRITE,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	_modification_time.dwHighDateTime = _modification_time.dwLowDateTime = 0;
	
	if ( IsFileUseSteganos(rohos_file_path) == true)
		GetFileTime(hFile, NULL, NULL, &_modification_time);

	// 
	if (hFile != INVALID_HANDLE_VALUE) 
	{ 

		BYTE buff[512];
		DWORD dwRead=0;

	

		// read first 512 byte to detect FS

		crypt_SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

		int i=0;
		for (i =1; i<=100; i++ )
		{
			crypt_ReadFile(hFile, (LPVOID)buff, 512, &dwRead, NULL);			

			_isNTFS=false;		
			// проверить NTFS или FAT ?		
			if( (buff[3]=='N')&&(buff[3+1]=='T')&&(buff[3+2]=='F')&&(buff[3+3]=='S')  )
			{ 
				_isNTFS=true;				
				WriteLog("Found NTFS %X", theDiskPartition->partOffset2.QuadPart);
				break;
			} 

			

			if ((buff[0x36] == 'F') && (buff[0x37] == 'A') && (buff[0x38] == 'T')) 
			{
				WriteLog("Found FAT32 %X", theDiskPartition->partOffset.QuadPart);				
				//theDiskPartition->partOffset2.QuadPart = theDiskPartition->partOffset.QuadPart;
				break;
			} 

			if ((buff[0x52] == 'F') && (buff[0x53] == 'A') && (buff[0x54] == 'T')) 
			{
				// Check for FAT32
				WriteLog("Found FAT %X", theDiskPartition->partOffset.QuadPart);				
				//theDiskPartition->partOffset2.QuadPart = theDiskPartition->partOffset.QuadPart;
				break;
			} 

			//WriteLog("%X %X", partOffset.QuadPart, partOffset2.QuadPart);				

			if (theDiskPartition)
				theDiskPartition->partOffset2.QuadPart += 512;
		}


		CloseHandle(hFile);

		if (i == 100)
		{
			WriteLog("partition not found");

			//return 0;
		}



	} else
	{
		WriteLog(" CFsys::OpenVolume error");
		return 0;
	}

	//_isNTFS=true;

	if ( _isNTFS )
	{
		_readOnly = FALSE;  /// NTFS открывает только в read only
	    //AfxMessageBox(get_msg(MSG_OPENED_READ_ONLY), MB_ICONINFORMATION)

		
		ntfs_volume = ntfs_mount(rohos_file_path, /*MS_RDONLY*/ 0);
		if (!ntfs_volume)
		{
			WriteLog(" CFsys::ntfs_mount error");

			ntfs_volume = ntfs_mount(rohos_file_path, MS_RECOVER | MS_IGNORE_HIBERFILE);
			if (!ntfs_volume)
			{
				WriteLog(" CFsys::ntfs_mount error recover");

				ntfs_volume = ntfs_mount(rohos_file_path, MS_RDONLY | MS_IGNORE_HIBERFILE);
				if (!ntfs_volume)
				{
					char bf[1024]={0};

					WriteLog(" CFsys::ntfs_mount error readonly");

					sprintf(bf,LS(IDS_BAD_DISKINFO),rohos_file_path);
					AfxMessageBox(bf, MB_ICONWARNING);
					return 0;
				}
				else{
					//AfxMessageBox(LS(IDS_MOUNT_AS_READONLY), MB_ICONINFORMATION);
					_readOnly = true;		
				}
				

			} 
			
		}		
		GetDiskFreeSpaceMB();
		GetDiskSizeMB();
		

		return 1;
	}
	else // if isFAT
	{
		_readOnly = FALSE;
		 int i = OpenFAT(rohos_file_path);		 		 

		 if (i < 0 )			 
		 {
			 WriteLog(" CFsys::OpenFAT error %d", i);

			 char bf[1024]={0};

			 sprintf(bf,LS(IDS_BAD_DISKINFO),rohos_file_path);
			 AfxMessageBox(bf, MB_ICONWARNING);

			 return 0;
		 }		

		 GetDiskFreeSpaceMB();
		 GetDiskSizeMB();
	   	
		// code...
		return i;
	}	  

 return 0;
}
//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Проверяет если файловая система - NTFS ////////////////////////////
//////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------|
int CFsys::isNTFS()
{
	return 0;	
}

// sorting 
// l = 0, r = sizeof(fld)
// 
int QuickSort_FS_FF(int l, int r, FS_FILE *fld);

void QuickSort_FS(int l, int r, FS_FILE *fld)
{
	int middle,i;
	if(l<r)
	{
		middle=QuickSort_FS_FF(l,r,fld);
		QuickSort_FS(l, middle-1, fld);
		QuickSort_FS(middle+1, r, fld);
	}
}

int QuickSort_FS_FF(int l, int r, FS_FILE *fld)
{
	//if (l > r || r<=0) return; 

	/*int i = l;
	int j = r;
	int center = l;
	FS_FILE fld2;	

	while (i<j)
	{		
		while ( stricmp(fld[center].ans_name, fld[i].ans_name) >= 0 && i<j)
			i++;			
	
		while ( stricmp(fld[center].ans_name, fld[j].ans_name) < 0 && j>0)
			j--;		

		if (i<j)
		{
			fld2=fld[i];
			fld[i]=fld[j];
			fld[j]=fld2;
			
		}
	}	
	
	return j;*/

	FS_FILE fld_r, fld2;
	fld_r = fld[r];

	int j = l -1;

	for (int i = l; i < r; i++) {

		if (/*x <= a[i]*/ stricmp(fld_r.ans_name, fld[i].ans_name) > 0) {
			j = j + 1;

			/*int temp = a[j];
			a[j] = a[i];
			a[i] = temp;*/

			fld2=fld[j];
			fld[j]=fld[i];
			fld[i]=fld2;

		}
	}
	fld[r] = fld[j+1]; //a[r] = a[j + 1];
	fld[j+1] = fld_r; //a[j + 1] = x;

	return (j + 1);

}

typedef struct NntfaDirent{
	ntfs_volume *vol;
	FS_FF *ff;
	bool showHiddenSystemFiles;
} ntfsls_dirent;



//-----------------------------------------------------------------------------------------
// NTFS callback function. list folder contents
//-----------------------------------------------------------------------------------------
int ntfs_enumerator(void *dirent, const ntfschar *name,
		const int name_len, const int name_type, const s64 pos,
		const MFT_REF mref, const unsigned dt_type,  const s64 data_size, const s64 last_data_change_time)
{
	char *filename = NULL;
	int result = 0;
	FS_FILE *fl;
	FS_FILE **ffl;
	DWORD *fl_c;

	ntfsls_dirent *dr = (ntfsls_dirent *)dirent;

	/*if (global_showHiddenSystemFiles == false)
	{
		if ( is_file_hidden(dr, mref) )
			return 0;

	}*/

	if(name_type !=0 && name_type !=1 && name_type!=3)
		return 0;

	filename =(char *) calloc (1, MAX_PATH);
	if (!filename)
		return -1;
	
	int cnt = WideCharToMultiByte( CP_ACP, 0, (LPWSTR)name, name_len,	filename, MAX_PATH, NULL, NULL );

	if(filename[0]=='$' || !strcmp(filename, "..") || !strcmp(filename, ".") || strlen(filename) == 0 )
	{
		free(filename);
		return 0;
	}
	if (dt_type != NTFS_DT_DIR )
	{
		fl  = dr->ff->fle;
		fl_c= &dr->ff->fl_c;
		ffl = &dr->ff->fle;
		//dr->ff->fle = fl;
		//sprintf(filename + strlen(filename), "  <DIR>");
	}else
	{
		fl  = dr->ff->fld;
		fl_c= &dr->ff->fd_c;
		ffl = &dr->ff->fld;
		//dr->ff->fld = fl;

	}

	fl = (FS_FILE *)realloc(fl,((*fl_c)+1)*sizeof(FS_FILE));

	if (dt_type != NTFS_DT_DIR )
	{
		fl[(*fl_c)].file_data_size = data_size;
		fl[(*fl_c)].file_last_change_date = last_data_change_time;

	}

	fl[(*fl_c)].inode = MREF(mref);
	fl[(*fl_c)].len = name_len;
	fl[(*fl_c)].uni_name = _wcsdup( (WCHAR *)name);
	fl[(*fl_c)].ans_name = filename;
	(*fl_c)++;
	(*ffl) = fl;
//	if(1)
//		add_lb(filename);
//	free(filename);
	return 0;
}


__int64 TimetToFileTime( time_t t)
{
	ULARGE_INTEGER ui;
    LONGLONG ll = UInt32x32To64(t, 10000000) + 116444736000000000;
    ui.LowPart = (DWORD) ll;
    ui.HighPart = ll >>32;	

	return ui.QuadPart;	
}


//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Достаёт список папок в указанном каталоге /////////////////////////
//////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
										//brows_path(full_path,&ff);
char CFsys::BrowsePath(const char *path, FS_FF *flfd, bool sort)
{

	setlocale(LC_ALL,"");


//	WriteLog(">>>>>CFsys::BrowsePath(%s, ...)",path);
	if ( _isNTFS)
	{
		
		//brows_path(path, flfd, _showHiddenSystemFiles);

		ntfs_inode *ni;
		ntfsls_dirent dirent;
		s64 pos;
		
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);

		if(!ni)
			return 0;
		pos = 0;
		memset(&dirent, 0, sizeof(dirent));
		dirent.vol = ntfs_volume;
		dirent.ff = flfd;
		dirent.showHiddenSystemFiles = _showHiddenSystemFiles;

		ntfs_readdir(ni, &pos, &dirent, (ntfs_filldir_t)ntfs_enumerator);
		ntfs_inode_close(ni);
		
		if (!sort)
			return 0;
		// todo : sorting 
		
		if (flfd->fl_c > 1)
			QuickSort_FS(0, flfd->fl_c-1, flfd->fle);

		if (flfd->fd_c > 1)
			QuickSort_FS(0, flfd->fd_c -1, flfd->fld);

		return 0;
	}
	else // if isFAT
	{
		
		
		DIR_STATE_STRUCT mDirState;
		DIR_STATE_STRUCT *mDirNextState;
		memset(&mDirState, 0, sizeof(mDirState));
		char filename[MAX_PATH];
		struct stat filestat;

		struct _reent r, rf;
		
		FILE_STRUCT mFile;
		mDirNextState = _FAT_diropen_r(&r, &mDirState, path);
		if (mDirState.validEntry)
		{
			while (mDirState.validEntry)
			{
				if (_showHiddenSystemFiles==false && 
					((mDirState.currentEntry.entryData[DIR_ENTRY_attributes] & ATTRIB_SYS) || 
					(mDirState.currentEntry.entryData[DIR_ENTRY_attributes] & ATTRIB_HID) ) )
				{
					_FAT_dirnext_r(&r, &mDirState, filename, &filestat);
					continue;
				}
				
				if (_FAT_directory_isDirectory(&mDirState.currentEntry))
				{
					//папка 
					/*
					if (strcmp(mDirState.currentEntry.filename, ".")==0 || 
						strcmp(mDirState.currentEntry.filename, "..")==0  ) 
						*/
					if( _FAT_directory_isDot (mDirState.currentEntry.filename) )
						
					{
						_FAT_dirnext_r(&r, &mDirState, filename, &filestat);
						continue;
					}					
					
					flfd->fld = (FS_FILE *)realloc(flfd->fld,(flfd->fd_c+1)*sizeof(FS_FILE));

					memset(&flfd->fld[flfd->fd_c],0,sizeof(FS_FILE));

					//flfd->fld[flfd->fd_c].ans_name = strdup(mDirState.currentEntry.filename);

					{
						char strOut[460];
						WideCharToMultiByte(CP_ACP, 0, (WCHAR*)mDirState.currentEntry.lfilename, -1, strOut, 255, NULL, NULL);
						if ( strlen(strOut) ) // если есть unicode значение то взять его. если нте взять ANSI 
							flfd->fld[flfd->fd_c].ans_name = strdup(strOut);
						else 
							flfd->fld[flfd->fd_c].ans_name = strdup(mDirState.currentEntry.filename);

					} 		

					flfd->fd_c++;

					
				}
				else
				{
					// файл 
	
					flfd->fle = (FS_FILE *)realloc(flfd->fle,(flfd->fl_c+1)*sizeof(FS_FILE));

					memset(&flfd->fle[flfd->fl_c],0,sizeof(FS_FILE));

					//if ( IsTextUnicode(mDirState.currentEntry.lfilename, 50, NULL) )
					//if ( wcslen(mDirState.currentEntry.lfilename) )
					{
						char strOut[460];
						WideCharToMultiByte(CP_ACP, 0, (WCHAR*)mDirState.currentEntry.lfilename, -1, strOut, 255, NULL, NULL);
						if ( strlen(strOut) ) // если есть unicode значение то взять его. если нте взять ANSI 
							flfd->fle[flfd->fl_c].ans_name = strdup(strOut);
						else 
							flfd->fle[flfd->fl_c].ans_name = strdup(mDirState.currentEntry.filename);

					} 				

					flfd->fle[flfd->fl_c].file_data_size =u8array_to_u32( mDirState.currentEntry.entryData, DIR_ENTRY_fileSize);


					flfd->fle[flfd->fl_c].file_last_change_date = TimetToFileTime( _FAT_filetime_to_time_t(u8array_to_u16( mDirState.currentEntry.entryData, DIR_ENTRY_mTime), u8array_to_u16( mDirState.currentEntry.entryData, DIR_ENTRY_mDate) ) );

					flfd->fl_c++;					
					memset(mDirState.currentEntry.filename, 0, 200);

					
				}


				
				_FAT_dirnext_r(&r, &mDirState, filename, &filestat);
			}
			_FAT_dirclose_r(&r, &mDirState);
			_FAT_dirclose_r(&r, mDirNextState);

			if (!sort)
				return 0;

			// todo : sorting 
			if (flfd->fl_c > 1)
				QuickSort_FS(0, flfd->fl_c-1, flfd->fle);

			if (flfd->fd_c > 1)
				QuickSort_FS(0, flfd->fd_c -1, flfd->fld);
		}
	   	
	return 0;
	}

	return 0;
  
}
//------------------------------------------------
//////////////////////////////////////////////////
// Проверяет является ли Токен директорией ///////
//////////////////////////////////////////////////
// return: 1 - folder. 0 - file. 3 - error
//------------------------------------------------
char CFsys::isDirectory(const char* pth)
{
//	WriteLog(">>>>CFsys::isDirectory(%s)",pth);

	if ( _isNTFS )
	{
		ntfs_inode *ni;
		char ret=0;		
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, pth);
		if(!ni)
			return 3;

		if (ni->mrec->flags & MFT_RECORD_IS_DIRECTORY) 
			ret = 1;
		else
			ret = 0;

		ntfs_inode_close(ni);
		return ret;
	}
	else // if isFAT
	{	
		DIR_ENTRY entry;
		TCHAR path[500]={0};
		path[0]='/';
		strcpy(path+1, pth);
		
		if(  ! _FAT_directory_entryFromPath (partition_, &entry, path, NULL)  )
			
		{
			WriteLog("_FAT_directory_entryFromPath FAILED.");			
			
			//MessageBox(0, "Rohos", LS(IDS_INT_ERR), 0);
			//return -1; 
			return 3;
		}
		// Эта функция возращяет 1 или 0							
		return _FAT_directory_isDirectory (&entry)?1:0;   //  А здемь надо 1 или 2.
	}  	
}
//----------------------------------------------------
/////////////////////////////////////////////////////
// Возвращяет размер файла  /////////////////////////
/////////////////////////////////////////////////////
//----------------------------------------------------
u64 CFsys::GetFileSize(const char *pth)
{
//	 WriteLog(">>>>>CFsys::GetFileSize(%s)",pth);

	if ( _isNTFS )
	{
		ntfs_inode *ni;
		ntfs_attr_search_ctx *ctx = NULL;
		u64 filesize=0;
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, pth);
		if(!ni)
			return 0;
		/*ctx = ntfs_attr_get_search_ctx(ni, ni->mrec);
		if(!ctx)
		return 0;
		ntfs_attr_lookup(AT_DATA, AT_UNNAMED, 0, (IGNORE_CASE_BOOL)0, 0,NULL, 0, ctx);
		filesize = ntfs_get_attribute_value_length(ctx->attr);


		ntfs_attr_put_search_ctx(ctx);*/
		filesize = ni->data_size;

		ntfs_inode_close(ni);
		return filesize;
	}
	else // if isFAT
	{	
		DIR_ENTRY entry;
		TCHAR path[500]={0};
		path[0]='/';
		strcpy(path+1, pth);	
		
		if(  ! _FAT_directory_entryFromPath (partition_, &entry, path, NULL)  )			
		{
			WriteLog("entryFromPath Failed");
			return 0;
		}

		return u8array_to_u32(entry.entryData, DIR_ENTRY_fileSize);
	
	}  	
	
}

//----------------------------------------------------
/////////////////////////////////////////////////////
// Монтирует FAT образ  /////////////////////////////
/////////////////////////////////////////////////////
//----------------------------------------------------
extern HANDLE hImageFilehandle;

int CFsys::OpenFAT(const char *path)
{
//	WriteLog(">>>>CFsys::OpenFAT(%s)",path);

	setlocale( LC_ALL, ".ACP" );


	DWORD file_attr = GetFileAttributes(path);
	if ( file_attr & FILE_ATTRIBUTE_READONLY) {
		file_attr ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(path, file_attr);
	}

	fatvolume_handle = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	 

	_readOnly = FALSE;

	if (fatvolume_handle == INVALID_HANDLE_VALUE)
	{
		WriteLog("CFsys::OpenFAT, read-write %X", GetLastError() );

		fatvolume_handle = CreateFile(path, GENERIC_READ , FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	 

		_readOnly = TRUE;
		WriteLog("CFsys::OpenFAT, open in readonly %X", GetLastError() );
		//AfxMessageBox(LS(IDS_MOUNT_AS_READONLY),MB_ICONINFORMATION);

		if (fatvolume_handle == INVALID_HANDLE_VALUE)
		{
			WriteLog("Error loading volume. %s (%X)", path, GetLastError() );
			//AfxMessageBox(get_msg(MSG_ERROR_LOADING_VOLUME), MB_ICONINFORMATION);
			return -1;
		}
	}

	
	partition_ = _FAT_partition_constructor(fatvolume_handle, 8);
	
	if (partition_ == NULL)
	{
		WriteLog("Does not appear to be a FAT partition.");
		//AfxMessageBox(get_msg(MSG_ERROR_LOADING_VOLUME), MB_ICONINFORMATION);

        fatvolume_handle = 0;
		return -1;
	}
	else if (partition_->filesysType == FS_UNKNOWN)
	{
		WriteLog("Does not appear to be a FAT partition.");
	
		_FAT_partition_destructor(partition_);
	
        fatvolume_handle = 0;
		return -1;
	}

	
		
	return 1;
}



int   CFsys::GetDiskSizeMB()
{
	if (_isNTFS)
	{
		if (ntfs_volume)
		{
			_disk_total_space.QuadPart = ntfs_volume_space(ntfs_volume);
			//_disk_total_space.QuadPart *= ntfs_volume->cluster_size;
		}

	}else
	{		
		if (partition_)
			_disk_total_space.QuadPart = partition_->totalSize;
		//WriteLog("%X", partition_->totalSize);
	}


	return _disk_total_space.QuadPart / (1024 * 1024);
}

// get disk free space 
int CFsys::GetDiskFreeSpaceMB()
{
	if (_isNTFS)
	{
		if (ntfs_volume){
			
			/*WriteLog("ntfs_volume_get_free_space %X %X", ntfs_volume->free_clusters, ntfs_volume->cluster_size);
			WriteLog("ntfs_volume_get_free_space %X", ntfs_volume->cluster_size);
			_disk_free_space.QuadPart = ntfs_volume->free_clusters;
			_disk_free_space.QuadPart *=ntfs_volume->cluster_size;*/

			_disk_free_space.QuadPart  = ntfs_volume_get_free_space2(ntfs_volume);

		}

	}else
	{		
		if (partition_)
		{
			_disk_free_space.QuadPart = partition_->fsinfo.freeClusters ;
		    _disk_free_space.QuadPart *= partition_->bytesPerCluster;
		}
		//WriteLog("%X %X", partition_->fsinfo.freeClusters , partition_->bytesPerCluster);
	}
	
	return _disk_free_space.QuadPart / (1024 * 1024);
}


/** виртуализировать всю папку + subfolders...
*/
bool CFsys::CreateVirtualFolder(const char *folderPath)
{
	TCHAR path1[500];
	//TCHAR in_path1[500] = {0};

	/*// create folder name 
	LPCTSTR fname = strrchr(folderPath, PATH_SEP);
	if (!fname)
		fname = folderPath;
	else 	
		fname++;*/
	
	FS_FF ff={0};
	BrowsePath(folderPath, &ff, false);

	//  Folders
	for(int i=0;i<ff.fd_c;i++)
	{
		_tcscpy(path1, folderPath);
		_tcscat(path1, PATH_SEP1);
		_tcscat(path1, ff.fld[i].ans_name);
		CreateVirtualFolder(path1);
	}


	//  Files
	for(int i=0;i<ff.fl_c;i++)
	{
		TCHAR virtFileName[500] = "";
		TCHAR virtPath[500] = "";

		strcpy(virtFileName, "c:\\");
		virtFileName[0] = _curr_folder[0];
		strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));

		::CreateDirectory(virtFileName, NULL);

		strcat(virtFileName, "\\");
		strcat(virtFileName, folderPath);
		strcat(virtFileName, "\\");
		strcat(virtFileName, ff.fle[i].ans_name);

	//	AfxMessageBox(virtFileName);

		_tcscpy(path1, folderPath);
		_tcscat(path1, PATH_SEP1);		
		_tcscat(path1, ff.fle[i].ans_name);		

		CreateVirtualFile(path1, virtFileName, false);
	}

	return true;
}


// initialize File Virtualization Engine.
bool CFsys::InitializeBoxedApp()
{
#ifdef _USE_BOXEDAPP_SDK

	if (!boxed_app)
	{
		
	// disable save to Recent Docs.
	_recent_docs_policy = ReadReg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoRecentDocsHistory", 0);
	//if (_recent_docs_policy)
	//	AfxMessageBox("1");
	WriteReg(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoRecentDocsHistory",1 );

		//_tcscpy(filename, "c:\\");			
		//filename[0] = _curr_folder[0];
		//SetCurrentDirectory(filename);

		BoxedAppSDK_SetContext("D6D4430A-10C1-47b6-B99F-8D176A8BAED1"); // <-- set the key here
		//
		boxed_app = BoxedAppSDK_Init();	 

		if (!boxed_app)
		{
			WriteLog( "Error initialize Virtualization Engine. %X", GetLastError() );
			return 0;
		}

		//BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_REGISTRY,	true);
		BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__EMBED_BOXEDAPP_IN_CHILD_PROCESSES, TRUE);
		//BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ALL_CHANGES_ARE_VIRTUAL, true);
		BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__INHERIT_OPTIONS, true);

		//PVOID pSHAddToRecentDocs = GetProcAddress(GetModuleHandle(_T("shell32.dll")), "SHAddToRecentDocs");     
		//hHook__SHAddToRecentDocs = BoxedAppSDK_HookFunction(pSHAddToRecentDocs, &My_SHAddToRecentDocs, TRUE);     
	
		
		
	}	

	// virtual changes
#endif

	return 1;

}

/** создать виртуальный файл и поместить его в массив _p_IFiles;
virtual_file_name - если надо можно указать полный путь и имя до виртуального файла (если он в подпапке.)
*/
bool CFsys::CreateVirtualFile(const char *s_pth, const char *virtual_file_name, bool quick_create /*= false*/)
{
#ifdef _USE_BOXEDAPP_SDK

	CIStreamFile* i_file =0;		

	LPCTSTR appname = _tcsrchr(s_pth, PATH_SEP);
	if (appname)
		appname++;
	else
		appname  = s_pth;	
	
	if (virtual_file_name)
	{
		appname = virtual_file_name;		
	}

	if ( _taccess(appname, 0) ==0 )
	{
		// file already virtualized.
		return true;
	}

	

	if (quick_create)
	{
		// qucik create - just CIStreamFile with a file name inside
		i_file = new CIStreamFile(this);

		if ( _isNTFS )
		{
			i_file->_ntfs_inode = ntfs_pathname_to_inode(ntfs_volume, NULL, s_pth);
		}

	} else 
	{

		if ( _isNTFS )
		{
			ntfs_inode *ni;
			ntfs_attr *attr;		

			ni=NULL;
			attr=NULL;

			ni = ntfs_pathname_to_inode(ntfs_volume, NULL, s_pth);
			if(!ni)
				return 0;

			attr = ntfs_attr_open (ni, AT_DATA, NULL, 0);

			if(!attr)
				return 0;

			i_file = new CIStreamFile(this);
			i_file->_ntfs_attr = attr;//
			i_file->_ntfs_inode = ni;
			i_file->m_dwSize.QuadPart = ni->data_size;

			STANDARD_INFORMATION *si;
			si = (STANDARD_INFORMATION *)ntfs_attr_readall(ni, AT_STANDARD_INFORMATION, AT_UNNAMED, 0, NULL);
			if (si) {
				i_file->ctime = si->creation_time;
				i_file->mtime = si->last_data_change_time;
				i_file->atime = si->last_access_time;
				free(si);
			}
			// create virtual file 

		}
		else // if isFAT
		{	
			//MessageBox(0, s_pth, d_pth, 0);

			FILE_STRUCT * mFile = new FILE_STRUCT;
			_reent *r =  new _reent;
			int fd = _FAT_open_r(r, mFile, s_pth, O_RDWR, 0);
			if (fd == -1)
				return 0;

			i_file = new CIStreamFile(this);
			i_file->_fat_entry = r;	
			i_file->_fat_fd = fd;

		}  
	}
		
	_tcscpy(i_file->virtualFileName, appname);

	InitializeBoxedApp();



	HANDLE hFile =  BoxedAppSDK_CreateVirtualFileBasedOnIStream( appname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0,
			NULL, i_file);

	

	
	FILETIME ftCreated, tfLastAccess, ftLastWrite;
		
	ftCreated.dwHighDateTime = (u32)(i_file->ctime >> 32);
	ftCreated.dwLowDateTime = (u32)(i_file->ctime  & 0x00000000ffffffff);
	tfLastAccess.dwHighDateTime = (u32)(i_file->atime >> 32);
	tfLastAccess.dwLowDateTime = (u32)(i_file->atime  & 0x00000000ffffffff);
	ftLastWrite.dwHighDateTime = (u32)(i_file->mtime >> 32);
	ftLastWrite.dwLowDateTime = (u32)(i_file->mtime & 0x00000000ffffffff);
	SetFileTime(hFile, &ftCreated, &tfLastAccess, &ftLastWrite);	
	
	CloseHandle(hFile);	
	arrayIFiles.Add(i_file);

#endif

	return true;

}

/**
если s_pth - EXE, то Запустить приложение или открыть файл 
если документ - открыть асоциированое приложение и запустить его с виртуальным файлом.

*/
char  CFsys::ShellExecute(const char *s_pth, bool openas, bool isolate, bool create_tree)
{
	char error[500], txt[500];
	TCHAR str_exepath[MAX_PATH+MAX_PATH];
	TCHAR filename[500] = "";
	TCHAR virtFileName[500] = "";
	TCHAR virtPath[500] = "";

	strcpy(virtFileName, "c:\\");
	virtFileName[0] = _curr_folder[0];
	strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));

	//::CreateDirectory(virtFileName, NULL);
	strcat(virtFileName, "\\");	


	if (!boxed_app)
	{
		_tcscpy(filename, "c:\\");			
		filename[0] = _curr_folder[0];
		SetCurrentDirectory(filename);
	}
	InitializeBoxedApp();	

	if (isolate)
	
		Enable_VirtualIsolation();
	 else
		Disable_VirtualIsolation();


	LPCTSTR appname = _tcsrchr(s_pth, PATH_SEP);
	if (appname)
		appname++;
	else
		appname  = s_pth;	

	strcat(virtFileName, appname);
	
	int ret =0;

	CString  s1; 

	// separate code for PortableApps
	if ( StrStrI(s_pth, "Portable") || StrStrI(s_pth, ".exe") )
	{
		/*if ( !CreateVirtualFile(s_pth, s_pth) )
		{
			MessageBox(0,  LS(IDS_ERROR_OPENING_FILE), "Rohos Disk Browser", 0);
			return 0;
		}*/

		// virtuaize "PortableApps" "Documents" folder.s
		TCHAR path[500];

		_tcscpy(path, s_pth);

		LPTSTR c = _tcsrchr(path, PATH_SEP);
		if (c)
			*c = 0;	
		else 
			path[0]=0;
		
		//_tcscat(path, "/PortableApps");

		if ( StrStrI(s_pth, "Portable") || create_tree)
			CreateVirtualFolder(path); // виртуализировать всю папку...

		strcpy(virtFileName, "c:\\");
		virtFileName[0] = _curr_folder[0];
		strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));

		strcpy(virtPath, virtFileName);

		strcat(virtFileName, "\\");
		strcat(virtFileName, s_pth);				

		WriteLog("runExe, %s", (LPCTSTR)virtFileName);

		//ret = WinExec(virtFileName, SW_SHOW);
		ret = (int)::ShellExecute(0, "open", virtFileName, "", virtPath,  SW_SHOW);
		//BoxedAppSDK_ExecuteDotNetApplication(virtFileName, "");

		return 1;

	}
	else
	{
		if (  create_tree)
		{
			TCHAR path[500];
			_tcscpy(path, s_pth);
			LPTSTR c = _tcsrchr(path, PATH_SEP);
			if (c)
				*c = 0;	
			else 
				path[0]=0;

			CreateVirtualFolder(path); // виртуализировать всю папку...

			strcpy(virtFileName, "c:\\");
			virtFileName[0] = _curr_folder[0];
			strcat(virtFileName, LS(IDS_VIRTUAL_FOLDER));			

			strcat(virtFileName, "\\");
			strcat(virtFileName, s_pth);		
		}
		else
		if ( !CreateVirtualFile(s_pth, virtFileName, false) )
		{
			MessageBox(0,  LS(IDS_ERROR_OPENING_FILE), "Rohos Disk Browser", 0);
			return 0;
		}
		
		strcpy(virtPath, "c:\\");
		virtPath[0] = _curr_folder[0];
		strcat(virtPath, LS(IDS_VIRTUAL_FOLDER));
		//strcat(virtPath, "\\");
		BOOL b  = SetCurrentDirectory(virtPath);
		if (!b)
			WriteLog("%s", (LPCTSTR)virtPath);
	}

	if ( StrStrI(appname, ".exe")  )
	{
		//_tcscpy(str_exepath, appname);

		 // запуск виртуального приложения.
		WriteLog("run1, %s", (LPCTSTR)appname);
		//ret = WinExec(appname, SW_SHOW);
		ret = (int)::ShellExecute(0, "open", virtFileName, "", virtPath,  SW_SHOW);
		

	} else
		if (openas == false)
	{
		// открыть в асоциированной программе виртуальный файл 		

		ret = 0;

		if (IsWindowsVista())
		{
			LPCTSTR ext = _tcsrchr(s_pth, '.');
			if (ext)
			{	
				TCHAR extid[350] = {0}; // 
				ReadReg(HKEY_CLASSES_ROOT, ext, "", extid, 100);

				_tcscat(extid, "\\shell\\open\\command");
				ReadReg(HKEY_CLASSES_ROOT, extid, "", str_exepath, MAX_PATH);

				if ( strlen (str_exepath) )
				{

					if ( strstr(str_exepath, "PhotoViewer.dll") )
					{

						
						// rundll32.exe "c:\Program Files\Windows Photo Viewer\PhotoViewer.dll", ImageView_Fullscreen c:\Desert.jpg
						//_tcscpy(str_exepath, "rundll32.exe \"c:\\Program Files\\Windows Photo Viewer\\PhotoViewer.dll\",ImageView_Fullscreen ");

						// c:\windows\system32\rundll32.exe C:\WINDOWS\System32\shimgvw.dll,ImageView_Fullscreen
						_tcscpy(str_exepath, "rundll32.exe shimgvw.dll,ImageView_Fullscreen ");
						
						//_tcscat(str_exepath, " \"");
						_tcscat(str_exepath, virtFileName);
						//_tcscat(str_exepath, "\"");
						
						//SetCurrentDirectory("C:\\Windows\\System32\\");
						ret = WinExec(str_exepath, SW_SHOW);
						WriteLog("run3, %s %d", (LPCTSTR)str_exepath, ret);

						//ret = 32;
					}

					//WriteLog("Error")

				}

			}
			/*FindExecutable(appname, NULL, str_exepath);

			_tcscat(str_exepath, " \"");
			_tcscat(str_exepath, virtFileName);
			_tcscat(str_exepath, "\"");

			WriteLog("run3, %s", (LPCTSTR)str_exepath);
			ret = WinExec(str_exepath, SW_SHOW);	*/
		}

		if ( ret <=31 )
		{
			//WriteLog("run4, %s", (LPCTSTR)virtFileName);
			ret = (int)::ShellExecute(0, "open", virtFileName, "", virtPath,  SW_SHOW);
		}

	} else // openas == true
		{

			PFUNCTION_ENTRYPOINT pEntryPoint=NULL;
			HINSTANCE hInst=AfxGetInstanceHandle();
			HWND hwnd = AfxGetApp()->m_pMainWnd->m_hWnd;

			HMODULE  hModule = LoadLibrary("Shell32.dll");		
			ret = 31;
			if (hModule) 
			{
				pEntryPoint = (PFUNCTION_ENTRYPOINT) GetProcAddress(hModule, "OpenAs_RunDLL");
			}

			if (pEntryPoint)
			{
				ret = 32;
				pEntryPoint(hwnd, hInst, (LPSTR)virtFileName, SW_SHOW);
			}
		}


	if ( ret <= 31  )
	{
		_tcscpy(error, LS(IDS_ERROR_OPENING_FILE) );
		_tcscat(error, "\n");
		_tcscat(error, virtFileName);
		MessageBox(0, error, "Rohos Disk Browser", 0);
		return 0;
	}

	return 1;

}
//-----------------source--------destination--------------------------
/////////////////////////////////////////////////////
// Копирует файл  "s_pth"   в    "d_pth" /////////////////////////
//
// if callback_function it calls callback_function(0, 0, pli1, pli2 )
// wparam1 PLARGE_INTEGER - total size fo the processed item
// lParam2 PLARGE_INTEGER - current posiiton of the item
//----------------------------------------------------
char CFsys::SaveFile(const char *s_pth,  const char *d_pth, u64 sz,DWORD addr , DLGPROC callback_function)
{
	//WriteLog(">>>>CFsys::SaveFile(%s, %s)",s_pth,d_pth);
	char error[500], txt[500];

	ULARGE_INTEGER  liCurrentPosition;
	liCurrentPosition.QuadPart =0;

	ULARGE_INTEGER  liFileSize;
	liFileSize.QuadPart =0;
	//liFileSize.LowPart = ::GetFileSize(m_hFile, &liFileSize.HighPart);	

	if ( _isNTFS )
	{
		ntfs_inode *ni;
		ntfs_attr *attr;
		const long b_sz=0x10000;
		DWORD w_sz;
		char *buf;
		s64 ofs,k;
		HANDLE hfile;
		
		buf = (char *)malloc(b_sz);
		
		ni=NULL;
		attr=NULL;

		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, s_pth);
		if(!ni)
			return -1;

		attr = ntfs_attr_open (ni, AT_DATA, NULL, 0);

		if(!attr)
			return -1;

		hfile = CreateFile(d_pth, GENERIC_WRITE | GENERIC_READ,	FILE_SHARE_WRITE,	NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
		char error[500], txt[500];

		if (hfile == INVALID_HANDLE_VALUE)
		{			
			ntfs_attr_close(attr);
			ntfs_inode_close(ni);
			return -1;
		}

		ofs=0;
		while( (k=ntfs_attr_pread( attr, ofs, b_sz, buf)) )
		{
			if(k<b_sz)
				w_sz = k;
			else
				w_sz = b_sz;

			liCurrentPosition.QuadPart += w_sz;

			if (callback_function)
			{
				// user may cancel this
				if ( callback_function(0, 0, (WPARAM)&liFileSize, (LPARAM)&liCurrentPosition ))
					break;
			}

			WriteFile(hfile,buf,w_sz,&w_sz,NULL);
			ofs+=k;//+1;
		}

		
		FILETIME ftCreated, tfLastAccess, ftLastWrite;
		//attr = ntfs_attr_open(ni, AT_STANDARD_INFORMATION, NULL, 0);
		STANDARD_INFORMATION *si;
		si = (STANDARD_INFORMATION *)ntfs_attr_readall(ni, AT_STANDARD_INFORMATION, AT_UNNAMED, 0, NULL);
		if (si) {
			//ni->flags = si->file_attributes;
			ftCreated.dwHighDateTime = (u32)(si->creation_time >> 32);
			ftCreated.dwLowDateTime = (u32)(si->creation_time  & 0x00000000ffffffff);
			tfLastAccess.dwHighDateTime = (u32)(si->last_access_time >> 32);
			tfLastAccess.dwLowDateTime = (u32)(si->last_access_time  & 0x00000000ffffffff);
			ftLastWrite.dwHighDateTime = (u32)(si->last_data_change_time >> 32);
			ftLastWrite.dwLowDateTime = (u32)(si->last_data_change_time & 0x00000000ffffffff);

			SetFileTime(hfile, &ftCreated, &tfLastAccess, &ftLastWrite);
			SetFileAttributes(d_pth, si->file_attributes);
			free(si);
		}

		
		CloseHandle(hfile);

		ntfs_attr_close(attr);
		ntfs_inode_close(ni);
		free(buf);
		//ntfs_close_read(ni,attr);
		return 0;
	}
	else // if isFAT
	{	

		//MessageBox(0, s_pth, d_pth, 0);
		
		HANDLE m_hFile = CreateFileA(d_pth, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);

		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			WriteLog("Error creating file.");
			GetSystemErrorMessage( GetLastError(), error);

			strcpy(txt, LS(IDS_ERROR_OPENING_FILE) );
			strcat(txt, "\n" );	
			strcat(txt, error );

			AfxMessageBox(txt, MB_ICONINFORMATION);			
			return -1;
		}
		else
		{
			FILE_STRUCT mFile;
			struct _reent r;
			int fd = _FAT_open_r(&r, &mFile, s_pth, O_RDONLY, 0);
			if (fd == -1)
			{
				WriteLog("Unable to open file!");
				return -1;
			}
			else
			{
				int retVal = 0;
				char *fileBuf = (char*)malloc(1024*1024+1);
				DWORD dwBytesWritten = 0;
				while ((retVal = _FAT_read_r(&r, fd, fileBuf, 1024*1024)) > 0)
				{
					liCurrentPosition.QuadPart += retVal;

					if (callback_function)
					{
						// user may cancel this
						if ( callback_function(0, 0, (WPARAM)&liFileSize, (LPARAM)&liCurrentPosition ) )
							break;
					}

					WriteFile(m_hFile, fileBuf, retVal, &dwBytesWritten, FALSE);
				}
				
				retVal = _FAT_close_r(&r, fd);
				//SecureZeroMemory(fileBuf, sizeof(1024*1024));
				free(fileBuf);
			}
			CloseHandle(m_hFile);
			
			return 0;
		}
		return -1;		
	}  	
}

// convert filetime
time_t FILETIME_to_time_t(const FILETIME *ft)
{

	// takes the last modified date
	LARGE_INTEGER date, adjust;
	date.HighPart = ft->dwHighDateTime;
	date.LowPart = ft->dwLowDateTime;

	// 100-nanoseconds = milliseconds * 10000
	adjust.QuadPart = 11644473600000 * 10000;

	// removes the diff between 1970 and 1601
	date.QuadPart -= adjust.QuadPart;

	// converts back from 100-nanoseconds to seconds
	return (time_t)(date.QuadPart / 10000000);

}


//-----------------source--------destination--------------------------
/////////////////////////////////////////////////////
// Load file into CFsys partition /////////////////////////
/////////////////////////////////////////////////////
//
// if callback_function it calls callback_function(0, 0, pli1, pli2 )
// wparam1 PLARGE_INTEGER - total size fo the processed item
// lParam2 PLARGE_INTEGER - current posiiton of the item
// retun == -1  error
//----------------------------------------------------
char CFsys::LoadFile(const char *source, const char *destination , DLGPROC callback_function)
{
	if ( _readOnly )
	{
		AfxMessageBox(LS(IDS_MOUNT_AS_READONLY), MB_ICONINFORMATION);
		return -1;
	}

	WriteLog(">>>>CFsys::LoadFile(%s, %s)",source, destination);

	

	// open destination file
	HANDLE m_hFile = CreateFileA(source, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog("Error opening file!");
		AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);
		return -1;
	}

	ULARGE_INTEGER  liFileSize;
	liFileSize.LowPart = ::GetFileSize(m_hFile, &liFileSize.HighPart);	

	GetDiskFreeSpaceMB();
	if ( _disk_free_space.QuadPart < liFileSize.QuadPart)
	{
		WriteLog("Not enought disk free space %X", _disk_free_space.QuadPart / 1024);
		return -1;
	}

	ULARGE_INTEGER  liCurrentPosition;
	liCurrentPosition.QuadPart =0;

	int res = 0;
	
	if ( _isNTFS )	
	{
		
		char *name;
		WCHAR uname[MAX_PATH+1];
		ntfs_inode *dir_ni = NULL, *ni;
		char *path;
		int uname_len;
		bool existing_file = true;
		ntfs_attr *na = NULL;		

		path = strdup(destination);

		/* Open file for writing. */	
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
		if (!ni) {

			/* Generate unicode filename. */
			name = strrchr(path, '/');
			if (!name)
				return -1;

			name++;

			uname_len =MultiByteToWideChar( CP_ACP, 0, name, -1, uname, MAX_PATH );
			uname_len -= 1; // len should not include NULL terminator!

			/* Open parent directory. */
			*name = 0;
			dir_ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
			if (!dir_ni) {				
				goto exit;
			}

			ni = ntfs_create(dir_ni, (ntfschar*)uname, uname_len, S_IFREG & S_IFMT); // create regualr file

			if (!ni)
			{
				ntfs_inode_close(dir_ni);
				goto exit;
			}		

			FILETIME ftCreated, tfLastAccess, ftLastWrite;
			GetFileTime(m_hFile, &ftCreated, &tfLastAccess, &ftLastWrite);
			ni->creation_time = FILETIME_to_time_t(&ftCreated);//
			ni->last_access_time = FILETIME_to_time_t(&tfLastAccess); //
			ni->last_data_change_time = FILETIME_to_time_t(&ftLastWrite); //
			ni->flags = (FILE_ATTR_FLAGS)GetFileAttributes(source);
			ntfs_inode_update_times(ni, (ntfs_time_update_flags)0);


			ntfs_inode_close(ni);
			ntfs_inode_update_times(dir_ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);
			ntfs_inode_close(dir_ni);		

			free(path);

			path = strdup(destination);

			/* Open file for writing. */	
			ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
			if (!ni) {
				goto exit;
			}

			

			existing_file = false;
			
		}


		
		__int64 offset = 0;
		__int64 total =0;

		na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);

		if (!na){
			ntfs_inode_close(ni);
			goto exit;
		}
		
		char *fileBuf = (char*)malloc(1024*1024+1);
		DWORD dwBytesRead = 0;
		
		do	{
			if (ReadFile(m_hFile, fileBuf, 1024*1024, &dwBytesRead, FALSE) == 0)
			{
				WriteLog("ReadFile error");
				res = -1;
				goto exit;
			}			
			int dwBytesToWrite = dwBytesRead;

			liCurrentPosition.QuadPart += dwBytesRead;

			if (callback_function)
			{
				// exit if user cancel 
				if ( callback_function(0, 0, (WPARAM)&liFileSize, (LPARAM)&liCurrentPosition ) )
					break;
			}

			while (dwBytesToWrite) {
				s64 ret = ntfs_attr_pwrite(na, offset, dwBytesToWrite, fileBuf);
				if (0 <= ret && ret < (s64)dwBytesToWrite)
					WriteLog("ntfs_attr_pwrite partial write to '%s'"
					" (%lld: %lld <> %lld)", path, (long long)offset,
					(long long)dwBytesToWrite, (long long)ret);
				if (ret <= 0) {
					res = -1;
					goto exit;
				}
				dwBytesToWrite   -= ret;
				offset += ret;
				total  += ret;
			}
		} while (dwBytesRead); 

		free(fileBuf);

		if (existing_file)
			ntfs_attr_truncate(na, total);

		res = total;

		if (res > 0 && existing_file == true)
			ntfs_inode_update_times(na->ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);

		if (na)
			ntfs_attr_close(na);
		
		ntfs_inode_close(ni);						
		 		

exit:
		if (path)
			free(path);
		CloseHandle(m_hFile);
		GetDiskFreeSpaceMB();
		return res;
		
	}
	else // if isFAT
	{			
		//  Если в начале стоит этот символ
		if(destination[0]=='/')
		{
			// убираю его
			destination++;
		}

		//MessageBox(0, destination, "d", 0);

		FILE_STRUCT mFile;
		struct _reent r;
		int fd = _FAT_open_r(&r, &mFile, destination, O_CREAT | O_TRUNC | O_RDWR, 0);
		if (fd == -1)
		{
			WriteLog("_FAT_open_r returned: (-1)");
			//AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);
			CloseHandle(m_hFile);
			return -1;
		}
		else
		{
			int retVal = 1;
			
			char *fileBuf = (char*)malloc(1024*1024+1);
			DWORD dwBytesWritten = 0;

			while (retVal > 0)
			{
				if (ReadFile(m_hFile, fileBuf, 1024*1024, &dwBytesWritten, FALSE) == 0)
				{
					WriteLog("Error reading file!");
					res = -1;
					break;
				}
				liCurrentPosition.QuadPart += dwBytesWritten;

				if (callback_function)
				{	// user may cancel this
					if ( callback_function(0, 0, (WPARAM)&liFileSize, (LPARAM)&liCurrentPosition ) )
						break;
				}

				retVal = _FAT_write_r (&r, fd, fileBuf, dwBytesWritten);				
			}

			retVal = _FAT_close_r(&r, fd);
			SecureZeroMemory(fileBuf, sizeof(1024*1024));
			free(fileBuf);
		}
		CloseHandle(m_hFile);
		GetDiskFreeSpaceMB();

		return res;

	}  	
}

static int ntfs_fuse_rename(const char *old_path, const char *new_path);

ntfs_volume *ntfs_vol; 

/** Rename file or folder
*/
bool CFsys::RenameItem(char *s_pth, char* new_path)
{
	TCHAR path1[MAX_PATH] = {0};
	TCHAR new_path1[MAX_PATH] = {0};

	if ( _isNTFS )	
	{
		ntfs_vol = ntfs_volume;
		_tcscpy(path1, PATH_SEP1);
		_tcscat(path1, s_pth);

		_tcscpy(new_path1, PATH_SEP1);
		_tcscat(new_path1, new_path);

		int ret = ntfs_fuse_rename(path1, new_path1);

		if (!ret)
			return true;

	} else
	{

	}

	return false;

}


// read raw data from a Disk by offset
//
int	CFsys::ReadDiskData( LPBYTE buff, DWORD size, LARGE_INTEGER offset)
{
	DWORD dwRead =0;
	HANDLE h=0;

	if (_pDiskPartitionClass == NULL && _isNTFS && ntfs_volume)
	{
		h = (HANDLE)ntfs_volume->dev->d_handle;
	}
	else if (_pDiskPartitionClass == NULL && _isNTFS==false)
	{
		h = fatvolume_handle;
	}

	BOOL open_file_here = false;

	if (h==0)
	{
		// открыть файл и дешифровать первые 512 байт
		h = CreateFile(rohos_file_path,    // file to open
			GENERIC_READ,          // open for reading and writing
			FILE_SHARE_READ | FILE_SHARE_WRITE,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template

		// 
		if (h == INVALID_HANDLE_VALUE) 
		{ 
			return 0;
		}
		open_file_here = true;
	}

	crypt_SetFilePointer(h, offset.LowPart, &offset.HighPart, FILE_BEGIN);
	crypt_ReadFile(h, buff, size, &dwRead, NULL);

	if (open_file_here)
		CloseHandle(h);

	return dwRead;
}

// If Yes - all changes made by Rohos and Apps (new files, registry) are remain in memory 
//  this prevent data leak outside Rohos Disk Browser.
//
void CFsys::Enable_VirtualIsolation()
{	
#ifdef _USE_BOXEDAPP_SDK
	if (!boxed_app)
	{
		return;
	}

	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_REGISTRY,	true);
	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ALL_CHANGES_ARE_VIRTUAL, true);
	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__INHERIT_OPTIONS, true);

#endif 

}

// If No - all changes made by Rohos and Apps (new files, registry) are real 
//
void CFsys::Disable_VirtualIsolation()
{
#ifdef _USE_BOXEDAPP_SDK
	if (!boxed_app)
	{
		return;
	}

	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_REGISTRY,	false);
	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ALL_CHANGES_ARE_VIRTUAL, false);	
	BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__INHERIT_OPTIONS, true);
#endif 	

}


/////////////////////////////////////////////////////
// Читает первые 10кб файл для PREVIEW /////////////////////////
/////////////////////////////////////////////////////
char *CFsys::ReadWholeFile(char *s_pth, DWORD *sz)
{ 
	static char str[16000] = {0};
	strcpy(str, "");

	if ( _isNTFS )
	{
		ntfs_inode *ni;
		ntfs_attr *attr;
		long b_sz;//=0x10000;
		

		b_sz = GetFileSize(s_pth);

		if (b_sz> 15000)
			b_sz = 15000;		
		
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, s_pth);
		if(!ni)
		{
			//AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);			
			return 0;
		}

		attr = ntfs_attr_open (ni, AT_DATA, NULL, 0);

		if(!attr)
		{
			//AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);			
			ntfs_inode_close(ni);
			return 0;
		}

		s64 ret = ntfs_attr_pread( attr, 0, b_sz, str);			

		ntfs_attr_close(attr);
		ntfs_inode_close(ni);	
		
		if (sz) *sz = ret;
		return str;
		
	}
	else // if isFAT
	{	
		
		FILE_STRUCT mFile;
		struct _reent r;		
		
		
		
		int fd = _FAT_open_r(&r, &mFile, s_pth, O_RDONLY, 0);
		if (fd == -1)
		{
			//AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);			
			return str;
		}
		else
		{
			
			int retVal = 0;			
			DWORD dwBytesWritten = 0;
			retVal = _FAT_read_r(&r, fd, str, 15000);
			str[retVal]=0;
			
			//WriteLog("The File Contains:(%s)",str);

			if (sz) *sz = retVal;
			
			retVal = _FAT_close_r(&r, fd);
						
			return str;
		}	
		
		
	}  	
	
	return NULL;
}


//-----------------source--------destination--------------------------
/////////////////////////////////////////////////////
// Копирует файл в образ /////////////////////////
/////////////////////////////////////////////////////
//
// if callback_function it calls callback_function(0, 0, pli1, pli2 )
// wparam1 PLARGE_INTEGER - total size fo the processed item
// lParam2 PLARGE_INTEGER - current posiiton of the item
//
//----------------------------------------------------
char CFsys::WriteWholeFile(LPBYTE pbData, DWORD data_size, const char *destination , DLGPROC callback_function)
{
	if ( _readOnly )
	{
		//AfxMessageBox(LS(IDS_MOUNT_AS_READONLY), MB_ICONINFORMATION);
		return -1;
	}

	WriteLog(">>>>CFsys::WriteFile(%s)", destination, data_size);


	ULARGE_INTEGER  liDataSize = {0,0} ;
	liDataSize.LowPart = data_size;	

	GetDiskFreeSpaceMB();
	if ( _disk_free_space.QuadPart < liDataSize.QuadPart)
	{
		WriteLog("Not enought disk free space %X", _disk_free_space.QuadPart / 1024);
		return -1;
	}

	ULARGE_INTEGER  liCurrentPosition;
	liCurrentPosition.QuadPart =0;

	
	if ( _isNTFS )	
	{
		
		char *name;
		WCHAR uname[MAX_PATH+1];
		ntfs_inode *dir_ni = NULL, *ni;
		char *path;
		int res = 0, uname_len;
		bool existing_file = true;

		path = strdup(destination);

		/* Open file for writing. */	
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
		if (!ni) {

			/* Generate unicode filename. */
			name = strrchr(path, '/');
			if (!name)
				return -1;

			name++;

			uname_len =MultiByteToWideChar( CP_ACP, 0, name, -1, uname, MAX_PATH );
			uname_len -= 1; // len should not include NULL terminator!

			/* Open parent directory. */
			*name = 0;
			dir_ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
			if (!dir_ni) {				
				goto exit;
			}

			ni = ntfs_create(dir_ni, (ntfschar*)uname, uname_len, S_IFREG & S_IFMT); // create regualr file

			if (!ni)
			{
				ntfs_inode_close(dir_ni);
				goto exit;
			}

			ntfs_inode_close(ni);
			ntfs_inode_update_times(dir_ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);
			ntfs_inode_close(dir_ni);		

			free(path);

			path = strdup(destination);

			/* Open file for writing. */	
			ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
			if (!ni) {
				goto exit;
			}
			existing_file = false;
			
		}


		ntfs_attr *na = NULL;		
		__int64 offset = 0;
		__int64 total =0;

		na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);

		if (!na){
			ntfs_inode_close(ni);
			goto exit;
		}
		
		char *fileBuf = (char*)pbData;
		
		DWORD dwBytesRead =0;
		
		do	{
			
			int dwBytesToWrite = data_size;
			liCurrentPosition.QuadPart += dwBytesToWrite;
			if (callback_function)
			{
				// exit if user cancel 
				if ( callback_function(0, 0, (WPARAM)&liDataSize, (LPARAM)&liCurrentPosition ) )
					break;
			}

			while (dwBytesToWrite) {
				s64 ret = ntfs_attr_pwrite(na, offset, dwBytesToWrite, fileBuf);
				if (0 <= ret && ret < (s64)dwBytesToWrite)
					WriteLog("ntfs_attr_pwrite partial write to '%s'"
					" (%lld: %lld <> %lld)", path, (long long)offset,
					(long long)dwBytesToWrite, (long long)ret);
				if (ret <= 0) {
					res = -errno;
					goto exit;
				}
				dwBytesToWrite   -= ret;
				offset += ret;
				total  += ret;
			}
		} while (dwBytesRead); 

		free(fileBuf);

		if (existing_file)
			ntfs_attr_truncate(na, total);

		res = total;

		if (res > 0)
			ntfs_inode_update_times(na->ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);

		if (na)
			ntfs_attr_close(na);
		
		ntfs_inode_close(ni);						
		 		

exit:
		if (path)
			free(path);
		
		
		return res;
		
	}
	else // if isFAT
	{			
		//  Если в начале стоит этот символ
		if(destination[0]=='/')
		{
			// убираю его
			destination++;
		}

		//MessageBox(0, destination, "d", 0);

		FILE_STRUCT mFile;
		struct _reent r;
		int fd = _FAT_open_r(&r, &mFile, destination, O_CREAT | O_TRUNC | O_RDWR, 0);
		if (fd == -1)
		{
			WriteLog("_FAT_open_r returned: (-1)");
			return -1;
		}
		else
		{
			int retVal = 1;
			
			char *fileBuf = (char*)pbData;
			DWORD dwBytesRead = data_size;

			while (retVal > 0)
			{
				liCurrentPosition.QuadPart += dwBytesRead;

				if (callback_function)
				{	// user may cancel this
					if ( callback_function(0, 0, (WPARAM)&liDataSize, (LPARAM)&liCurrentPosition ) )
						break;
				}

				retVal = _FAT_write_r (&r, fd, fileBuf, dwBytesRead);
				dwBytesRead =0;
			}

			retVal = _FAT_close_r(&r, fd);
			SecureZeroMemory(fileBuf, sizeof(1024*1024));
			free(fileBuf);
		}

		return 0;

	}  	
}
//------------------------------------------------------------------------------------
int CFsys::CreateDirectory(const char *path, const char *folder_name)
{
	char fullpath[500] = {0};

	//MessageBox(0, folder_name, path, 0);

	if ( _isNTFS )
	{		

		char *name;
		WCHAR uname[MAX_PATH+1];
		ntfs_inode *dir_ni = NULL, *ni;
		
		int res = 0, uname_len;
		
		//if ( strlen(path) ) 
		{
			strcpy(fullpath, path);
			strcat(fullpath, "/");
		}
		strcat(fullpath, folder_name);

		

		/* Open file for writing. */	
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, fullpath);
		if (!ni) {

			/* Generate unicode filename. */
			//name = strrchr(path, '/');
			//name++;

			uname_len =MultiByteToWideChar( CP_ACP, 0, folder_name, -1, uname, MAX_PATH );
			uname_len -= 1; // len should not include NULL terminator!

			/* Open parent directory. */
			//*name = 0;
			dir_ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
			if (!dir_ni) {
				res = -errno;
				return 0;
			}
			ni = ntfs_create(dir_ni, (ntfschar*)uname, uname_len, S_IFDIR); // create regualr file			

			if (!ni) 
				{
					ntfs_inode_close(dir_ni);						
					return 0;
				}
			else {
				ntfs_inode_close(ni);						
			} 

			if (dir_ni)
			{
				ntfs_inode_update_times(dir_ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);
				ntfs_inode_close(dir_ni);						
			}
		}
		
		return  1;

	}
	else // if isFAT
	{	
		struct _reent r;

		//  Если в начале стоит этот символ		
		if(path[0]=='/')
		{
			// убираю его
			path++;
		}

		if ( strlen(path) ) {
			strcpy(fullpath, path);
			strcat(fullpath, "/");
		}
		strcat(fullpath, folder_name);

		/**/
		
		if (_FAT_mkdir_r(&r, fullpath, 0) == -1)
		{
			//AfxMessageBox("Can't create folder.", MB_ICONINFORMATION);
			WriteLog("error FAT create folder %s", folder_name );
			return 0;
		}
		return 1;
	}  
	return 0;	
}
//------------------------------------------------------------------------------------
// delete file or empty path
// 
int CFsys::Delete(const char *filepath)
{ 
	if ( _isNTFS )
	{		
		WCHAR uname[MAX_PATH+1];
		ntfs_inode *dir_ni = NULL, *ni;
		char *path, *name;
		int res = 0, uname_len;

		path = strdup(filepath);

		/* Open file for writing. */	
		ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);

		/* Generate unicode filename. */
		name = strrchr(path, '/');
		name++;

		uname_len =MultiByteToWideChar( CP_ACP, 0, name, -1, uname, MAX_PATH );
		uname_len -= 1; // len should not include NULL terminator!

		/* Open parent directory. */
		*name = 0;
		dir_ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
		if (!dir_ni) {
			res = -errno;			
		}

		if (ntfs_delete(ni, dir_ni, (ntfschar*)uname, uname_len))
			res = -errno;
		/* ntfs_delete() always closes ni and dir_ni */
		ni = dir_ni = NULL;		
		free(path);

		return  -1;
	}
	else // if isFAT
	{	

		struct _reent r;
		if (_FAT_unlink_r(&r, filepath) == -1)
		{
			AfxMessageBox("Unable to delete file");
			WriteLog("Unable to delete file");

			return 0;
		}
	}

  return 1;
}












// ******************************************
//теаы AT_UNNAMED[]={0};


//ntfs_volume *MNT_VOL=NULL;

BOOL global_showHiddenSystemFiles = 0;

/*
//-----------------------------------------------------------------------------------------
char brows_path(ntfs_volume* ntfs_volume,char *path,FS_FF *flfd)
{
	ntfs_inode *ni;
	ntfsls_dirent dirent;
	s64 pos;
	
	ni = ntfs_pathname_to_inode(ntfs_volume, NULL, path);
	
	if(!ni)
		return 0;
	pos = 0;
	memset(&dirent, 0, sizeof(dirent));
	dirent.vol = ntfs_volume;
	dirent.ff = flfd;
	dirent.showHiddenSystemFiles = true;

	ntfs_readdir(ni, &pos, &dirent, (ntfs_filldir_t)ntfs_enumerator);
	ntfs_inode_close(ni);
	return 1;
}
//-----------------------------------------------------------------------------------------
*/
int is_file_hidden(ntfsls_dirent *dr,const MFT_REF mref )
{
	ntfs_inode *ni;
	ntfs_attr_search_ctx *ctx = NULL;
	FILE_NAME_ATTR *file_name_attr;
	ATTR_RECORD *attr;
	
	int ret = 0;	
	
	
	ni = ntfs_inode_open(dr->vol, mref);
	if (!ni)
		goto release;
	
	ctx = ntfs_attr_get_search_ctx(ni, ni->mrec);
	if (!ctx)
		goto release;
	
	if (ntfs_attr_lookup(AT_FILE_NAME, AT_UNNAMED, 0, (IGNORE_CASE_BOOL)0, 0, NULL,
		0, ctx))  //AT_ATTRIBUTE_LIST
		goto release;
	attr = ctx->attr;
	
	file_name_attr = (FILE_NAME_ATTR *)((char *)attr +
		/*le16_to_cpu*/(attr->value_offset));
		/*if( (file_name_attr->file_attributes & FILE_ATTR_HIDDEN) == FILE_ATTR_HIDDEN ||
			(file_name_attr->file_attributes & FILE_ATTR_SYSTEM) == FILE_ATTR_SYSTEM
		)
		ret =1;*/

		if((file_name_attr->file_attributes & 0x02)==0x02)
			ret =1;

release:
	if (ctx)
		ntfs_attr_put_search_ctx(ctx);
	if (ni)
		ntfs_inode_close(ni);
	return ret;
}

//-----------------------------------------------------------------------------------------
//список файлов
void free_brows_structs(FS_FF *ff)
{
	DWORD i;
	//for(i=0i<ff->
	free(ff->fld);
	free(ff->fle);

}

static u32 ntfs_sequence;

static void set_fuse_error(int *err)
{
	
}

/**
 * ntfs_fuse_is_named_data_stream - check path to be to named data stream
 * @path:	path to check
 *
 * Returns 1 if path is to named data stream or 0 otherwise.
 */
static int ntfs_fuse_is_named_data_stream(const char *path)
{
	//if (strchr(path, ':') && ctx->streams == NF_STREAMS_INTERFACE_WINDOWS)
	//	return 1;
	return 0;
}

static void ntfs_fuse_update_times(ntfs_inode *ni, ntfs_time_update_flags mask)
{
	//if (ctx->atime == ATIME_DISABLED)
	//	mask &= ~NTFS_UPDATE_ATIME;
	//else 
		
		/*if (ctx->atime == ATIME_RELATIVE && mask == NTFS_UPDATE_ATIME &&
			ni->last_access_time >= ni->last_data_change_time &&
			ni->last_access_time >= ni->last_mft_change_time)
		return;*/
	ntfs_inode_update_times(ni, mask);
}




/**
 * ntfs_fuse_parse_path - split path to path and stream name.
 * @org_path:		path to split
 * @path:		pointer to buffer in which parsed path saved
 * @stream_name:	pointer to buffer where stream name in unicode saved
 *
 * This function allocates buffers for @*path and @*stream, user must free them
 * after use.
 *
 * Return values:
 *	<0	Error occurred, return -errno;
 *	 0	No stream name, @*stream is not allocated and set to AT_UNNAMED.
 *	>0	Stream name length in unicode characters.
 */
static int ntfs_fuse_parse_path(const char *org_path, char **path,
		ntfschar **stream_name)
{
	char *stream_name_mbs;
	int res;

	stream_name_mbs = strdup(org_path);
	if (!stream_name_mbs)
		return -errno;
	/*if (ctx->streams == NF_STREAMS_INTERFACE_WINDOWS) {
		*path = strsep(&stream_name_mbs, ":");
		if (stream_name_mbs) {
			*stream_name = NULL;
			res = ntfs_mbstoucs(stream_name_mbs, stream_name);
			if (res < 0)
				return -errno;
			return res;
		}
	} else*/

	*path = stream_name_mbs;
	*stream_name = AT_UNNAMED;
	return 0;
}

static int ntfs_fuse_rm(const char *org_path)
{
	char *name;
	ntfschar *uname = NULL;
	ntfs_inode *dir_ni = NULL, *ni;
	char *path;
	int res = 0, uname_len;

	path = strdup(org_path);
	if (!path)
		return -errno;
	/* Open object for delete. */
	ni = ntfs_pathname_to_inode(ntfs_vol, NULL, path);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	/* Generate unicode filename. */
	name = strrchr(path, '/');
	name++;
	uname_len = ntfs_mbstoucs(name, &uname);
	if (uname_len < 0) {
		res = -errno;
		goto exit;
	}
	/* Open parent directory. */
	*name = 0;
	dir_ni = ntfs_pathname_to_inode(ntfs_vol, NULL, path);
	if (!dir_ni) {
		res = -errno;
		goto exit;
	}
	
	if (ntfs_delete(ni, dir_ni, uname, uname_len))
		res = -errno;
	/* ntfs_delete() always closes ni and dir_ni */
	ni = dir_ni = NULL;
exit:
	if (ntfs_inode_close(dir_ni))
		set_fuse_error(&res);
	if (ntfs_inode_close(ni))
		set_fuse_error(&res);
	free(uname);
	free(path);
	return res;
}

static int ntfs_fuse_rm_stream(const char *path, ntfschar *stream_name,
		const int stream_name_len)
{
	ntfs_inode *ni;
	int res = 0;

	ni = ntfs_pathname_to_inode(ntfs_vol, NULL, path);
	if (!ni)
		return -errno;
	
	if (ntfs_attr_remove(ni, AT_DATA, stream_name, stream_name_len))
		res = -errno;

	if (ntfs_inode_close(ni))
		set_fuse_error(&res);
	return res;
}


static int ntfs_fuse_unlink(const char *org_path)
{
	char *path = NULL;
	ntfschar *stream_name;
	int stream_name_len;
	int res = 0;

	stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;
	if (!stream_name_len)
		res = ntfs_fuse_rm(path);
	else
		res = ntfs_fuse_rm_stream(path, stream_name, stream_name_len);
	free(path);
	if (stream_name_len)
		free(stream_name);
	return res;
}

static int ntfs_fuse_link(const char *old_path, const char *new_path)
{
	char *name;
	ntfschar *uname = NULL;
	ntfs_inode *dir_ni = NULL, *ni;
	char *path;
	int res = 0, uname_len;

	if (ntfs_fuse_is_named_data_stream(old_path))
		return -EINVAL; /* n/a for named data streams. */
	if (ntfs_fuse_is_named_data_stream(new_path))
		return -EINVAL; /* n/a for named data streams. */
	path = strdup(new_path);
	if (!path)
		return -errno;
	/* Open file for which create hard link. */
	ni = ntfs_pathname_to_inode(ntfs_vol, NULL, old_path);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	
	/* Generate unicode filename. */
	name = strrchr(path, '/');
	name++;
	uname_len = ntfs_mbstoucs(name, &uname);
	if (uname_len < 0) {
		res = -errno;
		goto exit;
	}
	/* Open parent directory. */
	*name = 0;
	dir_ni = ntfs_pathname_to_inode(ntfs_vol, NULL, path);
	if (!dir_ni) {
		res = -errno;
		goto exit;
	}

	if (ntfs_link(ni, dir_ni, uname, uname_len)) {
		res = -errno;
		goto exit;
	}

	ntfs_fuse_update_times(ni, NTFS_UPDATE_CTIME);
	ntfs_fuse_update_times(dir_ni, (ntfs_time_update_flags)NTFS_UPDATE_MCTIME);
exit:
	/* 
	 * Must close dir_ni first otherwise ntfs_inode_sync_file_name(ni)
	 * may fail because ni may not be in parent's index on the disk yet.
	 */
	if (ntfs_inode_close(dir_ni))
		set_fuse_error(&res);
	if (ntfs_inode_close(ni))
		set_fuse_error(&res);
	free(uname);
	free(path);
	return res;
}



static int ntfs_fuse_safe_rename(const char *old_path, 
				 const char *new_path, 
				 const char *tmp)
{
	int ret;

	ntfs_log_trace("Entering\n");
	
	ret = ntfs_fuse_link(new_path, tmp);
	if (ret)
		return ret;
	
	ret = ntfs_fuse_unlink(new_path);
	if (!ret) {
		
		ret = ntfs_fuse_link(old_path, new_path);
		if (ret)
			goto restore;
		
		ret = ntfs_fuse_unlink(old_path);
		if (ret) {
			if (ntfs_fuse_unlink(new_path))
				goto err;
			goto restore;
		}
	}
	
	goto cleanup;
restore:
	if (ntfs_fuse_link(tmp, new_path)) {
err:
		ntfs_log_perror("Rename failed. Existing file '%s' was renamed "
				"to '%s'", new_path, tmp);
	} else {
cleanup:
		ntfs_fuse_unlink(tmp);
	}
	return 	ret;
}

static int ntfs_fuse_rename_existing_dest(const char *old_path, const char *new_path)
{
	int ret, len;
	char tmp[MAX_PATH]={0};
	const char *ext = ".ntfs-3g-";

	ntfs_log_trace("Entering\n");	
	
	
	ret = sprintf(tmp, "%s%s%010d", new_path, ext, ++ntfs_sequence);
	if (ret != len - 1) {
		ntfs_log_error("snprintf failed: %d != %d\n", ret, len - 1);
		ret = -EOVERFLOW;
	} else
		ret = ntfs_fuse_safe_rename(old_path, new_path, tmp);
	
	//free(tmp);
	return 	ret;
}

static int ntfs_fuse_rename(const char *old_path, const char *new_path)
{
	int ret, stream_name_len;
	char *path = NULL;
	ntfschar *stream_name;
	ntfs_inode *ni;
	
	ntfs_log_debug("rename: old: '%s'  new: '%s'\n", old_path, new_path);
	
	/*
	 *  FIXME: Rename should be atomic.
	 */
	stream_name_len = ntfs_fuse_parse_path(new_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;
	
	ni = ntfs_pathname_to_inode(ntfs_vol, NULL, path);
	if (ni) {
		ret = ntfs_check_empty_dir(ni);
		if (ret < 0) {
			ret = -errno;
			ntfs_inode_close(ni);
			goto out;
		}
		
		if (ntfs_inode_close(ni)) {
			set_fuse_error(&ret);
			goto out;
		}
		
		ret = ntfs_fuse_rename_existing_dest(old_path, new_path);
		goto out;
	}

	ret = ntfs_fuse_link(old_path, new_path);
	if (ret)
		goto out;
	
	ret = ntfs_fuse_unlink(old_path);
	if (ret)
		ntfs_fuse_unlink(new_path);
out:
	free(path);

	if (stream_name_len)
		free(stream_name);
	return ret;
}
