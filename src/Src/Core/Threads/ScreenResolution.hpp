#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <regex>


namespace Core::Threads {
	class cScreenResolution {
	public:

		void MoveAndSetResolution() {
			g_Variables.g_hGameWindow = FindWindowA(xorstr("grcWindow"), nullptr);
			if (!g_Variables.g_hGameWindow)
			{
				//Gui::RunCleaner( true );
			}

			POINT pos, size;
			GetTargetInfo(g_Config.General->SecondMonitor, pos, size);

			g_Variables.g_vGameWindowPos = { (float)pos.x, (float)pos.y };
			g_Variables.g_vGameWindowSize = { (float)size.x, (float)size.y };
			g_Variables.g_vGameWindowCenter = { g_Variables.g_vGameWindowSize.x * 0.5f, g_Variables.g_vGameWindowSize.y * 0.5f };

			MoveWindow(g_Variables.g_hCheatWindow, pos.x, pos.y, size.x, size.y - 1, true);
		}

		void _stdcall Update() {
			static int tick = 0;
			bool last = g_Config.General->SecondMonitor;

			while (true) {
				try {
					if (tick++ % 34 == 0) {
						bool has2nd = GetAll().size() >= 2;
						if (!has2nd && last) {
							g_Config.General->SecondMonitor = false;
							std::thread([] {

								}).detach();
						}
					}

					if (g_Config.General->SecondMonitor != last)
						last = g_Config.General->SecondMonitor;

					MoveAndSetResolution();
				}
				catch (const std::exception& e) {

				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}

	private:
		static std::vector<HMONITOR> GetAll() {
			std::vector<HMONITOR> m;
			EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR h, HDC, LPRECT, LPARAM p) {
				reinterpret_cast<std::vector<HMONITOR>*>(p)->push_back(h);
				return TRUE;
				}, (LPARAM)&m);
			return m;
		}

		static std::pair<POINT, POINT> GetDims(HMONITOR h) {
			MONITORINFO mi{ sizeof(mi) };
			GetMonitorInfo(h, &mi);
			return {
				{ mi.rcMonitor.left, mi.rcMonitor.top },
				{ mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top }
			};
		}

		static HMONITOR GetPrimary() {
			for (auto h : GetAll()) {
				MONITORINFO mi{ sizeof(mi) };
				if (GetMonitorInfo(h, &mi) && (mi.dwFlags & MONITORINFOF_PRIMARY))
					return h;
			}
			return nullptr;
		}

		static HMONITOR GetSecondary() {
			for (auto h : GetAll()) {
				MONITORINFO mi{ sizeof(mi) };
				if (GetMonitorInfo(h, &mi) && !(mi.dwFlags & MONITORINFOF_PRIMARY))
					return h;
			}
			return nullptr;
		}

		static void _stdcall GetTargetInfo(bool use2nd, POINT& pos, POINT& size) {
			auto mon = use2nd ? GetSecondary() : GetPrimary();
			if (mon) {
				auto [p, s] = GetDims(mon);
				pos = p; size = s;
			}
			else {
				pos = { (LONG)g_Variables.g_vGameWindowPos.x, (LONG)g_Variables.g_vGameWindowPos.y };
				size = { (LONG)g_Variables.g_vGameWindowSize.x, (LONG)g_Variables.g_vGameWindowSize.y };
			}
		}

	public:
		static void _stdcall MapToSecondary(ImVec2& from, ImVec2& to) {
			auto p = GetPrimary(), s = GetSecondary();
			if (!p || !s || p == s) {
				to = from;
				return;
			}

			auto [pp, ps] = GetDims(p);
			auto [sp, ss] = GetDims(s);

			to.x = from.x * ((float)ss.x / ps.x) + sp.x;
			to.y = from.y * ((float)ss.y / ps.y) + sp.y;
		}
	};

	inline cScreenResolution g_ScreenResolution;
}