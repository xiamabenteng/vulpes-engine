#include "stdafx.h"
#include "VulpesEngineRate.h"
#include "VulpesEngineCore.h"

CVulpesEngineRate::CVulpesEngineRate(void* lpData) {

	m_pEngine = lpData;

	m_VulpesEngineParamDataAll.Init();
	m_VulpesEngineParamDataTmp.Init();
	m_VulpesEngineParamDataBase.Init();

	m_VulpesEngineParamDataAll.m_nThreshold = 65536;
	m_VulpesEngineParamDataTmp.m_nThreshold = 65536;
	m_VulpesEngineParamDataBase.m_nThreshold = 65536;

	m_VulpesEngineParamDataAll.m_dPeriodTime = 2.0;
	m_VulpesEngineParamDataTmp.m_dPeriodTime = 2.0;
	m_VulpesEngineParamDataBase.m_dPeriodTime = 2.0;

	m_nParamId = 0;
	m_nMinMaxValid = 0;
	m_nCal = 0;
	m_fCalMin = 0.f;
	m_fCalMax = 0.f;
	m_fBaseMax = 0.f;
	m_fBaseMin = 0.f;
	m_fRate = 0.f;
	m_fRate1 = 1.f;
	m_fRate2 = 2.f;
	m_fRatev = 0.f;
	m_fRate1v = 1.f;
	m_fRate2v = 2.f;
	m_fStatLevel = 0.f;
}

CVulpesEngineRate::~CVulpesEngineRate(void) {

}

void CVulpesEngineRate::AddVal(float fVal) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (m_nCal) {
		if (pEngine->m_sParamData.value[STATEMENT_MODE].nx == 2) {
			AutoAddVal(fVal);
		}
		else {
			double dbTemp = pEngine->m_dCurrentFrameTime;
			if (m_VulpesEngineParamDataAll.m_nStoreCnt != m_VulpesEngineParamDataAll.m_nCurrentIndex) {
				m_VulpesEngineParamDataAll.m_dPeriodTime = (double)pEngine->m_sParamData.value[STATEMENT_PERIOD].fx;
				m_VulpesEngineParamDataAll.PutValue(dbTemp, fVal);
			}

			if (pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx != 1) {
				if (pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx == 2) {
					m_VulpesEngineParamDataBase.m_dPeriodTime = 86400.0;
				}
				else {
					m_VulpesEngineParamDataBase.m_dPeriodTime = (double)pEngine->m_sParamData.value[STATEMENT_PERIOD_BASE].fx;
				}
				m_VulpesEngineParamDataBase.PutValue(dbTemp, fVal);
			}
		}
	}
	else {
		if (m_nMinMaxValid) {
			Init();
		}
	}
}

void CVulpesEngineRate::AutoAddVal(float fVal) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (pEngine->m_sParamData.value[STATEMENT_STARTED].nx == 0) {
		pEngine->m_sParamData.value[STATEMENT_STARTED].nx = 1;
		pEngine->SetCallbackParamData(STATEMENT_STARTED, 0);
	}

	double dTime = pEngine->m_dCurrentFrameTime;
	m_VulpesEngineParamDataAll.m_dPeriodTime = (double)pEngine->m_sParamData.value[STATEMENT_PERIOD].fx;
	m_VulpesEngineParamDataAll.PutValue(dTime, fVal);

	m_VulpesEngineParamDataBase.m_dPeriodTime = (double)pEngine->m_sParamData.value[STATEMENT_PERIOD_BASE].fx;
	float fVal1 = (float)m_VulpesEngineParamDataAll.m_pdStoreData[1 + (m_VulpesEngineParamDataAll.m_nStartIndex << 1)];
	m_VulpesEngineParamDataBase.PutValue(dTime, fVal1);

	m_VulpesEngineParamDataTmp.m_nThreshold = m_VulpesEngineParamDataBase.m_nThreshold;
	m_VulpesEngineParamDataTmp.m_nStartIndex = m_VulpesEngineParamDataBase.m_nStartIndex;
	m_VulpesEngineParamDataTmp.m_nCurrentIndex = m_VulpesEngineParamDataBase.m_nCurrentIndex;

	if (m_VulpesEngineParamDataTmp.m_nStoreCnt != m_VulpesEngineParamDataBase.m_nStoreCnt) {
		if (m_VulpesEngineParamDataTmp.m_pdStoreData) {
			delete m_VulpesEngineParamDataTmp.m_pdStoreData;
			m_VulpesEngineParamDataTmp.m_pdStoreData = nullptr;
		}
		m_VulpesEngineParamDataTmp.m_pdStoreData = new double[m_VulpesEngineParamDataBase.m_nStoreCnt << 1];
		m_VulpesEngineParamDataTmp.m_nStoreCnt = m_VulpesEngineParamDataBase.m_nStoreCnt;
	}
	//m_theImgData_70.
	memcpy(m_VulpesEngineParamDataTmp.m_pdStoreData, m_VulpesEngineParamDataBase.m_pdStoreData, sizeof(double)*(m_VulpesEngineParamDataBase.m_nStoreCnt << 1));
	m_VulpesEngineParamDataTmp.m_dPeriodTime = m_VulpesEngineParamDataBase.m_dPeriodTime;
	m_VulpesEngineParamDataTmp.m_dValAvg = m_VulpesEngineParamDataBase.m_dValAvg;
	m_VulpesEngineParamDataTmp.m_dValSum = m_VulpesEngineParamDataBase.m_dValSum;

	CalRate(1);
}

