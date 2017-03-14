//----------------------------------------------------------
// Euresys Coaxlink Class
//----------------------------------------------------------
// Programmed by William Kim
//----------------------------------------------------------
// Last Update : 2017-03-14 13:13
// Modified by William Kim
//----------------------------------------------------------
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
public:
	//******************************************************************************************************************
	/// \brief					CoaxlinkEx ������.
	/// \param [in] nIf			Interface Index �Է�.
	/// \param [in] nDv			Device Index �Է�.
	CCoaxlinkEx(int nIf, int nDv);
	~CCoaxlinkEx(void);

public :
	//******************************************************************************************************************
	/// \brief					Interface Ȯ�� �Լ�.
	/// \param [out] nValue		Interface �� Ȯ��.
	/// \param bool				��� ��ȯ.
	static bool GetNumberOfInterfaces(int &nValue);
	//******************************************************************************************************************
	/// \brief					Device Ȯ�� �Լ�.
	/// \param [out] nValue		Device �� Ȯ��.
	/// \param bool				��� ��ȯ.
	static bool GetNumberOfDevices(int nIfIdx, int &nValue);
	//******************************************************************************************************************
	/// \brief					Interface �̸� Ȯ�� �Լ�.
	/// \param [in] nIfIdx		Interface Index ����.
	/// \param [out] strValue	Interface Name Ȯ��.
	/// \param bool				��� ��ȯ.
	static bool GetInterfaceName(int nIfIdx, CString &strValue);
	//******************************************************************************************************************
	/// \brief					Device �̸� Ȯ�� �Լ�.
	/// \param [in] nIfIdx		Interface Index ����.
	/// \param [in] nDvIdx		Device Index ����.
	/// \param [out] strValue	Device Name Ȯ��.
	/// \param bool				��� ��ȯ.
	static bool GetDeviceName(int nIfIdx, int nDvIdx, CString &strValue);
	//******************************************************************************************************************
	/// \brief					Remote Device �̸� Ȯ�� �Լ�.
	/// \param [in] nIfIdx		Interface Index ����.
	/// \param [in] nDvIdx		Device Index ����.
	/// \param [out] strValue	Remote Device Name Ȯ��.
	/// \param bool				��� ��ȯ.
	static bool GetRemoteDeviceName(int nIfIdx, int nDvIdx, CString &strValue);

