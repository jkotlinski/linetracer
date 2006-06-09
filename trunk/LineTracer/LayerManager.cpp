#include "StdAfx.h"
#include "layermanager.h"

#include "Logger.h"
#include "RawImage.h"
#include "Layer.h"
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
#include "AreaContourizer.h"

#include "LineTracerView.h"

using namespace ImageProcessing;
using namespace boost;

static void InvalidateLayers(unsigned int startLayer = 0)
{
	CLayerManager * l_lm = CLayerManager::Instance();
	for(unsigned int i=startLayer; i < l_lm->m_Layers.size(); i++) {
		l_lm->m_Layers.at(i)->SetValid(false);

		if(i< CLayerManager::BINARIZER) {
			CBinarizer::Instance()->Init();
		}
	}
}

CLayerManager::CLayerManager(void)
: m_processThread(NULL)
, m_lineTracerView(NULL)
, m_isProcessing(false)
, m_Layers()
{
	//LOG("init layermanager\n");
	CLayer *layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CDeSaturator::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	/*layer=DEBUG_NEW CLayer(  );
	layer->SetImageProcessor(CGaussian::Instance());
	m_Layers.push_back(layer);*/

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CBinarizer::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CHoleFiller::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(AreaContourizer::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CThinner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CSkeletonizer::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);
	
	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CTailPruner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer();
	layer->SetImageProcessor(CLineSegmentor::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer = DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CForkHandler::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer();
	layer->SetImageProcessor(CKneeSplitter::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=DEBUG_NEW CLayer( );
	layer->SetImageProcessor(CBezierMaker::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	m_processThread = AfxBeginThread ( 
		IPEventLoop, 
		static_cast<LPVOID>( this )
		);
}

CLayerManager::~CLayerManager(void)
{
	m_message_queue_gui_to_ip.PostMsg(Message(MSG_KILL_THREAD,0,0));
	while( !m_message_queue_gui_to_ip.IsEmpty() );
	try {
		for(unsigned int i=0; i<m_Layers.size(); i++) {
			delete m_Layers.at(i);
		}
	} catch (...) {
		try {
			ASSERT ( false );
		} catch (...) {
		}
	}
}

CLayerManager* CLayerManager::Instance() {
    static CLayerManager inst;
    return &inst;
}

CLayer* CLayerManager::GetLayer(int layer)
{
	return m_Layers.at(layer);
}

unsigned int CLayerManager::LayerCount(void)
const
{
	return static_cast<unsigned int> ( m_Layers.size() );
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

void TheProcessing(CProjectSettings * a_project_settings)
{
	CLogger::Instance()->Activate();

	CLayerManager *l_lm = CLayerManager::Instance();
	l_lm->m_isProcessing = true;

	ASSERT ( l_lm->LayerCount() > 0 );

	int l_loopCount = 0;

	ASSERT ( l_loopCount++ < 1000 );

	CLayer *layer = l_lm->GetLayer(0);
	CSketchImage *img = layer->GetSketchImage();
	if ( 0 == img ) {
		return;
	}

	#ifdef _DEBUG
	CMemoryState l_oldMemState;
	l_oldMemState.Checkpoint();
	#endif

	for(UINT l_activeLayerIndex = 1; 
	(l_activeLayerIndex < l_lm->LayerCount()); 
	l_activeLayerIndex++) 
	{
		CLogger::Instance()->Activate();
		TRACE("process layer: %i\n",l_activeLayerIndex);
		layer = l_lm->GetLayer (l_activeLayerIndex);
		layer->Lock();

		if ( layer->IsValid() == false )
		{
			char *l_messageStrBuf = DEBUG_NEW char[100];
			const CString *l_string = layer->GetName();
			const char *l_layerName = (LPCTSTR)(*l_string);
			sprintf(l_messageStrBuf, "Processing %s...", l_layerName);
			l_lm->m_lineTracerView->PostMessage(
				WM_UPDATE_STATUSBAR_WITH_STRING, 
				reinterpret_cast<WPARAM>(l_messageStrBuf), 0);

			layer->Process(*a_project_settings, img);
		}

		if ( !CLayerManager::Instance()->m_message_queue_gui_to_ip.IsEmpty() ) {
			//whooops... more events to process! quit immediately
			layer->Unlock();
			return;
		}

		#ifdef _DEBUG
		CMemoryState l_postProcessLayerMemState;
		l_postProcessLayerMemState.Checkpoint();

		TRACE ( "--- statistics - memory gain after processsing layer %i\n", l_activeLayerIndex );
		CMemoryState l_diffMemState;
		l_diffMemState.Difference(l_oldMemState, l_postProcessLayerMemState);
		l_diffMemState.DumpStatistics();
		l_oldMemState = l_postProcessLayerMemState;
		#endif

		CLogger::Instance()->Activate();
		LOG( "process done: %i\n", l_activeLayerIndex );

		//redraw!
		if ( layer->IsVisible() ) {
			l_lm->GetLineTracerView()->Invalidate( FALSE );
		}

		img = layer->GetSketchImage();

		layer->Unlock();
	}

	LOG("LayerManager::ProcessLayers() done\n");

	char *l_message = DEBUG_NEW char[100];
	strcpy(l_message, "Ready");
	l_lm->m_lineTracerView->PostMessage(
		WM_UPDATE_STATUSBAR_WITH_STRING, 
		reinterpret_cast<WPARAM>(l_message), 0);

	l_lm->m_isProcessing = false;
	l_lm->GetLineTracerView()->Invalidate( FALSE );

	return;
}

UINT CLayerManager::IPEventLoop(LPVOID pParam)
{
	CLayerManager *l_lm = static_cast<CLayerManager*>(pParam);
	static CProjectSettings * l_project_settings = 0;

	for (;;) {
		Message l_message = CLayerManager::Instance()->m_message_queue_gui_to_ip.GetMsg();
		//TRACE ( "got msg %i\n", l_message.GetId() );

		switch ( l_message.GetId() ) {
			case MSG_PROJECT_SETTINGS:
				delete l_project_settings;
				l_project_settings = reinterpret_cast<CProjectSettings*>(l_message.GetWParam());
				break;

			case MSG_INVALIDATE_FROM_LAYER:
				{
					int l_layer_id = l_message.GetLParam();
					TRACE("invalidate from layer %i\n", l_layer_id);
					InvalidateLayers(l_layer_id);
				}

			case MSG_START_PROCESSING:
				TheProcessing(l_project_settings);
				break;

			case MSG_KILL_THREAD:
				AfxEndThread(0);
		}
	}
}

#include <windows.h>
#include ".\layermanager.h"
void ErrorExit(LPTSTR lpszFunction) 
{ 
    TCHAR szBuf[80]; 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    (void) FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    wsprintf(szBuf, 
        "%s failed with error %d: %s", 
        lpszFunction, dw, lpMsgBuf); 
 
	LOG ( "%s\n", szBuf );
    //MessageBox(NULL, szBuf, "Error", MB_OK); 

    (void) LocalFree(lpMsgBuf);
    ExitProcess(dw); 
}

void CLayerManager::SetLineTracerView(CView* a_lineTracerView)
{
	ASSERT ( a_lineTracerView != NULL );

	m_lineTracerView = a_lineTracerView;
}

CView* CLayerManager::GetLineTracerView(void)
{
	return m_lineTracerView;
}

void CLayerManager::DrawAllLayers(Graphics & a_graphics)
{
	LOG ( "layermanager->DrawAllLayers()\n" );
	
	if ( IsProcessing() == false )
	{
		//clear background
		SolidBrush l_whiteBrush (Color(255, 255, 255, 255));
		(void) a_graphics.FillRectangle(&l_whiteBrush, 0, 0, 
			GetLayer(0)->GetImageWidth(),
			GetLayer(0)->GetImageHeight() 
			);
	}

	// paint all visible layers
	for ( unsigned int l_layerIndex = 1; 
		l_layerIndex < LayerCount(); 
		l_layerIndex++ )
	{
		CLayer *l_layer = GetLayer( l_layerIndex );
		l_layer->Lock();

		if ( l_layer->IsVisible() && l_layer->IsValid() )
		{
			if ( IsProcessing() && l_layer->HasBeenDrawn() )
			{
				l_layer->Unlock();
				continue;
			}
			l_layer->DrawUsingGraphics ( a_graphics );
		}
		l_layer->Unlock();
	}
}

void CLayerManager::SetOriginalLayerVisibility(bool a_isVisible)
{
	//GetLayer(HOLEFILLER)->SetVisible(a_isVisible);
	GetLayer(BINARIZER)->SetVisible(a_isVisible);
}

void CLayerManager::SetVectorLayerVisibility(bool a_isVisible)
{
	GetLastLayer()->SetVisible(a_isVisible);
}

bool CLayerManager::IsProcessing(void) 
const
{
	return m_isProcessing;
}

