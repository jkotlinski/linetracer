#pragma once

#include "layer.h"
#include <vector>

using namespace std;

class CLayerManager
{
public:
	~CLayerManager(void);
	static CLayerManager* Instance(void);

	enum layer { DECOLORIZER, GAUSSIAN, BINARIZER, SKELETONIZER };

protected:
	CLayerManager(void);

	vector<CLayer*> m_Layers;

public:
	void InvalidateLayers(unsigned int startLayer=0);
	void ProcessLayers(Bitmap* inputBitmap);
	CLayer* GetLayer(int layer);
	Bitmap* MakeBitmap(void);
	int Layers(void);
	void Serialize(CArchive &ar);
};
