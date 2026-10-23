#pragma once
#include <Gui/Overlay/Overlay.hpp>
#include <Includes/Includes.hpp>
#include <Core/SDK/Memory.hpp>
#include <Includes/Utils.hpp>
#include <Core/SDK/SDK.hpp>
#include <Core/Core.hpp>
#include <Gui/gui.hpp>
#include <winternl.h>
#include <windows.h>
#include <dwmapi.h>
#include <tchar.h>
#include <vector>
#include <regex>

#include <Security/AntiCrack.hpp>
#include <Includes/CustomWidgets/Custom.hpp>
#include <Bypass/GeneralCleaner.hpp>
#include <Bypass/Prefetch/Prefetch.hpp>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define debug 0
bool WindowFound = false;

using namespace Core;

auto InitConsoleDebug( ) -> void {
	AllocConsole( );
	FILE* f;
	freopen_s( &f , xorstr( "CONOUT$" ) , xorstr( "w" ) , stdout );
	freopen_s( &f , xorstr( "CONOUT$" ) , xorstr( "w" ) , stderr );
	freopen_s( &f , xorstr( "CONIN$" ) , xorstr( "r" ) , stdin );
}

int APIENTRY WinMain( HINSTANCE hInst , HINSTANCE hInstPrev , PSTR cmdline , int cmdshow ) {
	{
		while ( !g_Variables.g_hGameWindow ) {

			g_Variables.g_hGameWindow = FindWindowA( xorstr( "grcWindow" ) , nullptr );
			if ( g_Variables.g_hGameWindow ) {
				auto WindowInfo = Utils::GetWindowPosAndSize( g_Variables.g_hGameWindow );
				g_Variables.g_vGameWindowSize = WindowInfo.second;
				g_Variables.g_vGameWindowPos = WindowInfo.first;
				g_Variables.g_vGameWindowCenter = { g_Variables.g_vGameWindowSize.x / 2, g_Variables.g_vGameWindowSize.y / 2 };
				break;
			}

		}
		GetWindowThreadProcessId( g_Variables.g_hGameWindow , &g_Variables.ProcIdFiveM );
		Core::SetupOffsets( );
		Gui::cOverlay.Render( );

		return 0;

	}
}

BOOL APIENTRY DllMain( HINSTANCE hInstDLL , DWORD fdwReason , LPVOID lpReserved ) {
	switch ( fdwReason ) {
	case DLL_PROCESS_ATTACH:
		CloseHandle( CreateThread( NULL , 0 , ( LPTHREAD_START_ROUTINE ) WinMain , hInstDLL , 0 , NULL ) );
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}