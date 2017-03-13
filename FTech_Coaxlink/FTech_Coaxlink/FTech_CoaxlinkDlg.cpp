
// FTech_CoaxlinkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FTech_Coaxlink.h"
#include "FTech_CoaxlinkDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFTech_CoaxlinkDlg dialog


UINT DisplayThread(LPVOID param)
{
	CFTech_CoaxlinkDlg* pMain = (CFTech_CoaxlinkDlg*)param;

	DWORD dwResult=0;
	while(pMain->m_bThDsp)
	{
		dwResult = WaitForSingleObject(pMain->m_pGrabber->GetHandleGrabDone(), 1000);
		if (dwResult == WAIT_OBJECT_0)
		{
			pMain->UpdateDisplay();
			pMain->m_pGrabber->ResetHandleGrabDone();
		}
	}
	return 0;
}

CFTech_CoaxlinkDlg::CFTech_CoaxlinkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFTech_CoaxlinkDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pGrabber = NULL;
	m_pThDisplay = NULL;
	m_pBitmapInfo = NULL;
	m_bThDsp = false;

	m_nWidth	= 0;
	m_nHeight	= 0;
	m_nBpp		= 0;
}

void CFTech_CoaxlinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFTech_CoaxlinkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CFTech_CoaxlinkDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_CONNECTION1, &CFTech_CoaxlinkDlg::OnBnClickedBtnConnection1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ACQ1, &CFTech_CoaxlinkDlg::OnBnClickedBtnAcq1)
	ON_BN_CLICKED(IDC_RBTN_CONT, &CFTech_CoaxlinkDlg::OnBnClickedRbtnCont)
	ON_BN_CLICKED(IDC_RBTN_SOFT, &CFTech_CoaxlinkDlg::OnBnClickedRbtnSoft)
	ON_BN_CLICKED(IDC_RBTN_IMMEDIATE, &CFTech_CoaxlinkDlg::OnBnClickedRbtnImmediate)
	ON_BN_CLICKED(IDC_BTN_EVENT, &CFTech_CoaxlinkDlg::OnBnClickedBtnEvent)
END_MESSAGE_MAP()


// CFTech_CoaxlinkDlg message handlers

BOOL CFTech_CoaxlinkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LTC_LIST);
	pList->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	pList->DeleteAllItems();
	pList->InsertColumn(0, _T(""), LVCFMT_LEFT, 0, -1);
	pList->InsertColumn(1, _T("Interfaces"), LVCFMT_CENTER, 150, -1);
	pList->InsertColumn(2, _T("Index"), LVCFMT_CENTER, 40, -1);
	pList->InsertColumn(3, _T("Devices"), LVCFMT_CENTER, 60, -1); 
	pList->InsertColumn(4, _T("Index"), LVCFMT_CENTER, 40, -1);
	pList->InsertColumn(5, _T("Remote Devices"), LVCFMT_CENTER, 110, -1); 
	
	CheckDlgButton(IDC_RBTN_CONT,1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTech_CoaxlinkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFTech_CoaxlinkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFTech_CoaxlinkDlg::OnBnClickedBtnRefresh()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LTC_LIST);
	pList->DeleteAllItems();

	int interfaces=0;
	CString nameIf=_T(""), nameDv=_T(""), nameRDv=_T(""), nameStatus=_T("");

	bool ret=false;

	BeginWaitCursor();

	ret = CCoaxlinkEx::GetNumberOfInterfaces(interfaces);
	if (ret == true)
	{
		for (int i=0; i<interfaces; i++)
		{
			CCoaxlinkEx::GetInterfaceName(i,nameIf);

			int devices=0;
			ret = CCoaxlinkEx::GetNumberOfDevices(i,devices);
			if (ret == true)
			{
				for (int j=0; j<devices; j++)
				{
					CCoaxlinkEx::GetDeviceName(i,j,nameDv);
					CCoaxlinkEx::GetRemoteDeviceName(i,j,nameRDv);
					
					CString tmp=_T("");
					pList->InsertItem(i, _T(""));
					pList->SetItem(j, 1, LVIF_TEXT, nameIf, 0, 0, 0, NULL ); 
					tmp.Format(_T("%d"),i);
					pList->SetItem(j, 2, LVIF_TEXT, tmp, 0, 0, 0, NULL ); 
					pList->SetItem(j, 3, LVIF_TEXT, nameDv, 0, 0, 0, NULL ); 
					tmp.Format(_T("%d"),j);
					pList->SetItem(j, 4, LVIF_TEXT, tmp, 0, 0, 0, NULL ); 
					pList->SetItem(j, 5, LVIF_TEXT, nameRDv, 0, 0, 0, NULL ); 
				}
			}
		}
	}

	EndWaitCursor();
}


