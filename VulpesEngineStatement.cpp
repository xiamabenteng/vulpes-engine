#include "stdafx.h"
#include "VulpesEngineCore.h"


CVulpesEngineStatement::CVulpesEngineStatement(void* lpData)
{
	m_pEngine = lpData;
	CString str;
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_pRateCal[i] = new CVulpesEngineRate(lpData);
	}
}


CVulpesEngineStatement::~CVulpesEngineStatement()
{
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		delete m_pRateCal[i];
		m_pRateCal[i] = nullptr;
	}
}


void CVulpesEngineStatement::Init() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_pRateCal[i]->m_fCalMin = 0.f;
		m_pRateCal[i]->m_fCalMax = 0.f;
		m_pRateCal[i]->m_fBaseMin = 0.f;
		m_pRateCal[i]->m_fBaseMax = 0.f;
		m_pRateCal[i]->m_fRate = 0.f;
		m_pRateCal[i]->m_fRatev = 0.f;
		m_pRateCal[i]->m_nRateBase = 0;
		m_pRateCal[i]->m_nMinMaxValid = 0;
		if (m_pRateCal[i]->m_VulpesEngineParamDataTmp.m_pdStoreData) {
			m_pRateCal[i]->m_VulpesEngineParamDataTmp.Init();
		}
		if (m_pRateCal[i]->m_VulpesEngineParamDataAll.m_pdStoreData) {
			m_pRateCal[i]->m_VulpesEngineParamDataAll.Init();
		}
	}

	pEngine->SetParamValueF(STAT_RESULT_STATEMENT_CRITICAL, 0.f, 0.f);
	pEngine->SetParamValueFx(STAT_RESULT_STATEMENT_QUANTITY_INDICATOR, 0.f);
}


void CVulpesEngineStatement::SetStatementMode() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx == 2) {
		pEngine->m_sParamData.value[STATEMENT_STARTED].nx = 0;
		pEngine->SetCallbackParamData(STATEMENT_STARTED, 0);
	}
	else {
		pEngine->m_sParamData.value[STATEMENT_STARTED].nx = 1;
		pEngine->SetCallbackParamData(STATEMENT_STARTED, 0);
	}
}


void CVulpesEngineStatement::SetStatement(int nParamId) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;
	int nStatementModeCMP = pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx;
	if (nParamId == STATEMENT_MODE_CMP) {
		if (nStatementModeCMP == 2) {
			Reset();
		}
	}
	else if (nParamId == STATEMENT_STARTED) {
		if (pEngine->m_sParamData.value[STATEMENT_STARTED].nx == 0) {
			Init();
		}
		else {
			CalculateStatement();
		}
	}
	else if (nParamId == STATEMENT_ENABLE) {
		if (pEngine->m_sParamData.value[STATEMENT_ENABLE].nx == 0) {
			if (pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx == 2) {
				pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx = 1;
				pEngine->SetCallbackParamData(STATEMENT_MODE_CMP, 0);
			}
			pEngine->m_sParamData.value[STATEMENT_STARTED].nx = 0;
			pEngine->SetCallbackParamData(STATEMENT_STARTED, 0);
		}
	}
}


void CVulpesEngineStatement::AddValue(int nParamId, float fValue) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	int nStatementEnable = pEngine->m_sParamData.value[STATEMENT_ENABLE].nx;
	int nStatementMode = pEngine->m_sParamData.value[STATEMENT_MODE].nx;
	int nStatementModeCMP = pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx;

	if (nStatementEnable) {

		if (nStatementModeCMP && nStatementMode == 2) {
			pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx = 0;
			pEngine->SetCallbackParamData(STATEMENT_MODE_CMP, 0);
		}
		m_pRateCal[nParamId]->m_nParamId = nParamId;
		m_pRateCal[nParamId]->AddVal(fValue);
	}

}


void CVulpesEngineStatement::CalculateStatement() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	if (pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx == 2) {
		return;
	}
	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++)
	{
		m_pRateCal[i]->CompareDataProcess();
	}
	Calculate(0);
}


