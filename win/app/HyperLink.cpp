// HyperLink.cpp : implementation file
//

#include "stdafx.h"
#include "HyperLink.h"
#include "BufferDC.h"



// CHyperLink

IMPLEMENT_DYNAMIC(CHyperLink, CStatic)
CHyperLink::CHyperLink()
{
	Initialize();
}

CHyperLink::~CHyperLink()
{
	if (m_lpLinkText != NULL)
		delete [] m_lpLinkText;
	if (m_lpLinkUrl != NULL)
		delete [] m_lpLinkUrl;

	m_hReg  = NULL;
	m_hHand = NULL;

	m_hFont.DeleteObject();

	if (m_hIconUp != NULL)
	{
		DestroyIcon(m_hIconUp);
	}

	if (m_hIconHover != NULL)
	{
		DestroyIcon(m_hIconHover);
	}

	if (m_hIconDown != NULL)
	{
		DestroyIcon(m_hIconDown);
	}

	if (m_hIconDisabled != NULL)
	{
		DestroyIcon(m_hIconDisabled);
	}
}

void CHyperLink::Initialize() 
{
	m_lpLinkText = NULL;
	m_lpLinkUrl  = NULL;

	m_hFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,DEFAULT_PITCH | FF_DONTCARE,_T("Arial"));

	m_bUnderlineAlways = FALSE;
	m_bIsLink          = TRUE;
	m_bLockInPosition  = FALSE;
	m_bMultiline       = FALSE;

	m_crLinkUp     = RGB(0,0,255);
	m_crLinkHover  = RGB(255,0,0);
	m_crLinkDown   = RGB(200,0,225);
	m_crBackGround = ::GetSysColor(COLOR_BTNFACE);

	m_crBorder     = RGB(0,0,0);
	m_nBorderSize  = 0;
	m_nBorderStyle = PS_SOLID;

	m_hReg  = NULL;
	m_hHand = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));

	m_hIconUp		= NULL;
	m_hIconHover	= NULL;
	m_hIconDown		= NULL;
	m_hIconDisabled = NULL;
	m_uShowIcon		= SI_ICONUP_ON		 | SI_ICONUP_LEFT		|   //Default: icons on the left Up/Hover/Down/Disabled
					  SI_ICONHOVER_ON	 | SI_ICONHOVER_LEFT	| 
					  SI_ICONDOWN_ON	 | SI_ICONDOWN_LEFT     |
					  SI_ICONDISABLED_ON | SI_ICONDISABLED_LEFT;

	m_bAutoGenerateDisabled = FALSE;
	m_bAutoGenerated        = FALSE;

	m_pToolTip			= &m_ToolTip;

	//Status and Mouse control variables
	m_bDisabled  = FALSE;

	m_bMouseIn   = FALSE;
	m_bMouseDown = FALSE;

	m_bMovingWindow = FALSE;

	CreatePalette();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

//Functions
int CHyperLink::OpenUrl()
{
	if (m_lpLinkUrl != NULL && m_bIsLink)
        HINSTANCE result = ShellExecute(NULL, _T("open"), m_lpLinkUrl, NULL,NULL, SW_SHOWDEFAULT);
	else
		return -1;

	return 0;
}

//Public
TCHAR* CHyperLink::GetLinkText()
{
	if (m_lpLinkText == NULL)
		return NULL;

	size_t len = _tcslen(m_lpLinkText) + 1;

	TCHAR* lpRet = new TCHAR[len];
	_tcsncpy_s(lpRet,len,m_lpLinkText,len);
	return lpRet;
}

void CHyperLink::SetLinkText(TCHAR* lpText)
{
	if (m_lpLinkText != NULL)
	{
		delete [] m_lpLinkText;
	}

	if (lpText != NULL)
	{
		size_t len = _tcslen(lpText)+1;
		m_lpLinkText = new TCHAR[len];
		_tcsncpy_s(m_lpLinkText,len,lpText,len);
	}
	else
		m_lpLinkText = NULL;
}

TCHAR* CHyperLink::GetLinkUrl()
{
	if (m_lpLinkUrl == NULL)
		return NULL;

	size_t len = _tcslen(m_lpLinkUrl)+1;
	TCHAR* lpRet = new TCHAR[len];
	_tcsncpy_s(lpRet,len,m_lpLinkUrl,len);
	return lpRet;
}

void CHyperLink::SetLinkUrl(TCHAR* lpUrl)
{
	if (m_lpLinkUrl != NULL)
	{
		delete [] m_lpLinkUrl;
	}

	if (lpUrl != NULL)
	{
		size_t len = _tcslen(lpUrl)+1;
		m_lpLinkUrl = new TCHAR[len];
		_tcsncpy_s(m_lpLinkUrl,len,lpUrl,len);
	}
	else
		m_lpLinkUrl = NULL;

	m_pToolTip->UpdateTipText(m_lpLinkUrl,this);
}