void CFTech_CoaxlinkDlg::OnBnClickedBtnConnection1()
{
	CString caption = _T("");
	GetDlgItemText(IDC_BTN_CONNECTION1, caption);
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LTC_LIST);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	int nItem=-1;
	bool ret = false;
	while (pos)
	{
		nItem = pList->GetNextSelectedItem(pos);
	}
	if (nItem == -1) return;

	if (caption == _T("Connect"))
	{
		CString strIf=_T(""),strDv=_T("");
		strIf = pList->GetItemText(nItem, 2);
		strDv = pList->GetItemText(nItem, 4);
		
		int nIf=0,nDv=0;
		nIf = _ttoi(strIf);
		nDv = _ttoi(strDv);
		
		if (m_pGrabber != NULL)
		{
			delete m_pGrabber;
			m_pGrabber = NULL;
		}

		m_pGrabber = new CCoaxlinkEx(nIf, nDv);
		if (m_pGrabber == NULL) return;

		CString strName=_T(""),strSN=_T("");
		m_pGrabber->GetValueStringCamera(_T("DeviceModelName"), strName);
		m_pGrabber->GetValueStringCamera(_T("DeviceSerialNumber"), strSN);

		
		m_pGrabber->GetWidth(m_nWidth);
		m_pGrabber->GetHeight(m_nHeight);
		m_pGrabber->GetBpp(m_nBpp);

		OnCreateBmpInfo(m_nWidth,m_nHeight,m_nBpp);

		SetDlgItemText(IDC_LB_MODEL1, strName);
		SetDlgItemText(IDC_LB_SN1, strSN);
		SetDlgItemText(IDC_BTN_CONNECTION1, _T("Disconnect"));
	}
	else
	{
		delete m_pGrabber;
		m_pGrabber = NULL;

		SetDlgItemText(IDC_LB_MODEL1, _T(""));
		SetDlgItemText(IDC_LB_SN1, _T(""));
		SetDlgItemText(IDC_BTN_CONNECTION1, _T("Connect"));
	}
}


void CFTech_CoaxlinkDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_bThDsp = false;
	if (m_pThDisplay != NULL)
		WaitForSingleObject(m_pThDisplay->m_hThread, 5000);

	if (m_pGrabber != NULL)
	{
		m_pGrabber->OnStopAcquisition();
	}

	if (m_pBitmapInfo != NULL)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (m_pGrabber != NULL)
	{
		delete m_pGrabber;
		m_pGrabber = NULL;
	}
}


void CFTech_CoaxlinkDlg::OnBnClickedBtnAcq1()
{
	CString caption = _T("");
	GetDlgItemText(IDC_BTN_ACQ1, caption);
	bool ret = false;

	if (caption == _T("Start"))
	{
		ret = m_pGrabber->OnStartAcquisition();
		if (ret == true)
		{
			m_bThDsp = true;

			m_pThDisplay = AfxBeginThread(DisplayThread, this);
			SetDlgItemText(IDC_BTN_ACQ1, _T("Stop"));
		}
	}
	else
	{
		m_bThDsp = false;
		WaitForSingleObject(m_pThDisplay->m_hThread, 5000);

		ret = m_pGrabber->OnStopAcquisition();
		if (ret == true)
			SetDlgItemText(IDC_BTN_ACQ1, _T("Start"));
	}
}


void CFTech_CoaxlinkDlg::OnBnClickedRbtnCont()
{
	bool ret=false;
	ret = m_pGrabber->SetContinuousMode();
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("TriggerMode"), _T("Off"));
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("ExposureMode"), _T("Timed"));
	if (ret == false) return;
}


void CFTech_CoaxlinkDlg::OnBnClickedRbtnSoft()
{
	bool ret=false;
	ret = m_pGrabber->SetSoftTriggerMode();
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("TriggerMode"), _T("On"));
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("TriggerSource"), _T("LinkTrigger0"));
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("ExposureMode"), _T("TriggerWidth"));
	if (ret == false) return;
}


void CFTech_CoaxlinkDlg::OnBnClickedRbtnImmediate()
{
	bool ret=false;
	ret = m_pGrabber->SetImmediateMode();
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("TriggerMode"), _T("On"));
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("TriggerSource"), _T("LinkTrigger0"));
	if (ret == false) return;
	ret = m_pGrabber->SetValueStringCamera(_T("ExposureMode"), _T("TriggerWidth"));
	if (ret == false) return;
}


void CFTech_CoaxlinkDlg::OnBnClickedBtnEvent()
{
	m_pGrabber->OnTriggerEvent();
}

void CFTech_CoaxlinkDlg::UpdateDisplay()
{
	CClientDC dc(GetDlgItem(IDC_VIEW_CAMERA1));

	CRect rect;
	GetDlgItem(IDC_VIEW_CAMERA1)->GetClientRect(&rect);

	dc.SetStretchBltMode(COLORONCOLOR); 

	BYTE *pBuffer = m_pGrabber->GetImageBuffer();
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_nWidth, m_nHeight, pBuffer, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

void CFTech_CoaxlinkDlg::OnCreateBmpInfo(int nWidth, int nHeight, int nBpp)
{
	if (m_pBitmapInfo != NULL)
	{
		delete []m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (nBpp == 8)
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];
	else if (nBpp == 24)
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = nBpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;

	if (nBpp == 8)
		m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	else if (nBpp == 24)
		m_pBitmapInfo->bmiHeader.biSizeImage = (((nWidth * 24 + 31) & ~31) >> 3) * nHeight;

	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (nBpp == 8)
	{
		for (int i = 0 ; i < 256 ; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = nWidth;
	m_pBitmapInfo->bmiHeader.biHeight = -nHeight;
}
