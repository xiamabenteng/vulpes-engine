#include "stdafx.h"
#include "VulpesEngineParamData2.h"


CVulpesEngineParamData2::CVulpesEngineParamData2(void) {
	m_nStartIndex = 0;
	m_nCurrentIndex = 0;
	m_dPeriodTime = 0.0;
	m_nThreshold = 65536;
	m_nStoreCnt = 0;
	m_pdStoreData = nullptr;
	m_dValAvg = 0.0;
	m_dValSum = 0.0;
}

CVulpesEngineParamData2::~CVulpesEngineParamData2(void) {

	if (m_pdStoreData) {
		delete m_pdStoreData;
		m_pdStoreData = nullptr;
	}
}

void CVulpesEngineParamData2::Init() {

	if (m_pdStoreData) {
		delete m_pdStoreData;
	}
	m_pdStoreData = nullptr;
	m_nStartIndex = 0;
	m_nCurrentIndex = 0;
	m_nStoreCnt = 0;
	m_dValAvg = 0.0;
	m_dValSum = 0.0;
}

//Alloc double Array and store more value;
void CVulpesEngineParamData2::Reset() {

	if (m_nStoreCnt == 0) {
		Init();
		m_nStoreCnt = 256;
		m_pdStoreData = new double[m_nStoreCnt * 2];
		memset(m_pdStoreData, 0, sizeof(double) * m_nStoreCnt * 2);
		return;
	}
	//no longer alloc and use Init
	if (m_nStoreCnt == m_nThreshold) {
		Init();
		return;
	}
	// Increase by multiple of 2£¬until threshold exceeded
	int nNewStoreCnt = m_nStoreCnt << 1;

	if (nNewStoreCnt > m_nThreshold) {
		nNewStoreCnt = m_nThreshold;
	}

	double* pdNewStoreData = new double[nNewStoreCnt * 2];
	memset(pdNewStoreData, 0, sizeof(double) * nNewStoreCnt * 2);
	int nCnt = (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt;
	int nIndex = 0;
	for (int i = 0; i < nCnt; i++) {
		MoveValue(nCnt - i - 1, &pdNewStoreData[nIndex * 2], &pdNewStoreData[nIndex * 2 + 1]);
	}
	m_nStartIndex = 0;
	m_nCurrentIndex = nCnt;
	if (m_pdStoreData) {
		delete m_pdStoreData;
		m_pdStoreData = nullptr;
	}
	m_nStoreCnt = nNewStoreCnt;
	m_pdStoreData = new double[m_nStoreCnt * 2];
	memset(m_pdStoreData, 0, sizeof(double) * m_nStoreCnt * 2);
	memcpy(m_pdStoreData, pdNewStoreData, sizeof(double) * m_nStoreCnt * 2);
	delete pdNewStoreData;
	pdNewStoreData = nullptr;
}

void CVulpesEngineParamData2::PutValue(double dVal1, double dVal2) {

	if (m_pdStoreData == nullptr || m_nStoreCnt == 0 || (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt >= (m_nStoreCnt - 1)) {
		Reset();
	}

	m_dValSum += dVal2;
	m_pdStoreData[m_nCurrentIndex * 2] = dVal1;
	m_pdStoreData[m_nCurrentIndex * 2 + 1] = dVal2;
	m_nCurrentIndex = (m_nCurrentIndex + 1) % m_nStoreCnt;

	if (m_dPeriodTime <= 0.0) {
		Init();
		m_dValSum = 0.0;
	}
	else {
		int nCurrentIndex = (m_nCurrentIndex - 1 + m_nStoreCnt) % m_nStoreCnt;
		double dStoreDataCurrentVal = m_pdStoreData[nCurrentIndex * 2];
		double dStoreDataDeltaVal = dStoreDataCurrentVal - m_pdStoreData[m_nStartIndex * 2];
		if (dStoreDataDeltaVal >= m_dPeriodTime) {
			do {
				m_dValSum -= m_pdStoreData[m_nStartIndex * 2 + 1];
				m_nStartIndex = (m_nStartIndex + 1) % m_nStoreCnt;
				dStoreDataDeltaVal = dStoreDataCurrentVal - m_pdStoreData[m_nStartIndex * 2];
			} while (dStoreDataDeltaVal >= m_dPeriodTime);
		}
	}

	if (m_nStoreCnt > 0 && (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt != 0) {
		int nCnt = (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt;
		m_dValAvg = m_dValSum / (nCnt * 1.0);
	}
	else {
		m_dValAvg = 0.0;
	}
}

void CVulpesEngineParamData2::AddValue(double dVal1, double dVal2) {

	if (m_nStoreCnt == 0 || (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt >= (m_nStoreCnt - 1)) {
		Reset();
	}
	int nidx = m_nCurrentIndex * 2;
	m_pdStoreData[nidx] = dVal1;
	m_pdStoreData[nidx + 1] = dVal2;
	m_nCurrentIndex = (m_nCurrentIndex + 1) % m_nStoreCnt;
	GetStartIndex();
}

double* CVulpesEngineParamData2::GetValue(int nIndex) {

	if (m_nStoreCnt == 0) {
		Reset();
	}
	int nStoreIndex = ((m_nCurrentIndex - nIndex + m_nStoreCnt - 1) % m_nStoreCnt) * 2;
	return &m_pdStoreData[nStoreIndex];
}

// Move old array value to new alloc array
void CVulpesEngineParamData2::MoveValue(int nIndex, double* dVal1, double* dVal2) {

	if (m_nStoreCnt == 0) {
		Reset();
	}
	int nStoreIndex = ((m_nCurrentIndex - nIndex + m_nStoreCnt - 1) % m_nStoreCnt) * 2;
	*dVal1 = m_pdStoreData[nStoreIndex];
	*dVal2 = m_pdStoreData[nStoreIndex + 1];
}

void CVulpesEngineParamData2::GetAverage(int nCnt) {
	if (nCnt == 0) {
		m_dValSum = 0.0;
		m_dValAvg = 0.0;
		Init();
	}

	while (nCnt < GetPeriodCnt()) {
		m_dValSum -= m_pdStoreData[(m_nStartIndex << 1) + 1];
		m_nStartIndex = (m_nStartIndex + 1) % m_nStoreCnt;
	}
	if (m_nStoreCnt > 0 && (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt != 0) {
		int nCnt = (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt;
		m_dValAvg = m_dValSum / (nCnt * 1.0);
	}
	else {
		m_dValAvg = 0.0;
	}
}

bool CVulpesEngineParamData2::GetMinMaxValue(double* pdMinVal, double* pdMaxVal) {

	if (m_nStoreCnt == 0) {
		return false;
	}

	*pdMinVal = m_pdStoreData[(m_nStartIndex << 1) + 1];
	*pdMaxVal = *pdMinVal;

	int nCurrentCnt = m_nCurrentIndex;
	int nIndex = m_nCurrentIndex;
	if (m_nCurrentIndex < m_nStartIndex) {
		nCurrentCnt += m_nStoreCnt;
	}
	for (int i = m_nStartIndex; i < nCurrentCnt; i++) {
		nIndex = i % m_nStoreCnt;
		if (*pdMinVal > m_pdStoreData[(nIndex << 1) + 1]) {
			*pdMinVal = m_pdStoreData[(nIndex << 1) + 1];
		}
		if (*pdMaxVal < m_pdStoreData[(nIndex << 1) + 1]) {
			*pdMaxVal = m_pdStoreData[(nIndex << 1) + 1];
		}
	}
	return true;
}

//In the m_dPeriodTime collect sample count
int CVulpesEngineParamData2::GetPeriodCnt() {
	if (m_nStoreCnt == 0) {
		return 0;
	}
	return (m_nCurrentIndex - m_nStartIndex + m_nStoreCnt) % m_nStoreCnt;
}

//In the m_dPeriodTime(unit:second) find the startIndex in array
void CVulpesEngineParamData2::GetStartIndex() {
	int nCnt = GetPeriodCnt();
	if (nCnt) {
		int nCurrentIndex = (m_nCurrentIndex - 1 + m_nStoreCnt) % m_nStoreCnt;
		double dCurrentImageData = m_pdStoreData[nCurrentIndex * 2];
		double dDeltaImageData = 0.0;
		while (1) {
			double dPreviousImageData = m_pdStoreData[m_nStartIndex * 2];
			dDeltaImageData = dCurrentImageData - dPreviousImageData;
			if (dDeltaImageData < 0.0) {
				break;
			}
			if (dDeltaImageData >= m_dPeriodTime) {
				m_nStartIndex = (m_nStartIndex + 1) % m_nStoreCnt;
				nCnt = GetPeriodCnt();
				if (nCnt) {
					continue;
				}
			}
			return;
		}
		Init();
	}
}
