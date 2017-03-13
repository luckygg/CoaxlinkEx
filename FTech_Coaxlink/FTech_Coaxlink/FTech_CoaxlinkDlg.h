
// FTech_CoaxlinkDlg.h : header file
//

#pragma once
#include "CoaxlinkEx.h"

// CFTech_CoaxlinkDlg dialog
class CFTech_CoaxlinkDlg : public CDialogEx
{
// Construction
public:
	CFTech_CoaxlinkDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FTECH_COAXLINK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public :
	CCoaxlinkEx*	m_pGrabber;
	CWinThread*		m_pThDisplay;
	BITMAPINFO*		m_pBitmapInfo;

	bool	m_bThDsp;
	int64_t m_nWidth;
	int64_t m_nHeight;
	int64_t m_nBpp;

	void UpdateDisplay();
	void OnCreateBmpInfo(int nWidth, int nHeight, int nBpp);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnConnection1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnAcq1();
	afx_msg void OnBnClickedRbtnCont();
	afx_msg void OnBnClickedRbtnSoft();
	afx_msg void OnBnClickedRbtnImmediate();
	afx_msg void OnBnClickedBtnEvent();
};