void CHyperLink::LockInPosition( BOOL bLockInPosition /*= FALSE*/ )
{
	if (m_bLockInPosition != bLockInPosition)
	{
		m_bLockInPosition = bLockInPosition; 
	}
}

void CHyperLink::SetMultiline( BOOL bMultiline )
{
	if (m_bMultiline != bMultiline)
	{ 
		m_bMultiline = bMultiline;
	}
}

void CHyperLink::IsLink( BOOL bIsLink /*= TRUE*/ )
{
	if (m_bIsLink != bIsLink)
	{
		m_bIsLink = bIsLink; 
	}
}

void CHyperLink::UnderlineAlways( BOOL bUnderlineAlways /*= FALSE*/ )
{
	if (m_bUnderlineAlways != bUnderlineAlways)
	{ 
		m_bUnderlineAlways = bUnderlineAlways; 
	}
}

void CHyperLink::SetColors(COLORREF crLinkUp, COLORREF crLinkHover, COLORREF crLinkDown, COLORREF crBackGround /* = ::GetSysColor(COLOR_BTNFACE) */)
{
	m_crLinkUp     = crLinkUp;
	m_crLinkHover  = crLinkHover;
	m_crLinkDown   = crLinkDown;
	m_crBackGround = crBackGround;
}

void CHyperLink::SetBackgroundColor( COLORREF crBackGround /*= ::GetSysColor(COLOR_BTNFACE)*/ )
{
	m_crBackGround = crBackGround;
}

void CHyperLink::SetBorderColor( COLORREF crBorder /*= RGB(0,0,0)*/ )
{
	m_crBorder = crBorder;
}

void CHyperLink::SetBorderSize( int nSize )
{
	m_nBorderSize = nSize;
}

void CHyperLink::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
}

void CHyperLink::SetIcons(HICON hIconUp, HICON hIconHover, HICON hIconDown, UINT uShowIcons, HICON hIconDisabled /*= NULL*/)
{
	if (m_hIconUp == NULL || hIconUp != NULL)
	{
		if (m_hIconUp != NULL)
		{
			DestroyIcon(m_hIconUp);
		}

		m_hIconUp    = hIconUp;
	}

	if (m_hIconHover == NULL || hIconHover != NULL)
	{
		if (m_hIconHover != NULL)
		{
			DestroyIcon(m_hIconHover);
		}

		m_hIconHover = hIconHover;
	}

	if (m_hIconDown == NULL || hIconDown != NULL)
	{
		if (m_hIconDown != NULL)
		{
			DestroyIcon(m_hIconDown);
		}

		m_hIconDown = hIconDown;
	}

	if (m_hIconDisabled == NULL || hIconDisabled != NULL)
	{
		if (m_hIconDisabled != NULL)
		{
			DestroyIcon(m_hIconDisabled);
		}

		m_hIconDisabled  = hIconDisabled;

		if (m_hIconDisabled != NULL)
		{
			m_bAutoGenerated = FALSE;
		}
	}

	m_uShowIcon  = uShowIcons;
}

void CHyperLink::SetIconUp( int nIconUp, int dx /*= 0*/, int dy /*= 0*/ )
{
	if (m_hIconUp != NULL)
	{
		DestroyIcon(m_hIconUp);
	}

	m_hIconUp = (HICON)::LoadImage(
		AfxFindResourceHandle(MAKEINTRESOURCE(nIconUp), RT_GROUP_ICON),
		MAKEINTRESOURCE(nIconUp), IMAGE_ICON, dx, dy, 0);

	if (m_bAutoGenerateDisabled && m_hIconDisabled != NULL)
	{
		DestroyIcon(m_hIconDisabled);

		m_hIconDisabled  = NULL;
		m_bAutoGenerated = FALSE;
	}
}

void CHyperLink::SetIconHover( int nIconHover, int dx /*= 0*/, int dy /*= 0*/ )
{
	if (m_hIconHover != NULL)
	{
		DestroyIcon(m_hIconHover);
	}

	m_hIconHover = (HICON)::LoadImage(
		AfxFindResourceHandle(MAKEINTRESOURCE(nIconHover), RT_GROUP_ICON),
		MAKEINTRESOURCE(nIconHover), IMAGE_ICON, dx, dy, 0);

	if (m_bAutoGenerateDisabled && m_hIconDisabled != NULL)
	{
		DestroyIcon(m_hIconDisabled);

		m_hIconDisabled  = NULL;
		m_bAutoGenerated = FALSE;
	}
}