bool CVulpesEngineRate::CalRate(int nFlag) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (!m_nCal || !pEngine->m_sParamData.value[STATEMENT_STARTED].nx) {
		if (m_nMinMaxValid) {
			Init();
		}
		return false;
	}

	if (m_VulpesEngineParamDataAll.m_nStartIndex == m_VulpesEngineParamDataAll.m_nCurrentIndex || m_VulpesEngineParamDataTmp.m_nStartIndex == m_VulpesEngineParamDataTmp.m_nCurrentIndex) {
		return false;
	}

	double dCMax, dCMin;

	m_VulpesEngineParamDataAll.GetMinMaxValue(&dCMin, &dCMax);
	m_fCalMax = (float)dCMax; m_fCalMin = (float)dCMin;

	if (nFlag) {
		m_VulpesEngineParamDataTmp.GetMinMaxValue(&dCMin, &dCMax);
		m_fBaseMax = (float)dCMax; m_fBaseMin = (float)dCMin;
	}
	
	DWORD dwVal1 = 0;

	float fValMin = m_fBaseMin - (fabsf(m_fBaseMin) * (m_fStatLevel));
	if (fValMin > m_fCalMin) {
		dwVal1 = 1;
	}
	float fValMax = m_fBaseMax + (fabsf(m_fBaseMax) * (m_fStatLevel));
	if (fValMax < m_fCalMax) {
		dwVal1++;
	}
	m_nRateBase = dwVal1;

	if (m_nRateBase == 0) {
		m_fRatev = 0.f;
		m_fRate = 0.f;
		m_nMinMaxValid = 1;
	}
	else if (m_nRateBase == 1) {
		m_fRate = m_fRate1;
		m_fRatev = m_fRate1v;
	}
	else if (m_nRateBase == 2) {
		m_nMinMaxValid = 1;
		m_fRate = m_fRate2;
		m_fRatev = m_fRate2v;
	}
	return true;
}

void CVulpesEngineRate::Init() {

	m_fBaseMax = 0;
	m_fBaseMin = 0;
	m_fCalMax = 0;
	m_fCalMin = 0;
	m_fRate = 0;
	m_fRatev = 0;
	m_nRateBase = 0;
	m_VulpesEngineParamDataTmp.Init();
	m_VulpesEngineParamDataAll.Init();
	m_nMinMaxValid = 0;
}

void CVulpesEngineRate::CompareDataProcess() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (m_nCal && pEngine->m_sParamData.value[STATEMENT_MODE].nx != 2) {

		if (m_VulpesEngineParamDataBase.m_nStoreCnt > 0) {
			if (m_VulpesEngineParamDataTmp.m_nStoreCnt > 0) {
				m_VulpesEngineParamDataTmp.Init();
			}
			m_VulpesEngineParamDataTmp.m_nStartIndex = m_VulpesEngineParamDataBase.m_nStartIndex;
			m_VulpesEngineParamDataTmp.m_nCurrentIndex = m_VulpesEngineParamDataBase.m_nCurrentIndex;
			m_VulpesEngineParamDataTmp.m_nThreshold = m_VulpesEngineParamDataBase.m_nThreshold;
			m_VulpesEngineParamDataTmp.m_pdStoreData = new double[m_VulpesEngineParamDataBase.m_nStoreCnt << 1];
			m_VulpesEngineParamDataTmp.m_nStoreCnt = m_VulpesEngineParamDataBase.m_nStoreCnt;
			memcpy(m_VulpesEngineParamDataTmp.m_pdStoreData, m_VulpesEngineParamDataBase.m_pdStoreData, sizeof(double) * (m_VulpesEngineParamDataBase.m_nStoreCnt << 1));
		}
		m_VulpesEngineParamDataAll.GetAverage(1);
	}
	CalRate(1);
}
