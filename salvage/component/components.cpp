//--------------------------------------------------------------------------------------
// File: components.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "file_system.h"
#include "hw.h"
#include "material.h"
#include "model_structs.h"
#include "shader.h"
#include <float.h>
#include "world_db.h"

#include "game_object.h"
#include "components_basic.h"
#include "components_light.h"
#include "vectormath/cpp/vectormath_aos.h"
#include "faff.h"
#include "material_manager.h"
#include "shader_manager.h"
#include "technique.h"
#include "console.h"
#include "cvar.h"
#include "input.h"

#include "anim_structs.h"

#include "renderpass_default.h"
#include "renderpass_lightprepass.h"
RenderPass *g_renderPasses[ _RP_End ] = { 0 };
static int outkey[256] = { 0 };

int Win32Translate( int c )
{
	static int inited = false;
	if ( !inited )
	{
		outkey['Q'] = in::KEY_Q;
		outkey['W'] = in::KEY_W;
		outkey['E'] = in::KEY_E;
		outkey['R'] = in::KEY_R;
		outkey['T'] = in::KEY_T;
		outkey['Y'] = in::KEY_Y;
		outkey['U'] = in::KEY_U;
		outkey['I'] = in::KEY_I;
		outkey['O'] = in::KEY_O;
		outkey['P'] = in::KEY_P;
		outkey['A'] = in::KEY_A;
		outkey['S'] = in::KEY_S;
		outkey['D'] = in::KEY_D;
		outkey['F'] = in::KEY_F;
		outkey['G'] = in::KEY_G;
		outkey['H'] = in::KEY_H;
		outkey['J'] = in::KEY_J;
		outkey['K'] = in::KEY_K;
		outkey['L'] = in::KEY_L;
		outkey['Z'] = in::KEY_Z;
		outkey['X'] = in::KEY_X;
		outkey['C'] = in::KEY_C;
		outkey['V'] = in::KEY_V;
		outkey['B'] = in::KEY_B;
		outkey['N'] = in::KEY_N;
		outkey['M'] = in::KEY_M;
		outkey[VK_UP] = in::KEY_UP;
		outkey[VK_DOWN] = in::KEY_DOWN;
		outkey[VK_LEFT] = in::KEY_LEFT;
		outkey[VK_RIGHT] = in::KEY_RIGHT;
		inited = true;
	}
	return outkey[ c ];
}

static CVar r_render( "r_render", "1" );

//#define DEBUG_VS   // Uncomment this line to debug D3D9 vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug D3D9 pixel shaders 


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Direct3D 9 resources
ID3DXFont*                  g_pFont9 = NULL;
ID3DXSprite*                g_pSprite9 = NULL;

WorldDB						g_worlddb;
Console						*g_console;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext );
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D9LostDevice( void* pUserContext );
void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

void InitApp();
void RenderText();

#if 0
template< class C, class T >
void GatherOfType( const C &container, std::vector<T*> &gather )
{
	C::const_iterator c = container.begin();
	while ( c != container.end() )
	{
		(*c)->FindComponentsChildren( gather );
		c++;
	}
}
#endif

