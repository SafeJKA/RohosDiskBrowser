#include "pub.h"




PATH_TOKEN PT_TOK={0};
#define CMP_STR(x,y)	CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,x,-1,y,-1)




bool IsWindowsSeven() {

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&verInfo);

	if ( verInfo.dwMajorVersion == 6  && verInfo.dwMinorVersion == 1  )
		return true;

	return false;
   
}

bool IsWindowsVista2() {

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&verInfo);

	if (verInfo.dwMajorVersion == 6  && verInfo.dwMinorVersion == 0  )
		return true;

	return false;
   
}
// select an item in a tree that corresponds to a path in PATH_TOKEN* pt
void select_folder_tree(PATH_TOKEN *pt,CTreeCtrl &f_tree)
{
	HTREEITEM hti=0, htt, htit;

	hti = f_tree.GetRootItem();

	TVITEM tvi={0};
	char item_name[1024]={0};
	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText = item_name;
	tvi.cchTextMax = 1024;

	for(int k=0; k<pt->t_cnt+1; k++)
	{
		if( (htt=f_tree.GetChildItem(hti)) )	
		{
			if(k<pt->t_cnt)
				while(htt)
				{
					tvi.hItem = htt;
					f_tree.GetItem(&tvi);
					if(CMP_STR(pt->token[k].ans_name,item_name)==CSTR_EQUAL)
					{
						hti = htt;						
						break;
					}
					htt=f_tree.GetNextItem(htt,TVGN_NEXT);
				}
		}
		

		/*if(k<pt->t_cnt)
		{			
			hti = htit;
			//sz=sz+sprintf(full_path+sz,"/%s",pt->token[k].ans_name);
		}*/
	
	}

	f_tree.Expand(hti,TVE_EXPAND);
	//f_tree.SetItemState( f_tree.GetSelectedItem( ) , 0, TVIS_SELECTED);
	//f_tree.SetItemState(hti, TVIS_SELECTED , TVIS_SELECTED );

	f_tree.SelectItem(hti);

	return;

}

//------------------------------------------------------------------------------------
void make_folder_tree(PATH_TOKEN *pt,CTreeCtrl &f_tree, LPCTSTR in3)
{
	DWORD i,k,sz;

	char full_path[MAX_PATH]={"/"};
	char nme[1024]={0};
	FS_FF ff={0};
	HTREEITEM hti=0,htt,htit;
	TVITEM tvi={0};
	
	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText = nme;
	tvi.cchTextMax = 1024;
//	f_tree.DeleteAllItems();
	/*if ( _tcslen(pt->pth)>1 )
	{
		hti = f_tree.GetSelectedItem();
		_tcscpy(full_path, pt->pth);
	}*/

	if (!hti)
		hti = f_tree.GetRootItem();

	f_tree.SetRedraw(false);

	bool is_vista = IsWindowsVista2();
	bool is_seven = IsWindowsSeven();
	bool is_win8 = IsWindows8();

	if(!hti)
	{
		// there is no Root item, add drive icon as root.

		/*
		if(strlen(in3)>=0)
		{
			
			hti = f_tree.InsertItem(in3,6,6,TVI_ROOT,TVI_LAST);
		}
		else
		{
			hti = f_tree.InsertItem("/",6,6,TVI_ROOT,TVI_LAST); 
		}
		*/

		// finds the appropriate icons for win versions.
		if ( is_vista )
			hti = f_tree.InsertItem("/",5/*icon*/,  5/*selected icon*/,TVI_ROOT,TVI_LAST); 
		else if (is_seven)
			hti = f_tree.InsertItem("/",7/*icon*/,  7/*selected icon*/,TVI_ROOT,TVI_LAST); 
		else 
			hti = f_tree.InsertItem("/",5/*icon*/,  5/*selected icon*/,TVI_ROOT,TVI_LAST); 

	}
	sz=0;

	
	for(k=0;k<pt->t_cnt+1;k++)
	{
		
		if(!(htt=f_tree.GetChildItem(hti)))	
		{
			memset(&ff,0,sizeof(FS_FF));	
			_FSys.BrowsePath(full_path,&ff);

			// item has no childs, 
			// and childs (subtree) with folders from path
			for(i=0;i<ff.fd_c;i++)
			{
				if ( is_vista )
					htt = f_tree.InsertItem(ff.fld[i].ans_name,6/*icon*/, 6/*selected icon*/, hti,TVI_LAST);
				else if (is_seven)
					htt = f_tree.InsertItem(ff.fld[i].ans_name,1,1,hti,TVI_LAST);
				else if (is_win8)
					htt = f_tree.InsertItem(ff.fld[i].ans_name,4,4,hti,TVI_LAST);
				else
					htt = f_tree.InsertItem(ff.fld[i].ans_name,4,6,hti,TVI_LAST);

				if(k<pt->t_cnt)
					if(CMP_STR(pt->token[k].ans_name,ff.fld[i].ans_name)==CSTR_EQUAL)
						htit = htt;
				
			}
			free_brows_structs(&ff);

		}else
		{
			// there is childs so we need to find subchild that correspond next folder in a path
			if(k<pt->t_cnt)
				while(htt)
				{
					tvi.hItem = htt;
					f_tree.GetItem(&tvi);
					if(CMP_STR(pt->token[k].ans_name,nme)==CSTR_EQUAL)
					{
						htit = htt;
						break;
					}
					htt=f_tree.GetNextItem(htt,TVGN_NEXT);
				}
		}
		
		f_tree.Expand(hti,TVE_EXPAND);
		//f_tree.SelectItem(hti);
		if(k<pt->t_cnt)
		{
			
			hti = htit;
			sz=sz+sprintf(full_path+sz,"/%s",pt->token[k].ans_name);
		}
		

	}

	f_tree.SetRedraw();
}	



