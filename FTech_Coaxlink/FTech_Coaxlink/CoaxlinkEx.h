#pragma once

#define EURESYS_BUFFER_COUNT 20

#include <EGrabber.h>
#include <EuresysGenTL.h>
//#include <EuresysSharedGenTL.h>

using namespace Euresys;
using namespace GenICam::Client;

enum EModule{EInterface=0, EDevice, ERemoteDevice, EStreamModule};
typedef EGrabber<CallbackMultiThread> EGrabberMultiThread;

class CCoaxlinkEx : public EGrabberMultiThread
{
	//static SharedGenTL m_GenTL;
	static GenTL m_GenTL;

public :
	static bool GetNumberOfInterfaces(int &nValue);
	static bool GetNumberOfDevices(int nIfIdx, int &nValue);
	static bool GetInterfaceName(int nIfIdx, CString &strValue);
	static bool GetDeviceName(int nIfIdx, int nDvIdx, CString &strValue);
	static bool GetRemoteDeviceName(int nIfIdx, int nDvIdx, CString &strValue);

public :
	bool OnStartAcquisition(uint64_t nCount=GENTL_INFINITE);
	bool OnStopAcquisition();

	bool SetContinuousMode();
	bool SetImmediateMode();
	bool SetSoftTriggerMode();
	bool SetHardTriggerMode();
	bool OnTriggerEvent();

	bool GetWidth(int64_t &nValue);
	bool GetHeight(int64_t &nValue);
	bool GetBpp(int64_t &nValue);

	BYTE* GetImageBuffer() { return m_pbyBuffer; }

	HANDLE GetHandleGrabDone() { return m_hGrabDone; }
	void ResetHandleGrabDone() { ResetEvent(m_hGrabDone); }

public :
	bool GetValueStringCamera(CString strNodeName, CString &strValue);
	bool SetValueStringCamera(CString strNodeName, CString strValue);
	bool GetValueIntCamera(CString strNodeName, int64_t &nValue);
	bool SetValueIntCamera(CString strNodeName, int64_t nValue);
	bool GetValueDoubleCamera(CString strNodeName, double &dValue);
	bool SetValueDoubleCamera(CString strNodeName, double dValue);

private :
	bool GetValueString(EModule eModule, char* pNodeName, CString &strValue);
	bool SetValueString(EModule eModule, char* pNodeName, CString strValue);
	bool GetValueInt(EModule eModule, char* pNodeName, int64_t &nValue);
	bool SetValueInt(EModule eModule, char* pNodeName, int64_t nValue);
	bool GetValueDouble(EModule eModule, char* pNodeName, double &dValue);
	bool SetValueDouble(EModule eModule, char* pNodeName, double dValue);
	bool ExecuteCommand(EModule eModule, char* pNodeName);
public:
	CCoaxlinkEx(int nIf, int nDv);
	~CCoaxlinkEx(void);

	HANDLE m_hGrabDone;
	BYTE* m_pbyBuffer;

	//virtual void GetBufferDataPtr(const NewBufferData& data);
	virtual void onNewBufferEvent(const NewBufferData& data) ;
	virtual void onCicEvent(const CicData& data) ;

	static std::string formatTimestamp(uint64_t timestamp);
};