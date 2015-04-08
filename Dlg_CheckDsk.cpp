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


#include "stdafx.h"
#include "DiskBrowserApp.h"
#include "Dlg_CheckDsk.h"

#ifdef _USE_BOXEDAPP_SDK
#include ".\\BoxedAppSDK\\include\\BoxedAppSDK.h"
#endif 

#include "CDiskRepartition.h"
//#include "ntddk.h"

// CDlg_CheckDsk dialog

HWND g_wnd;
//LARGE_INTEGER disk_size;

HANDLE img_hImageFilehandle; //
LARGE_INTEGER img_disksize; // image file size; do not write over the bounds
BOOL img_readonly;

// globals are setted from OnInitDialog() or FormatImage();
CDiskRepartition *theDiskPartition2;
BOOL is_partition_container2; // TRUE - read from %physicaldrive1%  device
BOOL is_encrypted_partition2; // TRUE this is encrypted partition.
LARGE_INTEGER diskDataOffset2;		// set by CFsys::OpenVolume();

//extern LARGE_INTEGER partOffset/*SetFilePointer position*/, partOffset2/*second partition starting offset 30208*/;
//extern BOOL is_encrypted_partition; // TRUE this is encrypted partition.


IMPLEMENT_DYNAMIC(CDlg_CheckDsk, CDialog)

CDlg_CheckDsk::CDlg_CheckDsk(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CheckDsk::IDD, pParent)
{

	m_FixErrors =0;
	_image_readonly =0;
	_re_formated = false;
	is_partition_container2 = false;
	
}

CDlg_CheckDsk::~CDlg_CheckDsk()
{
}

void CDlg_CheckDsk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_DiskInfo)
	DDX_Check(pDX, IDC_CHECK1, m_FixErrors);
	DDX_Check(pDX, IDC_CHECK2, m_SectorScan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_CheckDsk, CDialog)
	ON_BN_CLICKED(IDOK, &CDlg_CheckDsk::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_CHECK1, &CDlg_CheckDsk::OnBnClickedCheck1)
ON_BN_CLICKED(IDOK2, &CDlg_CheckDsk::OnBnClickedOk2)
END_MESSAGE_MAP()


// CDlg_CheckDsk message handlers

BOOL CDlg_CheckDsk::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_wnd = m_hWnd;
	img_disksize = _disk_size;
	img_hImageFilehandle = _hImageFilehandle;
	img_readonly =_image_readonly;

	diskDataOffset2.QuadPart = disk_data_offset_in_mb;		
	diskDataOffset2.QuadPart *= (1024 * 1024);

	//if (_image_readonly)
	//	enableItems(m_hWnd, 0, IDC_CHECK1, 0);

	theDiskPartition2 = (CDiskRepartition*)pDiskPartitionClass;

	if (pDiskPartitionClass)
		is_partition_container2 = true;
	
	is_encrypted_partition2 = encrypted;

	SetDlgItemText(IDC_COMBO1, "NTFS");

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}






//*****************************************************
typedef LONG NTSTATUS;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING *ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _FILE_INFORMATION_CLASS {

    FileDirectoryInformation         = 1,
    FileFullDirectoryInformation,   // 2
    FileBothDirectoryInformation,   // 3
    FileBasicInformation,           // 4  wdm
    FileStandardInformation,        // 5  wdm
    FileInternalInformation,        // 6
    FileEaInformation,              // 7
    FileAccessInformation,          // 8
    FileNameInformation,            // 9
    FileRenameInformation,          // 10
    FileLinkInformation,            // 11
    FileNamesInformation,           // 12
    FileDispositionInformation,     // 13
    FilePositionInformation,        // 14 wdm
    FileFullEaInformation,          // 15
    FileModeInformation,            // 16
    FileAlignmentInformation,       // 17
    FileAllInformation,             // 18
    FileAllocationInformation,      // 19
    FileEndOfFileInformation,       // 20 wdm
    FileAlternateNameInformation,   // 21
    FileStreamInformation,          // 22
    FilePipeInformation,            // 23
    FilePipeLocalInformation,       // 24
    FilePipeRemoteInformation,      // 25
    FileMailslotQueryInformation,   // 26
    FileMailslotSetInformation,     // 27
    FileCompressionInformation,     // 28
    FileObjectIdInformation,        // 29
    FileCompletionInformation,      // 30
    FileMoveClusterInformation,     // 31
    FileQuotaInformation,           // 32
    FileReparsePointInformation,    // 33
    FileNetworkOpenInformation,     // 34
    FileAttributeTagInformation,    // 35
    FileTrackingInformation,        // 36
    FileIdBothDirectoryInformation, // 37
    FileIdFullDirectoryInformation, // 38
    FileValidDataLengthInformation, // 39
    FileShortNameInformation,       // 40
    FileMaximumInformation

} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation       = 1,
    FileFsLabelInformation,      // 2
    FileFsSizeInformation,       // 3
    FileFsDeviceInformation,     // 4
    FileFsAttributeInformation,  // 5
    FileFsControlInformation,    // 6
    FileFsFullSizeInformation,   // 7
    FileFsObjectIdInformation,   // 8
    FileFsDriverPathInformation, // 9
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;



typedef HANDLE (NTAPI *P_ZwOpenFile)(  
     PHANDLE  FileHandle,
    ACCESS_MASK  DesiredAccess,
    OBJECT_ATTRIBUTES  *ObjectAttributes,
    PIO_STATUS_BLOCK  IoStatusBlock,
    ULONG  ShareAccess,
    ULONG  OpenOptions);

