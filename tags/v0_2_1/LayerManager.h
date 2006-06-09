#pragma once

#include "layer.h"
#include <vector>
#include "MessageQueue.h"
#include "ProjectSettings.h"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace Messaging;

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

	CLayer* GetLayer(int layer);
	unsigned int LayerCount(void) const;
	void Serialize(CArchive &ar);
	CLayer* GetLastLayer(void);
	//void SetLineTracerView();

public:
	vector<CLayer*> m_Layers;
private:
	//lint -e{1704} singleton
	CLayerManager(void);

	static UINT IPEventLoop(LPVOID pParam);
	//void AbortOldThread(void);
	CWinThread* m_processThread;
	static const bool K_USE_MULTITHREADING = true;
	
public:
	CView* m_lineTracerView;
	void SetLineTracerView(CView* a_lineTracerView);
	CView* GetLineTracerView(void);

public:
	void DrawAllLayers(Graphics & a_graphics);
	bool m_isProcessing;

public:
	MessageQueue m_message_queue_gui_to_ip;

	void SetOriginalLayerVisibility(bool a_isVisible);
	void SetVectorLayerVisibility(bool a_isVisible);
	bool IsProcessing(void) const;
	//void ResetProcessThread(void);
};
