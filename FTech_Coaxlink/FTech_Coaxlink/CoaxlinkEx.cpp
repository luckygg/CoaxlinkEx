#include "StdAfx.h"
#include "CoaxlinkEx.h"
#include <iomanip>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CCoaxlinkEx::GetNumberOfInterfaces(int &nValue)
{
	TL_HANDLE hTl = NULL;
	
	try
	{
		hTl = m_GenTL.tlOpen();
		m_GenTL.tlUpdateInterfaceList(hTl);
		uint32_t nInterfaces = m_GenTL.tlGetNumInterfaces(hTl);
		m_GenTL.tlClose(hTl);
		nValue = nInterfaces;

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		
		if (hTl != NULL) m_GenTL.tlClose(hTl);
		return false;
	}
}

bool CCoaxlinkEx::GetNumberOfDevices(int nIfIdx, int &nValue)
{
	TL_HANDLE hTl = NULL;
	IF_HANDLE hIf = NULL;
	
	try
	{
		hTl = m_GenTL.tlOpen();
		m_GenTL.tlUpdateInterfaceList(hTl);
		
		hIf = m_GenTL.tlOpenInterface(hTl, m_GenTL.tlGetInterfaceID(hTl, nIfIdx));
		
		m_GenTL.ifUpdateDeviceList(hIf);
		uint32_t nDevices = m_GenTL.ifGetNumDevices(hIf);

		nValue = nDevices;

		m_GenTL.ifClose(hIf);
		m_GenTL.tlClose(hTl);

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());

		if (hIf != NULL) m_GenTL.ifClose(hIf);
		if (hTl != NULL) m_GenTL.tlClose(hTl);
		return false;
	}
}

bool CCoaxlinkEx::GetInterfaceName(int nIfIdx, CString &strValue)
{
	TL_HANDLE hTl = NULL;
	
	try
	{
		hTl = m_GenTL.tlOpen();
		m_GenTL.tlUpdateInterfaceList(hTl);
		std::string name = m_GenTL.tlGetInterfaceID(hTl, nIfIdx);

		m_GenTL.tlClose(hTl);

		strValue = (CString)name.c_str();
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());

		if (hTl != NULL) m_GenTL.tlClose(hTl);
		return false;
	}
}

bool CCoaxlinkEx::GetDeviceName(int nIfIdx, int nDvIdx, CString &strValue)
{
	TL_HANDLE hTl = NULL;
	IF_HANDLE hIf = NULL;
	
	try
	{
		hTl = m_GenTL.tlOpen();
		m_GenTL.tlUpdateInterfaceList(hTl);
		
		hIf = m_GenTL.tlOpenInterface(hTl, m_GenTL.tlGetInterfaceID(hTl, nIfIdx));
		m_GenTL.ifUpdateDeviceList(hIf);
		
		std::string name = m_GenTL.ifGetDeviceID(hIf, nDvIdx);

		m_GenTL.ifClose(hIf);
		m_GenTL.tlClose(hTl);

		strValue = (CString)name.c_str();
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());

		if (hIf != NULL) m_GenTL.ifClose(hIf);
		if (hTl != NULL) m_GenTL.tlClose(hTl);
		return false;
	}
}

bool CCoaxlinkEx::GetRemoteDeviceName(int nIfIdx, int nDvIdx, CString &strValue)
{
	TL_HANDLE hTl = NULL;
	IF_HANDLE hIf = NULL;
	DEV_HANDLE hDv = NULL;
	PORT_HANDLE hPt = NULL;
	
	try
	{
		hTl = m_GenTL.tlOpen();
		m_GenTL.tlUpdateInterfaceList(hTl);

		hIf = m_GenTL.tlOpenInterface(hTl, m_GenTL.tlGetInterfaceID(hTl, nIfIdx));
		m_GenTL.ifUpdateDeviceList(hIf);

		hDv = m_GenTL.ifOpenDevice(hIf, m_GenTL.ifGetDeviceID(hIf, nDvIdx));
		
		hPt = m_GenTL.devGetPort(hDv);
		std::string name = m_GenTL.gcGetPortInfo<std::string>(hPt, PORT_INFO_MODEL);

		m_GenTL.devClose(hDv);
		m_GenTL.ifClose(hIf);
		m_GenTL.tlClose(hTl);

		strValue = (CString)name.c_str();
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());

		if (hDv != NULL) m_GenTL.devClose(hDv);
		if (hIf != NULL) m_GenTL.ifClose(hIf);
		if (hTl != NULL) m_GenTL.tlClose(hTl);
		return false;
	}
}

