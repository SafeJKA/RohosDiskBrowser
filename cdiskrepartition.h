#pragma once

#include <Winioctl.h>

class CDiskRepartition
{
	struct MyDRIVE_LAYOUT_INFORMATION_EX {
	    DWORD PartitionStyle;
	    DWORD PartitionCount;
	    union {
			DRIVE_LAYOUT_INFORMATION_MBR Mbr;
			DRIVE_LAYOUT_INFORMATION_GPT Gpt;
		};
		unsigned char PartitionEntry[4096*16];
	} driveinfo, *PMyDRIVE_LAYOUT_INFORMATION_EX;
	char DiskName;
	char DriveName[50];
	BYTE DriveNum;
	
	LARGE_INTEGER _unpartitioned_space; // unpart-d space
	LARGE_INTEGER _drive_space; // total disk size (all partitions + unpart-d space);
	
	DISK_GEOMETRY DiskGeometry; // disk drive geometry structure
	int CreatePrimaryPartition();
	int GetVolumesLetter(PCHAR VolumeName);
	int enumeratevolumes(void);
public:
	LARGE_INTEGER partOffset/*SetFilePointer position*/;
	LARGE_INTEGER partOffset2/*second partition starting offset (30208)*/;	
	
	LARGE_INTEGER _partitioned_space; // total disk size (all partitions + unpart-d space);
	HANDLE hDevice;
	
	CDiskRepartition();
	void SetUnpartitionedSpace(LARGE_INTEGER _unpartitioned_space);
	void SetDataOffset(PLARGE_INTEGER offs2);
	void SetFilePointer2(PLARGE_INTEGER offs2);
	LARGE_INTEGER  GetUnpartitionedSpace();
	LARGE_INTEGER  GetPartitionedSpace();
	LARGE_INTEGER  GetDriveSpace();
	HANDLE OpenDisk();
	void CloseDisk();
	void  SetDriveLetter(char driveletter);
	bool RePartition(LARGE_INTEGER unpart);
	bool CheckDisk();
	void SetDriveNum(int num);
	bool WriteData(LPBYTE buffer, DWORD len, PLARGE_INTEGER Offset);
	bool ReadData(LPBYTE buffer, DWORD len, PLARGE_INTEGER Offset);
};