void CHyperLink::SetIconDown( int nIconDown, int dx /*= 0*/, int dy /*= 0*/ )
{
	if (m_hIconDown != NULL)
	{
		DestroyIcon(m_hIconDown);
	}

	m_hIconDown = (HICON)::LoadImage(
		AfxFindResourceHandle(MAKEINTRESOURCE(nIconDown), RT_GROUP_ICON),
		MAKEINTRESOURCE(nIconDown), IMAGE_ICON, dx, dy, 0);

	if (m_bAutoGenerateDisabled && m_hIconDisabled != NULL)
	{
		DestroyIcon(m_hIconDisabled);

		m_hIconDisabled  = NULL;
		m_bAutoGenerated = FALSE;
	}
}

void CHyperLink::SetIconDisabled( int nIconDisabled, int dx /*= 0*/, int dy /*= 0*/ )
{
	if (m_hIconDisabled != NULL)
	{
		DestroyIcon(m_hIconDisabled);
	}

	m_hIconDisabled = (HICON)::LoadImage(
		AfxFindResourceHandle(MAKEINTRESOURCE(nIconDisabled), RT_GROUP_ICON),
		MAKEINTRESOURCE(nIconDisabled), IMAGE_ICON, dx, dy, 0);
}

void CHyperLink::SetShowIcons( UINT uShowIcons )
{
	m_uShowIcon = uShowIcons;
}

void CHyperLink::SetAutoGenerateDisabled( BOOL bAutoGenerate )
{
	if (m_bAutoGenerateDisabled != bAutoGenerate)
	{
		if (m_hIconDisabled != NULL)
		{
			DestroyIcon(m_hIconDisabled);
		}

		m_bAutoGenerateDisabled = bAutoGenerate;

		if (!m_bAutoGenerateDisabled && m_bAutoGenerated)
		{
			m_hIconDisabled  = NULL;
			m_bAutoGenerated = FALSE;
		}
	}
}

BOOL CHyperLink::SetFont(TCHAR* lpFaceName /* = "Arial" */, int nSize /* = 14 */, int nWeight /* = FW_NORMAL */, BOOL bItalic /* = FALSE */)
{
	m_hFont.DeleteObject();

	BOOL ret = m_hFont.CreateFont(nSize,0,0,0,nWeight,(BYTE)bItalic,0,0,
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,DEFAULT_PITCH | FF_DONTCARE,lpFaceName);

	if (ret)
	{
		if (::IsWindow(m_hWnd))
		{
			CWnd::SetFont(&m_hFont);
		}
	}

	return ret;
}

BOOL CHyperLink::SetFont( LPLOGFONT lpFont )
{
	m_hFont.DeleteObject();

	BOOL ret = m_hFont.CreateFontIndirect(lpFont);

	if (ret)
	{
		if (::IsWindow(m_hWnd))
		{
			CWnd::SetFont(&m_hFont);
		}
	}

	return ret;
}

void CHyperLink::Disable( BOOL bDisable /*= FALSE*/ )
{
	if (::IsWindow(this->GetSafeHwnd()))
	{
		EnableWindow(!bDisable);
	}
}

CToolTipCtrl* CHyperLink::GetTooltip()
{
	return m_pToolTip;
}

void CHyperLink::SetTooltip( CToolTipCtrl* pToolTip )
{
	if (pToolTip != NULL)
	{
		m_pToolTip = pToolTip;
	}
	else
	{
		m_pToolTip = &m_ToolTip;
	}

	CToolInfo toolInfo;
	toolInfo.cbSize    = sizeof(CToolInfo);
	toolInfo.szText[0] = (TCHAR)0;

	if (GetSafeHwnd() != NULL)
	{
		if (!m_pToolTip->GetToolInfo(toolInfo, this) ||
			(toolInfo.hwnd == NULL || ((toolInfo.uFlags & TTF_IDISHWND) == TTF_IDISHWND && toolInfo.uId == NULL)))
		{
			if (!::IsWindow(m_pToolTip->GetSafeHwnd()))
			{
				m_pToolTip->Create(this);
			}

			m_pToolTip->AddTool(this);
			m_pToolTip->UpdateTipText(m_lpLinkUrl,this);
		}
	}
}


// CHyperLink message handlers

