#include "pub.h"

HWND rsz_wnd[2]={0};
#define RSZ_OFST 3
#define RSZ_UP_OFST	44
#define RSZ_DOWN_OFST 20
void init_resizer(HWND hwnd,int pos)
{
	SetWindowLong(hwnd,GWL_WNDPROC,(long)RESIZER);
	SetWindowPos(hwnd,0,pos,RSZ_UP_OFST,0,0,SWP_NOSIZE);
	resize_wnd(hwnd);
}

void set_lr_wnd(HWND hl,HWND hr)
{
	rsz_wnd[0]=hl;
	rsz_wnd[1]=hr;

}

void resize_wnd(HWND hwnd)
{
	RECT rc1/*,rc2*/,rc3;
	HWND pt;
	DWORD x;
	char *tmp;
	pt = GetParent(hwnd);
	GetWindowRect(hwnd,&rc1);

	
	GetClientRect(pt,&rc3);
	ScreenToClient(pt,(POINT *)(&rc1));
	tmp = ((char *)&rc1)+8;
	ScreenToClient(pt,(POINT *) tmp);
//lft
	x = rc1.left - (RSZ_OFST<<1);
	SetWindowPos(rsz_wnd[0],0,RSZ_OFST,RSZ_UP_OFST,x,rc3.bottom-RSZ_UP_OFST-RSZ_OFST-RSZ_DOWN_OFST,0);
//rght
	x = rc3.right - (rc1.left+(RSZ_OFST*3));
	SetWindowPos(rsz_wnd[1],0,rc1.left+(RSZ_OFST<<1),RSZ_UP_OFST,x,rc3.bottom-RSZ_UP_OFST-RSZ_OFST-RSZ_DOWN_OFST,0);
//rsz
	SetWindowPos(hwnd,0,0,0,RSZ_OFST,rc3.bottom-RSZ_UP_OFST-RSZ_OFST-RSZ_DOWN_OFST,SWP_NOMOVE);
}
void resizer_redraw(HWND hwnd,HDC dc=0)
{

	RECT rc;
	HBRUSH br;
	if(!dc)
		dc = GetDC(hwnd);
	GetClientRect(hwnd,&rc);
	br = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	SelectObject(dc,br);
	PatBlt(dc,0,0,rc.right,rc.bottom,PATCOPY);
	DrawEdge(dc,&rc,EDGE_ETCHED, BF_RECT |BF_FLAT);
	DeleteObject(br);
	ReleaseDC(hwnd,dc);
}

int CALLBACK RESIZER(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	char bt_up=1;
	HWND pt;
	POINT cp;
	RECT rc1,rc2,rc3;
	MSG wmsg;
	int	x;
	switch(msg)
	{
	
		case WM_PAINT:
		case WM_ERASEBKGND:
			resizer_redraw(hwnd,0);
		break;
		case WM_MOUSEMOVE:
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
		break;
		case WM_LBUTTONDOWN:
			if(!bt_up)
				break;
			bt_up=0;
			SetCapture(hwnd);
			pt = GetParent(hwnd);
			GetWindowRect(hwnd,&rc1);
			GetWindowRect(pt,&rc2);
			GetClientRect(pt,&rc3);
			rc2.top+=((rc2.bottom-rc2.top)-rc3.bottom)-GetSystemMetrics(SM_CYEDGE)*2;
			OffsetRect(&rc1,-rc2.left,-rc2.top);
			x = rc2.bottom-rc2.top-RSZ_UP_OFST;
			while(!bt_up)
			{
				GetCursorPos(&cp);
				ScreenToClient(pt,&cp);
				cp.x-=LOWORD(lp);
				cp.y= rc1.top;
				SetWindowPos(hwnd,0,cp.x,RSZ_UP_OFST,RSZ_OFST,x,SWP_NOSIZE);
				resize_wnd(hwnd);
				while(PeekMessage(&wmsg,NULL,NULL,NULL,PM_REMOVE))
				{
					TranslateMessage(&wmsg);
					DispatchMessage(&wmsg);
					if(wmsg.message == WM_LBUTTONUP)
						bt_up=1;
				}
			}
			ReleaseCapture();
		break;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}