typedef NTSTATUS (NTAPI *P_NtQueryInformationFile)(
 
  HANDLE                 hFile,
  PIO_STATUS_BLOCK       io,
  PVOID                  ptr,
  LONG                   len,
  FILE_INFORMATION_CLASS class1
 );

typedef NTSTATUS (NTAPI * P_NtDeviceIoControlFile)(
    HANDLE FileHandle,
    HANDLE Event,
    PVOID ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG IoControlCode,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength
);

typedef NTSTATUS (NTAPI * P_ZwReadFile) (
   HANDLE  FileHandle,
   HANDLE  Event  ,
   PVOID  ApcRoutine  ,
   PVOID  ApcContext  ,
    PIO_STATUS_BLOCK  IoStatusBlock,
    PVOID  Buffer,
   ULONG  Length,
   PLARGE_INTEGER  ByteOffset  ,
   PULONG  Key  
    );

typedef NTSTATUS (NTAPI * P_ZwWriteFile) (
   HANDLE  FileHandle,
   HANDLE  Event  ,
   PVOID  ApcRoutine  ,
   PVOID  ApcContext  ,
    PIO_STATUS_BLOCK  IoStatusBlock,
    PVOID  Buffer,
   ULONG  Length,
   PLARGE_INTEGER  ByteOffset  ,
   PULONG  Key  
    );

typedef NTSTATUS (NTAPI *  P_ZwClose)(
    HANDLE  Handle
    );

typedef NTSTATUS (NTAPI *  P_NtFsControlFile)(
	HANDLE FileHandle, HANDLE Event OPTIONAL, PVOID ApcRoutine OPTIONAL, PVOID ApcContext , PIO_STATUS_BLOCK IoStatusBlock, 
	ULONG FsControlCode, PVOID InputBuffer , ULONG InputBufferLength, PVOID OutputBuffer , ULONG OutputBufferLength ); 

typedef NTSTATUS (NTAPI *  P_NtSetVolumeInformationFile)(
	HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileSystemInformation, ULONG Length, 
	FS_INFORMATION_CLASS FileSystemInformationClass ); 


