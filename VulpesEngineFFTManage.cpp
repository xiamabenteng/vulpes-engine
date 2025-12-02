#include "stdafx.h"
#include "VulpesEngineCore.h"


CVulpesEngineFFTManage::CVulpesEngineFFTManage(void* lpData)
{
	m_pEngine = lpData;
	int nStatCnt = STAT_RESULT_END - STAT_RESULT_START - 1;
	memset(m_pVulpesEngineFFT, 0, sizeof(CVulpesEngineFFT*) * PARAM_CNT);
	m_pVulpesEngineFFTData = new VPE_FFT_DATA[nStatCnt];
	memset(m_pVulpesEngineFFTData, 0, sizeof(VPE_FFT_DATA) * nStatCnt);

	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_pVulpesEngineFFT[i] = new CVulpesEngineFFT(lpData);
		m_pVulpesEngineFFT[i]->m_pFFTData = &m_pVulpesEngineFFTData[i - STAT_RESULT_START - 1];
		m_pVulpesEngineFFT[i]->m_nSize = 128;
		m_pVulpesEngineFFT[i]->m_nParamId = i;
		m_pVulpesEngineFFT[i]->InitValue();
	}
}

CVulpesEngineFFTManage::~CVulpesEngineFFTManage() {

	if (m_pVulpesEngineFFTData) {
		delete[] m_pVulpesEngineFFTData;
		m_pVulpesEngineFFTData = nullptr;
	}
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		delete m_pVulpesEngineFFT[i];
		m_pVulpesEngineFFT[i] = nullptr;
	}
}
