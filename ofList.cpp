/**
Для контроля над открытыми файлами

*/
/*
class COpenedFiles
{
   public:
    int _Count;// Размер списка
	int _Ichel;// Index of CHanged ELement
     
	struct
	{
		char Path[500];// Путь к файлу
		char FileName[500];// Имя файла
		LPFILETIME Time;// Время когда файл был изменён в последний раз

	} _FList[50];//  Сам список


	FILETIME* GetFileTimeByName(CString fname, int FullPathFlag=0, LPFILETIME Created=NULL);// По имени файла определяет дату и время его последнего изменения
	int Add(char *Path, char *FileName);//Добавляет в конец списка ещё один файл
	int DeleteElement(DWORD in);//Удаляет Из списка заданный элемент 
	int Clear();//Затирает все элементы 
	int CheckForChanges(void);//Преверяет все открытые файлы на изменение


		COpenedFiles();
		~COpenedFiles();	
};
*/



// ofList.cpp: implementation of the COpenedFiles class.
//
//////////////////////////////////////////////////////////////////////
#include "pub.h"

#include "Shlwapi.h"// PathFileExists(...);
//#include "Dlg_FilesChanged.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
COpenedFiles::COpenedFiles()
{
	_Count=0;
}
COpenedFiles::~COpenedFiles(){}
//////////////////////////////////////////////////////////////////////////////////
//  Добавляет в список ещё один элемент ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int COpenedFiles::Add(char *Path, char *FileName)
{ 
	// Если в дальнейшем "к" не измениться то новый элемент будет записан последним
	// А если измениться то н.э. будет записан i-тым
	DWORD k=_Count;
	bool add=true;// вспомогательный флаг

	//Проверряю на предмет существования данной записи
	for(int i=0;i<=(_Count-1);i++)
	{
		//  Если входные 'путь и имя файла' совпадают с данными i-того элемента
		if( !_tcscmp(_FList[i].Path,Path)  &&  !_tcscmp(_FList[i].FileName, FileName)   )
		{	
			// то новый элемент надо вносить в позицию - i
			k=i;			
			add=false;// Увеличивать список не надо
			break;
		}
	}

   _tcscpy(_FList[k].Path    , Path);// Запись нового пути 
   _tcscpy(_FList[k].FileName, FileName);// и имени файла
      
   _FList[k].Time=new FILETIME;
    
   //   записывает сюда      время  изменения   этого    файла
   _FList[k].Time=GetFileTimeByName(FileName);
   
    // Если надо увеличить список то
   if(add)
   {
	   _Count++;// Список увеличевается
   }
 
   return 1;
}
//--------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//  Удаляет Из списка заданный элемент ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------
int COpenedFiles::DeleteElement(DWORD in)
{
	//		     i->	               -|
	//
	//				  <=      <=            <=
	//	[0][..][in][..][..][..][..][_Count-1][_Count]
	//			  <=	  <=      <=                
	//	Каждый последующий элемент замещяеться тем что стоит с право от него
	for (int i=in;i<=(_Count-1);i++)	 
	{
		_tcscpy(_FList[i].Path    , _FList[i+1].Path);// Путь
		_tcscpy(_FList[i].FileName, _FList[i+1].FileName);// Имя файла
		_FList[i].Time=_FList[i+1].Time;//  Время изменения файла
	}
	
	_Count--;// Список уменьшается
	
	return 1;
	
}
//////////////////////////////////////////////////////////////////////////////////
//  Затирает все элементы ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int COpenedFiles::Clear()
{ 
	// Обход всего списка
	for(int i=0;i<=(_Count-1);i++)
	{ 
		// Обнуление i - того элемента
		_tcscpy(_FList[i].FileName,"");
		_tcscpy(_FList[i].Path,"");
		        _FList[i].Time=NULL;
		
	}
	_Count=0;// Обнуление размера списка

	return 1;
}
//////////////////////////////////////////////////////////////////////////////////
//  Преверяет все открытые файлы на изменение ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int COpenedFiles::CheckForChanges(void)
{
    //  Обход всего списка
	for (int i=0; i<= (_Count -1); i++)
	{ 
		LPFILETIME ft=new FILETIME;

		ft=GetFileTimeByName(_FList[i].FileName);/// Вытаскиваю дату из файла
		if(!ft)		
		{
			// с этим файлом не получилось
			// смотрю далее...
			continue;
		}
			// Если файл был изменён
		if(0 != CompareFileTime( ft, _FList[i].Time) )
		{
			// Обновляю время изменения i-того элемента списка открытых файлов
			_FList[i].Time=ft;
			  _Ichel=i; 
		    return 1;
		}
	}
	
	return 0;
}
//------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
// По имени файла определяет дату и время его последнего изменения ///////////////
// Кроме даты последнего изменения функция возвращяет дату создания файла ////////
//  Через выходной параметр - Created ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
FILETIME* COpenedFiles::GetFileTimeByName(CString fname, int FullPathFlag, LPFILETIME Created)
{ 
	
	LPFILETIME ft;
	ft=new FILETIME; 

	TCHAR pnp[500];// pnp = Под Ногами Путь

	
	// Если "fname" Это только имя файла находящигося под ногами
	if(!FullPathFlag)
	{
	    GetPodNogamiPath_Temp(pnp);
		// Вернёт чтото такое: "D:\rohos\DiskViewer\TL_NTFSREADER___Win32_Debug\temporary files\"
	}
	else
	{// Если "fname" Это полный путь к файлу
		_tcscpy(pnp, "");	 
	}

	 //После этого:
	_tcscat(pnp,fname);
	// pnp= "D:\rohos\DiskViewer\TL_NTFSREADER___Win32_Debug\temporary files\filename.ext"
	
	// Handle to opened file
	HANDLE hfile = CreateFile(pnp, 0,
								FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,	OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,NULL);
	// Если чтото не удалось
	if(hfile==INVALID_HANDLE_VALUE)
	{
		 WriteLog("INVALID_HANDLE_VALUE: Filename:(%s)",pnp);			
		 // Здесь делать больше нечего
		 return NULL;
	}
	// вытаскивает из файла информация о его создании, открытии и изменения
	if( !GetFileTime(hfile, Created/*created*/, NULL/*last acces*/, ft/*last write*/) )
	{
		CloseHandle(hfile);
		// Не получилось...
	 //AfxMessageBox(get_msg(MSG_GETFILETIME_ERROR), MB_ICONINFORMATION);
		return NULL;
	}
	CloseHandle(hfile);
    return ft;
}