void CHyperLink::OnPaint()
{
	if (m_bMovingWindow)
	{
		return;
	}

	CBufferDC dc(this); // device context for painting

	dc.SaveDC();

	dc.SetMapMode(MM_TEXT);

	dc.SetBkMode(TRANSPARENT);

	dc.SelectObject(m_hFont);

	dc.SetBkColor(m_crBackGround);

	CRect rc;
    GetClientRect(&rc);
	CBrush cb;
	cb.CreateSolidBrush(m_crBackGround);
	dc.SelectObject(&cb);

	CBrush cb2;
	cb2.CreateSysColorBrush(COLOR_BTNFACE);
	dc.FillRect(&rc,&cb2);
	cb2.DeleteObject();

	CPen p;

	BOOL bWndTxt = FALSE;
	if (m_lpLinkText == NULL)
	{
		bWndTxt = TRUE;
		int nlen = GetWindowTextLength();
		m_lpLinkText = new TCHAR[nlen+1];
		m_lpLinkText[nlen] = (TCHAR)0;
		GetWindowText(m_lpLinkText,nlen+1);
	}

	GetWindowRect(rc);
	GetParent()->ScreenToClient(&rc);

	SIZE iconSz    = GetFirstIcon();
	SIZE iconSpace = { 0 };
	iconSpace.cx   = iconSz.cx;
	iconSpace.cy   = iconSz.cy;

	if (m_bMouseIn)
	{
		int i = 0;
		i++;
	}

	if (m_bLockInPosition)
	{
		iconSpace = GetBiggestIcon();
	}

	CSize sz = dc.GetTextExtent(m_lpLinkText,(int)_tcslen(m_lpLinkText));

	int x = rc.left;
	int y = sz.cy + 2;

	int* nlOff = NULL;
	int* nlXl  = NULL;
	int  nlXlM = sz.cx;
	int  nlc   = 0;
	size_t len = _tcslen(m_lpLinkText);

	if (m_bMultiline)
	{
		while (len > 0 && m_lpLinkText[len-1] == _T('\n'))
		{
			len--;
		}

		if (len > 0)
		{
			while (len--)
			{
				if (m_lpLinkText[len] == _T('\n'))
				{
					nlc++;
				}
			}
		}

		if (nlc > 0)
		{
			nlc++;
			len      = _tcslen(m_lpLinkText);
			nlOff    = new int[nlc];
			nlXl     = new int[nlc];
			nlXlM    = 0;
			int nlc2 = nlc;

			while (len > 0 && m_lpLinkText[len-1] == _T('\n'))
			{
				len--;
			}

			if (len > 0)
			{
				size_t len2 = len;

				nlOff[--nlc2] = (int)len;

				while (len--)
				{
					if (m_lpLinkText[len] == _T('\n'))
					{
						nlXl[nlc2] = dc.GetTextExtent(&m_lpLinkText[len+1],(int)(len2 - len - 1)).cx;
						nlXlM = nlXl[nlc2] > nlXlM ? nlXl[nlc2] : nlXlM;
						len2 = len;

						y += sz.cy + 2;

						nlOff[--nlc2] = (int)len;
					}
				}

				if (len == -1)
				{
					nlXl[0] = dc.GetTextExtent(m_lpLinkText,(int)(len2)).cx;
					nlXlM = nlXl[0] > nlXlM ? nlXl[0] : nlXlM;
					len2 = len;
				}
			}
		}
	}
	
	if (nlc == 0 && len > 0)
	{
		len = _tcslen(m_lpLinkText);

		while (len > 0 && m_lpLinkText[len-1] == _T('\n'))
		{
			len--;
		}

		if (len > 0)
		{
			if (nlOff != NULL)
			{
				delete [] nlOff;
			}

			nlc      = 1;
			nlOff    = new int[nlc];
			nlXl     = new int[nlc];
			nlOff[0] = (int)len;
			nlXl[0]  = nlXlM;
		}
	}

	y += nlc;

	if (y < iconSpace.cy + 2)
	{
		y = iconSpace.cy + 2;
	}

	y += (m_nBorderSize * 2);

	y = rc.Height() - y;
	rc.bottom = rc.bottom - (y);

	x += (1 + nlXlM + (m_nBorderSize * 2) + 2);

	if ((!m_bDisabled &&
		  ((m_uShowIcon & SI_ICONUP_LEFT && !m_bMouseIn)     || 
		  (m_uShowIcon & SI_ICONHOVER_LEFT && m_bMouseIn && !m_bMouseDown)    || 
		  (m_uShowIcon & SI_ICONDOWN_LEFT && m_bMouseDown && m_bMouseIn))   
		) || 
		(m_uShowIcon & SI_ICONDISABLED_LEFT && m_bDisabled)  || 
		m_bLockInPosition)
	{
		x = x + iconSpace.cx + 3;
	}

	if ((!m_bDisabled &&
		  ((m_uShowIcon & SI_ICONUP_RIGHT && !m_bMouseIn)  || 
		  (m_uShowIcon & SI_ICONHOVER_RIGHT && m_bMouseIn && !m_bMouseDown)  || 
		  (m_uShowIcon & SI_ICONDOWN_RIGHT && m_bMouseDown && m_bMouseIn)) 
		) || 
		(m_uShowIcon & SI_ICONDISABLED_RIGHT && m_bDisabled) || 
		m_bLockInPosition)
	{
		x = x + 2 + iconSpace.cx + 1;
	}

	rc.right = x;

	m_bMovingWindow = TRUE;

	MoveWindow(rc.left,rc.top,rc.Width(),rc.Height(),FALSE);

	m_bMovingWindow = FALSE;

	GetClientRect(rc);

	dc.SetBoundsRect(&rc, DCB_DISABLE);

	dc.FillRect(&rc,&cb);

	//draw border
	if (m_nBorderSize > 0)
	{
		CRect brect = rc;
		CPen  pen, *oldPen;

		if (!m_bDisabled)
		{
			pen.CreatePen(m_nBorderStyle, m_nBorderSize, m_crBorder);
		}
		else
		{
			pen.CreatePen(m_nBorderStyle, m_nBorderSize, ::GetSysColor(COLOR_BTNSHADOW));
		}

		oldPen = dc.SelectObject(&pen);

		int bspacer = (m_nBorderSize - (m_nBorderSize % 2)) / 2;

		brect.DeflateRect(bspacer, bspacer);

		if (m_nBorderSize % 2 == 0)
		{
			brect.right++;
			brect.bottom++;
		}

		dc.Rectangle(&brect);

		dc.SelectObject(oldPen);
	}


	int yTxt = ((sz.cy * nlc) + (nlc * 3) - (nlc - 1));
	y = rc.Height() - iconSz.cy;
	y = (y - (y % 2)) / 2;

	x = 1 + m_nBorderSize;

	if (m_bLockInPosition)
	{
		x += ((iconSpace.cx - iconSz.cx) / 2);
	}
 
	if (m_bDisabled)
	{
		if (m_hIconDisabled != NULL)
		{
			if ((m_uShowIcon & SI_ICONDISABLED_ON && m_uShowIcon & SI_ICONDISABLED_LEFT))
			{
				DrawIconEx(dc.m_hDC,x,y,m_hIconDisabled,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);

				x = iconSpace.cx + 4 + m_nBorderSize;

				if (m_bLockInPosition)
				{
					x += ((iconSpace.cx - iconSz.cx) / 2);
				}
			}
			else if (m_bLockInPosition)
			{
				x = iconSpace.cx + 4 + m_nBorderSize;
				x += ((iconSpace.cx - iconSz.cx) / 2);
			}

			if ((m_uShowIcon & SI_ICONDISABLED_ON && m_uShowIcon & SI_ICONDISABLED_RIGHT))
			{
				DrawIconEx(dc.m_hDC,x + nlXlM + 3,y,m_hIconDisabled,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);
			}
		}

		dc.SetTextColor(::GetSysColor(COLOR_BTNSHADOW));
	}
	else if (!m_bDisabled && m_bMouseIn && !m_bMouseDown)
	{
		if ((m_uShowIcon & SI_ICONHOVER_ON && m_uShowIcon & SI_ICONHOVER_LEFT) && m_hIconHover != NULL)
		{
			DrawIconEx(dc.m_hDC,x,y,m_hIconHover,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);

			x = iconSpace.cx + 4 + m_nBorderSize;

			if (m_bLockInPosition)
			{
				x += ((iconSpace.cx - iconSz.cx) / 2);
			}
		}
		else if (m_bLockInPosition)
		{
			x = iconSpace.cx + 4 + m_nBorderSize;
			x += ((iconSpace.cx - iconSz.cx) / 2);
		}

		if ((m_uShowIcon & SI_ICONHOVER_ON && m_uShowIcon & SI_ICONHOVER_RIGHT) && m_hIconHover != NULL)
		{
			DrawIconEx(dc.m_hDC,x + nlXlM + 3,y,m_hIconHover,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);
		}

		p.CreatePen(PS_SOLID,1,m_crLinkHover);
		dc.SetTextColor(m_crLinkHover);
	}
	else if (!m_bDisabled && m_bMouseIn && m_bMouseDown)
	{
		p.CreatePen(PS_SOLID,1,m_crLinkDown);
		if ((m_uShowIcon & SI_ICONDOWN_ON && m_uShowIcon & SI_ICONDOWN_LEFT) && m_hIconDown != NULL)
		{
			DrawIconEx(dc.m_hDC,x,y,m_hIconDown,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);

			x = iconSpace.cx + 4 + m_nBorderSize;

			if (m_bLockInPosition)
			{
				x += ((iconSpace.cx - iconSz.cx) / 2);
			}
		}
		else if (m_bLockInPosition)
		{
			x = iconSpace.cx + 4 + m_nBorderSize;
			x += ((iconSpace.cx - iconSz.cx) / 2);
		}

		if ((m_uShowIcon & SI_ICONDOWN_ON && m_uShowIcon & SI_ICONDOWN_RIGHT) && m_hIconDown != NULL)
		{
			DrawIconEx(dc.m_hDC,x + nlXlM + 3,y,m_hIconDown,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);
		}

		dc.SetTextColor(m_crLinkDown);
	}
	else if (!m_bDisabled && !m_bMouseIn)
	{
		if (m_bUnderlineAlways)
			p.CreatePen(PS_SOLID,1,m_crLinkUp);
		else
			p.CreatePen(PS_SOLID,1,m_crBackGround);

		if ((m_uShowIcon & SI_ICONUP_ON && m_uShowIcon & SI_ICONUP_LEFT) && m_hIconUp != NULL)
		{
			DrawIconEx(dc.m_hDC,x,y,m_hIconUp,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);

			x = iconSpace.cx + 4 + m_nBorderSize;

			if (m_bLockInPosition)
			{
				x += ((iconSpace.cx - iconSz.cx) / 2);
			}
		}
		else if (m_bLockInPosition)
		{
			x = iconSpace.cx + 4 + m_nBorderSize;
			x += ((iconSpace.cx - iconSz.cx) / 2);
		}

		if ((m_uShowIcon & SI_ICONUP_ON && m_uShowIcon & SI_ICONUP_RIGHT) && m_hIconUp != NULL)
		{
			DrawIconEx(dc.m_hDC,x + nlXlM + 3,y,m_hIconUp,iconSz.cx,iconSz.cy,0,NULL,DI_IMAGE | DI_MASK);
		}

		dc.SetTextColor(m_crLinkUp);
	}

	if (m_bLockInPosition)
	{
		x = iconSpace.cx + 4 + m_nBorderSize;
	}

	y = rc.Height() - ((sz.cy * nlc) + (nlc * 3) - (nlc - 1));
	if (y < 0)
	{
		y = 0;
	}
	y = (y - (y % 2)) / 2;

	if (nlc > 0)
	{
		dc.TextOut(x,y,m_lpLinkText,(int)nlOff[0]);
		dc.MoveTo(x,y+sz.cy);
		dc.SelectObject(&p);
		dc.LineTo(x+nlXl[0],y+sz.cy);

		for (int i = 1; i < nlc; i++)
		{
			y += sz.cy + 2;
			dc.TextOut(x,y,&m_lpLinkText[nlOff[i-1]+1],(int)(nlOff[i] - nlOff[i-1] - 1));
			dc.MoveTo(x,y+sz.cy);
			dc.SelectObject(&p);
			dc.LineTo(x+nlXl[i],y+sz.cy);
		}
	}

	if (bWndTxt)
	{
		delete [] m_lpLinkText;
		m_lpLinkText = NULL;
	}

	if (nlOff != NULL)
	{
		delete [] nlOff;
	}

	if (nlXl != NULL)
	{
		delete [] nlXl;
	}

	dc.RestoreDC(-1);
}