typedef struct _FILE_ALIGNMENT_INFORMATION {
  ULONG  AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION;
/*
typedef enum _MEDIA_TYPE {
    Unknown
} MEDIA_TYPE;

typedef struct _DISK_GEOMETRY {
  LARGE_INTEGER Cylinders;
  MEDIA_TYPE    MediaType;
  DWORD         TracksPerCylinder;
  DWORD         SectorsPerTrack;
  DWORD         BytesPerSector;
} DISK_GEOMETRY;

typedef struct {
  LARGE_INTEGER Length;
} GET_LENGTH_INFORMATION;

typedef struct _PARTITION_INFORMATION_MBR {
  BYTE    PartitionType;
  BOOLEAN BootIndicator;
  BOOLEAN RecognizedPartition;
  DWORD   HiddenSectors;
} PARTITION_INFORMATION_MBR, *PPARTITION_INFORMATION_MBR;

typedef enum _PARTITION_STYLE {
    PARTITION_STYLE_MBR,
    PARTITION_STYLE_GPT,
    PARTITION_STYLE_RAW
} PARTITION_STYLE;


typedef struct {
  PARTITION_STYLE PartitionStyle;
  LARGE_INTEGER   StartingOffset;
  LARGE_INTEGER   PartitionLength;
  DWORD           PartitionNumber;
  BOOLEAN         RewritePartition;
  
   PARTITION_INFORMATION_MBR Mbr;    
  
} PARTITION_INFORMATION_EX;
*/
typedef struct _MOUNTDEV_NAME {
  USHORT  NameLength;
  WCHAR  Name[1];
} MOUNTDEV_NAME, *PMOUNTDEV_NAME;


P_NtQueryInformationFile g_pNtQueryInformationFile;
P_ZwOpenFile g_pZwOpenFile;  
P_NtDeviceIoControlFile g_pNtDeviceIoControlFile;
P_ZwReadFile g_pZwReadFile;
P_ZwWriteFile g_pZwWriteFile;
P_ZwClose g_pZwClose;
P_NtFsControlFile g_pNtFsControlFile;
P_NtSetVolumeInformationFile g_pNtSetVolumeInformationFile;

char api_output[50000];
char str[1500];
WCHAR strw[1500];
   

HANDLE fakehandle = (HANDLE)0x00777;

HANDLE NTAPI My_ZwOpenFile(  
     PHANDLE  FileHandle,
    ACCESS_MASK  DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PIO_STATUS_BLOCK  IoStatusBlock,
    ULONG  ShareAccess,
    ULONG  OpenOptions)
 {
	// WriteLog(L"-> %s" ,ObjectAttributes->ObjectName->Buffer); 
	 //swprintf(strw, L"->  %s\n",ObjectAttributes->ObjectName->Buffer);
	 wcstombs(str, ObjectAttributes->ObjectName->Buffer, 200);
	 //strcat(api_output, str);

	//WriteLog("open %s", str);


	 if ( wcsstr(ObjectAttributes->ObjectName->Buffer, L"Y:"))
	 {
		 //strcat(api_output, "fakehandle \n");
		 WriteLog("open %s", str);
		 *FileHandle = fakehandle;
		 IoStatusBlock->Information=0;
		 IoStatusBlock->Status=0;
		return 0;
	 }
	 else return g_pZwOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock,ShareAccess, OpenOptions); 
 }

 NTSTATUS NTAPI My_NtQueryInformationFile( 
  HANDLE                 hFile,
  PIO_STATUS_BLOCK       io,
  PVOID                  ptr,
  LONG                   len,
  FILE_INFORMATION_CLASS class1
 )
 {
	 //WriteLog(L"%X, -> info %d" ,hFile, class1); 
	 //sprintf(str, "%X, -> info %d\n", hFile, class1);
	// strcat(api_output, str);

	//WriteLog("info, %X", class1);

	 NTSTATUS ret;	 

	 if (hFile ==fakehandle)
	 {
		 WriteLog("info, %X", class1);

		 //WriteLog(L"%X, -> info %d" ,hFile, class1); 
		 if (class1 == 17/*aligment*/)
		 {
			 FILE_ALIGNMENT_INFORMATION *ai = (FILE_ALIGNMENT_INFORMATION*)ptr;

			 //sprintf(str, "-> aligment info %d\n", ai->AlignmentRequirement );
			 //strcat(api_output, str);

			 ai->AlignmentRequirement =3;

			 io->Information = 4;
			 io->Status = 0;
			 io->Pointer = 0;

		 }

		 ret  = 0;// STATUS_SUCCESS


	 }
	 else ret = g_pNtQueryInformationFile(hFile, io, ptr, len, class1); 
	 
	 return ret; 

 }

 //IOCTL_DISK_GET_LENGTH_INFO
 /*

 70024 - IOCTL_DISK_IS_WRITABLE
 70000 - IOCTL_DISK_GET_DRIVE_GEOMETRY
 7405C - IOCTL_DISK_GET_LENGTH_INFO
 70048 - IOCTL_DISK_GET_PARTITION_INFO_EX
2D1080 - invalid 
4D0008 - IOCTL_MOUNTDEV_QUERY_DEVICE_NAME - 
 */

 NTSTATUS NTAPI My_NtDeviceIoControlFile(
    HANDLE FileHandle,
    HANDLE Event,
    PVOID ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG IoControlCode,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength
)
 {
	 //WriteLog(L"%X, -> io %d" ,FileHandle, IoControlCode); 
	 //sprintf(str, "%X, -> io %X\n", FileHandle, IoControlCode);
	 //strcat(api_output, str);

	  NTSTATUS ret=0;

	 //WriteLog("ioctl, %X", IoControlCode);

	 //ret = g_pNtDeviceIoControlFile(FileHandle, Event, ApcRoutine,ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength );

	 if (FileHandle == fakehandle)
	 {
		 WriteLog("io, %X", IoControlCode);
		 
		 if (IoControlCode ==0x70024)
		 {
			 IoStatusBlock->Information=0;
			 IoStatusBlock->Status=0;
			 return 0;

		 }

		 if (IoControlCode ==0x70000)
		 {
			 DISK_GEOMETRY* dg = (DISK_GEOMETRY*)(OutputBuffer);
			 //return ret;
			 dg->BytesPerSector = 512;
			 dg->Cylinders.QuadPart = img_disksize.QuadPart / dg->BytesPerSector;
			 dg->MediaType = (MEDIA_TYPE)12;
			 dg->SectorsPerTrack = 1;
			 dg->TracksPerCylinder = 1;

			 return 0;

		 }
		  if (IoControlCode ==0x7405C)
		{
			GET_LENGTH_INFORMATION* li = (GET_LENGTH_INFORMATION*)(OutputBuffer);
			//return ret;
			li->Length = img_disksize;
			return 0;

		  }
		   if (IoControlCode ==0x70048/*PARTITION_INFORMATION_EX*/)
			{
			PARTITION_INFORMATION_EX* pi = (PARTITION_INFORMATION_EX*)(OutputBuffer);
			//return ret;
			pi->PartitionStyle = PARTITION_STYLE_MBR;
			pi->PartitionLength = img_disksize;
			pi->PartitionNumber = 0;
			pi->RewritePartition = 1;
			pi->StartingOffset.QuadPart = 512;
			pi->Mbr.BootIndicator = FALSE;
			pi->Mbr.PartitionType = 0;// PARTITION_ENTRY_UNUSED
			pi->Mbr.RecognizedPartition = 0;
			return 0;			
			}

		   if (IoControlCode == 0x2D1080 )
		   {
			   return 0x10;
		   }

		   if (IoControlCode == 0x2D1400 ) // IOCTL_STORAGE_QUERY_PROPERTY
		   {
			   return  0x10;
		   }

		   if (IoControlCode == 0x2D0c14 ) // IOCTL_STORAGE_GET_HOTPLUG_INFO
		   {
			   return  0x10;
		   }

		   if (IoControlCode == 0x4D0008 )
		   {
			   PMOUNTDEV_NAME mn = (PMOUNTDEV_NAME)(OutputBuffer);
			   //return ret;
			   wcscpy(mn->Name, L"\\Device\\Y:00000077");
			   mn->NameLength = wcslen(mn->Name);			   
			   return 0;
		   }

		   if (IoControlCode == 0x70014 )
		   {
			   // 70014 - IOCTL_DISK_VERIFY

			   IoStatusBlock->Information=0;
			   IoStatusBlock->Status=0;

			   return 0;

		   }

		   
		   // 7C008  - IOCTL_DISK_SET_PARTITION_INFO
		    //strcat(api_output, "unknown ioctl");
			WriteLog("unknown io");

			IoStatusBlock->Information=0;
			IoStatusBlock->Status=0;

			return 0;

	 }

	 else ret = g_pNtDeviceIoControlFile(FileHandle, Event, ApcRoutine,ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength );

	 return ret;
 }

extern char IV[64];
extern char Key[32];

