#include "pub.h"

struct jmp_far
{
  BYTE instr_push;  // push
  DWORD arg;         //аргумент push
  BYTE  instr_ret;    // ret
};


char old[6]; // 6 затираемых байт начала функции
DWORD adr_fnx; //будущий адрес оригинальной функции
DWORD written; 
jmp_far jump; 
HANDLE h_proc;

void inj_code(char *lib,char *fnx,long addr_new_fnx)
{

  //char buf[256];
  //сначала получим абсолютный адрес функции для перехвата
	adr_fnx = (DWORD)GetProcAddress(GetModuleHandle(lib),fnx);
/*
  if(!adr_fnx)
  {
    sprintf(buf,"Немогу найти адрес функции \" %s \" в \" %s\"",fnx,lib);
    MessageBox(0, buf, "Error!", MB_OK );
    return;
  }*/
	h_proc = GetCurrentProcess();
	jump.instr_push = 0x68;
	jump.arg = (DWORD)addr_new_fnx;
	jump.instr_ret = 0xC3;
	ReadProcessMemory(h_proc,(void*) adr_fnx,(void*)&old, 6, &written);
	WriteProcessMemory(h_proc, (void*)adr_fnx,(void*)&jump, sizeof(jmp_far), &written);
}

void make_real_rf(void)
{
	WriteProcessMemory(h_proc, (void*)adr_fnx,(void*)&old, 6, &written);
}

void make_virtual_rf()
{
	WriteProcessMemory(h_proc, (void*)adr_fnx,(void*)&jump, sizeof(jmp_far), &written);
}
