#include "StdAfx.h"
#include ".\layermanager.h"

#include "RawImage.h"
#include "Layer.h"
#include "LineImagePainter.h"
#include "ToolBox.h"

#include "Binarizer.h"
#include "DeSaturator.h"
#include "Gaussian.h"
#include "Skeletonizer.h"
#include "TailPruner.h"
#include "LineSegmentor.h"
#include "KneeSplitter.h"
#include "HoleFiller.h"
#include "Thinner.h"
#include "BezierMaker.h"
#include "ForkHandler.h"

CLayerManager::CLayerManager(void)
{
	TRACE("init layermanager\n");
	
	CLayer *layer=new CLayer();
	layer->SetImageProcessor(CDeSaturator::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CGaussian::Instance());
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CBinarizer::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CHoleFiller::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CThinner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CSkeletonizer::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);
	
	layer=new CLayer();
	layer->SetImageProcessor(CTailPruner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CLineSegmentor::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer = new CLayer();
	layer->SetImageProcessor(CForkHandler::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CKneeSplitter::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CBezierMaker::Instance());
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

		if(i<BINARIZER) {
			CBinarizer::Instance()->Init();
		}
	}
}

void CLayerManager::ProcessLayers()
{
	TRACE("ProcessLayers() start\n");
	if(m_Layers.size()) {
		CLayer *layer = m_Layers.at(0);
		CSketchImage *img = layer->GetSketchImage();

		for(UINT i=1; i<m_Layers.size(); i++) {
			TRACE("process layer: %i\n",i);
			layer = m_Layers.at(i);

			layer->Process(img);
			TRACE("process done: %i\n",i);
			img = layer->GetSketchImage();
		}
	}
	TRACE("LayerManager::ProcessLayers() done\n");
	CToolBox::Instance()->UpdateParams();
}

CLayer* CLayerManager::GetLayer(int layer)
{
	return m_Layers.at(layer);
}

