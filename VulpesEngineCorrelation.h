#pragma once

typedef struct tagVPE_CORRELATION_VALUE {
	float fVal;
	double dTime;
}VPE_CORRELATION_VALUE;

typedef struct tagVPE_CORRELATION_DATA {
	double dValSum;
	double dRefVal;
	list<VPE_CORRELATION_VALUE> valueList;
}VPE_CORRELATION_DATA;

class CVulpesEngineCorrelation
{
public:
	CVulpesEngineCorrelation(void* lpData);
	~CVulpesEngineCorrelation();

public:
	void AddValue(int nParamId, double dTime, float fVal);
	float GetCorrelation(int idx, int idy, float* pXY, int* nsize);
	void Clear();

public:
	void* m_pEngine;
	VPE_CORRELATION_DATA m_correlationData[PARAM_CNT];
	CCriticalSection m_csResult;
	bool m_bReady;
	bool m_bProcess;
};