SIZE CHyperLink::GetBiggestIcon()
{
	HICON    hIco   = NULL;
	ICONINFO icInfo = { 0 };
	SIZE     iconSz = { 0 };
	SIZE     tempSz = { 0 };

	hIco = m_hIconUp;

	if (hIco != NULL)
	{
		GetIconInfo(hIco, &icInfo);

		iconSz.cx = icInfo.xHotspot * 2;
		iconSz.cy = icInfo.yHotspot * 2;
	}

	hIco = m_hIconHover;

	if (hIco != NULL)
	{
		GetIconInfo(hIco, &icInfo);

		tempSz.cx = icInfo.xHotspot * 2;
		tempSz.cy = icInfo.yHotspot * 2;

		if (tempSz.cx > iconSz.cx)
		{
			iconSz.cx = tempSz.cx;
			iconSz.cy = tempSz.cy;
		}
	}

	hIco = m_hIconDown;

	if (hIco != NULL)
	{
		GetIconInfo(hIco, &icInfo);

		tempSz.cx = icInfo.xHotspot * 2;
		tempSz.cy = icInfo.yHotspot * 2;

		if (tempSz.cx > iconSz.cx)
		{
			iconSz.cx = tempSz.cx;
			iconSz.cy = tempSz.cy;
		}
	}

	hIco = m_hIconDisabled;

	if (hIco != NULL)
	{
		GetIconInfo(hIco, &icInfo);

		tempSz.cx = icInfo.xHotspot * 2;
		tempSz.cy = icInfo.yHotspot * 2;

		if (tempSz.cx > iconSz.cx)
		{
			iconSz.cx = tempSz.cx;
			iconSz.cy = tempSz.cy;
		}
	}

	return iconSz;
}

