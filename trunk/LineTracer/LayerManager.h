#pragma once

#include "layer.h"
#include <vector>

using namespace std;

class CLayerManager
{
public:
	~CLayerManager(void);
	static CLayerManager* Instance(void);

	enum LayerTypes { 
		DESATURATOR, 
		GAUSSIAN, 
		BINARIZER, 
		HOLEFILLER, 
		THINNER, 
		SKELETONIZER, 
		TAILPRUNER, 
		LINESEGMENTOR, 
		FORKHANDLER, 
		KNEESPLITTER, 
		BEZIERMAKER,
		INVALID_TYPE
	};

	void InvalidateLayers(unsigned int startLayer=0);
	void ProcessLayers();
	CLayer* GetLayer(int layer);
	unsigned int LayerCount(void);
	void Serialize(CArchive &ar);
	CLayer* GetLastLayer(void);
	void ChangedLayerVisibleState(void);
	//void SetLineTracerView();

private:
	//lint -e{1704} singleton
	CLayerManager(void);
	vector<CLayer*> m_Layers;

	static UINT DoProcessLayers(LPVOID pParam);
	//void AbortOldThread(void);
	CWinThread* m_processThread;
	static const bool K_USE_MULTITHREADING = true;
	
	CView* m_lineTracerView;
public:
	void SetLineTracerView(CView* a_lineTracerView);
private:
	CView* GetLineTracerView(void);

	volatile bool m_restartProcess;
public:
	void DrawAllLayers(Graphics & a_graphics);
};