#define STATUS_END_OF_FILE 0xC0000011L
#define STATUS_SHARING_VIOLATION 0xC0000043L

 NTSTATUS crypt_ZwReadFile(HANDLE hFile, LPVOID buf, DWORD psz, PIO_STATUS_BLOCK  IoStatusBlock, PLARGE_INTEGER  ByteOffset)
{
	DWORD ret;
	DWORD nr_sz;	
	//DWORD pl,ph,pph,ppl;
	u64	  pos;
	ret = 1;
	LARGE_INTEGER li; 
	
	{
		
		
		pos = (ByteOffset->HighPart<<32)|ByteOffset->LowPart;		
		// позиция файла должна быть кратная 512 		
		pos = ((pos>>9)<<9);
		//if((psz % 512))
			//n__sz=n_sz =  (((psz>>9)+1)<<9);
		//	pos = pos;
		//else
		//	n__sz=n_sz = psz;

		if (is_partition_container2)
		{
			
			
			li.QuadPart = ByteOffset->QuadPart; 
			theDiskPartition2->SetFilePointer2(&li);

			//theDiskPartition2->partOffset.QuadPart+= theDiskPartition2->partOffset2.QuadPart;
			ret = theDiskPartition2->ReadData( (LPBYTE)buf, psz, &theDiskPartition2->partOffset);
			IoStatusBlock->Information = psz;
			IoStatusBlock->Status = 0;

			if (is_encrypted_partition2 )
				decrypt((char *)buf, (pos >>9), IoStatusBlock->Information);	 // pos>>9 - sector number				

			if (ret) 
				ret=0;	
			else 
				ret = 1;

			//WriteLog("read %X", ret);	 // C0000008	

			return ret;
		}
			

		
		li.QuadPart = ByteOffset->QuadPart ;
		li.QuadPart += diskDataOffset2.QuadPart;
		
		ret = g_pZwReadFile(hFile, NULL, 0, 0, IoStatusBlock, buf, psz, &li, 0);		

		if ( ret == 0 ) 
		{
			//WriteLog("readok %X", IoStatusBlock->Information);		
			decrypt((char *)buf, (pos>>9), IoStatusBlock->Information);		// pos>>9 - sector number
		}		 else
		{
			if (ret == STATUS_END_OF_FILE )
			{
				memset(buf, 0, psz);
				IoStatusBlock->Information = psz;
				IoStatusBlock->Status =0;
				return 0;
			}

			WriteLog("read err %X", ret);	 // C0000008	
		}
			

		
	}
	return ret;
}




  NTSTATUS NTAPI My_ZwReadFile (
   HANDLE  FileHandle,
   HANDLE  Event  ,
   PVOID  ApcRoutine  ,
   PVOID  ApcContext  ,
    PIO_STATUS_BLOCK  IoStatusBlock,
    PVOID  Buffer,
   ULONG  Length,
   PLARGE_INTEGER  ByteOffset  ,
   PULONG  Key  
    )

  {
	  

	  //WriteLog("read, %X %X", ByteOffset->LowPart, Length);

	 if (FileHandle == fakehandle)
	 {
		 //sprintf(str, "%X, reading (%X, %X)\n", FileHandle, Event, Length);
		//strcat(api_output, str);

		//WriteLog("read, %X %X", ByteOffset->LowPart, Length);

		 if ( ByteOffset->QuadPart > img_disksize.QuadPart)		  
		 {

			 IoStatusBlock->Information = Length;
			 IoStatusBlock->Status =0;
			 memset(Buffer, 0, Length);
			 WriteLog("read-over, %X %X", ByteOffset->LowPart, Length);
			 return /*STATUS_END_OF_FILE*/0;
		 }
		 NTSTATUS ret =0;
	
		 if ( (ret=crypt_ZwReadFile(img_hImageFilehandle, Buffer, Length, IoStatusBlock, ByteOffset))==0)
		 {			
			 //WriteLog("ReadOK");
			return 0;
			}
		 else
		 {
			 if ( (ret) && ByteOffset->QuadPart < img_disksize.QuadPart)
			 {
				 IoStatusBlock->Information = Length;
				IoStatusBlock->Status =0;
				memset(Buffer, 0, Length);
				return 0;
			 }

			 return 1;
		 }

	 }else return g_pZwReadFile(FileHandle, Event, ApcRoutine,ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key );

  }