//SharedGenTL CCoaxlinkEx::m_GenTL;
GenTL CCoaxlinkEx::m_GenTL;
CCoaxlinkEx::CCoaxlinkEx(int nIf, int nDv)
	: EGrabberMultiThread(m_GenTL,nIf,nDv,0)
{
	m_hGrabDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pbyBuffer = NULL;

	//reallocBuffers(EURESYS_BUFFER_COUNT);
}


CCoaxlinkEx::~CCoaxlinkEx(void)
{
	CloseHandle(m_hGrabDone);

	if (m_pbyBuffer != NULL)
	{
		delete []m_pbyBuffer;
		m_pbyBuffer = NULL;
	}
}

void CCoaxlinkEx::onNewBufferEvent(const NewBufferData& data)
{
	//ScopedBuffer는 소멸할 때 'push'를 하기 때문에 queue에 연속적으로 데이터가 들어온다.
	//Buffer는 그렇지 않기 때문에 'push'를 따로 호출해야 한다.
	
	ScopedBuffer buffer(*this, data);
	//Buffer buffer(data);
	size_t size        = buffer.getInfo<size_t>(gc::BUFFER_INFO_SIZE_FILLED);
	uint64_t timestamp = buffer.getInfo<uint64_t>(gc::BUFFER_INFO_TIMESTAMP);

	memento ("onNewBufferEvent: "" timestamp=" + formatTimestamp(timestamp));

	uint8_t *ptrImg = buffer.getInfo<uint8_t *>(gc::BUFFER_INFO_BASE);

	memcpy(m_pbyBuffer, ptrImg, size);
	//buffer.push(*this);
	SetEvent(m_hGrabDone);
}

void CCoaxlinkEx::onCicEvent(const CicData& data) 
{
	switch (data.numid) 
	{
	case gc::Euresys::EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_RISING_EDGE:
		memento("onCicEvent: Camera trigger rising edge: timestamp=" + formatTimestamp(data.timestamp));
		break;
	case gc::Euresys::EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_FALLING_EDGE:
		memento("onCicEvent: Camera trigger falling edge: timestamp=" +formatTimestamp(data.timestamp));
		break;
	case gc::Euresys::EVENT_DATA_NUMID_CIC_ALLOW_NEXT_CYCLE:
		memento("onCicEvent: Allow next cycle: timestamp=" + formatTimestamp(data.timestamp));
		//execute<DeviceModule>("StartCycle");
		break;
	default:
		break;
	}
}

std::string CCoaxlinkEx::formatTimestamp(uint64_t timestamp)
{
	std::stringstream ss;
	ss << std::setw(0) << (timestamp / 1000000) << "."
		<< std::setw(6) << std::setfill('0') << (timestamp % 1000000);
	return ss.str();
}