int CVulpesEngineStatement::Calculate(int nCase) {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	int nStatementEnable = pEngine->m_sParamData.value[STATEMENT_ENABLE].nx;
	int nStatementModeCMP = pEngine->m_sParamData.value[STATEMENT_MODE_CMP].nx;
	float fThreshold = pEngine->m_sParamData.value[STATEMENT_LTH].fx;
	if (!nStatementEnable || nStatementModeCMP == 2) {
		return 0;
	}

	float fRateSum = 0.f;
	float fRateVSum = 0.f;
	float fRate2Sum = 0.f;
	float fRateVal = 0.f;

	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		if (m_pRateCal[i]->CalRate(0)) {
			fRateSum += m_pRateCal[i]->m_fRate;
			fRateVSum += m_pRateCal[i]->m_fRatev;
			if (m_pRateCal[i]->m_fRate2 > 0.f) {
				fRate2Sum += m_pRateCal[i]->m_fRate2;
			}
		}
	}

	if (fRate2Sum <= 0.f) {
		fRate2Sum += 1.f;
	}

	if (pEngine->m_sParamData.value[STATEMENT_STARTED].nx == 0) {
		pEngine->SetParamValueF(STAT_RESULT_STATEMENT_CRITICAL, 0.0f, 0.0f);
		pEngine->SetParamValueFx(STAT_RESULT_STATEMENT_QUANTITY_INDICATOR, 0.0f);
	}else{

		if (fRate2Sum > 0.f) {
			fRateVal = fRateSum / fRate2Sum;
		}

		if (fThreshold > fRateVal) {
			fRate2Sum = 0.f;
		}
		else {
			fRate2Sum = (fRateVal - fThreshold) / (1.f - fThreshold);
		}
		pEngine->SetParamValueF(STAT_RESULT_STATEMENT_CRITICAL, fRate2Sum, fRateVSum);
		pEngine->SetParamValueFx(STAT_RESULT_STATEMENT_QUANTITY_INDICATOR, (fRateVSum + 10) * 0.05f);
	}
	return 1;
}


void CVulpesEngineStatement::Reset() {

	for (int i = STAT_RESULT_START + 1; i < STAT_RESULT_END; i++) {
		m_pRateCal[i]->m_VulpesEngineParamDataBase.Init();
	}
	Init();
}


void CVulpesEngineStatement::CalRate() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;
	float fStatLevel = 0.f;
	for (int i = STAT_RESULT_AM_FAST; i < STAT_RESULT_STATEMENT_CRITICAL; i++) {

		m_pRateCal[i]->m_nCal = 1;

		if (i == STAT_RESULT_FREQ || i == STAT_RESULT_FREQ_PERIOD_TIME_DELTA || i == STAT_RESULT_FREQ_PERIOD_TIME_CV) {
			m_pRateCal[i]->m_nCal = 0;
		}
		m_pRateCal[i]->CompareDataProcess();

		fStatLevel = G_GetStatLevelDef(i);
		m_pRateCal[i]->m_fStatLevel = fStatLevel;
		m_pRateCal[i]->m_fRate1 = 1.f;
		m_pRateCal[i]->m_fRate1v = 1.f;
		m_pRateCal[i]->m_fRate2 = 2.f;
		m_pRateCal[i]->m_fRate2v = 2.f;
	}
}


