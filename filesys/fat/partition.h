/*
 partition.h
 Functions for mounting and dismounting partitions
 on various block devices.

 Copyright (c) 2006 Michael "Chishm" Chisholm
	
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	2006-07-11 - Chishm
		* Original release
*/

/* 2006-10-18 modifications for OTFExplorer by JHH */

#ifndef _PARTITION_H
#define _PARTITION_H

#include <windows.h> // 2006-10-18 added for OTFExplorer by JHH

#include "common.h"

//#include "disc_io/disc.h" // 2006-10-18 removed for OTFExplorer by JHH
#include "cache.h"

// Device name
extern const char* DEVICE_NAME;

// Filesystem type
typedef enum {FS_UNKNOWN, FS_FAT12, FS_FAT16, FS_FAT32} FS_TYPE;

#ifdef NDS
typedef enum {PI_DEFAULT, PI_SLOT_1, PI_SLOT_2, PI_CUSTOM} PARTITION_INTERFACE;
#else
typedef enum {PI_CART_SLOT} PARTITION_INTERFACE;
#endif

// 2006-10-18 added for OTFExplorer by JHH
#define FAT32_FSINFO_SIG1 0x41615252
#define FAT32_FSINFO_SIG2 0x61417272

typedef struct {
	u32 sig1;
	u32 unused1[120];
	u32 sig2;
	u32 freeClusters;
	u32 nextCluster;
	u32 unused2[4];
} FAT32_FSINFO;

typedef struct {
	u32 fatStart;
	u32 sectorsPerFat;
	u32 lastCluster;
	u32 firstFree;
} FAT;

typedef struct {
	HANDLE disc; // 2006-10-18 changed for OTFExplorer by JHH
	CACHE* cache;
	// Info about the partition
	bool readOnly;		// If this is set, then do not try writing to the disc
	FS_TYPE filesysType;
	__int64 totalSize;
	u32 rootDirStart;
	u32 rootDirCluster;
	u32 numberOfSectors;
	u32 dataStart;
	u32 bytesPerSector;
	u32 sectorsPerCluster;
	u32 bytesPerCluster;
	FAT fat;

	// 2006-10-18 added for OTFExplorer by JHH
	FAT32_FSINFO fsinfo;
	u8 fatCount;
	u16 rootCount;
	u16 RsvrdCount;

	// Values that may change after construction
	u32 cwdCluster;			// Current working directory cluser
	u32 openFileCount;
} PARTITION;

// 2006-10-18 added for OTFExplorer by JHH
PARTITION* _FAT_partition_constructor ( HANDLE disc, u32 cacheSize);
void _FAT_partition_destructor (PARTITION* partition);

/* 2006-10-18 removed for OTFExplorer by JHH
Mount the device specified by partitionDevice
PD_DEFAULT is not allowed, use _FAT_partition_setDefaultDevice
PD_CUSTOM is not allowed, use _FAT_partition_mountCustomDevice
*
bool _FAT_partition_mount (PARTITION_INTERFACE partitionNumber, u32 cacheSize);

/*
Mount a partition on a custom device
*
bool _FAT_partition_mountCustomInterface (const IO_INTERFACE* device, u32 cacheSize);

/*
Unmount the partition specified by partitionNumber
If there are open files, it will fail
*
bool _FAT_partition_unmount (PARTITION_INTERFACE partitionNumber);

/*
Forcibly unmount the partition specified by partitionNumber
Any open files on the partition will become invalid
The cache will be invalidated, and any unflushed writes will be lost
*
bool _FAT_partition_unsafeUnmount (PARTITION_INTERFACE partitionNumber);

/*
Set the default device for access by fat: and fat0:,
based on the device number
*
bool _FAT_partition_setDefaultInterface (PARTITION_INTERFACE partitionNumber);
*/

/*
Set the default device for access by fat: and fat0:, 
based on the partition pointer
*/
bool _FAT_partition_setDefaultPartition (PARTITION* partition);

/*
Return the partition specified in a path
For instance, "fat0:", "fat:", "/" and "fat:/" will all
return the default partition
*/
PARTITION* _FAT_partition_getPartitionFromPath (const char* path);

#endif // _PARTITION_H
