#include "pub.h"
#include "af.h"
#include "dlgs.h"
//#include "TL_NTFSREADER.cpp"
//------------------------------------------------------------------------------------
typedef struct {
		PATH_TOKEN *pt_tok;
		CListCtrl *ff_list;
} PARAMS;
//------------------------------------------------------------------------------------
PARAMS prm={0};
static CListCtrl f_list;
static CProgressCtrl f_prog;
static HWND h_copy_dlg;

//------------------------------------------------------------------------------------
WNDPROC realDlgProc;

TCHAR selectedDriveName[510] = {""};
BOOL selectedDriveNameOk = false;

int CALLBACK OFNHookProc3(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	
	switch(msg)
	{
	
		case WM_COMMAND :
		{
			// we need get the 'Open Partition' click
			if (wp == chx16)
			{
				if ( strchr(selectedDriveName, ':')  )
				{
					selectedDriveNameOk = true;
					SetWindowLong(hwnd, DWL_MSGRESULT, 1);
					EndDialog(hwnd, 1);

				}
				
			}

		}
		break;
	}
	return realDlgProc(hwnd,msg,wp,lp);
}

UINT_PTR CALLBACK OFNHookProc2(      
    HWND hdlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam
)
{

	switch (uiMsg)
	{		

	case WM_NOTIFY :
		{

		OFNOTIFY *lpOfNotify = (LPOFNOTIFY) lParam;

		// get currently selected item
		if ( lpOfNotify->hdr.code == CDN_SELCHANGE )
		{
			HWND wnd = GetParent(hdlg);

			SendMessage(wnd,  CDM_GETFILEPATH, 500 , (LPARAM) selectedDriveName );
			if ( strlen(selectedDriveName) == 3 )
			{
				enableItems(wnd, 1, chx16, 0);
			} else
			{
				strcpy(selectedDriveName, "");
				enableItems(wnd, 0, chx16, 0);
			}

			//SetWindowText( GetDlgItem(wnd, chx16), selectedDriveName);
			break;

		}

		// get OK click
		/*if ( lpOfNotify->hdr.code == CDN_FILEOK)
		{			
			if ( _tcslen(lpOfNotify->lpOFN->lpstrFile)==0 && ListView_GetSelectedCount( GetDlgItem(hdlg, lst1) ) == 0)
			{
				LPCTSTR s = lpOfNotify->lpOFN->lpstrInitialDir;
				SetWindowLong(hdlg, DWL_MSGRESULT, 1);
				return 1;
			}
			
			return 1;
		}*/

		// after Init of the dialog box done
		if ( lpOfNotify->hdr.code == CDN_INITDONE)
		{
			
			HWND wnd = GetParent(hdlg);
			selectedDriveNameOk = false;

			CommDlg_OpenSave_HideControl(wnd, edt1);
			CommDlg_OpenSave_HideControl(wnd, cmb1);
			CommDlg_OpenSave_HideControl(wnd, stc2);
			
			
			CWnd *pFD = new CWnd();
			pFD->Attach(wnd);

			CRect rectCancel; 
			pFD->GetDlgItem(IDOK)->GetWindowRect(&rectCancel);
			pFD->ScreenToClient(&rectCancel);

			//rectCancel.MoveToY( rectCancel.bottom + 3);			
			rectCancel.MoveToX( rectCancel.left - rectCancel.Width()*2);			

			//pFD->GetDlgItem(IDOK)->SetWindowPos(0,rectCancel.left, rectCancel.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			// get Cancel button coordinates

			
			
			pFD->GetDlgItem(cmb1)->GetWindowRect(&rectCancel);
			pFD->ScreenToClient(&rectCancel);		
			//rectCancel.MoveToY( rectCancel.bottom + 5);			
			//rectCancel.MoveToX( rectCancel.left - rectCancel.Width()*3);			
			//rectCancel.left -= 20;			
			rectCancel.right -= rectCancel.Width() / 2;

			CWnd *wndtb;
			wndtb = new CWnd();
			wndtb->Create("BUTTON", "Open partition", WS_CHILD | WS_VISIBLE |WS_DISABLED, *((LPRECT)rectCancel),  pFD, chx16, NULL);			

			//return 1;

			HDC hdc = GetDC(wnd);
			int nHeight = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			HFONT hBoldFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, VARIABLE_PITCH|FF_SWISS, TEXT("MS Shell Dlg") );		

			SendDlgItemMessage( wnd, chx16, WM_SETFONT , (WPARAM)hBoldFont, MAKELPARAM(1,0) );

			enableItems(wnd, 0, chx16, 0);

			// lets Hook DLGPROC becasue we need get the 'Open Partition' click
			realDlgProc = (WNDPROC)GetWindowLong(wnd,GWL_WNDPROC);	
			SetWindowLong(wnd,GWL_WNDPROC,(long)OFNHookProc3);		
			return 1;					
		}

		break;
		}

	case WM_TIMER:
		KillTimer(hdlg, 1);
		break;
	
	case WM_INITDIALOG :
		
		break;

	}

	return 0;

}

