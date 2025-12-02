#include "stdafx.h"
#include "VulpesEngineCore.h"

CVulpesEngineStoreCapture::CVulpesEngineStoreCapture(void* lpData)
{
	m_pEngineProc = lpData;

	::InitializeCriticalSectionAndSpinCount(&m_csResult, 0);

	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_dQPC);
	m_dQPCData[0] = m_dQPC;
	m_dQPCData[1] = 0.0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_dQPC);
	m_dQPCData[1] = m_dQPC / m_dQPCData[0] - m_dQPCData[1];

	for (int i = 0; i < PARAM_CNT; i++) {
		m_sVulpesEngineParamData[i].Init();
	}

	m_sVulpesEngineCapture.pCStoreCapture = this;
	m_nStoreCaptureStart = 0;

	EnterCriticalSection(&m_csResult);
}

CVulpesEngineStoreCapture::~CVulpesEngineStoreCapture()
{
	EnterCriticalSection(&m_csResult);

	gVulpesEngineCaptureList.clear();

	for (int i = 0; i < PARAM_CNT; i++) {
		m_sVulpesEngineParamData[i].Init();
	}
	LeaveCriticalSection(&m_csResult);

	::DeleteCriticalSection(&m_csResult);
}

void CVulpesEngineStoreCapture::putVal(int id, VPE_PARAM_VALUE* pParamValue, double dQPC)
{
	if (m_nStoreCaptureStart && id < PARAM_CNT) {
		m_sVulpesEngineParamData[id].PutValue(pParamValue, &dQPC);
	}
}

VPE_PARAM_VALUE* CVulpesEngineStoreCapture::GetStoreCaptureParamValue(VPE_PARAM_VALUE* pParamValue, int id, double t) {

	int nIndex = 0;
	double dTime, dVal1, dVal2, dDeltaVal1, dDeltaVal2;
	EnterCriticalSection(&m_csResult);

	if (!m_sVulpesEngineParamData[id].m_nStoreCnt) {
		pParamValue->fx = 0.0f;
		pParamValue->fy = 0.0f;
		pParamValue->nx = 0;
		pParamValue->ny = 0;
	}
	else {

		int nCnt = (m_sVulpesEngineParamData[id].m_nCurrentIndex - m_sVulpesEngineParamData[id].m_nStartIndex + m_sVulpesEngineParamData[id].m_nStoreCnt) % m_sVulpesEngineParamData[id].m_nStoreCnt;
		nIndex = ((m_sVulpesEngineParamData[id].m_nCurrentIndex - 1 + m_sVulpesEngineParamData[id].m_nStoreCnt) % m_sVulpesEngineParamData[id].m_nStoreCnt) * 3;
		dTime = m_sVulpesEngineParamData[id].m_pdStoreData[nIndex];
		dVal1 = m_sVulpesEngineParamData[id].m_pdStoreData[nIndex + 1];
		dVal2 = m_sVulpesEngineParamData[id].m_pdStoreData[nIndex + 2];
		dDeltaVal1 = t - dTime;
		for (int i = 0; i < nCnt; i++) {
			nIndex = ((m_sVulpesEngineParamData[id].m_nCurrentIndex - i - 1 + m_sVulpesEngineParamData[id].m_nStoreCnt) % m_sVulpesEngineParamData[id].m_nStoreCnt) * 3;
			dDeltaVal2 = t - m_sVulpesEngineParamData[id].m_pdStoreData[nIndex + 1];
			if (dDeltaVal2 <= dDeltaVal1) {
				dDeltaVal1 = dDeltaVal2;
			}
		}
		pParamValue->fx = (float)dVal1;
		pParamValue->fy = (float)dVal2;
		pParamValue->nx = (int)dDeltaVal1;
		pParamValue->ny = (int)dDeltaVal2;
	}

	LeaveCriticalSection(&m_csResult);

	return pParamValue;
}

int	CVulpesEngineStoreCapture::GetStoreCaptureParamData(int id, double t, double *pt, VPE_PARAM_VALUE* pParamValue) {

	if (!m_sVulpesEngineParamData[id].m_nStoreCnt) {
		pParamValue->fx = 0.0f;
		pParamValue->fy = 0.0f;
		pParamValue->nx = 0;
		pParamValue->ny = 0;
		return 0;
	}
	int nIndex = 0;
	int nCnt = (m_sVulpesEngineParamData[id].m_nCurrentIndex - m_sVulpesEngineParamData[id].m_nStartIndex + m_sVulpesEngineParamData[id].m_nStoreCnt) % m_sVulpesEngineParamData[id].m_nStoreCnt;
	for (int i = 0; i < nCnt; i++) {
		nIndex = ((m_sVulpesEngineParamData[id].m_nCurrentIndex - i - 1 + m_sVulpesEngineParamData[id].m_nStoreCnt) % m_sVulpesEngineParamData[id].m_nStoreCnt) * 3;
		if (t > m_sVulpesEngineParamData[id].m_pdStoreData[nIndex]) {
			break;
		}
		if (pt) {
			pt[i] = m_sVulpesEngineParamData[id].m_pdStoreData[nIndex];
		}
		if (pParamValue) {
			pParamValue[i].fx = (float)m_sVulpesEngineParamData[id].m_pdStoreData[nIndex + 1];
			pParamValue[i].fy = (float)m_sVulpesEngineParamData[id].m_pdStoreData[nIndex + 2];
		}
	}
	return nCnt;
}

void CVulpesEngineStoreCapture::StoreStart()
{
	EnterCriticalSection(&m_csResult);
	m_nStoreCaptureStart = 1;
	for (int i = 0; i < PARAM_CNT; i++) {
		m_sVulpesEngineParamData[i].Init();
	}
	LeaveCriticalSection(&m_csResult);
}