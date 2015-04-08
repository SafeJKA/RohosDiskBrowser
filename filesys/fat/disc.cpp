/*
 disc.cpp
 Functions for reading data from a file handle with/without encryption/decryption

 Copyright (c) 2006 Josh Harris

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

	2006-10-18 - JHH
		* Original release
*/

#include "disc.h"
#include "TraceMaker.h"



extern "C" 
{ 
	BOOL crypt_ReadFile(HANDLE hFile, LPVOID buf, DWORD psz, LPDWORD sz, LPOVERLAPPED lpOverlapped);
	BOOL crypt_WriteFile(HANDLE hFile, LPVOID buf, DWORD psz, LPDWORD sz, LPOVERLAPPED lpOverlapped);
	DWORD crypt_SetFilePointer(  HANDLE hFile,   LONG lDistanceToMove,  PLONG lpDistanceToMoveHigh,  DWORD dwMoveMethod );
}



BOOL _FAT_disc_readSectors (HANDLE disc, u32 sector, u32 numSectors, void* buffer)
{
	DWORD dwResult;

	
	LARGE_INTEGER m_i64Pos;

	m_i64Pos.QuadPart = (INT64)sector * BYTES_PER_READ;
	dwResult = crypt_SetFilePointer(disc, m_i64Pos.LowPart, &m_i64Pos.HighPart, FILE_BEGIN);
	//WriteLog("_FAT_disc_readSectors");
	//BOOL ret = ReadFile(disc, buffer, numSectors * BYTES_PER_READ, &dwResult, NULL);

	return crypt_ReadFile(disc, (LPVOID)buffer, numSectors * BYTES_PER_READ, &dwResult, NULL);

	//return ret;

}

BOOL _FAT_disc_writeSectors (HANDLE disc, u32 sector, u32 numSectors, const void* buffer)
{
	DWORD dwResult;
	
	LARGE_INTEGER m_i64Pos;	
	 


	//BYTE* buff2 = (BYTE*)malloc(numSectors * BYTES_PER_READ + 32);
	//memcpy(buff2, (BYTE*)buffer, numSectors * BYTES_PER_READ);


	m_i64Pos.QuadPart = (INT64)sector * BYTES_PER_READ;


	dwResult = crypt_SetFilePointer(disc, m_i64Pos.LowPart, &m_i64Pos.HighPart, FILE_BEGIN);
    
	//bool ret =	WriteFile(disc, buff2, numSectors * BYTES_PER_READ, &dwResult, NULL);	
	//free(buff2);
	//return ret;

	return crypt_WriteFile(disc, (LPVOID)buffer, numSectors * BYTES_PER_READ, &dwResult, NULL);

}
