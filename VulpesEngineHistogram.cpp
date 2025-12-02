#include "stdafx.h"
#include "VulpesEngineHistogram.h"

CVulpesEngineHistogram::CVulpesEngineHistogram() {

	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthAllocMemorySize = 0;
	m_nHeightAllocMemorySize = 0;

	m_pfHistWidth = nullptr;
	m_pfHistWidthEx = nullptr;
	m_pfHistHeight = nullptr;
	m_pfHistHeightEx = nullptr;

	m_npHistWidthAvg = 0;
	m_npHistHeightAvg = 0;
	m_fpHistWidthAvg = 0.f;
	m_fpHistHeightAvg = 0.f;
}


CVulpesEngineHistogram::~CVulpesEngineHistogram()
{
	FreeFrameMemory();
}

void CVulpesEngineHistogram::AllocFrameMemory(int nWidth, int nHeight)
{
	FreeFrameMemory();

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_nWidthAllocMemorySize = nWidth << 2;
	m_nHeightAllocMemorySize = nHeight << 2;

	if (m_nWidthAllocMemorySize > 0x1FFFF) {
		m_pfHistWidth = (float*)(BYTE*)VirtualAlloc(0, m_nWidthAllocMemorySize, MEM_COMMIT, 4);
	}
	else {
		m_pfHistWidth = (float*)_aligned_malloc((m_nWidthAllocMemorySize + 31) & 0xFFFFFFE0, 32);
	}
	memset(m_pfHistWidth, 0, m_nWidthAllocMemorySize);

	if (m_nWidthAllocMemorySize > 0x1FFFF) {
		m_pfHistWidthEx = (float*)(BYTE*)VirtualAlloc(0, m_nWidthAllocMemorySize, MEM_COMMIT, 4);
	}
	else {
		m_pfHistWidthEx = (float*)_aligned_malloc((m_nWidthAllocMemorySize + 31) & 0xFFFFFFE0, 32);
	}
	memset(m_pfHistWidthEx, 0, m_nWidthAllocMemorySize);

	if (m_nHeightAllocMemorySize > 0x1FFFF) {
		m_pfHistHeight = (float*)(BYTE*)VirtualAlloc(0, m_nHeightAllocMemorySize, MEM_COMMIT, 4);
	}
	else {
		m_pfHistHeight = (float*)_aligned_malloc((m_nHeightAllocMemorySize + 31) & 0xFFFFFFE0, 32);
	}
	memset(m_pfHistHeight, 0, m_nHeightAllocMemorySize);

	if (m_nHeightAllocMemorySize > 0x1FFFF) {
		m_pfHistHeightEx = (float*)(BYTE*)VirtualAlloc(0, m_nHeightAllocMemorySize, MEM_COMMIT, 4);
	}
	else {
		m_pfHistHeightEx = (float*)_aligned_malloc((m_nHeightAllocMemorySize + 31) & 0xFFFFFFE0, 32);
	}
	memset(m_pfHistHeightEx, 0, m_nHeightAllocMemorySize);
}

void CVulpesEngineHistogram::FreeFrameMemory() {

	if (m_pfHistWidth) {
		if (m_nWidthAllocMemorySize > 0x1FFFF) {
			VirtualFree(m_pfHistWidth, 0, MEM_RELEASE);
		}
		else {
			_aligned_free(m_pfHistWidth);
		}
		m_pfHistWidth = nullptr;
	}

	if (m_pfHistWidthEx) {
		if (m_nWidthAllocMemorySize > 0x1FFFF) {
			VirtualFree(m_pfHistWidthEx, 0, MEM_RELEASE);
		}
		else {
			_aligned_free(m_pfHistWidthEx);
		}
		m_pfHistWidthEx = nullptr;
	}

	if (m_pfHistHeight) {
		if (m_nHeightAllocMemorySize > 0x1FFFF) {
			VirtualFree(m_pfHistHeight, 0, MEM_RELEASE);
		}
		else {
			_aligned_free(m_pfHistHeight);
		}
		m_pfHistHeight = nullptr;
	}
	if (m_pfHistHeightEx) {
		if (m_nHeightAllocMemorySize > 0x1FFFF) {
			VirtualFree(m_pfHistHeightEx, 0, MEM_RELEASE);
		}
		else {
			_aligned_free(m_pfHistHeightEx);
		}
		m_pfHistHeightEx = nullptr;
	}
}

int CVulpesEngineHistogram::CalculateHistogram(float* pfHist, int n, float* pf, int* pn) {

	int bStatus = 0;
	float fXmm1Sum = 0.f;
	float fXmm0Sum = 0.f;

	__m128 xmm1 = _mm_set_ps1(0.f);
	__m128 xmm0 = _mm_set_ps1(0.f);
	__m128 xmmSwap = _mm_set_ps1(0.f);
	__m128 xmmStep = _mm_set_ps1(4.f);
	__m128 xmmN = _mm_set_ps1((float)n);

	float fVal[4] = { 0.f,1.f,2.f,3.f };
	__m128 xmmFVal = _mm_load_ps(fVal);

	for (int i = 0; i < n; i += 4) {
		xmmSwap = _mm_div_ps(_mm_load_ps(&pfHist[i]), xmmN);
		xmm1 = _mm_add_ps(_mm_mul_ps(xmmSwap, xmmFVal), xmm1);
		xmm0 = _mm_add_ps(xmmSwap, xmm0);
		xmmFVal = _mm_add_ps(xmmFVal, xmmStep);
	}

	fXmm1Sum = xmm1.m128_f32[0] + xmm1.m128_f32[1] + xmm1.m128_f32[2] + xmm1.m128_f32[3];
	fXmm0Sum = xmm0.m128_f32[0] + xmm0.m128_f32[1] + xmm0.m128_f32[2] + xmm0.m128_f32[3];
	if (fXmm0Sum > 0.f) {
		*pf = fXmm1Sum / fXmm0Sum;
		*pn = (int)(*pf + 0.5f);
		bStatus = 1;
	}
	else {
		*pf = 0.f;
		*pn = 0;
	}
	return bStatus;
}

int CVulpesEngineHistogram::CalculateHistogramSum() {

	int bStatus = 0;
	int bCalWidth = CalculateHistogram(m_pfHistWidth, m_nWidth, &m_fpHistWidthAvg, &m_npHistWidthAvg);
	int bCalHeight = CalculateHistogram(m_pfHistHeight, m_nHeight, &m_fpHistHeightAvg, &m_npHistHeightAvg);

	if (bCalWidth && bCalHeight) {
		memcpy(m_pfHistHeightEx, m_pfHistHeight, m_nHeightAllocMemorySize);
		memcpy(m_pfHistWidthEx, m_pfHistWidth, m_nWidthAllocMemorySize);
		bStatus = 1;
	}
	return bStatus;
}