SIZE CHyperLink::GetFirstIcon()
{
	HICON    hIco   = NULL;
	ICONINFO icInfo = { 0 };
	SIZE     iconSz = { 0 };

	if ((!m_bDisabled &&
		  ((m_uShowIcon & SI_ICONUP_LEFT && !m_bMouseIn)      || 
		  (m_uShowIcon & SI_ICONHOVER_LEFT && m_bMouseIn && !m_bMouseDown)     || 
		  (m_uShowIcon & SI_ICONDOWN_LEFT && m_bMouseDown && m_bMouseIn)    || 
		  (m_uShowIcon & SI_ICONUP_RIGHT && !m_bMouseIn)      || 
		  (m_uShowIcon & SI_ICONHOVER_RIGHT && m_bMouseIn && !m_bMouseDown)    || 
		  (m_uShowIcon & SI_ICONDOWN_RIGHT && m_bMouseDown && m_bMouseIn))
		) ||
		(m_uShowIcon & SI_ICONDISABLED_LEFT && m_bDisabled) || 
		(m_uShowIcon & SI_ICONDISABLED_RIGHT && m_bDisabled))
	{
		if (m_bDisabled)
		{
			if (m_hIconDisabled == NULL && m_bAutoGenerateDisabled)
			{
				HICON hico = m_hIconUp;

				if (hico == NULL)
				{
					hico = m_hIconHover;
				}

				if (hico == NULL)
				{
					hico = m_hIconDown;
				}

				if (hico != NULL)
				{
					m_hIconDisabled  = CreateGrayscaleIcon(hico);
					m_bAutoGenerated = TRUE;
				}
			}

			hIco = m_hIconDisabled;
		}
		else if (!m_bDisabled && m_bMouseIn && !m_bMouseDown)
		{
			hIco = m_hIconHover;
		}
		else if (!m_bDisabled && m_bMouseIn && m_bMouseDown)
		{
			hIco = m_hIconDown;
		}
		else if (!m_bDisabled && !m_bMouseIn)
		{
			hIco = m_hIconUp;
		}

		if (hIco != NULL)
		{
			GetIconInfo(hIco, &icInfo);

			iconSz.cx = icInfo.xHotspot*2;
			iconSz.cy = icInfo.yHotspot*2;
		}
	}

	return iconSz;
}

