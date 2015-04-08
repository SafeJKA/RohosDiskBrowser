/*
 common.h
 Common definitions and included files for the FATlib

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

#ifndef _COMMON_H
#define _COMMON_H

// 2006-10-18 added for OTFExplorer by JHH
#ifdef OTF_VC7
#include <memory.h>
#endif

#include "mingw.h" // Added for TrueCryptExplorer when compiling using MinGW

#define BYTES_PER_READ 512


// 2006-10-18 added for OTFExplorer by JHH
#define FEATURE_MEDIUM_CANREAD		0x00000001
#define FEATURE_MEDIUM_CANWRITE		0x00000002
#define FEATURE_SLOT_GBA			0x00000010
#define FEATURE_SLOT_NDS			0x00000020

#define EOVERFLOW 75
#define ENOTSUP 99

#define 	O_ACCMODE (O_RDONLY | O_RDWR | O_WRONLY)
#define 	S_IRGRP   0000040
#define 	S_IROTH   0000004
#define 	S_IRUSR   0000400
#define 	S_IWGRP   0000020
#define 	S_IWOTH   0000002
#define 	S_IWUSR   0000200

#ifndef s8
//typedef signed char s8;
#endif

typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

#ifdef OTF_VC7
//size_t strnlen (const char *string, size_t maxlen);
#endif

#ifndef strncasecmp
#define strncasecmp strnicmp
#endif

#ifndef strcasecmp
#define strcasecmp strcmp
#endif

struct _reent {
	int _errno;
};

#endif // _COMMON_H
