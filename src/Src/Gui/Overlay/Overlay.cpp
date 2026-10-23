#pragma once
#include "Overlay.hpp"
#include <Includes/ImGui/Files/imgui_freetype.h>
#include <Includes/CustomWidgets/Notify.hpp>
#include <Includes/ImGui/Images.hpp>
#include <Includes/ImGui/Fonts.hpp>
#include <Core/Core.hpp>
#include <Gui/Gui.hpp>
#include <dwmapi.h>
#include <tchar.h>
#include <thread>

//#define FULLSCREEN_EX

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Gui
{
	typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

	inline HHOOK hKeyboardHook;

	inline LRESULT CALLBACK KeyboardCallBack(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode >= 0)
		{
			//	if ( GetForegroundWindow( ) == g_Variables.g_hCheatWindow )
				//{
			KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
			PostMessage(g_Variables.g_hCheatWindow, wParam, pKeyboard->vkCode, 0);
			//}
		}
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}

	void Overlay::Render()
	{

#ifdef FULLSCREEN_EX
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		CreateWindowInBand pCreateWindowInBand = reinterpret_cast<CreateWindowInBand>(GetProcAddress(LoadLibraryA(xorstr("user32.dll")), xorstr("CreateWindowInBand")));
		PrepareForUIAccess();
#endif // FULLSCREEN_EX

		WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, Overlay::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L" ", NULL };
		ATOM RegClass = RegisterClassExW(&wc);

#ifdef FULLSCREEN_EX
		g_Variables.g_hCheatWindow = pCreateWindowInBand(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, RegClass, L" ", WS_POPUP, static_cast<int>(g_Variables.g_vGameWindowPos.x), static_cast<int>(g_Variables.g_vGameWindowPos.y), static_cast<int>(g_Variables.g_vGameWindowSize.x), static_cast<int>(g_Variables.g_vGameWindowSize.y), NULL, NULL, wc.hInstance, NULL, 2/*ZBID_UIACCESS*/);
#else
		g_Variables.g_hCheatWindow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, wc.lpszClassName, L" ", WS_POPUP, g_Variables.g_vGameWindowPos.x, g_Variables.g_vGameWindowPos.y, g_Variables.g_vGameWindowSize.x, g_Variables.g_vGameWindowSize.y, NULL, NULL, wc.hInstance, NULL);
#endif // FULLSCREEN_EX		

		SetLayeredWindowAttributes(g_Variables.g_hCheatWindow, RGB(0, 0, 0), 255, LWA_ALPHA);
		MARGINS Margin = { g_Variables.g_vGameWindowPos.x, g_Variables.g_vGameWindowPos.y, g_Variables.g_vGameWindowSize.x,g_Variables.g_vGameWindowSize.y };
		DwmExtendFrameIntoClientArea(g_Variables.g_hCheatWindow, &Margin);

		if (!CreateDeviceD3D(g_Variables.g_hCheatWindow))
		{
			CleanupDeviceD3D();
			UnregisterClassW(wc.lpszClassName, wc.hInstance);
			return;
		}


		SetWindowDisplayAffinity(g_Variables.g_hCheatWindow, WDA_EXCLUDEFROMCAPTURE);

		ShowWindow(g_Variables.g_hCheatWindow, SW_SHOWDEFAULT);
		UpdateWindow(g_Variables.g_hCheatWindow);


		ImGui::CreateContext();

		Style();
		Fonts();

		ImGui_ImplWin32_Init(g_Variables.g_hCheatWindow);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

#ifdef FULLSCREEN_EX
		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallBack, NULL, 0);
