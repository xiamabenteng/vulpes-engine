#pragma once

#include "VulpesEngineFFT.h"

class CVulpesEngineFFTManage
{
public:
	CVulpesEngineFFTManage(void* lpData);
	~CVulpesEngineFFTManage();

public:
	void* m_pEngine;
	CVulpesEngineFFT* m_pVulpesEngineFFT[PARAM_CNT];
	VPE_FFT_DATA* m_pVulpesEngineFFTData;
};