public :
	//******************************************************************************************************************
	/// \brief					���� ��� ���� �Լ�.
	/// \param [in] nCount		��� �� ��(Frame Count) ����.
	/// \param bool				��� ��ȯ.
	bool OnStartAcquisition(uint64_t nCount=GENTL_INFINITE);
	//******************************************************************************************************************
	/// \brief					���� ��� ���� �Լ�.
	/// \param bool				��� ��ȯ.
	bool OnStopAcquisition();
	//******************************************************************************************************************
	/// \brief					Continuous Mode ���� �Լ�.
	/// \param bool				��� ��ȯ.
	bool SetContinuousMode();
	//******************************************************************************************************************
	/// \brief					Immediate Trigger Mode ���� �Լ�.
	/// \param bool				��� ��ȯ.
	bool SetImmediateMode();
	//******************************************************************************************************************
	/// \brief					Software Trigger Mode ���� �Լ�.
	/// \param bool				��� ��ȯ.
	bool SetSoftTriggerMode();
	//******************************************************************************************************************
	/// \brief					Hardware Trigger Mode ���� �Լ�.
	/// \param bool				��� ��ȯ.
	bool SetHardTriggerMode();
	//******************************************************************************************************************
	/// \brief					Software Trigger Event �߻� �Լ�.
	/// \param bool				��� ��ȯ.
	bool OnTriggerEvent();
	//******************************************************************************************************************
	/// \brief					���� �ػ� ȹ�� �Լ�. (by Stream Module)
	/// \param [out] nValue		���� �ػ� Ȯ��.
	/// \param bool				��� ��ȯ.
	bool GetWidth(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					���� �ػ� ȹ�� �Լ�. (by Stream Module)
	/// \param [out] nValue		���� �ػ� Ȯ��.
	/// \param bool				��� ��ȯ.
	bool GetHeight(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					���� �ػ� ���� �Լ�. (to Device Module)
	/// \param [in] nValue		���� �ػ� �Է�.
	/// \param bool				��� ��ȯ.
	bool SetWidth(int64_t nValue);
	//******************************************************************************************************************
	/// \brief					���� �ػ� ���� �Լ�. (to Device Module)
	/// \param [in] nValue		���� �ػ� �Է�.
	/// \param bool				��� ��ȯ.
	bool SetHeight(int64_t nValue);
	//******************************************************************************************************************
	/// \brief					�ȼ� ���� ȹ�� �Լ�. (by Stream Module)
	/// \param [out] nValue		�ȼ� ���� Ȯ��.
	/// \param bool				��� ��ȯ.
	bool GetBpp(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					�̹��� ������ ȹ�� �Լ�.
	/// \param bool				�̹��� ������ ������ ��ȯ.
	BYTE* GetImageBuffer() { return m_pbyBuffer; }
	//******************************************************************************************************************
	/// \brief					�̹��� ȹ���� �Ϸ�ƴ��� Ȯ���ϴ� Handle ��ȯ �Լ�.
	/// \param bool				�ڵ� ��ȯ.
	HANDLE GetHandleGrabDone() { return m_hGrabDone; }
	//******************************************************************************************************************
	/// \brief					�̹��� ȹ���� �Ϸ�ƴ��� Ȯ���ϴ� Handle�� �ʱ�ȭ(Reset)��Ű�� �Լ�.
	void ResetHandleGrabDone() { ResetEvent(m_hGrabDone); }

public :
	//******************************************************************************************************************
	/// \brief					Remote Device�� �Ķ���� ���� Type���� ��� �Լ�.
	/// \param [in] strNodeName	�Ķ������ Node Name �Է�.
	/// \param [out] Value		�Ķ������ �� Ȯ��.
	/// \param bool				��� ��ȯ.
	bool GetValueStringCamera(CString strNodeName, CString &strValue);
	bool SetValueStringCamera(CString strNodeName, CString strValue);
	bool GetValueIntCamera(CString strNodeName, int64_t &nValue);
	bool SetValueIntCamera(CString strNodeName, int64_t nValue);
	bool GetValueDoubleCamera(CString strNodeName, double &dValue);
	bool SetValueDoubleCamera(CString strNodeName, double dValue);

private :
	//******************************************************************************************************************
	/// \brief					Module���� �Ķ���� ���� Type���� ��� �Լ�.
	/// \param [in] eModule		Interface / Device / Remote Device / Stream Moudle �Է�.
	/// \param [in] pNodeName	�Ķ������ Node Name �Է�.
	/// \param [out] Value		�Ķ������ �� Ȯ��.
	/// \param bool				��� ��ȯ.
	bool GetValueString(EModule eModule, char* pNodeName, CString &strValue);
	bool SetValueString(EModule eModule, char* pNodeName, CString strValue);
	bool GetValueInt(EModule eModule, char* pNodeName, int64_t &nValue);
	bool SetValueInt(EModule eModule, char* pNodeName, int64_t nValue);
	bool GetValueDouble(EModule eModule, char* pNodeName, double &dValue);
	bool SetValueDouble(EModule eModule, char* pNodeName, double dValue);
	bool ExecuteCommand(EModule eModule, char* pNodeName);

private :
	HANDLE m_hGrabDone;	// onNewBufferEvent (�̹��� ȹ�� Callback event) ȣ�� �� SetEvent �Ǵ� �ڵ�.
	BYTE* m_pbyBuffer;	// onNewBufferEvent ���� image data�� copy�ϴ� �޸�.

	//static SharedGenTL m_GenTL;
	static GenTL m_GenTL; // Coaxlink �ڵ�.

	virtual void onNewBufferEvent(const NewBufferData& data);
	virtual void onCicEvent(const CicData& data);

	static std::string formatTimestamp(uint64_t timestamp);
};