//------------------------------------------------------------------------------------
void load_list_files(char *path,CTreeCtrl &f_tree,CListCtrl &f_list,CImageList &f_img)
{
//	WriteLog("load_list_files(..)");
	FS_FF ff={0};
	DWORD i,k;
	LVITEM lvi;
	HICON  hico;
	char	*ext;
	int itm;


	//brows_path(path,&ff);
	_FSys.BrowsePath(path,&ff);
	
	
	f_list.DeleteAllItems();

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;// | LVIF_PARAM | LVIF_STATE; 
	lvi.state = 0; 
	lvi.stateMask = 0; 
	lvi.iImage = 0;
	lvi.iSubItem = 0;
/////////////////////////////////////////////////////
	SHFILEINFO sfi;
 /*
	DWORD ImageHandle = SHGetFileInfo("",
		0,
		&sfi,
		sizeof(sfi),
		SHGFI_ICON |
		SHGFI_SMALLICON |
		SHGFI_SYSICONINDEX);

	if (ImageHandle != 0)
	{
		f_img.m_hImageList= (HIMAGELIST)ImageHandle;
	}

		
		f_list.SetImageList(&f_img, LVSIL_SMALL); */

	bool is_vista = IsWindowsVista2();
	bool is_seven = IsWindowsSeven();

	for(i=0;i<ff.fd_c;i++)
	{	
		// For Forlders
		
		lvi.iItem = i;
		//lvI.lParam = (LPARAM) &rgPetInfo[index];

		lvi.iImage = 4;

		if ( is_vista )
			lvi.iImage = 6; 
		
		if ( is_seven )
			lvi.iImage = 1; 
		
			
		// Folder Icon = 4
		lvi.pszText = ff.fld[i].ans_name;  //LPSTR_TEXTCALLBACK
		lvi.lParam = 1; // folder
		//list.InsertItem(LVIF_TEXT | LVIF_STATE, i, ff.fld[i].ans_name);
		
		itm = f_list.InsertItem(&lvi);
		f_list.SetItemData(itm, 1/*folder*/);
	}
	k = i;
	
	for(i=0;i<ff.fl_c;i++)
	{
		// For Files
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;// | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = i+k;
		ext = get_ext(ff.fle[i].ans_name);
		/*hico = extract_ext_icon(ext);
		if(hico)
			lvi.iImage = f_img.Add(hico);
		else
			lvi.iImage = -1;
		
		DestroyIcon(hico);*/
		lvi.pszText = ff.fle[i].ans_name;  //LPSTR_TEXTCALLBACK
		lvi.lParam = 0;
		

		HIMAGELIST hSysImageList = 
			(HIMAGELIST) SHGetFileInfo(ext, FILE_ATTRIBUTE_NORMAL /*FILE_ATTRIBUTE_DIRECTORY*/, &sfi, sizeof(sfi), 
			SHGFI_SMALLICON /*| SHGFI_SHELLICONSIZE*/ | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES); 
		if(!hSysImageList)
		{
			WriteLog("Non-executable file or an error condition "); 
		}
		else
		{
		//	WriteLog("hSysImageList=%X",hSysImageList); 
		}
		if ( f_img.m_hImageList == NULL)
			f_img.Attach(hSysImageList); 
		f_list.SetImageList(&f_img, LVSIL_SMALL); 
		lvi.iImage = sfi.iIcon; 
	

		itm = f_list.InsertItem(&lvi);
		// setup file data for colums (size, ext, date)
		char str[90];
		StrFormatByteSize64( ff.fle[i].file_data_size,  str, 40); 
		f_list.SetItem(itm, 1, LVIF_TEXT, str, 0, 0, 0, ff.fle[i].file_data_size); // file size column

		strcpy(str, "");
		const char* ext = strrchr(ff.fle[i].ans_name, '.');
		if (ext == NULL) 
			ext = str; 
		else 
			ext+=sizeof(TCHAR); // remove .
		f_list.SetItemText(itm, 2, ext); // file type column

		SYSTEMTIME stUTC, stLocal;
		FILETIME ft;
		ULARGE_INTEGER ui; ui.QuadPart = ff.fle[i].file_last_change_date;
		ft.dwHighDateTime = ui.HighPart; ft.dwLowDateTime = ui.LowPart; 
		// Convert the last-write time to local time.
		FileTimeToSystemTime(&ft, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		// Build a string showing the date and time.
		sprintf(str,  TEXT("%02d/%02d/%d  %02d:%02d"),  stLocal.wMonth, stLocal.wDay, stLocal.wYear,  stLocal.wHour, stLocal.wMinute);
		
		f_list.SetItemText(itm, 3, str); // date
		f_list.SetItemData(itm, 0);

	}

	free_brows_structs(&ff);
}