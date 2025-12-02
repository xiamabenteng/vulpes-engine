#pragma once

#include "VulpesEngineParamData.h"

typedef struct tagAIENGINE_STORE_CAPTURE
{
	void* pCStoreCapture;
} AIENGINE_STORE_CAPTURE;

typedef list<AIENGINE_STORE_CAPTURE> AIENGINE_STORE_CAPTUR_LIST;
typedef AIENGINE_STORE_CAPTUR_LIST::iterator AIENGINE_STORE_CAPTUR_LIST_ITER;

class CVulpesEngineStoreCapture
{
public:
	CVulpesEngineStoreCapture(void* lpData);
	~CVulpesEngineStoreCapture();
	void putVal(int id, VPE_PARAM_VALUE* pParamValue, double dQPC);
	VPE_PARAM_VALUE* GetStoreCaptureParamValue(VPE_PARAM_VALUE* pParamValue, int id, double t);
	int GetStoreCaptureParamData(int id, double t, double *pt, VPE_PARAM_VALUE* pParamValue);
	void StoreStart();

public:
	void* m_pEngineProc;
	CRITICAL_SECTION m_csResult;
	double m_dQPC;						//QueryPerformanceCounter
	double m_dQPCData[2];				//QueryPerformanceCounter
	AIENGINE_STORE_CAPTURE m_sVulpesEngineCapture;
	int m_nStoreCaptureStart;
	CVulpesEngineParamData m_sVulpesEngineParamData[PARAM_CNT];
	AIENGINE_STORE_CAPTUR_LIST gVulpesEngineCaptureList;
};