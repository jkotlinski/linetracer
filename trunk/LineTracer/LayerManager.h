/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