Bitmap* CLayerManager::MakeBitmap(void)
{
	CLayer* l = GetLayer(0);

	TRACE("MakeBitmap() start\n");
	int width=l->GetSketchImage()->GetWidth();
	int height=l->GetSketchImage()->GetHeight();

	int SCALE = int(CSkeletonizer::Instance()->GetParam("scale"));

	TRACE("out size %i %i\n",width,height);

	CRawImage<ARGB> dst(width*SCALE,height*SCALE);
	dst.Clear();
	TRACE("dst size %i %i\n",dst.GetWidth(),dst.GetHeight());

	if(GetLayer(BINARIZER)->IsVisible()) {
		TRACE("paint binarizer\n");
		CRawImage<bool> *src = static_cast<CRawImage<bool>*>(GetLayer(HOLEFILLER)->GetSketchImage());
		for(int x=0; x<width*SCALE; x+=SCALE) {
			for(int y=0; y<height*SCALE; y+=SCALE) {
				ARGB p=src->GetPixel(x/SCALE,y/SCALE)?0xffffff:0;
				//ARGB p=CBinarizer::Instance()->m_distanceMap->GetPixel(x/SCALE,y/SCALE);
				if(!p) {
				//if(p)
					for(int i=0; i<SCALE; i++) {
						for(int j=0; j<SCALE; j++) {
							dst.SetPixel(x+i, y+j, 0xff000000 | p);
						}
					}
				}
			}
		}
	} else if(GetLayer(GAUSSIAN)->IsVisible()) {
		CRawImage<unsigned char> *src = static_cast<CRawImage<unsigned char>*>(GetLayer(GAUSSIAN)->GetSketchImage());
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	} else if(GetLayer(DESATURATOR)->IsVisible()) {
		CRawImage<unsigned char> *src = static_cast<CRawImage<unsigned char>*>(GetLayer(GAUSSIAN)->GetSketchImage());
		for(int i=0; i<width*height; i++) {
			ARGB p=src->GetPixel(i);
			dst.SetPixel(i, 0xff000000 | p | p<<8 | p<<16);
		}
	}

	if(GetLayer(THINNER)->IsVisible()) {
		TRACE("paint thinner\n");
		CRawImage<bool> *src = static_cast<CRawImage<bool>*>(GetLayer(THINNER)->GetSketchImage());
		for(int x=0; x<width*SCALE; x+=SCALE) {
			for(int y=0; y<height*SCALE; y+=SCALE) {
				ARGB p=src->GetPixel(x/SCALE,y/SCALE);
				if(p) {
					for(int i=0; i<SCALE; i++) {
						for(int j=0; j<SCALE; j++) {
							ARGB tmp = dst.GetPixel(x+i,y+j);
							dst.SetPixel(x+i, y+j, tmp|0xff00);
						}
					}
				}
			}
		}
	}

	if(GetLayer(SKELETONIZER)->IsVisible()) {
		TRACE("skeletonizer is visible\n");
		CRawImage<ARGB> *tmp = new CRawImage<ARGB>(width*SCALE,height*SCALE);
		tmp->Clear();
		CLineImage *src = static_cast<CLineImage*>(GetLayer(FORKHANDLER)->GetSketchImage());
		CLineImagePainter::Paint(tmp,src);
		for(int i=0; i<width*height; i++) {
			ARGB p=tmp->GetPixel(i);
			if(p) dst.SetPixel(i, p);
		}
		delete tmp;
	}
	if(GetLayer(BEZIERMAKER)->IsVisible()) {
		CRawImage<ARGB> *tmp = new CRawImage<ARGB>(width*SCALE,height*SCALE);
		tmp->Clear();
		CLineImage *src = static_cast<CLineImage*>(GetLayer(BEZIERMAKER)->GetSketchImage());
		CLineImagePainter::Paint(tmp,src);
		for(int i=0; i<width*height; i++) {
			ARGB p=tmp->GetPixel(i);
			if(p) dst.SetPixel(i, p);
		}
		delete tmp;
	}

	if(GetLayer(FORKHANDLER)->IsVisible()) {
		vector<CFPoint> *forks = CForkHandler::Instance()->TForks;
		vector<CFPoint>::iterator iter;

		for(iter = forks->begin(); iter != forks->end(); iter++) {
			CFPoint p = *iter;

			static const ARGB TFORK_COL = 0xff00ff00;

			dst.SetPixel((int)p.x-2, (int)p.y-1, TFORK_COL);
			dst.SetPixel((int)p.x-2, (int)p.y, TFORK_COL);
			dst.SetPixel((int)p.x-2, (int)p.y+1, TFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y-1, TFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y, TFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y+1, TFORK_COL);
			dst.SetPixel((int)p.x-1, (int)p.y-2, TFORK_COL);
			dst.SetPixel((int)p.x, (int)p.y-2, TFORK_COL);
			dst.SetPixel((int)p.x+1, (int)p.y-2, TFORK_COL);
			dst.SetPixel((int)p.x-1, (int)p.y+2, TFORK_COL);
			dst.SetPixel((int)p.x, (int)p.y+2, TFORK_COL);
			dst.SetPixel((int)p.x+1, (int)p.y+2, TFORK_COL);
		}

		forks = CForkHandler::Instance()->YForks;
		for(iter = forks->begin(); iter != forks->end(); iter++) {
			CFPoint p = *iter;
			static const ARGB YFORK_COL = 0xff0000ff;
			dst.SetPixel((int)p.x-2, (int)p.y-1, YFORK_COL);
			dst.SetPixel((int)p.x-2, (int)p.y, YFORK_COL);
			dst.SetPixel((int)p.x-2, (int)p.y+1, YFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y-1, YFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y, YFORK_COL);
			dst.SetPixel((int)p.x+2, (int)p.y+1, YFORK_COL);
			dst.SetPixel((int)p.x-1, (int)p.y-2, YFORK_COL);
			dst.SetPixel((int)p.x, (int)p.y-2, YFORK_COL);
			dst.SetPixel((int)p.x+1, (int)p.y-2, YFORK_COL);
			dst.SetPixel((int)p.x-1, (int)p.y+2, YFORK_COL);
			dst.SetPixel((int)p.x, (int)p.y+2, YFORK_COL);
			dst.SetPixel((int)p.x+1, (int)p.y+2, YFORK_COL);
			TRACE("yfork distance (%i,%i): %i\n",int(p.x),int(p.y),CBinarizer::Instance()->m_distanceMap->GetPixel(int(p.x),int(p.y)));
		}
	}

	TRACE("MakeBitmap() done\n");

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

CLayer* CLayerManager::GetLastLayer(void)
{
	return m_Layers.at(m_Layers.size()-1);
}
