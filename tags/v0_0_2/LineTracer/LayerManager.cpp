#include "StdAfx.h"
#include ".\layermanager.h"

#include "RawImage.h"
#include "Layer.h"
#include "LineImagePainter.h"

#include "Binarizer.h"
#include "DeColorizer.h"
#include "Gaussian.h"
#include "Skeletonizer.h"

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
    static CLayerManager inst;
    return &inst;
}

void CLayerManager::InvalidateLayers(unsigned int startLayer)
{
	for(unsigned int i=startLayer; i<m_Layers.size(); i++) {
		m_Layers.at(i)->SetValid(false);
	}
}

void CLayerManager::ProcessLayers(Bitmap* inputBitmap)
{
	CLayer *layer;
	CSketchImage *inputBmp = new CRawImage(inputBitmap);
	CSketchImage *newBmp = inputBmp;

	for(UINT i=0; i<m_Layers.size(); i++) {
		TRACE("process layer: %i\n",i);
		layer = m_Layers.at(i);

		layer->Process(newBmp);
		newBmp = layer->GetSketchImage();
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

	int width=l->GetSketchImage()->GetWidth();
	int height=l->GetSketchImage()->GetHeight();

	CRawImage dst(width,height);
	dst.Clear();

	if(GetLayer(BINARIZER)->IsVisible()) {
		CRawImage *src = static_cast<CRawImage*>(GetLayer(BINARIZER)->GetSketchImage());
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	} else if(GetLayer(GAUSSIAN)->IsVisible()) {
		CRawImage *src = static_cast<CRawImage*>(GetLayer(GAUSSIAN)->GetSketchImage());
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	}

	if(GetLayer(SKELETONIZER)->IsVisible()) {
		CLineImagePainter::Paint(&dst,static_cast<CLineImage*>(GetLayer(SKELETONIZER)->GetSketchImage()));
	}

	return dst.MakeBitmap();
}

int CLayerManager::Layers(void)
{
	return (int)m_Layers.size();
}

void CLayerManager::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		for(unsigned int i=0; i<m_Layers.size(); i++) {
			ar << GetLayer(i)->IsVisible();
		}
	}
	else
	{
		// TODO: add loading code here
		for(unsigned int i=0; i<m_Layers.size(); i++) {
			bool b;
			ar >> b;
			GetLayer(i)->SetVisible(b);
		}
	}

}