char * file_dlg(char *filter, char* title, char save_open, char *def_ext, char *def_path)
{
    OPENFILENAME ofn;
    char *filen,ret;
    int sz=2048;
    static char *cur_path=NULL;

    FillMemory(&ofn,sizeof(ofn),0);
    filen = (char *)m_alloc(sz);
    FillMemory(filen,sz,0);

	if(def_path)
	{
		m_free(cur_path);
		cur_path=def_path;
	}
    ofn.lpstrFile = filen;
    ofn.nMaxFile = sz;
    ofn.lpstrInitialDir = cur_path;
    ofn.lpstrFilter = filter;
    ofn.lpstrTitle=title;
    ofn.Flags = OFN_PATHMUSTEXIST;
    ofn.lStructSize  = sizeof(ofn);
	ofn.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;

    if(save_open)  
    {
       ofn.lpstrDefExt=def_ext;
       ofn.Flags|=OFN_OVERWRITEPROMPT;
       ret = GetSaveFileName(&ofn);
    }
    else
    {
       ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
	   ofn.lpfnHook = OFNHookProc2;
	   ret = ::GetOpenFileName(&ofn);	   

	   if (ret && selectedDriveNameOk )
	   {
		   return selectedDriveName;
	   }

    }



    filen=(char *)re_alloc(filen,str_len(filen)+1);
    filen[str_len(filen)]=0;	

	if(!def_path)
	{
		m_free(cur_path);
		cur_path=str_dup(filen);
	}else
		cur_path=NULL;

    if(ret)
        return filen;
    else
        return NULL;

}
//------------------------------------------------------------------------------------
////////////////////////
///дилог параметров
int CALLBACK PROP_PROC(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	DWORD c1,c2,n;
	char tx[2048];
	CListCtrl *ff_list;
	char *pt[5]={"Bytes","KB","MB","GB","GB"};
	u64 fsz;
	double tsz;
	switch(msg)
	{
		case WM_INITDIALOG:
			
			//accept_dlg_lang(hwnd,IDD_PROP);
			
			ShowWindow(hwnd,1);
			
			n=-1;
			fsz=0;
			c1=c2=0;
			ff_list=prm.ff_list;
			do
			{
				n = ff_list->GetNextItem(n,LVNI_SELECTED);
				if(n==-1)
					break;
				ff_list->GetItemText(n, 0, tx	,2048);
				char path[500];
				strcpy(path, prm.pt_tok->pth);
				strcat(path, "/");
				strcat(path, tx);
				fsz+=_FSys.GetFileSize(path);  //count_file_size(prm.pt_tok,tx,&c1,&c2);
				c1 ++;


			}while(n!=-1);
			sprintf(tx,"%i files  %i folders",c1,c2);
			SetWindowText(GetDlgItem(hwnd,IDC_PROP_PROP2),tx);
			
			tsz = (double)(s64)fsz;
			for(n=0;n<4;n++)
			{
				if(tsz<1024)
					break;
				tsz/=1024;
			}
			sprintf(tx,"%.2f %s ( %I64u )",tsz,pt[n],fsz);
			SetWindowText(GetDlgItem(hwnd,IDC_PROP_PROP3),tx);
			//process_events();
		break;

		case WM_CLOSE:
			ShowWindow(hwnd,0);
			EndDialog(hwnd,1);
		break;

		case WM_COMMAND:
			if(wp==BT_PROP_CLOSE)
			{
				ShowWindow(hwnd,0);
				EndDialog(hwnd,1);
			}

		break;
	}
	return 0;
}


//------------------------------------------------------------------------------------
void show_prop_dlg(HWND h_prt,CListCtrl *f_lst,PATH_TOKEN *pt_tok)
{
	int n;
	
	
	n=-1;
	n = f_lst->GetNextItem(n,LVNI_SELECTED);
	if(n==-1)
		return;
	prm.pt_tok = pt_tok;
	prm.ff_list = f_lst;
	DialogBox( GetModuleHandle(NULL),(char *)IDD_PROP,h_prt,PROP_PROC);
	SetForegroundWindow(h_prt);
	SetFocus(h_prt);
}
//------------------------------------------------------------------------------------
/////////////////////////
//ѕоиск