float CVulpesEngineStatement::G_GetStatLevelDef(int id)
{
	float result = 0.f;

	switch (id) {
	case STAT_RESULT_AM:
	case STAT_RESULT_AVG_AM:
	case STAT_RESULT_AVG_ACCUM_AM:
	case STAT_RESULT_MAX_ACCUM_AM:
	case STAT_RESULT_AM_FAST:
	case STAT_RESULT_AVG_AM_FAST:
		result = 1.0f;
		break;
	case STAT_RESULT_FREQ:
	case STAT_RESULT_FREQ_FAST:
	case STAT_RESULT_FREQ_DELTA:
	case STAT_RESULT_AVG_ACCUM_FREQ:
	case STAT_RESULT_MAX_ACCUM_FREQ:
	case STAT_RESULT_MAX_ACCUM_FREQ_MAX:
	case STAT_RESULT_FREQ_FFT:
	case STAT_RESULT_FREQ_FFT_FAST:
	case STAT_RESULT_FREQ_PERIOD_TIME_AVG:
	case STAT_RESULT_FREQ_PERIOD_TIME_SD:
	case STAT_RESULT_MIN_ACCUM_AM_CV_VALUE_RATE:
	case STAT_RESULT_MIN_ACCUM_AM_CV_NUM_RATE:
	case STAT_RESULT_AVG_ACCUM_AM_CV_VALUE_RATE:
	case STAT_RESULT_AVG_ACCUM_AM_CV_NUM_RATE:
	case STAT_RESULT_MAX_ACCUM_AM_CV_VALUE_RATE:
	case STAT_RESULT_MAX_ACCUM_AM_CV_NUM_RATE:
	case STAT_RESULT_MAX_ACCUM_AM_CV_DIFFERENCE:
		result = 0.2f;
		break;
	case STAT_RESULT_AVG_ACCUM_FREQ_RMSD:
	case STAT_RESULT_AVG_ACCUM_FREQ_SCALE:
	case STAT_RESULT_MAX_ACCUM_FREQ_RMSD:
	case STAT_RESULT_MAX_ACCUM_FREQ_SCALE:
		result = 0.5f;
		break;
	default:
		break;
	}
	return result;
}


int CVulpesEngineStatement::GetStat(int id, float *cMin, float *cMax, float *bMin, float *bMax, float *rate, int *rate_base)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		*bMin = 0.f;
		*bMax = 0.f;
		*cMax = 0.f;
		*cMin = 0.f;
		*rate = 0.f;
		*rate_base = 0;
		return 0;
	}
	if (cMin)
		*cMin = m_pRateCal[id]->m_fCalMin;
	if (cMax)
		*cMax = m_pRateCal[id]->m_fCalMax;
	if (rate)
		*rate = m_pRateCal[id]->m_fRate;
	if (rate_base)
		*rate_base = m_pRateCal[id]->m_nRateBase;
	if (m_pRateCal[id]->m_nMinMaxValid)
	{
		if (bMin)
			*bMin = m_pRateCal[id]->m_fBaseMin;
		if (bMax)
			*bMax = m_pRateCal[id]->m_fBaseMax;
	}
	else
	{
		if (bMin)
			*bMin = 0;
		if (bMax)
			*bMax = 0;
	}
	return m_pRateCal[id]->m_nCal && m_pRateCal[id]->m_nMinMaxValid;
}


void CVulpesEngineStatement::PutStatCalc(int id, int bCalc)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}

	m_pRateCal[id]->m_nCal = bCalc;

	if (bCalc) {
		m_pRateCal[id]->CompareDataProcess();
	}
	else {
		m_pRateCal[id]->Init();
	}
}


void CVulpesEngineStatement::PutStatRate1(int id, float v)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}
	m_pRateCal[id]->m_fRate1 = v;
}


void CVulpesEngineStatement::PutStatRate2(int id, float v)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}
	m_pRateCal[id]->m_fRate2 = v;
}


void CVulpesEngineStatement::PutStatRate1v(int id, float v)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}
	m_pRateCal[id]->m_fRate1v = v;
}


void CVulpesEngineStatement::PutStatRate2v(int id, float v)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}
	m_pRateCal[id]->m_fRate2v = v;
}


void CVulpesEngineStatement::PutStatLevel(int id, float v)
{
	if (id >= STAT_RESULT_END || id < STAT_RESULT_START + 1) {
		return;
	}
	m_pRateCal[id]->m_fStatLevel = v;
}