void CHyperLink::CreatePalette()
{
	for(int i = 0; i < 256; i++)
	{
		m_dwszGrayScalePalette[i] = RGB(255-i, 255-i, 255-i);
	}
}

HICON CHyperLink::CreateGrayscaleIcon( HICON hIcon)
{
	if (hIcon == NULL)
	{
		return NULL;
	}

	HDC hdc = ::GetDC(NULL);

	HICON      hGrayIcon      = NULL;
	ICONINFO   icInfo         = { 0 };
	ICONINFO   icGrayInfo     = { 0 };
	LPDWORD    lpBits         = NULL;
	LPBYTE     lpBitsPtr      = NULL;
	SIZE sz;
	DWORD c1 = 0;
	BITMAPINFO bmpInfo        = { 0 };
	bmpInfo.bmiHeader.biSize  = sizeof(BITMAPINFOHEADER);

	if (::GetIconInfo(hIcon, &icInfo))
	{
		if (GetDIBits(hdc, icInfo.hbmColor, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS) != 0)
		{
			bmpInfo.bmiHeader.biCompression = BI_RGB;

			sz.cx = bmpInfo.bmiHeader.biWidth;
			sz.cy = bmpInfo.bmiHeader.biHeight;
			c1 = sz.cx * sz.cy;

			lpBits = (LPDWORD)GlobalAlloc(GMEM_FIXED, (c1) * 4);

			if (lpBits && GetDIBits(hdc, icInfo.hbmColor, 0, sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS) != 0)
			{
				lpBitsPtr     = (LPBYTE)lpBits;
				UINT off      = 0;

				for (UINT i = 0; i < c1; i++)
				{
					off = (UINT)( 255 - (( lpBitsPtr[0] + lpBitsPtr[1] + lpBitsPtr[2] ) / 3) );

					if (lpBitsPtr[3] != 0 || off != 255)
					{
						if (off == 0)
						{
							off = 1;
						}

						lpBits[i] = m_dwszGrayScalePalette[off] | ( lpBitsPtr[3] << 24 );
					}

					lpBitsPtr += 4;
				}

				icGrayInfo.hbmColor = CreateCompatibleBitmap(hdc, sz.cx, sz.cy);

				if (icGrayInfo.hbmColor != NULL)
				{
					SetDIBits(hdc, icGrayInfo.hbmColor, 0, sz.cy, lpBits, &bmpInfo, DIB_RGB_COLORS);

					icGrayInfo.hbmMask = icInfo.hbmMask;
					icGrayInfo.fIcon   = TRUE;

					hGrayIcon = CreateIconIndirect(&icGrayInfo);

					::DeleteObject(icGrayInfo.hbmColor);
				}

				GlobalFree(lpBits);
				lpBits = NULL;
			}
		}

		::DeleteObject(icInfo.hbmColor);
		::DeleteObject(icInfo.hbmMask);
	}

	::ReleaseDC(NULL,hdc);

	return hGrayIcon;
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(FALSE);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(TRUE);
	}

	if (m_bMouseIn)
		return;

	TRACKMOUSEEVENT EventTrack;
	EventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
	EventTrack.dwFlags = TME_LEAVE;
	EventTrack.dwHoverTime = 0;
	EventTrack.hwndTrack = m_hWnd;
	TrackMouseEvent(&EventTrack);

	m_bMouseIn = TRUE;

	if (nFlags & MK_LBUTTON)
		m_bMouseDown = TRUE;
	else
		m_bMouseDown = FALSE;

	Invalidate();
}