void add_found_file(char *pth)
{
	//DWORD i,k;
	LVITEM lvi;
	
	
	
	lvi.mask = LVIF_TEXT;// | LVIF_IMAGE;// | LVIF_PARAM | LVIF_STATE; 
	lvi.state = 0; 
	lvi.stateMask = 0; 
	lvi.iImage = 0;
	lvi.iSubItem = 0;
	
	lvi.iItem = 0;
	//lvi.iImage = 0;// f_img.Add(hico);
	lvi.pszText = pth;  //LPSTR_TEXTCALLBACK
	//list.InsertItem(LVIF_TEXT | LVIF_STATE, i, ff.fld[i].ans_name);
	f_list.InsertItem(&lvi);
}
/*
//------------------------------------------------------------------------------------
void make_found(char *mask,PATH_TOKEN *pt_tok,char *el_pth)
{
	RETSTR *crs;
	DWORD afc,ofs;
	AF *af;

	afc=ofs=0;
	af= make_af(mask,0,&afc);	
	crs=parse_string(el_pth,0,af,afc,1);
	if(crs)
	{
		add_found_file(pt_tok->pth);
		free_crs(crs,afc);
	}
	free_af(af,afc);
}
//------------------------------------------------------------------------------------
char find_file(PATH_TOKEN *pt_tok,char *el_pth,char *mask)
{
	u64 fsz;
	FS_FF ff={0};
	DWORD i;
	
//	if(start)
	fsz=0;
	// todo 
	path_down(pt_tok,el_pth);
	switch(_FSys.isDirectory(pt_tok->pth))
	{
		case 2:
			make_found(mask,pt_tok,el_pth);
		break;
		case 1:
			make_found(mask,pt_tok,el_pth);

			brows_path(pt_tok->pth,&ff,true);

			for(i=0;i<ff.fl_c;i++)
				find_file(pt_tok,ff.fle[i].ans_name,mask);

			for(i=0;i<ff.fd_c;i++)
				find_file(pt_tok,ff.fld[i].ans_name,mask);
			
			free_brows_structs(&ff);
		break;
	}
	path_up(pt_tok);
	return fsz;
}*/
//------------------------------------------------------------------------------------
void make_mask(char *buf,int sz)
{
	int i;
	i=0;
	while(i<sz && buf[i])
	{
		if(buf[i]=='*')
		{
			memcpy(buf+i+1,buf+i,sz-i-1);
			buf[i]='%';
			i++;
		}
		i++;
	}
}
//------------------------------------------------------------------------------------
int CALLBACK SEARCH_PROC(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	static char tx[2048];
	int n;
	char *tmp;
	switch(msg)
	{
		case WM_INITDIALOG:
			f_list.Attach(GetDlgItem(hwnd,IDC_LST_FOUND));
			//accept_dlg_lang(hwnd,IDD_SEARCH);
			ShowWindow(hwnd,1);
		break;

		case WM_CLOSE:
			ShowWindow(hwnd,0);
			EndDialog(hwnd,1);
		break;

		case WM_COMMAND:
			switch(wp)
			{
				case BT_SEARCH:
					
					GetWindowText(GetDlgItem(hwnd,IDC_SEARCH),tx,256);
					make_mask(tx,256);
					f_list.DeleteAllItems();
					//if(!prm.pt_tok->pth)
					
					tmp = strdup(prm.pt_tok->token[prm.pt_tok->t_cnt-1].ans_name);
					path_up(prm.pt_tok);
					//find_file(prm.pt_tok,tmp,tx);
					path_down(prm.pt_tok,tmp);
					free(tmp);
				break;
				case BT_GOTO:
					n=-1;
					n = f_list.GetNextItem(n,LVNI_SELECTED);
					if(n!=-1)
					{
						f_list.GetItemText(n,0,tx,2048);
						free_path_token(prm.pt_tok);
						
						strpath_to_token(tx,prm.pt_tok);
						path_up(prm.pt_tok);
						ShowWindow(hwnd,0);
						EndDialog(hwnd,1);
					}
				break;
			}
		break;
	}
	return 0;
}
//------------------------------------------------------------------------------------
void show_search_dlg(HWND h_prt,PATH_TOKEN *pt_tok)
{
	if(!pt_tok->pth)
		return;
	prm.pt_tok = pt_tok;
	prm.ff_list = NULL;

	DialogBox( GetModuleHandle(NULL),(char *)IDD_SEARCH,h_prt,SEARCH_PROC);
	SetForegroundWindow(h_prt);
	SetFocus(h_prt);
}
//------------------------------------------------------------------------------------
////////////////////
int CALLBACK BrowsCB(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pd)
{
	static char *pth;
	
	if(hwnd==NULL & uMsg==0)
		pth = (char *)lp;
	if(uMsg == BFFM_INITIALIZED && pth)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pth);
	}

	return 0;

}
//------------------------------------------------------------------------------------
void get_folder(char *fld)
{

    BROWSEINFO bi;
	LPITEMIDLIST pidl;
	LPMALLOC pSA;

	CHAR txt[100] = "Please choose the Folder where to save selected files."; 
 
	
	memset(&bi,0,sizeof(BROWSEINFO));
	bi.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
	bi.lpfn		= BrowsCB;
	bi.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;
	bi.lpszTitle = txt;
	/*
	if(!str_len(fld))
		lstrcpy(fld,CUR_PATH);
	else
	{
		if(fld[0]=='\\')
			lstrcpy(fld,cur_path_and(fld));

	}
*/
	BrowsCB(NULL,0,(LPARAM)fld,NULL);
	pidl = SHBrowseForFolder(&bi);
	if(pidl)
		SHGetPathFromIDList(pidl,fld);


	SHGetMalloc(&pSA);
	pSA->Free((void*)pidl);
	pSA->Release();
}
//------------------------------------------------------------------------------------
////////////////////////
///////Hex View
int CALLBACK HEX_PROC(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	const char *buf;
	DWORD sz=500;
	switch(msg)
	{
		case WM_INITDIALOG:
			SetWindowText(hwnd,prm.pt_tok->pth);
			buf = _FSys.ReadWholeFile(prm.pt_tok->pth,&sz); //ntfs_read_whole_file(prm.pt_tok->pth,&sz);
			if (buf==NULL)
			{
				AfxMessageBox(LS(IDS_ERROR_OPENING_FILE), MB_ICONINFORMATION);
			}
			else
			{
				::SetDlgItemText(hwnd,IDC_HEX_EDIT,buf);
			}

			ShowWindow( ::GetDlgItem(hwnd,IDC_HEX_EDIT) ,1);
			ShowWindow(hwnd,1);

		break;

		case WM_CLOSE:
			EndDialog(hwnd,1);

		case WM_COMMAND:
			switch(wp)
			{
				case IDCANCEL:
					EndDialog(hwnd,0);
					break;
			}


		break;
	}
	return 0;
}
//------------------------------------------------------------------------------------
void show_hex_dlg(HWND h_prt,PATH_TOKEN *pt_tok)
{
	if(!pt_tok->pth)
		return;
	prm.pt_tok = pt_tok;
	prm.ff_list = NULL;

	DialogBox( GetModuleHandle(NULL),(char *)IDD_HEXVIEW,h_prt,HEX_PROC, (LPARAM)&prm);
	SetForegroundWindow(h_prt);
	SetFocus(h_prt);
}
//------------------------------------------------------------------------------------
////////////////////
////копирование
void make_cpy_progress(int prg,char f,char *f_nme)
{
	switch(f)
	{
		case 0:
			SetWindowText(GetDlgItem(h_copy_dlg,IDC_CPY_NAME),f_nme);
			f_prog.SetPos(0);
			f_prog.SetRange32(0,prg);
			
		break;
		case 1:
			f_prog.SetPos(prg);
		break;
	}
}
//------------------------------------------------------------------------------------
int CALLBACK COPY_PROC(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	
	switch(msg)
	{
		case WM_INITDIALOG:
			//accept_dlg_lang(hwnd,IDD_COPY);
			f_prog.Attach(GetDlgItem(hwnd,IDC_CPY_PRG));
			h_copy_dlg = hwnd;
			ShowWindow(hwnd,1);
			
		break;
	}
	return 0;
}
//------------------------------------------------------------------------------------
DWORD WINAPI show_copy_dlg(void* h)
{
	HWND h_prt=(HWND)h;
	

	DialogBox( GetModuleHandle(NULL),(char *)IDD_COPY,h_prt,COPY_PROC);
	SetForegroundWindow(h_prt);
	SetFocus(h_prt);
	ExitThread(0);
	return 0;
}
//------------------------------------------------------------------------------------
void exit_copy_dlg(void)
{
	ShowWindow(h_copy_dlg,0);
	EndDialog(h_copy_dlg,1);
}