#endif // FULLSCREEN_EX

		Core::StartThreads();
		if (!Core::ThreadsStarted) return;

		std::thread([&]() { NotifyManager::Send(xorstr("Injected!"), 4000); }).detach();

		static RECT old_rc;
		ZeroMemory(&Message, sizeof(MSG));

		while (Message.message != WM_QUIT)
		{
			if (PeekMessage(&Message, g_Variables.g_hCheatWindow, 0, 0, PM_REMOVE)) {
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
			else {
				// PeekMessage nao bloqueia. Sem esta espera o loop desenha o mais rapido
				// que a CPU permitir, mesmo com o menu fechado - foi o que deixava a
				// maquina lenta e, depois de um tempo, o processo sem responder.
				// Quando o VSync esta ligado o Present ja limita; com ele desligado, nao.
				if (g_Config.General->VSync)
					WaitMessage();
				else
					Sleep(1);
			}

			HWND ActiveWindow = GetForegroundWindow();

			if (GetAsyncKeyState(g_Config.General->MenuKey) & 1) {

				if (g_MenuInfo.IsOpen == false) {
					if (ActiveWindow == g_Variables.g_hGameWindow) {
						g_MenuInfo.IsOpen = true;

#ifdef FULLSCREEN_EX
						SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
#else
						SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
#endif // FULLSCREEN_EX		
					}
					else {
						SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
					}
				}
				else {
					g_MenuInfo.IsOpen = false;
					SetWindowLong(g_Variables.g_hCheatWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
				}
			}

			static float BgAlpha = 0.f;
			BgAlpha = ImLerp(BgAlpha, g_MenuInfo.IsOpen ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 8);

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame();
			{


				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, BgAlpha);

				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(g_Variables.g_vGameWindowSize.x, g_Variables.g_vGameWindowSize.y), ImColor(0.f, 0.f, 0.f, BgAlpha >= 0.4f ? 0.4f : BgAlpha));
				Gui::Rendering();

				ImGui::PopStyleVar();
			}
			ImGui::EndFrame();

			const float ClearColor[4] = { 0 };
			g_pd3dDeviceContext->OMSetRenderTargets(1U, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, ClearColor);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_pSwapChain->Present(g_Config.General->VSync, 0U); //VSync
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		DestroyWindow(g_Variables.g_hCheatWindow);
		UnregisterClassW(wc.lpszClassName, wc.hInstance);

#ifdef FULLSCREEN_EX
		UnhookWindowsHookEx(hKeyboardHook);
#endif // FULLSCREEN_EX
		return;
	}

	void Overlay::Fonts()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.IniFilename = nullptr;

		ImFontConfig Cfg;
		Cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
		Cfg.FontDataOwnedByAtlas = false;

		ImFontConfig DrawCfg;
		DrawCfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_Bitmap;
		DrawCfg.FontDataOwnedByAtlas = false;


		g_Variables.m_FontBig = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 20.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
		g_Variables.m_FontBigSmall = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 18.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
		g_Variables.m_FontNormal = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 18.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
		g_Variables.m_FontSecundary = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 16.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
		g_Variables.m_FontSmaller = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 14.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
		g_Variables.m_ChangeLogFont = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 12.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());

		g_Variables.m_DrawFont = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ), 12.f, &DrawCfg, io.Fonts->GetGlyphRangesCyrillic());

		static const ImWchar FontAwesomeRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		static const ImWchar FontAwesomeRangesBrands[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };

		ImFontConfig FontAwesomeConfig;
		FontAwesomeConfig.PixelSnapH = true;
		FontAwesomeConfig.GlyphMinAdvanceX = 19;

		g_Variables.FontAwesomeSolid = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Solid_compressed_data, sizeof(FontAwesome6Solid_compressed_size), 19, &FontAwesomeConfig, FontAwesomeRanges);
		g_Variables.FontAwesomeSolidSmall = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Solid_compressed_data, sizeof(FontAwesome6Solid_compressed_size), 16, &FontAwesomeConfig, FontAwesomeRanges);
		g_Variables.FontAwesomeRegular = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Regular_compressed_data, sizeof(FontAwesome6Regular_compressed_size), 19, &FontAwesomeConfig, FontAwesomeRanges);

		g_Variables.m_Expand = io.Fonts->AddFontFromMemoryTTF(expand_binary, sizeof(expand_binary), 11.f, &Cfg, io.Fonts->GetGlyphRangesCyrillic());
	}

	void Overlay::Style()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle* style = &ImGui::GetStyle();
		{
			style->WindowPadding = ImVec2(0, 0);
			style->WindowBorderSize = 0.f;
			style->ItemSpacing = ImVec2(12, 12);
			style->ScrollbarSize = 4.f;
		}

		if (g_Variables.Logo == nullptr)
		{
			Texture::LoadFromMemory( g_pd3dDevice , logo_binary , sizeof( logo_binary ) , &g_Variables.Logo );

			Texture::LoadFromMemory( g_pd3dDevice , decor_background , sizeof( decor_background ) , &g_Variables.decor_background );
			//WeaponsIcons
			Overlay::LoadWeaponIcons();
		}
	}

	bool Overlay::CreateDeviceD3D(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	void Overlay::CleanupDeviceD3D()
	{
		CleanupRenderTarget();

		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	void Overlay::CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	void Overlay::CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

	LRESULT CALLBACK Overlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg) {
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
				Overlay::CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				Overlay::CreateRenderTarget();
			}
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
			break;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	DWORD Overlay::GetWinLogonToken(DWORD dwSessionId, DWORD dwDesiredAccess, PHANDLE phToken)
	{
		DWORD dwErr;
		PRIVILEGE_SET ps;

		ps.PrivilegeCount = 1;
		ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

		if (LookupPrivilegeValue(NULL, SE_TCB_NAME, &ps.Privilege[0].Luid)) {
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (INVALID_HANDLE_VALUE != hSnapshot) {
				BOOL bCont, bFound = FALSE;
				PROCESSENTRY32 pe;

				pe.dwSize = sizeof(pe);
				dwErr = ERROR_NOT_FOUND;

				for (bCont = Process32First(hSnapshot, &pe); bCont; bCont = Process32Next(hSnapshot, &pe)) {
					HANDLE hProcess;

					if (0 != _tcsicmp(pe.szExeFile, xorstr("winlogon.exe"))) {
						continue;
					}

					hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
					if (hProcess) {
						HANDLE hToken;
						DWORD dwRetLen, sid;

						if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) {
							BOOL fTcb;

							if (PrivilegeCheck(hToken, &ps, &fTcb) && fTcb) {
								if (GetTokenInformation(hToken, TokenSessionId, &sid, sizeof(sid), &dwRetLen) && sid == dwSessionId) {
									bFound = TRUE;
									if (DuplicateTokenEx(hToken, dwDesiredAccess, NULL, SecurityImpersonation, TokenImpersonation, phToken)) {
										dwErr = ERROR_SUCCESS;
									}
									else {
										dwErr = GetLastError();
									}
								}
							}
							CloseHandle(hToken);
						}
						CloseHandle(hProcess);
					}

					if (bFound) break;
				}

				CloseHandle(hSnapshot);
			}
			else {
				dwErr = GetLastError();
			}
		}
		else {
			dwErr = GetLastError();
		}


		return dwErr;
	}

	DWORD Overlay::CreateUIAccessToken(PHANDLE phToken)
	{
		DWORD dwErr;
		HANDLE hTokenSelf;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hTokenSelf)) {
			DWORD dwSessionId, dwRetLen;

			if (GetTokenInformation(hTokenSelf, TokenSessionId, &dwSessionId, sizeof(dwSessionId), &dwRetLen)) {
				HANDLE hTokenSystem;

				dwErr = GetWinLogonToken(dwSessionId, TOKEN_IMPERSONATE, &hTokenSystem);
				if (ERROR_SUCCESS == dwErr) {
					if (SetThreadToken(NULL, hTokenSystem)) {
						if (DuplicateTokenEx(hTokenSelf, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT, NULL, SecurityAnonymous, TokenPrimary, phToken)) {
							BOOL bUIAccess = TRUE;

							if (!SetTokenInformation(*phToken, TokenUIAccess, &bUIAccess, sizeof(bUIAccess))) {
								dwErr = GetLastError();
								CloseHandle(*phToken);
							}
						}
						else {
							dwErr = GetLastError();
						}
						RevertToSelf();
					}
					else {
						dwErr = GetLastError();
					}
					CloseHandle(hTokenSystem);
				}
			}
			else {
				dwErr = GetLastError();
			}

			CloseHandle(hTokenSelf);
		}
		else {
			dwErr = GetLastError();
		}

		return dwErr;
	}

	BOOL Overlay::CheckForUIAccess(DWORD* pdwErr, DWORD* pfUIAccess)
	{
		BOOL result = FALSE;
		HANDLE hToken;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
			DWORD dwRetLen;

			if (GetTokenInformation(hToken, TokenUIAccess, pfUIAccess, sizeof(*pfUIAccess), &dwRetLen)) {
				result = TRUE;
			}
			else {
				*pdwErr = GetLastError();
			}
			CloseHandle(hToken);
		}
		else {
			*pdwErr = GetLastError();
		}

		return result;
	}

	DWORD Overlay::PrepareForUIAccess()
	{
		DWORD dwErr;
		HANDLE hTokenUIAccess;
		DWORD fUIAccess;

		if (CheckForUIAccess(&dwErr, &fUIAccess)) {
			if (fUIAccess) {
				dwErr = ERROR_SUCCESS;
			}
			else {
				dwErr = CreateUIAccessToken(&hTokenUIAccess);
				if (ERROR_SUCCESS == dwErr) {
					STARTUPINFO si;
					PROCESS_INFORMATION pi;

					GetStartupInfo(&si);
					if (CreateProcessAsUser(hTokenUIAccess, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
						CloseHandle(pi.hProcess), CloseHandle(pi.hThread);
						ExitProcess(0);
					}
					else {
						dwErr = GetLastError();
					}

					CloseHandle(hTokenUIAccess);
				}
			}
		}

		return dwErr;
	}

	void Overlay::LoadWeaponIcons() {
		{
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::gadget_parachute , sizeof(FiveM_Weapons::gadget_parachute) , &g_Variables.FiveM_WeaponsPic.gadget_parachute );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_advancedrifle , sizeof(FiveM_Weapons::weapon_advancedrifle) , &g_Variables.FiveM_WeaponsPic.weapon_advancedrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_appistol , sizeof(FiveM_Weapons::weapon_appistol) , &g_Variables.FiveM_WeaponsPic.weapon_appistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_assaultrifle , sizeof(FiveM_Weapons::weapon_assaultrifle) , &g_Variables.FiveM_WeaponsPic.weapon_assaultrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_assaultrifle_mk2 , sizeof(FiveM_Weapons::weapon_assaultrifle_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_assaultrifle_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_assaultshotgun , sizeof(FiveM_Weapons::weapon_assaultshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_assaultshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_assaultsmg , sizeof(FiveM_Weapons::weapon_assaultsmg) , &g_Variables.FiveM_WeaponsPic.weapon_assaultsmg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_autoshotgun , sizeof(FiveM_Weapons::weapon_autoshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_autoshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_ball , sizeof(FiveM_Weapons::weapon_ball) , &g_Variables.FiveM_WeaponsPic.weapon_ball );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bat , sizeof(FiveM_Weapons::weapon_bat) , &g_Variables.FiveM_WeaponsPic.weapon_bat );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_battleaxe , sizeof(FiveM_Weapons::weapon_battleaxe) , &g_Variables.FiveM_WeaponsPic.weapon_battleaxe );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bottle , sizeof(FiveM_Weapons::weapon_bottle) , &g_Variables.FiveM_WeaponsPic.weapon_bottle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bullpuprifle , sizeof(FiveM_Weapons::weapon_bullpuprifle) , &g_Variables.FiveM_WeaponsPic.weapon_bullpuprifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bullpuprifle_mk2 , sizeof(FiveM_Weapons::weapon_bullpuprifle_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_bullpuprifle_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bullpupshotgun , sizeof(FiveM_Weapons::weapon_bullpupshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_bullpupshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_bzgas , sizeof(FiveM_Weapons::weapon_bzgas) , &g_Variables.FiveM_WeaponsPic.weapon_bzgas );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_carbinerifle , sizeof(FiveM_Weapons::weapon_carbinerifle) , &g_Variables.FiveM_WeaponsPic.weapon_carbinerifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_carbinerifle_mk2 , sizeof(FiveM_Weapons::weapon_carbinerifle_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_carbinerifle_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_ceramicpistol , sizeof(FiveM_Weapons::weapon_ceramicpistol) , &g_Variables.FiveM_WeaponsPic.weapon_ceramicpistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_combatmg , sizeof(FiveM_Weapons::weapon_combatmg) , &g_Variables.FiveM_WeaponsPic.weapon_combatmg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_combatmg_mk2 , sizeof(FiveM_Weapons::weapon_combatmg_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_combatmg_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_combatpdw , sizeof(FiveM_Weapons::weapon_combatpdw) , &g_Variables.FiveM_WeaponsPic.weapon_combatpdw );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_combatpistol , sizeof(FiveM_Weapons::weapon_combatpistol) , &g_Variables.FiveM_WeaponsPic.weapon_combatpistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_combatshotgun , sizeof(FiveM_Weapons::weapon_combatshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_combatshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_compactlauncher , sizeof(FiveM_Weapons::weapon_compactlauncher) , &g_Variables.FiveM_WeaponsPic.weapon_compactlauncher );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_compactrifle , sizeof(FiveM_Weapons::weapon_compactrifle) , &g_Variables.FiveM_WeaponsPic.weapon_compactrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_crowbar , sizeof(FiveM_Weapons::weapon_crowbar) , &g_Variables.FiveM_WeaponsPic.weapon_crowbar );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_dagger , sizeof(FiveM_Weapons::weapon_dagger) , &g_Variables.FiveM_WeaponsPic.weapon_dagger );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_dbshotgun , sizeof(FiveM_Weapons::weapon_dbshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_dbshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_doubleaction , sizeof(FiveM_Weapons::weapon_doubleaction) , &g_Variables.FiveM_WeaponsPic.weapon_doubleaction );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_fireextinguisher , sizeof(FiveM_Weapons::weapon_fireextinguisher) , &g_Variables.FiveM_WeaponsPic.weapon_fireextinguisher );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_firework , sizeof(FiveM_Weapons::weapon_firework) , &g_Variables.FiveM_WeaponsPic.weapon_firework );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_flare , sizeof(FiveM_Weapons::weapon_flare) , &g_Variables.FiveM_WeaponsPic.weapon_flare );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_flaregun , sizeof(FiveM_Weapons::weapon_flaregun) , &g_Variables.FiveM_WeaponsPic.weapon_flaregun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_flashlight , sizeof(FiveM_Weapons::weapon_flashlight) , &g_Variables.FiveM_WeaponsPic.weapon_flashlight );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_gadgetpistol , sizeof(FiveM_Weapons::weapon_gadgetpistol) , &g_Variables.FiveM_WeaponsPic.weapon_gadgetpistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_golfclub , sizeof(FiveM_Weapons::weapon_golfclub) , &g_Variables.FiveM_WeaponsPic.weapon_golfclub );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_grenade , sizeof(FiveM_Weapons::weapon_grenade) , &g_Variables.FiveM_WeaponsPic.weapon_grenade );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_grenadelauncher , sizeof(FiveM_Weapons::weapon_grenadelauncher) , &g_Variables.FiveM_WeaponsPic.weapon_grenadelauncher );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_grenadelauncher_smoke , sizeof(FiveM_Weapons::weapon_grenadelauncher_smoke) , &g_Variables.FiveM_WeaponsPic.weapon_grenadelauncher_smoke );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_gusenberg , sizeof(FiveM_Weapons::weapon_gusenberg) , &g_Variables.FiveM_WeaponsPic.weapon_gusenberg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_hammer , sizeof(FiveM_Weapons::weapon_hammer) , &g_Variables.FiveM_WeaponsPic.weapon_hammer );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_hatchet , sizeof(FiveM_Weapons::weapon_hatchet) , &g_Variables.FiveM_WeaponsPic.weapon_hatchet );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_hazardcan , sizeof(FiveM_Weapons::weapon_hazardcan) , &g_Variables.FiveM_WeaponsPic.weapon_hazardcan );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_heavypistol , sizeof(FiveM_Weapons::weapon_heavypistol) , &g_Variables.FiveM_WeaponsPic.weapon_heavypistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_heavyshotgun , sizeof(FiveM_Weapons::weapon_heavyshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_heavyshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_heavysniper , sizeof(FiveM_Weapons::weapon_heavysniper) , &g_Variables.FiveM_WeaponsPic.weapon_heavysniper );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_hominglauncher , sizeof(FiveM_Weapons::weapon_hominglauncher) , &g_Variables.FiveM_WeaponsPic.weapon_hominglauncher );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_knife , sizeof(FiveM_Weapons::weapon_knife) , &g_Variables.FiveM_WeaponsPic.weapon_knife );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_knuckle , sizeof(FiveM_Weapons::weapon_knuckle) , &g_Variables.FiveM_WeaponsPic.weapon_knuckle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_machete , sizeof(FiveM_Weapons::weapon_machete) , &g_Variables.FiveM_WeaponsPic.weapon_machete );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_machinepistol , sizeof(FiveM_Weapons::weapon_machinepistol) , &g_Variables.FiveM_WeaponsPic.weapon_machinepistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_marksmanpistol , sizeof(FiveM_Weapons::weapon_marksmanpistol) , &g_Variables.FiveM_WeaponsPic.weapon_marksmanpistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_marksmanrifle , sizeof(FiveM_Weapons::weapon_marksmanrifle) , &g_Variables.FiveM_WeaponsPic.weapon_marksmanrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_marksmanrifle_mk2 , sizeof(FiveM_Weapons::weapon_marksmanrifle_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_marksmanrifle_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_mg , sizeof(FiveM_Weapons::weapon_mg) , &g_Variables.FiveM_WeaponsPic.weapon_mg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_microsmg , sizeof(FiveM_Weapons::weapon_microsmg) , &g_Variables.FiveM_WeaponsPic.weapon_microsmg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_militaryrifle , sizeof(FiveM_Weapons::weapon_militaryrifle) , &g_Variables.FiveM_WeaponsPic.weapon_militaryrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_minigun , sizeof(FiveM_Weapons::weapon_minigun) , &g_Variables.FiveM_WeaponsPic.weapon_minigun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_minismg , sizeof(FiveM_Weapons::weapon_minismg) , &g_Variables.FiveM_WeaponsPic.weapon_minismg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_molotov , sizeof(FiveM_Weapons::weapon_molotov) , &g_Variables.FiveM_WeaponsPic.weapon_molotov );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_musket , sizeof(FiveM_Weapons::weapon_musket) , &g_Variables.FiveM_WeaponsPic.weapon_musket );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_navyrevolver , sizeof(FiveM_Weapons::weapon_navyrevolver) , &g_Variables.FiveM_WeaponsPic.weapon_navyrevolver );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_nightstick , sizeof(FiveM_Weapons::weapon_nightstick) , &g_Variables.FiveM_WeaponsPic.weapon_nightstick );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_petrolcan , sizeof(FiveM_Weapons::weapon_petrolcan) , &g_Variables.FiveM_WeaponsPic.weapon_petrolcan );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pipebomb , sizeof(FiveM_Weapons::weapon_pipebomb) , &g_Variables.FiveM_WeaponsPic.weapon_pipebomb );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pistol , sizeof(FiveM_Weapons::weapon_pistol) , &g_Variables.FiveM_WeaponsPic.weapon_pistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pistol50 , sizeof(FiveM_Weapons::weapon_pistol50) , &g_Variables.FiveM_WeaponsPic.weapon_pistol50 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pistol_mk2 , sizeof(FiveM_Weapons::weapon_pistol_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_pistol_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_poolcue , sizeof(FiveM_Weapons::weapon_poolcue) , &g_Variables.FiveM_WeaponsPic.weapon_poolcue );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_proxmine , sizeof(FiveM_Weapons::weapon_proxmine) , &g_Variables.FiveM_WeaponsPic.weapon_proxmine );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pumpshotgun , sizeof(FiveM_Weapons::weapon_pumpshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_pumpshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_pumpshotgun_mk2 , sizeof(FiveM_Weapons::weapon_pumpshotgun_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_pumpshotgun_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_railgun , sizeof(FiveM_Weapons::weapon_railgun) , &g_Variables.FiveM_WeaponsPic.weapon_railgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_raycarbine , sizeof(FiveM_Weapons::weapon_raycarbine) , &g_Variables.FiveM_WeaponsPic.weapon_raycarbine );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_rayminigun , sizeof(FiveM_Weapons::weapon_rayminigun) , &g_Variables.FiveM_WeaponsPic.weapon_rayminigun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_raypistol , sizeof(FiveM_Weapons::weapon_raypistol) , &g_Variables.FiveM_WeaponsPic.weapon_raypistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_revolver , sizeof(FiveM_Weapons::weapon_revolver) , &g_Variables.FiveM_WeaponsPic.weapon_revolver );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_revolver_mk2 , sizeof(FiveM_Weapons::weapon_revolver_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_revolver_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_rpg , sizeof(FiveM_Weapons::weapon_rpg) , &g_Variables.FiveM_WeaponsPic.weapon_rpg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_sawnoffshotgun , sizeof(FiveM_Weapons::weapon_sawnoffshotgun) , &g_Variables.FiveM_WeaponsPic.weapon_sawnoffshotgun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_smg , sizeof(FiveM_Weapons::weapon_smg) , &g_Variables.FiveM_WeaponsPic.weapon_smg );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_smg_mk2 , sizeof(FiveM_Weapons::weapon_smg_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_smg_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_smokegrenade , sizeof(FiveM_Weapons::weapon_smokegrenade) , &g_Variables.FiveM_WeaponsPic.weapon_smokegrenade );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_sniperrifle , sizeof(FiveM_Weapons::weapon_sniperrifle) , &g_Variables.FiveM_WeaponsPic.weapon_sniperrifle );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_snowball , sizeof(FiveM_Weapons::weapon_snowball) , &g_Variables.FiveM_WeaponsPic.weapon_snowball );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_snspistol , sizeof(FiveM_Weapons::weapon_snspistol) , &g_Variables.FiveM_WeaponsPic.weapon_snspistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_snspistol_mk2 , sizeof(FiveM_Weapons::weapon_snspistol_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_snspistol_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_specialcarbine , sizeof(FiveM_Weapons::weapon_specialcarbine) , &g_Variables.FiveM_WeaponsPic.weapon_specialcarbine );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_specialcarbine_mk2 , sizeof(FiveM_Weapons::weapon_specialcarbine_mk2) , &g_Variables.FiveM_WeaponsPic.weapon_specialcarbine_mk2 );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_stickybomb , sizeof(FiveM_Weapons::weapon_stickybomb) , &g_Variables.FiveM_WeaponsPic.weapon_stickybomb );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_stone_hatchet , sizeof(FiveM_Weapons::weapon_stone_hatchet) , &g_Variables.FiveM_WeaponsPic.weapon_stone_hatchet );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_stungun , sizeof(FiveM_Weapons::weapon_stungun) , &g_Variables.FiveM_WeaponsPic.weapon_stungun );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_switchblade , sizeof(FiveM_Weapons::weapon_switchblade) , &g_Variables.FiveM_WeaponsPic.weapon_switchblade );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_unarmed , sizeof(FiveM_Weapons::weapon_unarmed) , &g_Variables.FiveM_WeaponsPic.weapon_unarmed );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_vintagepistol , sizeof(FiveM_Weapons::weapon_vintagepistol) , &g_Variables.FiveM_WeaponsPic.weapon_vintagepistol );
			Texture::LoadFromMemory( g_pd3dDevice , FiveM_Weapons::weapon_wrench , sizeof(FiveM_Weapons::weapon_wrench) , &g_Variables.FiveM_WeaponsPic.weapon_wrench );
		}
	}
}