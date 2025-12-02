#pragma once

#include "VulpesEngineParamData.h"

class CVulpesEngineParamCal
{
public:
	CVulpesEngineParamCal(void* lpData);
	~CVulpesEngineParamCal();

public:
	void Init();
	double Calculate(float fVal1, float fVal2, int nParamId);

private:
	double _CalParamFastPixCV();

public:
	void* m_pEngine;
	CVulpesEngineParamData m_ParamDataFastPixCV;
};

