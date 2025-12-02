#pragma once
class CVulpesEngineFaceDetect
{
public:
	CVulpesEngineFaceDetect();
	~CVulpesEngineFaceDetect();
	void SetFile(LPCWSTR str);
	void RunFaceDetect(unsigned char* ptr, int nw, int nh, int bpp, int nQu, float flScale, int MinSize, int maxSize);
	DWORD64 GetCurrentMillisecond();

public:
	int m_nSize;
	RECT  rtList[100];
	int  pHist[100][256];
	bool StopFaceDetect();
};

