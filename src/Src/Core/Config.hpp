#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

using json = nlohmann::json;

namespace Core {

	class Config {
	public:

		struct General {
		public:
			inline static bool StreamProof = true;
			inline static bool WaterMark;
			inline static bool WaterMarkCol;
			inline static bool ArrayList;
			inline static bool ArrayListCol;
			inline static bool SecondMonitor;
			inline static bool BattleFive;
			inline static bool FixCamp;
			inline static bool EnglishMode;
			inline static bool VSync = true;
			inline static int ProcessPriority = 0;
			inline static int MenuKey = VK_INSERT;
			int UnloadKeybind;
		} *General;

		struct Aimbot {
		public:
			inline static bool Enabled;
			inline static bool ShowFov;
			inline static bool OnlyVisible;
			inline static bool IgnoreNPCs;
			inline static bool Prediction;
			inline static int FOV = 180;
			inline static int MaxDistance = 240;
			//inline static int SmoothHorizontal = 12;
			//inline static int SmoothVertical = 12;
			inline static int AimbotSpeed = 12;
			inline static int KeyBind;
			inline static ImColor FovColor{ 255, 255, 255 };
		} *Aimbot;

		struct TriggerBot {
		public:
			inline static bool Enabled;
			inline static bool ShowFov;
			inline static bool OnlyVisible;
			inline static bool IgnoreNPCs;
			inline static bool SmartTrigger;
			inline static int FOV = 20;
			inline static int MaxDistance = 200;
			inline static int Delay = 0;
			inline static int KeyBind;
			inline static ImColor FovColor{ 255, 255, 255 };
		} *TriggerBot;

		struct SilentAim {
		public:
			inline static bool Enabled;
			inline static bool ShowFov;
			inline static bool OnlyVisible;
			inline static bool IgnoreNPCs;
			inline static int FOV = 40;
			inline static int MissChance = 0;
			inline static int MaxDistance = 200;
			inline static int KeyBind;
			inline static ImColor FovColor{ 224, 94, 103, 200 };
		} *SilentAim;

		struct MagicBullet {
		public:
			inline static bool Enabled;
			inline static bool MagicBullets;
			inline static bool IgnoreNPCs;
			inline static int MissChance = 0;
			inline static int MaxDistance = 250;
			inline static int KeyBind;
		} *MagicBullet;

		struct ESP {
		public:
			inline static bool UpdateCfgESP;
			inline static bool Enabled;
			inline static bool Box;
			inline static bool RadarEnabled;
			inline static bool FilledBox;
			inline static int BoxState = 0;
			inline static bool Skeleton;
			inline static bool HealthBar;
			inline static ImVec2 HealthBarPos;
			inline static int HealthBarState = 1;
			inline static bool ArmorBar;
			inline static ImVec2 ArmorBarPos;
			inline static int ArmorBarState = 0;
			inline static bool WeaponName;
			inline static ImVec2 WeaponNamePos;
			inline static int WeaponNameState = 0;
			inline static bool SnapLines;
			inline static bool UserNames;
			inline static ImVec2 UserNamesPos;
			inline static int UserNamesState = 0;
			inline static bool HeadCircle;
			inline static bool IgnoreNPCs;
			inline static bool ShowLocalPlayer;
			inline static bool HighlightVisible;
			inline static bool IgnoreDead;
			inline static bool DistanceFromMe;
			inline static bool FriendsMarker;
			inline static int FriendsMarkerBind;
			inline static ImVec2 DistanceFromMePos;
			inline static int DistanceFromMeState = 0;
			inline static int MaxDistance = 200;
			inline static ImColor DistanceCol{ 230, 230, 230, 255 };
			inline static ImColor UserNamesCol{ 230, 230, 230, 255 };
			inline static ImColor WeaponNameCol{ 230, 230, 230, 255 };
			inline static ImColor SkeletonCol{ 255, 255, 255, 200 };
			inline static ImColor BoxCol{ 255, 255, 255, 200 };
			inline static ImColor FilledBoxCol{ 0, 0, 0, 40 };
			inline static ImColor SnapLinesCol{ 255, 255, 255, 200 };
			inline static ImColor FriendCol{ 255, 204, 0, 255 };
			inline static int KeyBind;
		} *ESP;

		struct VehicleESP {
		public:
			inline static bool Enabled;
			inline static bool SnapLines;
			inline static bool ShowLockUnlock;
			inline static bool VehName;
			inline static bool DistanceFromMe;
			inline static int MaxDistance = 200;
			inline static ImColor SnapLinesCol{ 255, 255, 255, 200 };
		} *VehicleESP;

		struct Player {
		public:
			inline static float CurrentHealthValue;
			inline static float CurrentArmorValue;
			inline static bool FastRun;
			inline static bool TpwayBypass;
			inline static float RunSpeed = 1.f;
			inline static bool InfiniteStamina;
			inline static bool WeaponOptions;
			inline static bool NoRecoilEnabled;
			inline static float RecoilValue;
			inline static bool NoSpreadEnabled;
			inline static float SpreadValue;
			inline static bool InfiniteAmmoEnabled;
			inline static bool NoReloadEnabled;
			inline static bool NoClipEnabled;
			inline static bool HandlingEditor;
			inline static int NoClipKey;
			inline static float NoClipSpeed = 2.0f;
			inline static bool InfiniteCombatRoll;
			inline static bool EnableGodMode;
			inline static bool VehicleGodMode;
			inline static bool SeatBelt;
			inline static bool ForceWeaponWheel;
			inline static bool ShrinkEnabled;
			inline static bool NoRagDollEnabled;
			inline static bool AntiHSEnabled;
			inline static bool StealCarEnabled;
			inline static int GodModeKey;
		} *Player;


		nlohmann::json ImColToJson(const ImColor& Col) {
			return nlohmann::json::array({ Col.Value.x, Col.Value.y, Col.Value.z, Col.Value.w });
		}

		ImColor JsonToImCol(const nlohmann::json& JsonCol) {
			if (JsonCol.is_array() && JsonCol.size() == 4) {
				float r = JsonCol[0];
				float g = JsonCol[1];
				float b = JsonCol[2];
				float a = JsonCol[3];
				return ImColor(r, g, b, a);
			}
			else {
				return ImColor(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}

	};
	inline Config g_Config;
}