LRESULT CHyperLink::OnMouseLeave(WPARAM, LPARAM)
{
	m_pToolTip->Pop();

	TRACKMOUSEEVENT EventTrack;
	EventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
	EventTrack.dwFlags = TME_CANCEL | TME_LEAVE;
	EventTrack.dwHoverTime = 0;
	EventTrack.hwndTrack = m_hWnd;
	TrackMouseEvent(&EventTrack);

	m_bMouseIn = FALSE;

	BOOL bRepaint = TRUE;
	RECT rect;
	POINT pt;

	GetClientRect(&rect);
	MapWindowPoints(NULL,(LPPOINT)&rect,2);
	GetCursorPos(&pt);

	if (PtInRect(&rect,pt) && (WindowFromPoint(pt) == this || WindowFromPoint(pt) == m_pToolTip))
	{
		bRepaint = FALSE;
		SetTimer(1001, 300, NULL);
	}

	if (!m_bDisabled)
	{
		ShowCursor(FALSE);
		::SetCursor(m_hReg);
		ShowCursor(TRUE);
	}

	if (bRepaint)
	{
		Invalidate();
	}

	return 0;
}

void CHyperLink::OnTimer( UINT_PTR nIDEvent )
{
	if (nIDEvent == 1001)
	{
		KillTimer(nIDEvent);

		if (m_bMouseDown)
		{
			return;
		}

		RECT rect;
		POINT pt;
		CWnd* wndInPt = NULL;

		GetClientRect(&rect);
		MapWindowPoints(NULL,(LPPOINT)&rect,2);
		GetCursorPos(&pt);

		if (!PtInRect(&rect,pt) || ((wndInPt = WindowFromPoint(pt)) != this && wndInPt != m_pToolTip))
		{
			m_bMouseIn = FALSE;
			Invalidate();
		}
	}
}

void CHyperLink::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(FALSE);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(TRUE);
	}

	m_bMouseDown = FALSE;

	POINT cPt;

	GetCursorPos(&cPt);

	if (cPt.x >= m_rcCapture.left &&
		cPt.x <= m_rcCapture.right &&
		cPt.y >= m_rcCapture.top &&
		cPt.y <= m_rcCapture.bottom)
	{
		m_bMouseIn = TRUE;
	}
	else
	{
		m_bMouseIn = FALSE;
	}

	ReleaseCapture();

	Invalidate();

	if (!m_bDisabled && m_bMouseIn)
	{
		OpenUrl();

		if (!m_bIsLink || (GetStyle() & SS_NOTIFY))
		{
			CWnd* pParent = GetParent();

			if (pParent != NULL)
			{
				WORD wID = GetDlgCtrlID();

				if (wID)
				{
					pParent->SendMessage(WM_COMMAND, MAKEWPARAM(wID,STN_CLICKED),(LPARAM)m_hWnd);
				}
			}
		}
	}
}

void CHyperLink::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisabled)
	{
		ShowCursor(FALSE);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(TRUE);
	}

	m_bMouseDown = TRUE;

	SetCapture();

	Invalidate();

	GetClientRect(&m_rcCapture);
	ClientToScreen(&m_rcCapture);
}

void CHyperLink::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_bDisabled && (!m_bIsLink || GetStyle() & SS_NOTIFY))
	{
		CStatic::OnLButtonDblClk(nFlags,point);
	}

	if (!m_bDisabled)
	{
		ShowCursor(FALSE);
		m_hReg = ::SetCursor(m_hHand);
		ShowCursor(TRUE);
	}

	m_bMouseDown = TRUE;

	SetCapture();

	Invalidate();

	GetClientRect(&m_rcCapture);
	ClientToScreen(&m_rcCapture);
}

void CHyperLink::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	ModifyStyle(SS_BITMAP | SS_ICON, SS_OWNERDRAW | SS_NOTIFY);

	if (GetStyle() & WS_DISABLED)
	{
		m_bDisabled = TRUE;
	}

	m_ToolTip.Create(this);
	m_ToolTip.AddTool(this);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 1200);
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 1600);
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE   || 
		pMsg->message == WM_LBUTTONDOWN || 
		pMsg->message == WM_LBUTTONUP   ||
		pMsg->message == WM_MOUSELEAVE)
	{
		if (m_pToolTip->m_hWnd)
		{
			m_pToolTip->RelayEvent(pMsg);
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CHyperLink::OnCaptureChanged( CWnd* pWnd )
{
}

void CHyperLink::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
}

LRESULT CHyperLink::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_ENABLE)
	{
		m_bDisabled  = !IsWindowEnabled();

		return TRUE;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