bool CCoaxlinkEx::OnStartAcquisition(uint64_t nCount)
{
	try
	{
		bool ret=false;
		int64_t w=0, h=0, bpp=0;
		ret = GetWidth(w);
		if (ret == false) return false;
		ret = GetHeight(h);
		if (ret == false) return false;
		ret = GetBpp(bpp);
		if (ret == false) return false;

		if (m_pbyBuffer != NULL)
		{
			delete []m_pbyBuffer;
			m_pbyBuffer = NULL;
		}

		m_pbyBuffer = new BYTE[w*h*bpp/8];
		ZeroMemory(m_pbyBuffer, w*h*bpp/8);

		// revoke current buffers (if any)
		// allocate bufferCount buffers (if bufferSize is zero, the size is determined automatically)
		// announce the new buffers to the data stream
		// queue the new buffers to the data stream input fifo
		reallocBuffers(EURESYS_BUFFER_COUNT);

		enableEvent<CicData>();

		start(nCount);

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::OnStopAcquisition()
{
	try
	{
		stop();

		flushEvent<CicData>();

		//discard pending buffers (if any)
		//queue all buffers to the data stream input fifo, in the initial order 
		//(i.e., the order in which they were announced with reallocBuffers or announceBuffer)
		resetBufferQueue();

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::SetContinuousMode()
{
	return SetValueString(EDevice, "CameraControlMethod", _T("NC"));
}

bool CCoaxlinkEx::SetSoftTriggerMode()
{
	bool ret=false;
	ret = SetValueString(EDevice, "CameraControlMethod", _T("RG"));
	if (ret == false) return false;
	ret = SetValueString(EDevice, "ExposureReadoutOverlap", _T("TRUE"));
	if (ret == false) return false;
	ret = SetValueString(EDevice, "CycleTriggerSource", _T("StartCycle"));
	if (ret == false) return false;

	return true;
}

bool CCoaxlinkEx::SetImmediateMode()
{
	bool ret=false;
	ret = SetValueString(EDevice, "CameraControlMethod", _T("RG"));
	if (ret == false) return false;
	ret = SetValueString(EDevice, "ExposureReadoutOverlap", _T("TRUE"));
	if (ret == false) return false;
	ret = SetValueString(EDevice, "CycleTriggerSource", _T("Immediate"));
	if (ret == false) return false;

	return true;
}

bool CCoaxlinkEx::OnTriggerEvent()
{
	return ExecuteCommand(EDevice, "StartCycle");
}

bool CCoaxlinkEx::GetWidth(int64_t &nValue)
{
	return GetValueInt(EStreamModule, "Width", nValue);
}

bool CCoaxlinkEx::GetHeight(int64_t &nValue)
{
	return GetValueInt(EStreamModule, "Height", nValue);
}

bool CCoaxlinkEx::SetWidth(int64_t nValue)
{
	return SetValueInt(ERemoteDevice, "Width", nValue);
}

bool CCoaxlinkEx::SetHeight(int64_t nValue)
{
	return SetValueInt(ERemoteDevice, "Height", nValue);
}

bool CCoaxlinkEx::GetBpp(int64_t &nValue)
{
	return GetValueInt(EStreamModule, "PixelSize", nValue);
}

bool CCoaxlinkEx::GetValueStringCamera(CString strNodeName, CString &strValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = GetValueString(ERemoteDevice, pNode, strValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::SetValueStringCamera(CString strNodeName, CString strValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = SetValueString(ERemoteDevice, pNode, strValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::GetValueIntCamera(CString strNodeName, int64_t &nValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = GetValueInt(ERemoteDevice, pNode, nValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::SetValueIntCamera(CString strNodeName, int64_t nValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = SetValueInt(ERemoteDevice, pNode, nValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::GetValueDoubleCamera(CString strNodeName, double &dValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = GetValueDouble(ERemoteDevice, pNode, dValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::SetValueDoubleCamera(CString strNodeName, double dValue)
{
	size_t szCvt = 0;
	char* pNode = new char[strNodeName.GetLength()+1];
	ZeroMemory(pNode, strNodeName.GetLength()+1);

	wcstombs_s(&szCvt, pNode, strNodeName.GetLength()+1, strNodeName, _TRUNCATE);

	bool ret = SetValueDouble(ERemoteDevice, pNode, dValue);

	delete []pNode;
	pNode = NULL;

	return ret;
}

bool CCoaxlinkEx::GetValueString(EModule eModule, char* pNodeName, CString &strValue)
{
	try
	{
		std::string value="";
		switch (eModule)
		{
		case EInterface :
			value = getString<InterfaceModule>(pNodeName);
			break;
		case EDevice :
			value = getString<DeviceModule>(pNodeName);
			break;
		case ERemoteDevice:
			value = getString<RemoteModule>(pNodeName);
			break;
		case EStreamModule:
			value = getString<StreamModule>(pNodeName);
			break;
		}

		strValue = (CString)value.c_str();

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::SetValueString(EModule eModule, char* pNodeName, CString strValue)
{
	try
	{
		ATL::CT2CA pszStr(strValue);
		std::string value(pszStr); 

		switch (eModule)
		{
		case EInterface :
			setString<InterfaceModule>(pNodeName,value);
			break;
		case EDevice :
			setString<DeviceModule>(pNodeName,value);
			break;
		case ERemoteDevice:
			setString<RemoteModule>(pNodeName,value);
			break;
		case EStreamModule:
			setString<StreamModule>(pNodeName,value);
			break;
		}

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::GetValueInt(EModule eModule, char* pNodeName, int64_t &nValue)
{
	try
	{
		int64_t value = 0;
		switch (eModule)
		{
		case EInterface :
			value = getInteger<InterfaceModule>(pNodeName);
			break;
		case EDevice :
			value = getInteger<DeviceModule>(pNodeName);
			break;
		case ERemoteDevice:
			value = getInteger<RemoteModule>(pNodeName);
			break;
		case EStreamModule:
			value = getInteger<StreamModule>(pNodeName);
			break;
		}

		nValue = value;
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::SetValueInt(EModule eModule, char* pNodeName, int64_t nValue)
{
	try
	{
		switch (eModule)
		{
		case EInterface :
			setInteger<InterfaceModule>(pNodeName,nValue);
			break;
		case EDevice :
			setInteger<DeviceModule>(pNodeName,nValue);
			break;
		case ERemoteDevice:
			setInteger<RemoteModule>(pNodeName,nValue);
			break;
		case EStreamModule:
			setInteger<StreamModule>(pNodeName, nValue);
			break;
		}
			
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::GetValueDouble(EModule eModule, char* pNodeName, double &dValue)
{
	try
	{
		double value = 0;
		
		switch (eModule)
		{
		case EInterface :
			value = getFloat<InterfaceModule>(pNodeName);
			break;
		case EDevice :
			value = getFloat<DeviceModule>(pNodeName);
			break;
		case ERemoteDevice:
			value = getFloat<RemoteModule>(pNodeName);
			break;
		case EStreamModule:
			value = getFloat<StreamModule>(pNodeName);
			break;
		}
			
		dValue = value;
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::SetValueDouble(EModule eModule, char* pNodeName, double dValue)
{
	try
	{
		switch (eModule)
		{
		case EInterface :
			setFloat<InterfaceModule>(pNodeName,dValue);
			break;
		case EDevice :
			setFloat<DeviceModule>(pNodeName,dValue);
			break;
		case ERemoteDevice:
			setFloat<RemoteModule>(pNodeName,dValue);
			break;
		case EStreamModule:
			setFloat<StreamModule>(pNodeName,dValue);
			break;
		}

		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}

bool CCoaxlinkEx::ExecuteCommand(EModule eModule, char* pNodeName)
{
	try
	{
		switch (eModule)
		{
		case EInterface :
			execute<InterfaceModule>(pNodeName);
			break;
		case EDevice :
			execute<DeviceModule>(pNodeName);
			break;
		case ERemoteDevice:
			execute<RemoteModule>(pNodeName);
			break;
		case EStreamModule:
			execute<StreamModule>(pNodeName);
			break;
		}
			
		return true;
	}
	catch (const std::exception &e)
	{
		m_GenTL.memento(e.what());
		return false;
	}
}