int memHook(int id, void *mem, size_t size, int use, long requestid, const unsigned char *, int)
{
	if ( requestid == 18538 )
	{
		printf("");
	}
	return 1;
}
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetAllocHook( memHook );
#endif

	fs::Init();


    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"components" );
    DXUTCreateDevice( true, 1280, 720 );
    DXUTMainLoop(); // Enter into the DXUT render loop


	g_worlddb.Purge();

	for (int j=_RP_Start; j<_RP_End; j++)
	{
		delete g_renderPasses[j];
		g_renderPasses[j] = NULL;
	}
	delete g_console;
	g_console = NULL;

	ResourceManager::PurgeAll();
	ResourceManager::ShutdownAll();
	FrameAllocDestroy();
	hw::Deinit();
	fs::Deinit();
	cvarManager()->Purge();

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );


    g_SampleUI.SetCallback( OnGUIEvent );

	FrameAllocInit( 4 * 1024 * 1024 );
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
    g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                      D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback defined by this app, so reject any device that 
    // doesn't support at least ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );

    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &g_pFont9 ) );

	hw::Init( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

	g_console = new Console();


	g_renderPasses[RP_LightPrepass] = new RenderPassLightPrePass;
	g_renderPasses[RP_LightPrepass_Lights] = new RenderPassLightPrePassLights;
	g_renderPasses[RP_LightPrepass_LitOpaque] = new RenderPassLightPrePassOpaque;
	g_renderPasses[RP_LitOpaque] = new RenderPassDefault;
	g_renderPasses[RP_Opaque] = new RenderPassDefault;
	g_renderPasses[RP_Transparent] = new RenderPassDefault;
	g_renderPasses[RP_LightPrepass_Stereoscopic] = new RenderPassLightPrePassStereoscopic;
	g_renderPasses[RP_Postprocess] = new RenderPassDefault;
	g_renderPasses[RP_Debug] = new RenderPassDefault;
	g_renderPasses[RP_Debug2D] = new RenderPassDefault;

	g_worlddb.Init();
	materialManager()->Load( "showtangents.material" );
	g_worlddb.Load( "testing_large.xml" );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice,
                                    const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

    if( g_pFont9 ) V_RETURN( g_pFont9->OnResetDevice() );

    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite9 ) );
    g_pTxtHelper = new CDXUTTextHelper( g_pFont9, g_pSprite9, NULL, NULL, 15 );

    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 350 );
    g_SampleUI.SetSize( 170, 300 );

	int slot = hw::GetParamSlot( "screenDim", 1 );
	hw::SetParamInSlot( slot, (float)pBackBufferSurfaceDesc->Width, (float)pBackBufferSurfaceDesc->Height, 1.f/pBackBufferSurfaceDesc->Width, 1.f/pBackBufferSurfaceDesc->Height );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	int count = 5;
	static double err = 1./30.;
	err += fElapsedTime;
	while ( err >= 1./30. && count )
	{
		g_worlddb.Update( ePREUPDATE );
		g_worlddb.TransformUpdate();
		g_worlddb.Update( eUPDATE );
		g_worlddb.Update( ePOSTUPDATE );
		err -= 1./30.;
		count--;
	}

	if ( count == 0 )
	{
		err = 0.;
	}
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		g_worlddb.Render( g_renderPasses );

		RenderCommand rcmds[ 2 * 1024 ];
		memset( rcmds, 0, sizeof(rcmds) );
		int rcmdsCount = 0;
		rcmdsCount += g_console->AddRenderCommands( &rcmds[rcmdsCount] );

		for (int i=0; i<rcmdsCount; i++)
		{
			const RenderCommand &rc = rcmds[i];
			g_renderPasses[rc.passIdx]->AddRenderCommand( &rcmds[i] );
		}

		std::vector< Light* > lightsEmpty;
		for (int j=_RP_Start2D; j<_RP_End; j++)
		{
			if ( g_renderPasses[j] == NULL )
			{
				continue;
			}
			DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"RP" ); // These events are to help PIX identify what the code is doing
			g_renderPasses[j]->Render( Vectormath::Aos::Matrix4::identity(), Vectormath::Aos::Matrix4::identity(), lightsEmpty );
			DXUT_EndPerfEvent();
		}

		for (int j=_RP_Start; j<_RP_End; j++)
		{
			if ( g_renderPasses[j] == NULL )
			{
				continue;
			}
			g_renderPasses[j]->Empty();
		}

        DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" ); // These events are to help PIX identify what the code is doing
        RenderText();
        V( g_SampleUI.OnRender( fElapsedTime ) );
        DXUT_EndPerfEvent();

        V( pd3dDevice->EndScene() );
    }

	FrameAllocNext();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	if ( uMsg == WM_CHAR  )
	{
		if ( (lParam & (1<<31)) == 0 )
		{
			if ( wParam == '`' )
			{
				g_console->Toggle();
				in::ResetKeys();
			} else
			{
				g_console->KeyPress( (char)wParam );
			}
		}
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if ( !g_console->IsEnabled() )
	{
		in::SetKeyState( Win32Translate(nChar), bKeyDown ? 1 : 0 );
	}
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
    }
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont9 ) g_pFont9->OnLostDevice();
    SAFE_RELEASE( g_pSprite9 );
    SAFE_DELETE( g_pTxtHelper );
}



//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE( g_pFont9 );
}


