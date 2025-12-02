#pragma once

#include "VulpesEngineParamData2.h"

class CVulpesEngineRate
{
public:
	CVulpesEngineRate(void* lpData);
	~CVulpesEngineRate();

	void Init();
	void AddVal(float fVal);
	void AutoAddVal(float fVal);
	bool CalRate(int nFlag);
	void CompareDataProcess();

public:
	void* m_pEngine;
	int m_nParamId;
	int m_nCal;
	int m_nMinMaxValid;
	float m_fCalMin;
	float m_fCalMax;
	float m_fBaseMin;
	float m_fBaseMax;
	float m_fRate;
	float m_fRate1;
	float m_fRate2;
	float m_fRatev;
	float m_fRate1v;
	float m_fRate2v;
	float m_fStatLevel;
	int m_nRateBase;

	CVulpesEngineParamData2 m_VulpesEngineParamDataAll;
	CVulpesEngineParamData2 m_VulpesEngineParamDataTmp;
	CVulpesEngineParamData2 m_VulpesEngineParamDataBase;
};

