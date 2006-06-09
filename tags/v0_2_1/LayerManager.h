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
		//GAUSSIAN, 
		BINARIZER, 
		HOLEFILLER,
		AREA_CONTOURIZER,
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
	unsigned int LayerCount(void) const;
	void Serialize(CArchive &ar);
	CLayer* GetLastLayer(void);
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
private:
	volatile bool m_isProcessing;
public:
	void SetOriginalLayerVisibility(bool a_isVisible);
	void SetVectorLayerVisibility(bool a_isVisible);
	bool IsProcessing(void) const;
	void ResetProcessThread(void);
};

static volatile bool m_stop_processing;
static CCriticalSection m_critical_layerchange_section;
