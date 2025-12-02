#include "stdafx.h"
#include "VulpesEngineCore.h"

CVulpesEngineCorrelation::CVulpesEngineCorrelation(void* lpData) {

	m_pEngine = lpData;
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_correlationData[i].dValSum = 0.0;
		m_correlationData[i].dRefVal = 0.0;
	}

	m_bReady = false;
	m_bProcess = false;
}

CVulpesEngineCorrelation::~CVulpesEngineCorrelation() {

}

void CVulpesEngineCorrelation::AddValue(int nParamId, double dTime, float fVal) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	float fEngineCorrelationTStat = pEngine->m_sParamData.value[CORRELATION_TSTAT].fx;

	if (fEngineCorrelationTStat < 1.f) {
		return;
	}

	VPE_CORRELATION_VALUE val;
	val.dTime = dTime;
	val.fVal = fVal;

	m_csResult.Lock();
	m_correlationData[nParamId].valueList.push_back(val);
	m_correlationData[nParamId].dValSum += fVal;
	if (m_correlationData[nParamId].valueList.size() > 1) {
		double dFrontTime = m_correlationData[nParamId].valueList.front().dTime;
		if ((dTime - dFrontTime) > fEngineCorrelationTStat) {
			m_correlationData[nParamId].dValSum -= m_correlationData[nParamId].valueList.front().fVal;
			m_correlationData[nParamId].valueList.pop_front();
			m_bProcess = true;
		}
		int nsize = (int)m_correlationData[nParamId].valueList.size();
		m_correlationData[nParamId].dRefVal = m_correlationData[nParamId].dValSum / nsize;
	}
	m_csResult.Unlock();
}

float CVulpesEngineCorrelation::GetCorrelation(int idx, int idy, float* pXY, int* nsize) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (!m_bProcess || m_correlationData[idx].valueList.size() < 2 || m_correlationData[idy].valueList.size() < 2) {
		return 0.f;
	}

	/*
	X, Y: reciprocity correlation
	*/

	m_csResult.Lock();
	m_bReady = true;
	list<VPE_CORRELATION_VALUE>::iterator iBlock1;
	list<VPE_CORRELATION_VALUE>::iterator iBlock2 = m_correlationData[idy].valueList.begin();


	double dx, dy;
	double dSum1 = 0.0;
	double dSum2 = 0.0;
	double dSum11 = 0.0;
	double dSum22 = 0.0;
	for (iBlock1 = m_correlationData[idx].valueList.begin(); iBlock1 != m_correlationData[idx].valueList.end(); iBlock1++) {
		if (m_correlationData[idx].valueList.size() < 1 || m_correlationData[idy].valueList.size() < 1) {
			return 0.f;
		}

		if (iBlock2 == m_correlationData[idy].valueList.end())
			break;

		dx = (double)iBlock1->fVal - m_correlationData[idx].dRefVal;
		dy = (double)iBlock2->fVal - m_correlationData[idy].dRefVal;

		dSum1 += dx * dy;
		dSum11 += dx * dx;
		dSum22 += dy * dy;
		iBlock2++;
	}

	m_csResult.Unlock();
	dSum2 = sqrt(dSum11 * dSum22);
	double dVal = 0.0;
	if (dSum2 > 0.0) {
		dVal = dSum1 / dSum2;
	}
	else {
		dVal = 0.0;
	}
	m_bReady = false;

	if (pXY) {
		*pXY = (float)dVal;
	}

	if (nsize) {
		*nsize = min(m_correlationData[idx].valueList.size(), m_correlationData[idy].valueList.size());
	}

	return (float)dVal;
}

void CVulpesEngineCorrelation::Clear() {

	while (m_bReady);
	m_bProcess = false;
	m_csResult.Lock();
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_correlationData[i].dValSum = 0.0;
		if (m_correlationData[i].valueList.size() > 0) {
			m_correlationData[i].valueList.clear();
		}
		m_correlationData[i].dRefVal = 0.0;
	}
	m_csResult.Unlock();
}