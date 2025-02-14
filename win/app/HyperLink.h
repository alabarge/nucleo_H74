#pragma once


// CHyperLink

class CHyperLink : public CStatic
{
	DECLARE_DYNAMIC(CHyperLink)

private:
	TCHAR* m_lpLinkText;
	TCHAR* m_lpLinkUrl;

	BOOL m_bUnderlineAlways;
	BOOL m_bIsLink;
	BOOL m_bLockInPosition;
	BOOL m_bMultiline;

	CFont m_hFont;

	COLORREF m_crLinkUp;
	COLORREF m_crLinkHover;
	COLORREF m_crLinkDown;
	//BackGround color
	COLORREF m_crBackGround;

	COLORREF m_crBorder;
	int      m_nBorderSize;
	int      m_nBorderStyle;

	HCURSOR m_hReg;
	HCURSOR m_hHand;

	HICON  m_hIconUp;
	HICON  m_hIconHover;
	HICON  m_hIconDown;
	HICON  m_hIconDisabled;
	UINT   m_uShowIcon;
	BOOL   m_bAutoGenerateDisabled;
	BOOL   m_bAutoGenerated;
	enum {
		SI_ICONUP_ON    = 0x0001, //Show icon when mouse Up (normal)
		SI_ICONUP_LEFT  = 0x0002, //Show icon when mouse up on the left
		SI_ICONUP_RIGHT = 0x0004, //Show icon when mouse up on the right
		//
		SI_ICONHOVER_ON    = 0x0010, //Show icon when mouse hover
		SI_ICONHOVER_LEFT  = 0x0020, //Show icon when mouse hover on the left
		SI_ICONHOVER_RIGHT = 0x0040, //Show icon when mouse hover on the right
		//
		SI_ICONDOWN_ON    = 0x0100, //Show icon when mouse down
		SI_ICONDOWN_LEFT  = 0x0200, //Show icon when mouse down on the left
		SI_ICONDOWN_RIGHT = 0x0400, //Show icon when mouse down on the right
		//
		SI_ICONDISABLED_ON    = 0x1000, //Show icon when hyperlink is disabled
		SI_ICONDISABLED_LEFT  = 0x2000, //Show icon when hyperlink is disabled on the left
		SI_ICONDISABLED_RIGHT = 0x4000  //Show icon when hyperlink is disabled on the right
	};

	CToolTipCtrl m_ToolTip;
	CToolTipCtrl* m_pToolTip;

private:
	//Status and Mouse control variables
	BOOL m_bDisabled;

	BOOL m_bMouseIn;
	BOOL m_bMouseDown;

	BOOL m_bMovingWindow;

	RECT m_rcCapture;

	COLORREF m_dwszGrayScalePalette[256];

private:
	void Initialize();

	int OpenUrl();

	SIZE GetBiggestIcon();
	SIZE GetFirstIcon();

	void CreatePalette();
	HICON CreateGrayscaleIcon(HICON hIcon);

public:
	TCHAR* GetLinkText();
	void SetLinkText(TCHAR* lpText);

	TCHAR* GetLinkUrl();
	void SetLinkUrl(TCHAR* lpUrl);

	void LockInPosition(BOOL bLockInPosition = FALSE);

	BOOL IsMultiline() { return m_bMultiline; }
	void SetMultiline(BOOL bMultiline);

	void IsLink(BOOL bIsLink = TRUE);

	void UnderlineAlways(BOOL bUnderlineAlways = FALSE);

	COLORREF GetUpColor() { return m_crLinkUp; }
	COLORREF GetHoverColor() { return m_crLinkHover; }
	COLORREF GetDownColor() { return m_crLinkDown; }
	COLORREF GetBackgroundColor() { return m_crBackGround; }
	void SetColors(COLORREF crLinkUp, COLORREF crLinkHover, COLORREF crLinkDown, COLORREF crBackGround = ::GetSysColor(COLOR_BTNFACE));
	void SetBackgroundColor(COLORREF crBackGround = ::GetSysColor(COLOR_BTNFACE));
	COLORREF GetBorderColor() { return m_crBorder; }
	void SetBorderColor(COLORREF crBorder = RGB(0,0,0));
	int GetBorderSize() { return m_nBorderSize; }
	void SetBorderSize(int nSize);
	int GetBorderStyle() { return m_nBorderStyle; }
	void SetBorderStyle(int nStyle);

	UINT GetShowIconsOpt() { return m_uShowIcon; }
	void SetIcons(HICON hIconUp, HICON hIconHover, HICON hIconDown, UINT uShowIcons, HICON hIconDisabled = NULL);
	void SetIconUp(int nIconUp, int dx = 0, int dy = 0);
	void SetIconHover(int nIconHover, int dx = 0, int dy = 0);
	void SetIconDown(int nIconDown, int dx = 0, int dy = 0);
	void SetIconDisabled(int nIconDisabled, int dx = 0, int dy = 0);
	void SetShowIcons(UINT uShowIcons);

	BOOL GetAutoGenerateDisabled() { return m_bAutoGenerateDisabled; }
	void SetAutoGenerateDisabled(BOOL bAutoGenerate);

	BOOL SetFont(TCHAR* lpFaceName = _T("Arial"), int nSize = 14, int nWeight = FW_NORMAL, BOOL bItalic = FALSE);
	BOOL SetFont(const LPLOGFONT lpFont);

	void Disable(BOOL bDisable = FALSE);

	CToolTipCtrl* GetTooltip();
	void SetTooltip(CToolTipCtrl* pToolTip);

public:
	CHyperLink();
	virtual ~CHyperLink();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void PreSubclassWindow();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


