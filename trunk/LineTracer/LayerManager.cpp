#include "StdAfx.h"
#include ".\layermanager.h"

#include "RawImage.h"
#include "Layer.h"

#include "Binarizer.h"
#include "DeColorizer.h"
#include "Gaussian.h"
#include "Skeletonizer.h"

CLayerManager* CLayerManager::_instance = 0;

CLayerManager::CLayerManager(void)
{
	CLayer *layer=new CLayer();
	layer->SetImageProcessor(CDeColorizer::Instance());
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CGaussian::Instance());
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CBinarizer::Instance());
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CSkeletonizer::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

}

CLayerManager::~CLayerManager(void)
{
	for(unsigned int i=0; i<m_Layers.size(); i++) {
		delete m_Layers.at(i);
	}
}

CLayerManager* CLayerManager::Instance() {
	if(!_instance) _instance = new CLayerManager;

	return _instance;
}

void CLayerManager::InvalidateLayers(unsigned int startLayer)
{
	for(unsigned int i=startLayer; i<m_Layers.size(); i++) {
		m_Layers.at(i)->SetValid(false);
	}
}

void CLayerManager::ProcessLayers(Bitmap* inputBitmap)
{
	CRawImage *inputBmp = new CRawImage(inputBitmap);
	CRawImage *newBmp = inputBmp;

	for(UINT i=0; i<m_Layers.size(); i++) {
		CLayer *layer = m_Layers.at(i);

		layer->Process(newBmp);
		newBmp = layer->GetRawImage();
	}
	delete inputBmp;
}

CLayer* CLayerManager::GetLayer(int layer)
{
	return m_Layers.at(layer);
}

Bitmap* CLayerManager::MakeBitmap(void)
{
	CLayer* l = GetLayer(0);

	int width=l->GetRawImage()->GetWidth();
	int height=l->GetRawImage()->GetHeight();

	CRawImage dst(width,height);
	dst.Clear();

	if(GetLayer(BINARIZER)->IsVisible()) {
		CRawImage *src = GetLayer(BINARIZER)->GetRawImage();
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	} else if(GetLayer(GAUSSIAN)->IsVisible()) {
		CRawImage *src = GetLayer(GAUSSIAN)->GetRawImage();
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	}

	if(GetLayer(SKELETONIZER)->IsVisible()) {
		CRawImage *src = GetLayer(SKELETONIZER)->GetRawImage();
		
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);

			if(p) {
				dst.SetPixel(i, 0xff000000 | p);
			}
		}
	}

	return dst.MakeBitmap();
}

int CLayerManager::Layers(void)
{
	return (int)m_Layers.size();
}