int crypt_ZwWriteFile(HANDLE hFile, LPVOID buf, DWORD psz,  PIO_STATUS_BLOCK  IoStatusBlock, PLARGE_INTEGER  ByteOffset)
{
	BOOL ret;
	DWORD nr_sz;	
	//DWORD pl,ph,pph,ppl;
	u64	  pos;
	ret = 1;
	char *n_buf;
	
	{
		
		
		pos = ByteOffset->QuadPart;
		LARGE_INTEGER li; 
		// позиция файла должна быть кратная 512 
		
		//pos = ((pos>>9)<<9);		

		n_buf = (char *)malloc(psz+32);
		memcpy(n_buf, buf, psz);

		if (is_encrypted_partition2 )
			encrypt((char *)n_buf, (pos>>9), psz);	 // >>9 - to convert position to sector number

		if (is_partition_container2)
		{
			li.QuadPart = ByteOffset->QuadPart; 
			theDiskPartition2->SetFilePointer2(&li);
			ret = theDiskPartition2->WriteData( (LPBYTE)n_buf, psz, &theDiskPartition2->partOffset);
			IoStatusBlock->Information = psz;
			IoStatusBlock->Status = 0;			

			free(n_buf);

			if (ret) 
				ret=0;	
			else 
				ret = 1;

			return ret;
		}

		
		li.QuadPart = ByteOffset->QuadPart ;
		li.QuadPart += diskDataOffset2.QuadPart;
		//WriteLog("Write %X", li.QuadPart);
		ret = g_pZwWriteFile(hFile, NULL, 0, 0, IoStatusBlock, n_buf, psz, &li, 0);

		if (ret)
			WriteLog("write err %X", ret);	 // C0000008	- wrong handle

		free(n_buf);
		
		
	}
	return ret;
}



  NTSTATUS NTAPI My_ZwWriteFile (
	  HANDLE  FileHandle,
	  HANDLE  Event  ,
	  PVOID  ApcRoutine  ,
	  PVOID  ApcContext  ,
	  PIO_STATUS_BLOCK  IoStatusBlock,
	  PVOID  Buffer,
	  ULONG  Length,
	  PLARGE_INTEGER  ByteOffset  ,
	  PULONG  Key  
	  )

  {
	  //WriteLog("write, %X %X", ByteOffset->LowPart, Length);


	  if (FileHandle == fakehandle)
	  {
		  //sprintf(str, "%X, reading (%X, %X)\n", FileHandle, Event, Length);
		  //strcat(api_output, str);

		 //WriteLog("write, %X %X", ByteOffset->LowPart, Length);

		  //if (img_readonly)
			//  return STATUS_SHARING_VIOLATION;

		  if ( ByteOffset->QuadPart > img_disksize.QuadPart)		 {
			  IoStatusBlock->Information = Length;
				IoStatusBlock->Status =0;
			WriteLog("write-over, %X %X", ByteOffset->LowPart, Length);
			  return /*STATUS_END_OF_FILE*/ 0;
		  }

		  int ret;

		  if ( (ret=crypt_ZwWriteFile(img_hImageFilehandle, Buffer, Length,  IoStatusBlock, ByteOffset)) == 0)
		  {			  
			  return 0;
		  }
		  else{
			  WriteLog("write err %X", ret);
			  return ret;
		  }

	  }else return g_pZwWriteFile(FileHandle, Event, ApcRoutine,ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key );

  }

 NTSTATUS NTAPI My_ZwClose(
    HANDLE  Handle
    )

 {
	 if (Handle == fakehandle)
	 {		 
		strcat(api_output, "close file \n");
		WriteLog("close");

		 return 0;
	 } else return g_pZwClose(Handle);
 }

 /*
 FSCTL_ALLOW_EXTENDED_DASD_IO
 FSCTL_LOCK_VOLUME 
 FSCTL_DISMOUNT_VOLUME 0x00090020
 FSCTL_UNLOCK_VOLUME 0x0009001c
 */
NTSTATUS NTAPI My_NtFsControlFile(
	HANDLE FileHandle, HANDLE Event OPTIONAL, PVOID ApcRoutine OPTIONAL, PVOID ApcContext , PIO_STATUS_BLOCK IoStatusBlock, 
	ULONG FsControlCode, PVOID InputBuffer , ULONG InputBufferLength, PVOID OutputBuffer , ULONG OutputBufferLength )
{
	//WriteLog("fs ioctl, %X", FsControlCode);

	 if (FileHandle == fakehandle)
	 {		 
		//strcat(api_output, "My_NtFsControlFile file \n");
		WriteLog("fs io, %X", FsControlCode);

		 IoStatusBlock->Information =0;
		 IoStatusBlock->Status =0;

		 return 0;
	 } else return g_pNtFsControlFile(FileHandle, Event, ApcRoutine, ApcContext,IoStatusBlock , FsControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);

}

 NTSTATUS NTAPI My_NtSetVolumeInformationFile(
	HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileSystemInformation, ULONG Length, 
	FS_INFORMATION_CLASS FileSystemInformationClass )
 {
	 if (FileHandle == fakehandle)
	 {		 
		//strcat(api_output, "My_NtFsControlFile file \n");
		WriteLog("setvolio, %X", FileSystemInformationClass);

		 IoStatusBlock->Information =0;
		 IoStatusBlock->Status =0;
		 return 0;

	 } else return g_pNtSetVolumeInformationFile(FileHandle, IoStatusBlock , FileSystemInformation, Length, FileSystemInformationClass);


 }



 //*********************************************

  typedef enum
{
  PROGRESS,
  DONEWITHSTRUCTURE,
  UNKNOWN2,
  UNKNOWN3,
  UNKNOWN4,
  UNKNOWN5,
  INSUFFICIENTRIGHTS,
  WRITEPROTECTED,
  UNKNOWN8,
  UNKNOWN9,
  UNKNOWNA,
  DONE,
  UNKNOWNC,
  UNKNOWND,
  OUTPUT,
  STRUCTUREPROGRESS
} CALLBACKCOMMAND;


 typedef BOOLEAN
   (NTAPI* PFMIFSCALLBACK)(
     IN CALLBACKCOMMAND Command,
     IN ULONG SubAction,
     IN PVOID ActionInfo);


    /* Chkdsk command in FMIFS */
typedef VOID (NTAPI   *P_Chkdsk) (
      IN PWCHAR DriveRoot,
      IN PWCHAR Format,
      IN BOOLEAN CorrectErrors,
      IN BOOLEAN Verbose,
      IN BOOLEAN CheckOnlyIfDirty,
      IN BOOLEAN ScanDrive,
      IN PVOID Unused2,
      IN PVOID Unused3,
     IN PFMIFSCALLBACK Callback);

// media flags
#define FMIFS_HARDDISK 0xC
#define FMIFS_FLOPPY   0x8

typedef VOID (__stdcall *PFORMATEX)( PWCHAR DriveRoot,
						  DWORD MediaFlag,
						  PWCHAR Format,
						  PWCHAR Label,
						  BOOL QuickFormat,
						  DWORD ClusterSize,
						  PFMIFSCALLBACK Callback );

