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
	/// \brief					CoaxlinkEx 생성자.
	/// \param [in] nIf			Interface Index 입력.
	/// \param [in] nDv			Device Index 입력.
	CCoaxlinkEx(int nIf, int nDv);
	~CCoaxlinkEx(void);

public :
	//******************************************************************************************************************
	/// \brief					Interface 확인 함수.
	/// \param [out] nValue		Interface 수 확인.
	/// \param bool				결과 반환.
	static bool GetNumberOfInterfaces(int &nValue);
	//******************************************************************************************************************
	/// \brief					Device 확인 함수.
	/// \param [out] nValue		Device 수 확인.
	/// \param bool				결과 반환.
	static bool GetNumberOfDevices(int nIfIdx, int &nValue);
	//******************************************************************************************************************
	/// \brief					Interface 이름 확인 함수.
	/// \param [in] nIfIdx		Interface Index 설정.
	/// \param [out] strValue	Interface Name 확인.
	/// \param bool				결과 반환.
	static bool GetInterfaceName(int nIfIdx, CString &strValue);
	//******************************************************************************************************************
	/// \brief					Device 이름 확인 함수.
	/// \param [in] nIfIdx		Interface Index 설정.
	/// \param [in] nDvIdx		Device Index 설정.
	/// \param [out] strValue	Device Name 확인.
	/// \param bool				결과 반환.
	static bool GetDeviceName(int nIfIdx, int nDvIdx, CString &strValue);
	//******************************************************************************************************************
	/// \brief					Remote Device 이름 확인 함수.
	/// \param [in] nIfIdx		Interface Index 설정.
	/// \param [in] nDvIdx		Device Index 설정.
	/// \param [out] strValue	Remote Device Name 확인.
	/// \param bool				결과 반환.
	static bool GetRemoteDeviceName(int nIfIdx, int nDvIdx, CString &strValue);

public :
	//******************************************************************************************************************
	/// \brief					영상 취득 시작 함수.
	/// \param [in] nCount		취득 장 수(Frame Count) 설정.
	/// \param bool				결과 반환.
	bool OnStartAcquisition(uint64_t nCount=GENTL_INFINITE);
	//******************************************************************************************************************
	/// \brief					영상 취득 종료 함수.
	/// \param bool				결과 반환.
	bool OnStopAcquisition();
	//******************************************************************************************************************
	/// \brief					Continuous Mode 설정 함수.
	/// \param bool				결과 반환.
	bool SetContinuousMode();
	//******************************************************************************************************************
	/// \brief					Immediate Trigger Mode 설정 함수.
	/// \param bool				결과 반환.
	bool SetImmediateMode();
	//******************************************************************************************************************
	/// \brief					Software Trigger Mode 설정 함수.
	/// \param bool				결과 반환.
	bool SetSoftTriggerMode();
	//******************************************************************************************************************
	/// \brief					Hardware Trigger Mode 설정 함수.
	/// \param bool				결과 반환.
	bool SetHardTriggerMode();
	//******************************************************************************************************************
	/// \brief					Software Trigger Event 발생 함수.
	/// \param bool				결과 반환.
	bool OnTriggerEvent();
	//******************************************************************************************************************
	/// \brief					가로 해상도 획득 함수. (by Stream Module)
	/// \param [out] nValue		가로 해상도 확인.
	/// \param bool				결과 반환.
	bool GetWidth(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					세로 해상도 획득 함수. (by Stream Module)
	/// \param [out] nValue		세로 해상도 확인.
	/// \param bool				결과 반환.
	bool GetHeight(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					가로 해상도 설정 함수. (to Device Module)
	/// \param [in] nValue		가로 해상도 입력.
	/// \param bool				결과 반환.
	bool SetWidth(int64_t nValue);
	//******************************************************************************************************************
	/// \brief					세로 해상도 설정 함수. (to Device Module)
	/// \param [in] nValue		세로 해상도 입력.
	/// \param bool				결과 반환.
	bool SetHeight(int64_t nValue);
	//******************************************************************************************************************
	/// \brief					픽셀 포맷 획득 함수. (by Stream Module)
	/// \param [out] nValue		픽셀 포맷 확인.
	/// \param bool				결과 반환.
	bool GetBpp(int64_t &nValue);
	//******************************************************************************************************************
	/// \brief					이미지 데이터 획득 함수.
	/// \param bool				이미지 데이터 포인터 반환.
	BYTE* GetImageBuffer() { return m_pbyBuffer; }
	//******************************************************************************************************************
	/// \brief					이미지 획득이 완료됐는지 확인하는 Handle 반환 함수.
	/// \param bool				핸들 반환.
	HANDLE GetHandleGrabDone() { return m_hGrabDone; }
	//******************************************************************************************************************
	/// \brief					이미지 획득이 완료됐는지 확인하는 Handle을 초기화(Reset)시키는 함수.
	void ResetHandleGrabDone() { ResetEvent(m_hGrabDone); }

public :
	//******************************************************************************************************************
	/// \brief					Remote Device의 파라미터 값을 Type별로 얻는 함수.
	/// \param [in] strNodeName	파라미터의 Node Name 입력.
	/// \param [out] Value		파라미터의 값 확인.
	/// \param bool				결과 반환.
	bool GetValueStringCamera(CString strNodeName, CString &strValue);
	bool SetValueStringCamera(CString strNodeName, CString strValue);
	bool GetValueIntCamera(CString strNodeName, int64_t &nValue);
	bool SetValueIntCamera(CString strNodeName, int64_t nValue);
	bool GetValueDoubleCamera(CString strNodeName, double &dValue);
	bool SetValueDoubleCamera(CString strNodeName, double dValue);

private :
	//******************************************************************************************************************
	/// \brief					Module별로 파라미터 값을 Type별로 얻는 함수.
	/// \param [in] eModule		Interface / Device / Remote Device / Stream Moudle 입력.
	/// \param [in] pNodeName	파라미터의 Node Name 입력.
	/// \param [out] Value		파라미터의 값 확인.
	/// \param bool				결과 반환.
	bool GetValueString(EModule eModule, char* pNodeName, CString &strValue);
	bool SetValueString(EModule eModule, char* pNodeName, CString strValue);
	bool GetValueInt(EModule eModule, char* pNodeName, int64_t &nValue);
	bool SetValueInt(EModule eModule, char* pNodeName, int64_t nValue);
	bool GetValueDouble(EModule eModule, char* pNodeName, double &dValue);
	bool SetValueDouble(EModule eModule, char* pNodeName, double dValue);
	bool ExecuteCommand(EModule eModule, char* pNodeName);

private :
	HANDLE m_hGrabDone;	// onNewBufferEvent (이미지 획득 Callback event) 호출 시 SetEvent 되는 핸들.
	BYTE* m_pbyBuffer;	// onNewBufferEvent 에서 image data를 copy하는 메모리.

	//static SharedGenTL m_GenTL;
	static GenTL m_GenTL; // Coaxlink 핸들.

	virtual void onNewBufferEvent(const NewBufferData& data);
	virtual void onCicEvent(const CicData& data);

	static std::string formatTimestamp(uint64_t timestamp);
};