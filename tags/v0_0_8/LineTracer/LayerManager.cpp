#include "StdAfx.h"
#include "layermanager.h"
#include <process.h>

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

#include "LineTracerView.h"

CLayerManager::CLayerManager(void)
: m_processThread(NULL)
, m_restartProcess(false)
, m_lineTracerView(NULL)
{
	LOG("init layermanager\n");

	CLayer *layer=new CLayer( );
	layer->SetImageProcessor(CDeSaturator::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer(  );
	layer->SetImageProcessor(CGaussian::Instance());
	m_Layers.push_back(layer);

	layer=new CLayer( );
	layer->SetImageProcessor(CBinarizer::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer( );
	layer->SetImageProcessor(CHoleFiller::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer( );
	layer->SetImageProcessor(CThinner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer( );
	layer->SetImageProcessor(CSkeletonizer::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);
	
	layer=new CLayer( );
	layer->SetImageProcessor(CTailPruner::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CLineSegmentor::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer = new CLayer( );
	layer->SetImageProcessor(CForkHandler::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer();
	layer->SetImageProcessor(CKneeSplitter::Instance());
	//layer->SetVisible(true);
	m_Layers.push_back(layer);

	layer=new CLayer( );
	layer->SetImageProcessor(CBezierMaker::Instance());
	layer->SetVisible(true);
	m_Layers.push_back(layer);
}

CLayerManager::~CLayerManager(void)
{
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

void CLayerManager::InvalidateLayers(unsigned int startLayer)
{
	LOG( "InvalidateLayers %i start\n", startLayer );

	for(unsigned int i=startLayer; i<m_Layers.size(); i++) {
		m_Layers.at(i)->SetValid(false);

		if(i<BINARIZER) {
			CBinarizer::Instance()->Init();
		}
	}

	LOG( "InvalidateLayers bye\n" );
}

void CLayerManager::ProcessLayers()
{
	if ( m_processThread != NULL )
	{
		m_restartProcess = true;
		LOG ( "restartProcess = true\n" );
	}
	else 
	{
		LOG( "ProcessLayers() this=%x\n", this );
		m_processThread = AfxBeginThread ( DoProcessLayers, (LPVOID)this );
		LOG ( "m_processThread == %x\n", m_processThread );
	}
}

CLayer* CLayerManager::GetLayer(int layer)
{
	return m_Layers.at(layer);
}

unsigned int CLayerManager::LayerCount(void)
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

UINT CLayerManager::DoProcessLayers(LPVOID pParam)
{
	static int K_ACTIVE_PROCESSES = 0;

	ASSERT ( K_ACTIVE_PROCESSES == 0 );
	K_ACTIVE_PROCESSES++;

	CLogger::Instance()->Activate();
	LOG( "DoProcessLayers() %i enter\n", K_ACTIVE_PROCESSES );

	CLayerManager *l_lm = static_cast<CLayerManager*>(pParam);

	ASSERT ( l_lm->LayerCount() > 0 );

	do {
		l_lm->m_restartProcess = false;

		CLayer *layer = l_lm->GetLayer(0);
		CSketchImage *img = layer->GetSketchImage();

		for(UINT l_activeLayerIndex = 1; 
			(l_activeLayerIndex < l_lm->LayerCount()); 
			l_activeLayerIndex++) 
		{
			CLogger::Instance()->Activate();
			LOG("process layer: %i\n",l_activeLayerIndex);
			layer = l_lm->GetLayer (l_activeLayerIndex);

			layer->Process(img);
		
			CLogger::Instance()->Activate();
			LOG( "process done: %i\n", l_activeLayerIndex );
			LOG ( "m_restartProcess: %x\n", l_lm->m_restartProcess );

			if ( l_lm->m_restartProcess ) {
				break;
			}

			BOOL l_result = PostMessage ( CToolBox::Instance()->m_hWnd, WM_UPDATE_TOOLBOX_DATA_FROM_LAYERS, 0, 0 );
			ASSERT ( l_result != 0 );

			//redraw!
			l_lm->GetLineTracerView()->Invalidate();

			img = layer->GetSketchImage();
		}

	} while ( l_lm->m_restartProcess );

	l_lm->m_restartProcess = false;

	l_lm->m_processThread = NULL;

	LOG("LayerManager::ProcessLayers() done\n");
	
	K_ACTIVE_PROCESSES--;
	return 0;
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

/*void CLayerManager::AbortOldThread(void)
{
	ASSERT ( 0 );
	LOG ( "AbortOldThread() start\n" );

	if ( m_processThread == NULL )
	{
		LOG ( "thread was null - no problem\n") ;
		LOG ( "AbortOldThread() exit\n") ;
		return;
	}

	// tell thread to terminate!
	LOG ( "AbortProcessing()\n" );
	//AbortProcessing();
	HANDLE l_threadHandle = m_processThread->m_hThread;
	LOG ( "WaitForSingleObject() start... %x\n", m_processThread );

	DWORD l_waitResult = ::WaitForSingleObject ( l_threadHandle, 5000L );
	
	LOG ( "waitResult: %x\n", l_waitResult);
	if ( l_waitResult != WAIT_OBJECT_0 )
	{
		// the thread is still running
		ErrorExit(NULL);
	}

	m_processThread = NULL;

	LOG ( "WaitForSingleObject() ...ok! %x\n", m_processThread );

	//ClearAbortProcessingFlag();
	LOG ( "AbortOldThread() bye\n" );
}*/

void CLayerManager::ChangedLayerVisibleState(void)
{
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
	LOG ( "layermanager->DrawAllLayers()" );

	bool l_noLayersVisible = true;

	// paint all visible layers
	for ( unsigned int l_layerIndex = 1; 
		l_layerIndex < LayerCount(); 
		l_layerIndex++ )
	{
		CLayer *l_layer = GetLayer( l_layerIndex );

		if ( l_layer->IsVisible() && l_layer->IsValid() )
		{
			CLogger::Activate();
			LOG ( "painted: " );
			LOG ( (LPCSTR)l_layer->GetName() );	
			LOG ( "\n" );
			l_layer->DrawUsingGraphics ( a_graphics );
			l_noLayersVisible = false;
		}
	}
}