P_Chkdsk p_Chkdsk;
PFORMATEX p_FormatEx;
TCHAR buf_output[85000] = {""};
BOOL Error = false;

 typedef struct
  {
    ULONG Lines;
    PCHAR Output;
  } TEXTOUTPUT, *PTEXTOUTPUT;



BOOLEAN WINAPI ChkdskCallback(
  CALLBACKCOMMAND Command,
  DWORD   Modifier,
  PVOID   Argument
  )
{
  PDWORD    percent;
  PBOOLEAN  status;
  PTEXTOUTPUT output;


 switch (Command) {
	   case INSUFFICIENTRIGHTS:
    
	strcat(buf_output, "INSUFFICIENTRIGHTS \n\0x0D\0x0A");
	SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);
	UpdateWindow( ::GetDlgItem(g_wnd, IDC_EDIT1) );
    break;

  case STRUCTUREPROGRESS:
    //wprintf(L"STRUCTUREPROGRESS\r");
    break;

  case DONEWITHSTRUCTURE:
    //wprintf(L"DONEWITHSTRUCTURE\r");
    break;

/* case CLUSTERSIZETOOSMALL:
    strcat(buf_output, "CLUSTERSIZETOOSMALL \n\0x0D\0x0A");
	SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);
	UpdateWindow( ::GetDlgItem(g_wnd, IDC_EDIT1) );
    break;*/

  case PROGRESS:
    percent = (PDWORD) Argument;
    //wprintf(L"%d percent completed.\r", *percent);
    break;

  case OUTPUT:
    output = (PTEXTOUTPUT) Argument;   
	if (strlen(buf_output) < 70000)
	{
	strcat(buf_output, output->Output);
	strcat(buf_output, "\n\0x0D\0x0A");
	SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);
	UpdateWindow( ::GetDlgItem(g_wnd, IDC_EDIT1) );
	}
    break;

  case DONE:
    status = (PBOOLEAN) Argument;
    if ( *status == TRUE )
    {
      
		strcat(buf_output, "Completed successfully.");
		strcat(buf_output, "\n");
		SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);

      Error = FALSE;
    } else
	{
		strcat(buf_output, "Was not able to complete successfully.");
		strcat(buf_output, "\n");
		SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);

      Error = TRUE;

	}
    break;

  default:
	  {
		  TCHAR str1[50];
		  sprintf(str1, "msg %d %X, \n\0x0D\0x0A", Command, Argument);
	     strcat(buf_output, str1);
		
		SetDlgItemText(g_wnd, IDC_EDIT1, buf_output);
		break;
	  }
	  
  }
  return TRUE;



}


// switches
BOOL  FixErrors = FALSE;
BOOL  SkipClean = FALSE;
BOOL  ScanSectors = FALSE;
BOOL  Verbose = TRUE;
PWCHAR  Drive = NULL;
WCHAR CurrentDirectory[1024];

BOOLEAN  LoadFMIFSEntryPoints_andHook(VOID)
  {
    HMODULE hm = LoadLibrary( "fmifs.dll" );
  
    if( !(p_Chkdsk = (P_Chkdsk) GetProcAddress(    GetModuleHandle("fmifs.dll"),     "Chkdsk" ))      )
    {
      return FALSE;
    }

	if( !(p_FormatEx = (PFORMATEX) GetProcAddress(    GetModuleHandle("fmifs.dll"),     "FormatEx" ))      )
    {
      return FALSE;
    }

	   return TRUE;
  }

HANDLE hHook__ZwOpenFile ;
HANDLE hHook__NtQueryInformationFile ;
HANDLE hHook__NtDeviceIoControlFile ;
HANDLE hHook__ZwReadFile;
HANDLE hHook__ZwWriteFile ;
HANDLE hHook__ZwClose ;
HANDLE hHook2 ;
HANDLE hHook3 ;


bool Hook_NT_Functions()
{

#ifdef _USE_BOXEDAPP_SDK

	strcpy(buf_output, "\n");
	api_output[0]=0;

			//
 BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__ENABLE_VIRTUAL_REGISTRY,false);
 BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__EMBED_BOXEDAPP_IN_CHILD_PROCESSES, TRUE);
   
 PVOID pZwOpenFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwOpenFile");     
 PVOID pNtQueryInformationFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQueryInformationFile");     
 PVOID pNtDeviceIoControlFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtDeviceIoControlFile");     
 PVOID pZwReadFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwReadFile");     
 PVOID pZwWriteFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwWriteFile");     
 PVOID pZwClose = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwClose");      
 PVOID pNtFsControlFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtFsControlFile");
 PVOID pNtSetVolumeInformationFile = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtSetVolumeInformationFile");


hHook__ZwOpenFile = BoxedAppSDK_HookFunction(pZwOpenFile, &My_ZwOpenFile, TRUE);     
hHook__NtQueryInformationFile = BoxedAppSDK_HookFunction(pNtQueryInformationFile, &My_NtQueryInformationFile, TRUE);     
hHook__NtDeviceIoControlFile = BoxedAppSDK_HookFunction(pNtDeviceIoControlFile, &My_NtDeviceIoControlFile, TRUE);     
hHook__ZwReadFile = BoxedAppSDK_HookFunction(pZwReadFile, &My_ZwReadFile, TRUE);     
hHook__ZwWriteFile = BoxedAppSDK_HookFunction(pZwWriteFile, &My_ZwWriteFile, TRUE);     
hHook__ZwClose = BoxedAppSDK_HookFunction(pZwClose, &My_ZwClose, TRUE);     
hHook2 = BoxedAppSDK_HookFunction(pNtFsControlFile, &My_NtFsControlFile, TRUE);     
hHook3 = BoxedAppSDK_HookFunction(pNtSetVolumeInformationFile, &My_NtSetVolumeInformationFile, TRUE);     

