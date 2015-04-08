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
**
**********************************************************************/

#include "pub.h"


#include ".\crypt-1.17\tomcrypt.h"

#define CRYPT_TEST

#ifdef CRYPT_TEST

int encryption_mode = 3;

// AES 256 test vectors by NIST.
// http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
//
// see rijndael_test()

char Key[32]={
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
				0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
				0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
			};

char IV[64]={0};


char pt_test[16]=      { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
		
char ct_test[16]={ 0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf, 
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89 };

#endif

//#include <shs.h>
//#include <shs512.h>
#include "Wincrypt.h"


#define RH_KEY_SIZE			64
#define RH_KEY_SIZE2		32
#define RH_IV_SIZE			64
#define RH_BLOCK_SIZE		16



char _IV[64];

symmetric_key aeskey2;

void XORBlock(
    IN   unsigned char*  InA,
    IN   unsigned char*  InB,
    OUT  unsigned char*  Out,
    IN   unsigned int    Size  // In Bytes
)
{
    unsigned int i;

    for (i=0; i<Size; i++)
        {
        Out[i] = InA[i] ^ InB[i];
        }

}


//------------------------------------------------------------------------------------
char init_crypto(char *key, char *iv)
{	
	// Init AES 256 bit key
	rijndael_setup( (unsigned char*)key, 32, 0, &aeskey2);

	//int test_ret = rijndael_test();
	
	return 0;
}

//------------------------------------------------------------------------------------
// pos - sector number 
void decrypt(char *buf, u64 sectorNo, DWORD sz)
{
	DWORD IV2[19], IV3[19];
	int s_sz=512;// data block size (sector size)
	DWORD j,k,pb;
	
	init_crypto(Key,NULL);	

	for(j=0; j<sz; j+=s_sz)                                                   
	{
		CopyMemory(IV2,IV,16);		
		IV2[0]=  (DWORD)(sectorNo&0xFFFFFFFF)	^ ((DWORD*)IV)[0]; 
		IV2[1]=  (DWORD)(sectorNo>>32)			^ ((DWORD*)IV)[1];								

		/*
		 Decrypt data using CBC mode
        
         CBC Mode DECRYPT:         
         *) For each block...
             1) Decrypt cyphertext with a key
             2) XOR output plaintext with IV
             3) IV = original Cyphertext data
		*/

		for(k=0;k<s_sz;k+=16)
		{
			CopyMemory(IV3, buf+k+j, 16); // backup Cyphertext data

			//1
			rijndael_ecb_decrypt((unsigned char*)buf+k+j, (unsigned char*)buf+k+j, &aeskey2 );
			//2
			XORBlock((unsigned char*)buf+k+j, (unsigned char*)IV2, (unsigned char*)buf+k+j, 16);
			//3
			CopyMemory(IV2, IV3, 16);
		}
		
		sectorNo++;
	}
	
}

void encrypt(char *buf, u64 sectorNo, DWORD sz)
{
	DWORD IV2[19], IV3[19];
	int s_sz=512; // data block size (sector size)
	DWORD j,k,pb;	

		
	init_crypto(Key,NULL);
	for(j=0; j<sz; j+=s_sz)                                                   
	{
		CopyMemory(IV2,IV,16);
		
		IV2[0]=  (DWORD)(sectorNo & 0xFFFFFFFF)	^ ((DWORD*)IV)[0]; 
		IV2[1]=  (DWORD)(sectorNo >> 32)		^ ((DWORD*)IV)[1];						
		

		/*
		 CBC Mode ENCRYPT:
         
         *) For each block...
             *) XOR plaintext with IV
             *) Encrypt XOR'd plaintext 
             *) IV = The just encrypted data
		*/

		for(k=0;k<s_sz;k+=16)
		{
			//1
			XORBlock((unsigned char*)buf+k+j, (unsigned char*)IV2, (unsigned char*)buf+k+j, 16);

			//2
			rijndael_ecb_encrypt((unsigned char*)buf+k+j, (unsigned char*)buf+k+j, &aeskey2 );
						
			//3
			CopyMemory(IV2, buf+k+j, 16); 
		}
		

		sectorNo++;
	}
	
}

							


// trasform val
//
BYTE GetArraySecByte2( unsigned long *prevSecByte, BYTE nextKeyByte )
{	
	unsigned long secByte = *prevSecByte;
	BYTE bit1Val = ((secByte & 0x800000)	> 0) == ((secByte & 0x100)	> 0); // compare bit#24 and bit #7
	BYTE bit2Val = ((secByte & 0x100000)	> 0) == ((secByte & 0x200)	> 0); // compare bit#20 and bit #10
	bit1Val = bit1Val << 7 | bit2Val;	
	BYTE v = ((secByte >> 3) ^ nextKeyByte ) ^ (secByte << 5) | bit1Val;	

	*prevSecByte = (secByte << (7+bit2Val)) + nextKeyByte;

	return v;	
}


/** возвращает 512bit ключ от данного пароля. на этом ключе что-то можно зашифровать.
размер буфера Key RH_KEY_SIZE
encryption_mode==1
*/
bool DeriveKeyFromPassw(LPCTSTR passw, BYTE Key[RH_KEY_SIZE], LPTSTR strKey)
{	
	

	return true;

}


/** returns 256bit key generated from password string. 
Key size should be RH_KEY_SIZE
*/
bool DeriveKeyFromPassw_v2(LPCTSTR passw, BYTE Key[RH_KEY_SIZE], LPTSTR strKey)
{
	
	symmetric_key  aeskey; // AES256 encryption

	int i;
	
	if (!passw ) {
		WriteLog("CPassword::DeriveKeyFromPassw2, null input");
		return false;
	}

	memset( _IV, 0, RH_IV_SIZE );
	memset( Key, 0, RH_KEY_SIZE );	
	
	// hash pwd
	hash_state sha_512;
	sha512_init( &sha_512 );	
	sha512_process( &sha_512, (unsigned char*)passw, _tcslen( passw ) );
	sha512_done( &sha_512, Key );	// output hash 64 chars
	
	// use hashed PWD now as encryption key.
	rijndael_setup( (unsigned char*)Key, RH_KEY_SIZE2, 0, &aeskey);	
	
	// modifying the hash a bit
	unsigned long SecByte1 = *((unsigned long*) Key ) + *((unsigned long*)( Key + 4 ) );
	for( i = 0; i < RH_KEY_SIZE; i++ )
		Key[i] = GetArraySecByte2( &SecByte1, Key[i] );
	
	// encrypt modified Hash by non-modified Hash - this would be the Key.	
	for(i=0; i<RH_KEY_SIZE; i+=16)	
	{
		
		XORBlock((unsigned char*)Key+i, (unsigned char*)_IV, (unsigned char*)Key+i, 16);
		rijndael_ecb_encrypt((unsigned char*)Key+i, (unsigned char*)Key+i, &aeskey );
		CopyMemory(_IV, Key+i, 16); 		
		
	}
	
	if (strKey)
	{
		// decode bytes as string... 
		_tcscpy(strKey, make_hex((BYTE*)Key, RH_KEY_SIZE) );
	}

	return true;

}


/**
encrypt and add PKCS#5 padding
BufferLen - data size in Buffer
BufferSize - size of Buffer, (it should be 16 bytes more)
*/
bool EncryptBuffer( BYTE *Buffer, DWORD *BufferLen, DWORD BufferSize, const BYTE Key[RH_KEY_SIZE], const BYTE Iv[RH_IV_SIZE] ) 
{
	
	if( !BufferLen || !Key || !Iv )
	{
		WriteLog("CPassword::EncryptBuffer, null input");
		return false;
	}
	
	// BufferSize must be big enough
	if( !Buffer || BufferSize < *BufferLen + RH_BLOCK_SIZE - *BufferLen % RH_BLOCK_SIZE )
	{
		*BufferLen =0;
		WriteLog("CPassword::EncryptBuffer, not enought buffer len");
		return false;
	}
	
	
	symmetric_key  aeskey;

	rijndael_setup( (unsigned char*)Key, RH_KEY_SIZE2, 0, &aeskey);
	
	
	// create PKCS5 padding
	int odd = RH_BLOCK_SIZE - *BufferLen % RH_BLOCK_SIZE;
	memset( Buffer + *BufferLen, odd , odd );
	*BufferLen += ( odd );
	
	// encrypt buffer - CBC Mode ENCRYPT:
	for(int k=0; k<*BufferLen; k+=16)
	{
		
		//1
		XORBlock((unsigned char*)Buffer+k, (unsigned char*)Iv, (unsigned char*)Buffer+k, 16);

		//2
		rijndael_ecb_encrypt((unsigned char*)Buffer+k, (unsigned char*)Buffer+k, &aeskey );
						
		//3
		CopyMemory((char*)Iv, Buffer+k, 16); 		
	}

	memset(&aeskey, 0, sizeof(aeskey) );	
	return true;
	
}

// Decrypt and verify PKCS#5 padding
// Buffer - data to decrypt
// BufferLen - size of data
//  Key, IV - encryption key data
// 
bool DecryptBuffer( BYTE *Buffer, DWORD *BufferLen, const BYTE Key[RH_KEY_SIZE], const BYTE Iv[RH_IV_SIZE] )
{

// check out buffer parameteres
	if( !Buffer || !BufferLen || *BufferLen ==0 || *BufferLen % RH_BLOCK_SIZE || !Key || !Iv )
	{
		WriteLog("CPassword::DecryptBuffer, null input");
		return false;
	}

	
	symmetric_key  aeskey;

	// init aes key
	rijndael_setup( (unsigned char*)Key, RH_KEY_SIZE2, 0, &aeskey);	

	DWORD IV2[19], IV3[19];
	CopyMemory(IV2, Iv, 16);

	// decrypt buffer - CBC Mode DECRYPT:
	for(int k=0; k<*BufferLen; k+=16)
	{
		
		CopyMemory(IV3, Buffer+k, 16); // backup Cyphertext data for IV
		rijndael_ecb_decrypt((unsigned char*)Buffer+k, (unsigned char*)Buffer+k, &aeskey );
		XORBlock((unsigned char*)Buffer+k, (unsigned char*)IV2, (unsigned char*)Buffer+k, 16);
		CopyMemory((char*)IV2, IV3, 16);
		
		
	}

	// verify PKCS#5 padding
	BYTE PadBuffer[ RH_BLOCK_SIZE ] = "";
	memset( PadBuffer, Buffer[ *BufferLen - 1 ], RH_BLOCK_SIZE );
    if( PadBuffer[0] < 1 || PadBuffer[0] > RH_BLOCK_SIZE ||
		memcmp( PadBuffer, Buffer + ( *BufferLen - PadBuffer[0] ), PadBuffer[0] ) )
    {
		WriteLog("CPassword::DecryptBuffer, wrong PKCS5 (wrong password).");
		return false;
	}

	// remove PKCS#5 padding
	memset(&aeskey, 0, sizeof(aeskey) );	
    *BufferLen -= PadBuffer[0];
	return true;


}