if (!hHook__ZwOpenFile || !hHook__NtQueryInformationFile || !hHook__NtDeviceIoControlFile ||!hHook__ZwReadFile ||!hHook2 )
	return false;

 g_pZwOpenFile = (P_ZwOpenFile)BoxedAppSDK_GetOriginalFunction(hHook__ZwOpenFile);  
 g_pNtQueryInformationFile = (P_NtQueryInformationFile)BoxedAppSDK_GetOriginalFunction(hHook__NtQueryInformationFile);  
 g_pNtDeviceIoControlFile = (P_NtDeviceIoControlFile)BoxedAppSDK_GetOriginalFunction(hHook__NtDeviceIoControlFile);  
 g_pZwReadFile = (P_ZwReadFile)BoxedAppSDK_GetOriginalFunction(hHook__ZwReadFile);  
 g_pZwWriteFile = (P_ZwWriteFile)BoxedAppSDK_GetOriginalFunction(hHook__ZwWriteFile);  
 g_pZwClose = (P_ZwClose)BoxedAppSDK_GetOriginalFunction(hHook__ZwClose);  
 g_pNtFsControlFile = (P_NtFsControlFile)BoxedAppSDK_GetOriginalFunction(hHook2);  
 g_pNtSetVolumeInformationFile = (P_NtSetVolumeInformationFile)BoxedAppSDK_GetOriginalFunction(hHook3);  

#endif 
 return true;
}

bool UnHook_NT_Functions()
{
#ifdef _USE_BOXEDAPP_SDK
	BoxedAppSDK_UnhookFunction(hHook__ZwOpenFile);  
	BoxedAppSDK_UnhookFunction(hHook__NtQueryInformationFile);  
	BoxedAppSDK_UnhookFunction(hHook__NtDeviceIoControlFile);  
	BoxedAppSDK_UnhookFunction(hHook__ZwReadFile);  
	BoxedAppSDK_UnhookFunction(hHook__ZwWriteFile);  
	BoxedAppSDK_UnhookFunction(hHook__ZwClose);  
	BoxedAppSDK_UnhookFunction(hHook2);  
	BoxedAppSDK_UnhookFunction(hHook3);  

#endif 
	return true;

}

void CDlg_CheckDsk::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	strcpy(buf_output, "\n");
	api_output[0]=0;

	WriteLog("Start check...");
	UpdateData(); 

	if (theDiskPartition2)
	{
		img_disksize = theDiskPartition2->GetUnpartitionedSpace();
	}	

	//if (m_FixErrors)
	_FSys.CloseVolume(true); // close current volume
	
#ifdef _USE_BOXEDAPP_SDK
	if ( !BoxedAppSDK_Init() || 
		!Hook_NT_Functions() || 
		!LoadFMIFSEntryPoints_andHook())
	{
		AfxMessageBox("Error loading virtualization engine");
		return;
	}

// encryption_mode = 1
	BoxedAppSDK_SetContext(""); // <-- set the key here
#endif 
	

	FixErrors = m_FixErrors;	
	ScanSectors = m_SectorScan;
	Error = true;

	CString fs;
	GetDlgItemText(IDC_COMBO1, fs);

	mbstowcs( _filesystem, (LPCTSTR)fs, 9);
	
	AfxGetApp()->BeginWaitCursor();

	p_Chkdsk(L"\\\\.\\Y:", _filesystem,  FixErrors,  Verbose,   SkipClean,    ScanSectors,     NULL,     NULL,    ChkdskCallback);

	UnHook_NT_Functions();

	AfxGetApp()->EndWaitCursor();

	//if (m_FixErrors)
	_re_formated = true;	

	//WriteLog(api_output);
	//TRACER.Print(api_output);
	//TRACER_STREAM(api_output);

	//FILE* f = fopen("1.txt", "r");     
	//*/

}


// Format
void CDlg_CheckDsk::OnBnClickedOk2()
{
	//WriteLog("Start format...");
	//TCHAR buf_output[500] = {""};
	
	UpdateData(); 	

	TCHAR fs[50] = {"NTFS"};
	
	::GetDlgItemText(g_wnd, IDC_COMBO1, fs, 45);
	if (fs==""){
		AfxMessageBox("Please choose filesystem");
		return;
	}

	WriteLog( "Formatting disk (%d MB %s)...", img_disksize.QuadPart / 1048576 , fs);	

	if (theDiskPartition2)
	{
		img_disksize = theDiskPartition2->GetUnpartitionedSpace();
	}	

	if (img_disksize.QuadPart > 3000 * 1024 * 1024)
	{
		// when formatting make the Disk Size less 20 mb
		img_disksize.QuadPart = 2500;
		img_disksize.QuadPart *= 1024 * 1024;
		WriteLog("Disk size mb %d", img_disksize.QuadPart / 1048576);

	}

	//strcpy(fs, "NTFS");
	mbstowcs( _filesystem, fs, 5);

	int disk_sz_mb = img_disksize.QuadPart / 1048576;
	sprintf(buf_output,  "Formatting disk (%d MB %s) ...", disk_sz_mb, fs ) ;
	

	if ( AfxMessageBox(LS(IDS_AREYOU_SURE), MB_YESNO) == IDNO )
		return;

	SetDlgItemText(IDC_EDIT1, buf_output);
	::UpdateWindow( ::GetDlgItem(g_wnd, IDC_EDIT1) );


#ifdef _USE_BOXEDAPP_SDK
	// encryption_mode = 1 
	BoxedAppSDK_SetContext(""); // <-- set the key here
	if ( !BoxedAppSDK_Init() || !Hook_NT_Functions() || !LoadFMIFSEntryPoints_andHook())
	{
		AfxMessageBox("Error loading virtualization engine");
		return;
	}
#else
	AfxMessageBox("Virtualization engine missed. Mount disk as vitual drive and use Disk Check. ");
	return;
#endif
	
	FixErrors = m_FixErrors;	
	ScanSectors = m_SectorScan;
	Error = true;

	AfxGetApp()->BeginWaitCursor();

	p_FormatEx(L"\\\\.\\Y:", FMIFS_HARDDISK, _filesystem,  L"Rohos",  TRUE,   1024, ChkdskCallback); //

	AfxGetApp()->EndWaitCursor();
	
	_re_formated = true;

	UnHook_NT_Functions();
}

bool FormatImage(HANDLE hImageFile, LARGE_INTEGER disk_size, HWND hWnd, LPCTSTR fs, CDiskRepartition *partitionClass, int diskOffset)
{
	g_wnd = hWnd;
	img_disksize = disk_size;
	img_hImageFilehandle = hImageFile;
	img_readonly =0;

	strcpy(buf_output, "\n");
	api_output[0]=0;

	diskDataOffset2.QuadPart = diskOffset;		
	diskDataOffset2.QuadPart *= (1024 * 1024);

	is_encrypted_partition2 = true; // TRUE this is encrypted partition.

	if (hImageFile == INVALID_HANDLE_VALUE)
	{
		theDiskPartition2=partitionClass;
		is_partition_container2 = true; // TRUE - read from %physicaldrive1%  device
		

		WriteLog("Start partition format. mb %d", disk_size.QuadPart / 1048576);
	} else 
		WriteLog("Start image format. mb %d", disk_size.QuadPart / 1048576);


	

	WCHAR filesystem[10];
	
	mbstowcs( filesystem, (LPCTSTR)fs, 9);

#ifdef _USE_BOXEDAPP_SDK
	// encryption_mode = 1
	BoxedAppSDK_SetContext(""); // <-- set the key here
	if ( !BoxedAppSDK_Init() || !Hook_NT_Functions() || !LoadFMIFSEntryPoints_andHook())
	{
		AfxMessageBox("Error loading virtualization engine");
		return false;
	}	
#else
	AfxMessageBox("Error loading virtualization engine. Format is not possible.");
	return false;
#endif
	
	Error = true;
	p_FormatEx(L"\\\\.\\Y:", FMIFS_HARDDISK, filesystem,  L"Rohos Disk",  TRUE,   1024, ChkdskCallback);
	
#ifdef _USE_BOXEDAPP_SDK
	UnHook_NT_Functions();
	BoxedAppSDK_Exit();
#endif 
	
	return true;
}

// format real USB drive (open part) after secure re-partitioning done
bool FormatRealDrive(HWND hWnd, LPCTSTR fs, LPCTSTR drive)
{
	g_wnd = hWnd;	
	strcpy(buf_output, "\n");
	api_output[0]=0;

	WriteLog("Start real format. %s %s", drive, fs);

	WCHAR filesystem[10];	
	mbstowcs( filesystem, (LPCTSTR)fs, 9);

	
	if ( !LoadFMIFSEntryPoints_andHook())
	{
		//AfxMessageBox("Error loading disk formatting engine");
		return false;
	}	

	TCHAR adrive[50] = {"Y:\\"};
	WCHAR wdrive[50];
	adrive[0]=drive[0];
	mbstowcs(wdrive, adrive, 40);

	

	Error = true;
	
	p_FormatEx(wdrive, FMIFS_HARDDISK, filesystem,  L"Disk",  TRUE,   0, ChkdskCallback);
	
	if (Error)
	{
		WriteLog(buf_output);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		//DWORD LastError;
		char cmd[ MAX_PATH ] = "";
		char *TmpName = "f.bat";
		char *TmpName2 = "y";
		//TCHAR path[4]=TEXT("A:\\");

		TCHAR Label[30] = {"Disk"};

		memset( &si, 0, sizeof( STARTUPINFO ) );
		memset( &pi, 0, sizeof( PROCESS_INFORMATION ) );
		si.cb = sizeof( STARTUPINFO );
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow =  SW_HIDE;//SW_SHOWNORMAL


		FILE *f,*f2;

		f = fopen( TmpName, "w" );
		f2 = fopen( TmpName2, "wb" );	


		sprintf( cmd, "format.com %c: /FS:%s /V:Disk /Q <y", drive[0], fs);	
		fputs( cmd, f );
		fclose( f );
		fwrite( "y\x0D\x0A\x0D\x0A", 1, 5, f2 );
		fclose( f2 );

		WriteLog(cmd);

		if( !CreateProcess( NULL, TEXT("f.bat"), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi ) )
		{
			WriteLog("CDiskPage::tryFormatDisk, Error run f.bat");
			return false;
		}

		switch( WaitForSingleObject( pi.hProcess, 60000L ) ) // INFINITE
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			break;
		case WAIT_ABANDONED:
			break;
		}



		if( pi.hThread != INVALID_HANDLE_VALUE ) 
			CloseHandle( pi.hThread );
		if( pi.hProcess != INVALID_HANDLE_VALUE ) 
			CloseHandle( pi.hProcess );

		remove( TmpName );
		remove( TmpName2 );


		Sleep(500);

		SetVolumeLabel(drive, Label );

		if (GetDiskSpaceMB(drive) >0)
			return TRUE;

		return false;

	}
	
	return true;
}
