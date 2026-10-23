#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <Core/Config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

using json = nlohmann::json;
using namespace Core;

namespace ConfigManager {

	static const std::string configPath = "C:\\Windows\\SysWOW64\\slmgr\\0416\\slmgrs.ini";

	static const std::string XOR_KEY = "@189Khkzmhakdolajkdj2!098394083276632uiuj9pu90hlka2JO";

	static std::string xorCrypt( const std::string& data ) {
		std::string out;
		out.resize( data.size( ) );
		size_t klen = XOR_KEY.size( );
		if ( klen == 0 ) return data;
		for ( size_t i = 0; i < data.size( ); ++i ) {
			out [ i ] = data [ i ] ^ XOR_KEY [ i % klen ];
		}
		return out;
	}

	static json ImVec2ToJson( const ImVec2& v ) {
		return json::array( { v.x, v.y } );
	}
	static ImVec2 JsonToImVec2( const json& j ) {
		if ( j.is_array( ) && j.size( ) == 2 ) {
			return ImVec2( ( float ) j [ 0 ] , ( float ) j [ 1 ] );
		}
		return ImVec2( 0.0f , 0.0f );
	}

	static json ReadFileDecrypted( ) {
		std::ifstream ifs( configPath , std::ios::binary );
		if ( !ifs.is_open( ) ) {
			return json::object( );
		}
		std::string encrypted( ( std::istreambuf_iterator<char>( ifs ) ) ,
			std::istreambuf_iterator<char>( ) );
		ifs.close( );

		if ( encrypted.empty( ) ) return json::object( );

		std::string decrypted = xorCrypt( encrypted );
		try {
			return json::parse( decrypted );
		}
		catch ( ... ) {
			return json::object( );
		}
	}

	static void WriteFileEncrypted( const json& j ) {
		std::string serialized = j.dump( 4 );
		std::string encrypted = xorCrypt( serialized );
		std::ofstream ofs( configPath , std::ios::binary | std::ios::trunc );
		if ( !ofs.is_open( ) ) {
			std::cerr << "[ConfigManager] Erro ao abrir " << configPath << " para escrita.\n";
			return;
		}
		ofs.write( encrypted.data( ) , ( std::streamsize ) encrypted.size( ) );
		ofs.close( );
	}

	void ListConfigs( ) {
		json root = ReadFileDecrypted( );
		std::cout << "Configs salvas em " << configPath << ":\n";
		if ( !root.is_object( ) || root.empty( ) ) {
			std::cout << "  (nenhuma configuração encontrada)\n";
			return;
		}
		for ( auto& it : root.items( ) ) {
			std::cout << " - " << it.key( ) << "\n";
		}
	}

	void CreateConfigs( const std::string& name ) {
		if ( name.empty( ) ) {
			std::cerr << "[ConfigManager] Nome de config vazio.\n";
			return;
		}

		json root = ReadFileDecrypted( );

		json cfg;

		// --- General ---
		cfg [ "General" ] = {
			{"StreamProof", Core::Config::General::StreamProof},
			{"WaterMark", Core::Config::General::WaterMark},
			{"WaterMarkCol", Core::Config::General::WaterMarkCol},
			{"ArrayList", Core::Config::General::ArrayList},
			{"ArrayListCol", Core::Config::General::ArrayListCol},
			{"SecondMonitor", Core::Config::General::SecondMonitor},
			{"FixCamp", Core::Config::General::FixCamp},
			{"englishs", Core::Config::General::EnglishMode},
			{"VSync", Core::Config::General::VSync},
			{"ProcessPriority", Core::Config::General::ProcessPriority},
			{"MenuKey", Core::Config::General::MenuKey}
		};

		// --- Aimbot ---
		cfg [ "Aimbot" ] = {
			{"Enabled", Core::Config::Aimbot::Enabled},
			{"ShowFov", Core::Config::Aimbot::ShowFov},
			{"OnlyVisible", Core::Config::Aimbot::OnlyVisible},
			{"IgnoreNPCs", Core::Config::Aimbot::IgnoreNPCs},
			{"Prediction", Core::Config::Aimbot::Prediction},
			{"FOV", Core::Config::Aimbot::FOV},
			{"MaxDistance", Core::Config::Aimbot::MaxDistance},
			{"AimbotSpeed", Core::Config::Aimbot::AimbotSpeed},
			{"KeyBind", Core::Config::Aimbot::KeyBind},
			{"FovColor", g_Config.ImColToJson( Core::Config::Aimbot::FovColor )}
		};

		// --- TriggerBot ---
		cfg [ "TriggerBot" ] = {
			{"Enabled", Core::Config::TriggerBot::Enabled},
			{"ShowFov", Core::Config::TriggerBot::ShowFov},
			{"OnlyVisible", Core::Config::TriggerBot::OnlyVisible},
			{"IgnoreNPCs", Core::Config::TriggerBot::IgnoreNPCs},
			{"SmartTrigger", Core::Config::TriggerBot::SmartTrigger},
			{"FOV", Core::Config::TriggerBot::FOV},
			{"MaxDistance", Core::Config::TriggerBot::MaxDistance},
			{"Delay", Core::Config::TriggerBot::Delay},
			{"KeyBind", Core::Config::TriggerBot::KeyBind},
			{"FovColor", g_Config.ImColToJson( Core::Config::TriggerBot::FovColor )}
		};

		// --- SilentAim ---
		cfg [ "SilentAim" ] = {
			{"Enabled", Core::Config::SilentAim::Enabled},
			{"ShowFov", Core::Config::SilentAim::ShowFov},
			{"OnlyVisible", Core::Config::SilentAim::OnlyVisible},
			{"IgnoreNPCs", Core::Config::SilentAim::IgnoreNPCs},
			{"FOV", Core::Config::SilentAim::FOV},
			{"MissChance", Core::Config::SilentAim::MissChance},
			{"MaxDistance", Core::Config::SilentAim::MaxDistance},
			{"KeyBind", Core::Config::SilentAim::KeyBind},
			{"FovColor", g_Config.ImColToJson( Core::Config::SilentAim::FovColor )}
		};

		// --- MagicBullet ---
		cfg [ "MagicBullet" ] = {
			{"Enabled", Core::Config::MagicBullet::Enabled},
			{"MagicBullets", Core::Config::MagicBullet::MagicBullets},
			{"IgnoreNPCs", Core::Config::MagicBullet::IgnoreNPCs},
			{"MissChance", Core::Config::MagicBullet::MissChance},
			{"MaxDistance", Core::Config::MagicBullet::MaxDistance},
			{"KeyBind", Core::Config::MagicBullet::KeyBind}
		};

		// --- ESP ---
		{
			json esp;
			esp [ "UpdateCfgESP" ] = Core::Config::ESP::UpdateCfgESP;
			esp [ "Enabled" ] = Core::Config::ESP::Enabled;
			esp [ "Box" ] = Core::Config::ESP::Box;
			esp [ "RadarEnabled" ] = Core::Config::ESP::RadarEnabled;
			esp [ "FilledBox" ] = Core::Config::ESP::FilledBox;
			esp [ "BoxState" ] = Core::Config::ESP::BoxState;
			esp [ "Skeleton" ] = Core::Config::ESP::Skeleton;
			esp [ "HealthBar" ] = Core::Config::ESP::HealthBar;
			esp [ "HealthBarPos" ] = ImVec2ToJson( Core::Config::ESP::HealthBarPos );
			esp [ "HealthBarState" ] = Core::Config::ESP::HealthBarState;
			esp [ "ArmorBar" ] = Core::Config::ESP::ArmorBar;
			esp [ "ArmorBarPos" ] = ImVec2ToJson( Core::Config::ESP::ArmorBarPos );
			esp [ "ArmorBarState" ] = Core::Config::ESP::ArmorBarState;
			esp [ "WeaponName" ] = Core::Config::ESP::WeaponName;
			esp [ "WeaponNamePos" ] = ImVec2ToJson( Core::Config::ESP::WeaponNamePos );
			esp [ "WeaponNameState" ] = Core::Config::ESP::WeaponNameState;
			esp [ "SnapLines" ] = Core::Config::ESP::SnapLines;
			esp [ "UserNames" ] = Core::Config::ESP::UserNames;
			esp [ "UserNamesPos" ] = ImVec2ToJson( Core::Config::ESP::UserNamesPos );
			esp [ "UserNamesState" ] = Core::Config::ESP::UserNamesState;
			esp [ "HeadCircle" ] = Core::Config::ESP::HeadCircle;
			esp [ "IgnoreNPCs" ] = Core::Config::ESP::IgnoreNPCs;
			esp [ "ShowLocalPlayer" ] = Core::Config::ESP::ShowLocalPlayer;
			esp [ "HighlightVisible" ] = Core::Config::ESP::HighlightVisible;
			esp [ "IgnoreDead" ] = Core::Config::ESP::IgnoreDead;
			esp [ "DistanceFromMe" ] = Core::Config::ESP::DistanceFromMe;
			esp [ "FriendsMarker" ] = Core::Config::ESP::FriendsMarker;
			esp [ "FriendsMarkerBind" ] = Core::Config::ESP::FriendsMarkerBind;
			esp [ "DistanceFromMePos" ] = ImVec2ToJson( Core::Config::ESP::DistanceFromMePos );
			esp [ "DistanceFromMeState" ] = Core::Config::ESP::DistanceFromMeState;
			esp [ "MaxDistance" ] = Core::Config::ESP::MaxDistance;
			esp [ "DistanceCol" ] = g_Config.ImColToJson( Core::Config::ESP::DistanceCol );
			esp [ "UserNamesCol" ] = g_Config.ImColToJson( Core::Config::ESP::UserNamesCol );
			esp [ "WeaponNameCol" ] = g_Config.ImColToJson( Core::Config::ESP::WeaponNameCol );
			esp [ "SkeletonCol" ] = g_Config.ImColToJson( Core::Config::ESP::SkeletonCol );
			esp [ "BoxCol" ] = g_Config.ImColToJson( Core::Config::ESP::BoxCol );
			esp [ "FilledBoxCol" ] = g_Config.ImColToJson( Core::Config::ESP::FilledBoxCol );
			esp [ "SnapLinesCol" ] = g_Config.ImColToJson( Core::Config::ESP::SnapLinesCol );
			esp [ "FriendCol" ] = g_Config.ImColToJson( Core::Config::ESP::FriendCol );
			esp [ "KeyBind" ] = Core::Config::ESP::KeyBind;
			cfg [ "ESP" ] = esp;
		}

		// --- VehicleESP ---
		cfg [ "VehicleESP" ] = {
			{"Enabled", Core::Config::VehicleESP::Enabled},
			{"SnapLines", Core::Config::VehicleESP::SnapLines},
			{"ShowLockUnlock", Core::Config::VehicleESP::ShowLockUnlock},
			{"VehName", Core::Config::VehicleESP::VehName},
			{"DistanceFromMe", Core::Config::VehicleESP::DistanceFromMe},
			{"MaxDistance", Core::Config::VehicleESP::MaxDistance},
			{"SnapLinesCol", g_Config.ImColToJson( Core::Config::VehicleESP::SnapLinesCol )}
		};

		// --- Player ---
		{
			json p;
			p [ "CurrentHealthValue" ] = Core::Config::Player::CurrentHealthValue;
			p [ "CurrentArmorValue" ] = Core::Config::Player::CurrentArmorValue;
			p [ "FastRun" ] = Core::Config::Player::FastRun;
			p [ "TpwayBypass" ] = Core::Config::Player::TpwayBypass;
			p [ "RunSpeed" ] = Core::Config::Player::RunSpeed;
			p [ "InfiniteStamina" ] = Core::Config::Player::InfiniteStamina;
			p [ "WeaponOptions" ] = Core::Config::Player::WeaponOptions;
			p [ "NoRecoilEnabled" ] = Core::Config::Player::NoRecoilEnabled;
			p [ "RecoilValue" ] = Core::Config::Player::RecoilValue;
			p [ "NoSpreadEnabled" ] = Core::Config::Player::NoSpreadEnabled;
			p [ "SpreadValue" ] = Core::Config::Player::SpreadValue;
			p [ "InfiniteAmmoEnabled" ] = Core::Config::Player::InfiniteAmmoEnabled;
			p [ "NoReloadEnabled" ] = Core::Config::Player::NoReloadEnabled;
			p [ "NoClipEnabled" ] = Core::Config::Player::NoClipEnabled;
			p [ "HandlingEditor" ] = Core::Config::Player::HandlingEditor;
			p [ "NoClipKey" ] = Core::Config::Player::NoClipKey;
			p [ "NoClipSpeed" ] = Core::Config::Player::NoClipSpeed;
			p [ "InfiniteCombatRoll" ] = Core::Config::Player::InfiniteCombatRoll;
			p [ "EnableGodMode" ] = Core::Config::Player::EnableGodMode;
			p [ "VehicleGodMode" ] = Core::Config::Player::VehicleGodMode;
			p [ "SeatBelt" ] = Core::Config::Player::SeatBelt;
			p [ "ForceWeaponWheel" ] = Core::Config::Player::ForceWeaponWheel;
			p [ "ShrinkEnabled" ] = Core::Config::Player::ShrinkEnabled;
			p [ "NoRagDollEnabled" ] = Core::Config::Player::NoRagDollEnabled;
			p [ "AntiHSEnabled" ] = Core::Config::Player::AntiHSEnabled;
			p [ "StealCarEnabled" ] = Core::Config::Player::StealCarEnabled;
			p [ "GodModeKey" ] = Core::Config::Player::GodModeKey;
			cfg [ "Player" ] = p;
		}

		root [ name ] = cfg;
		WriteFileEncrypted( root );

		std::cout << "[ConfigManager] Config '" << name << "' salva/atualizada com sucesso.\n";
	}

	void LoadConfigs( const std::string& name ) {
		if ( name.empty( ) ) {
			std::cerr << "[ConfigManager] Nome de config vazio.\n";
			return;
		}

		json root = ReadFileDecrypted( );
		if ( !root.is_object( ) || !root.contains( name ) ) {
			std::cerr << "[ConfigManager] Config '" << name << "' nao encontrada.\n";
			return;
		}

		json cfg = root [ name ];

		// --- General ---
		if ( cfg.contains( "General" ) ) {
			auto j = cfg [ "General" ];
			Core::Config::General::StreamProof = j.value( "StreamProof" , Core::Config::General::StreamProof );
			Core::Config::General::WaterMark = j.value( "WaterMark" , Core::Config::General::WaterMark );
			Core::Config::General::WaterMarkCol = j.value( "WaterMarkCol" , Core::Config::General::WaterMarkCol );
			Core::Config::General::ArrayList = j.value( "ArrayList" , Core::Config::General::ArrayList );
			Core::Config::General::ArrayListCol = j.value( "ArrayListCol" , Core::Config::General::ArrayListCol );
			Core::Config::General::SecondMonitor = j.value( "SecondMonitor" , Core::Config::General::SecondMonitor );
			Core::Config::General::FixCamp = j.value( "FixCamp" , Core::Config::General::FixCamp );
			Core::Config::General::EnglishMode = j.value( "englishs" , Core::Config::General::EnglishMode );
			Core::Config::General::VSync = j.value( "VSync" , Core::Config::General::VSync );
			Core::Config::General::ProcessPriority = j.value( "ProcessPriority" , Core::Config::General::ProcessPriority );
			Core::Config::General::MenuKey = j.value( "MenuKey" , Core::Config::General::MenuKey );
		}

		// --- Aimbot ---
		if ( cfg.contains( "Aimbot" ) ) {
			auto j = cfg [ "Aimbot" ];
			Core::Config::Aimbot::Enabled = j.value( "Enabled" , Core::Config::Aimbot::Enabled );
			Core::Config::Aimbot::ShowFov = j.value( "ShowFov" , Core::Config::Aimbot::ShowFov );
			Core::Config::Aimbot::OnlyVisible = j.value( "OnlyVisible" , Core::Config::Aimbot::OnlyVisible );
			Core::Config::Aimbot::IgnoreNPCs = j.value( "IgnoreNPCs" , Core::Config::Aimbot::IgnoreNPCs );
			Core::Config::Aimbot::Prediction = j.value( "Prediction" , Core::Config::Aimbot::Prediction );
			Core::Config::Aimbot::FOV = j.value( "FOV" , Core::Config::Aimbot::FOV );
			Core::Config::Aimbot::MaxDistance = j.value( "MaxDistance" , Core::Config::Aimbot::MaxDistance );
			Core::Config::Aimbot::AimbotSpeed = j.value( "AimbotSpeed" , Core::Config::Aimbot::AimbotSpeed );
			Core::Config::Aimbot::KeyBind = j.value( "KeyBind" , Core::Config::Aimbot::KeyBind );
			if ( j.contains( "FovColor" ) ) Core::Config::Aimbot::FovColor = g_Config.JsonToImCol( j [ "FovColor" ] );
		}

		// --- TriggerBot ---
		if ( cfg.contains( "TriggerBot" ) ) {
			auto j = cfg [ "TriggerBot" ];
			Core::Config::TriggerBot::Enabled = j.value( "Enabled" , Core::Config::TriggerBot::Enabled );
			Core::Config::TriggerBot::ShowFov = j.value( "ShowFov" , Core::Config::TriggerBot::ShowFov );
			Core::Config::TriggerBot::OnlyVisible = j.value( "OnlyVisible" , Core::Config::TriggerBot::OnlyVisible );
			Core::Config::TriggerBot::IgnoreNPCs = j.value( "IgnoreNPCs" , Core::Config::TriggerBot::IgnoreNPCs );
			Core::Config::TriggerBot::SmartTrigger = j.value( "SmartTrigger" , Core::Config::TriggerBot::SmartTrigger );
			Core::Config::TriggerBot::FOV = j.value( "FOV" , Core::Config::TriggerBot::FOV );
			Core::Config::TriggerBot::MaxDistance = j.value( "MaxDistance" , Core::Config::TriggerBot::MaxDistance );
			Core::Config::TriggerBot::Delay = j.value( "Delay" , Core::Config::TriggerBot::Delay );
			Core::Config::TriggerBot::KeyBind = j.value( "KeyBind" , Core::Config::TriggerBot::KeyBind );
			if ( j.contains( "FovColor" ) ) Core::Config::TriggerBot::FovColor = g_Config.JsonToImCol( j [ "FovColor" ] );
		}

		// --- SilentAim ---
		if ( cfg.contains( "SilentAim" ) ) {
			auto j = cfg [ "SilentAim" ];
			Core::Config::SilentAim::Enabled = j.value( "Enabled" , Core::Config::SilentAim::Enabled );
			Core::Config::SilentAim::ShowFov = j.value( "ShowFov" , Core::Config::SilentAim::ShowFov );
			Core::Config::SilentAim::OnlyVisible = j.value( "OnlyVisible" , Core::Config::SilentAim::OnlyVisible );
			Core::Config::SilentAim::IgnoreNPCs = j.value( "IgnoreNPCs" , Core::Config::SilentAim::IgnoreNPCs );
			Core::Config::SilentAim::FOV = j.value( "FOV" , Core::Config::SilentAim::FOV );
			Core::Config::SilentAim::MissChance = j.value( "MissChance" , Core::Config::SilentAim::MissChance );
			Core::Config::SilentAim::MaxDistance = j.value( "MaxDistance" , Core::Config::SilentAim::MaxDistance );
			Core::Config::SilentAim::KeyBind = j.value( "KeyBind" , Core::Config::SilentAim::KeyBind );
			if ( j.contains( "FovColor" ) ) Core::Config::SilentAim::FovColor = g_Config.JsonToImCol( j [ "FovColor" ] );
		}

		// --- MagicBullet ---
		if ( cfg.contains( "MagicBullet" ) ) {
			auto j = cfg [ "MagicBullet" ];
			Core::Config::MagicBullet::Enabled = j.value( "Enabled" , Core::Config::MagicBullet::Enabled );
			Core::Config::MagicBullet::MagicBullets = j.value( "MagicBullets" , Core::Config::MagicBullet::MagicBullets );
			Core::Config::MagicBullet::IgnoreNPCs = j.value( "IgnoreNPCs" , Core::Config::MagicBullet::IgnoreNPCs );
			Core::Config::MagicBullet::MissChance = j.value( "MissChance" , Core::Config::MagicBullet::MissChance );
			Core::Config::MagicBullet::MaxDistance = j.value( "MaxDistance" , Core::Config::MagicBullet::MaxDistance );
			Core::Config::MagicBullet::KeyBind = j.value( "KeyBind" , Core::Config::MagicBullet::KeyBind );
		}

		// --- ESP ---
		if ( cfg.contains( "ESP" ) ) {
			auto j = cfg [ "ESP" ];
			Core::Config::ESP::UpdateCfgESP = j.value( "UpdateCfgESP" , Core::Config::ESP::UpdateCfgESP );
			Core::Config::ESP::Enabled = j.value( "Enabled" , Core::Config::ESP::Enabled );
			Core::Config::ESP::Box = j.value( "Box" , Core::Config::ESP::Box );
			Core::Config::ESP::RadarEnabled = j.value( "RadarEnabled" , Core::Config::ESP::RadarEnabled );
			Core::Config::ESP::FilledBox = j.value( "FilledBox" , Core::Config::ESP::FilledBox );
			Core::Config::ESP::BoxState = j.value( "BoxState" , Core::Config::ESP::BoxState );
			Core::Config::ESP::Skeleton = j.value( "Skeleton" , Core::Config::ESP::Skeleton );
			Core::Config::ESP::HealthBar = j.value( "HealthBar" , Core::Config::ESP::HealthBar );
			if ( j.contains( "HealthBarPos" ) ) Core::Config::ESP::HealthBarPos = JsonToImVec2( j [ "HealthBarPos" ] );
			Core::Config::ESP::HealthBarState = j.value( "HealthBarState" , Core::Config::ESP::HealthBarState );
			Core::Config::ESP::ArmorBar = j.value( "ArmorBar" , Core::Config::ESP::ArmorBar );
			if ( j.contains( "ArmorBarPos" ) ) Core::Config::ESP::ArmorBarPos = JsonToImVec2( j [ "ArmorBarPos" ] );
			Core::Config::ESP::ArmorBarState = j.value( "ArmorBarState" , Core::Config::ESP::ArmorBarState );
			Core::Config::ESP::WeaponName = j.value( "WeaponName" , Core::Config::ESP::WeaponName );
			if ( j.contains( "WeaponNamePos" ) ) Core::Config::ESP::WeaponNamePos = JsonToImVec2( j [ "WeaponNamePos" ] );
			Core::Config::ESP::WeaponNameState = j.value( "WeaponNameState" , Core::Config::ESP::WeaponNameState );
			Core::Config::ESP::SnapLines = j.value( "SnapLines" , Core::Config::ESP::SnapLines );
			Core::Config::ESP::UserNames = j.value( "UserNames" , Core::Config::ESP::UserNames );
			if ( j.contains( "UserNamesPos" ) ) Core::Config::ESP::UserNamesPos = JsonToImVec2( j [ "UserNamesPos" ] );
			Core::Config::ESP::UserNamesState = j.value( "UserNamesState" , Core::Config::ESP::UserNamesState );
			Core::Config::ESP::HeadCircle = j.value( "HeadCircle" , Core::Config::ESP::HeadCircle );
			Core::Config::ESP::IgnoreNPCs = j.value( "IgnoreNPCs" , Core::Config::ESP::IgnoreNPCs );
			Core::Config::ESP::ShowLocalPlayer = j.value( "ShowLocalPlayer" , Core::Config::ESP::ShowLocalPlayer );
			Core::Config::ESP::HighlightVisible = j.value( "HighlightVisible" , Core::Config::ESP::HighlightVisible );
			Core::Config::ESP::IgnoreDead = j.value( "IgnoreDead" , Core::Config::ESP::IgnoreDead );
			Core::Config::ESP::DistanceFromMe = j.value( "DistanceFromMe" , Core::Config::ESP::DistanceFromMe );
			Core::Config::ESP::FriendsMarker = j.value( "FriendsMarker" , Core::Config::ESP::FriendsMarker );
			Core::Config::ESP::FriendsMarkerBind = j.value( "FriendsMarkerBind" , Core::Config::ESP::FriendsMarkerBind );
			if ( j.contains( "DistanceFromMePos" ) ) Core::Config::ESP::DistanceFromMePos = JsonToImVec2( j [ "DistanceFromMePos" ] );
			Core::Config::ESP::DistanceFromMeState = j.value( "DistanceFromMeState" , Core::Config::ESP::DistanceFromMeState );
			Core::Config::ESP::MaxDistance = j.value( "MaxDistance" , Core::Config::ESP::MaxDistance );
			if ( j.contains( "DistanceCol" ) ) Core::Config::ESP::DistanceCol = g_Config.JsonToImCol( j [ "DistanceCol" ] );
			if ( j.contains( "UserNamesCol" ) ) Core::Config::ESP::UserNamesCol = g_Config.JsonToImCol( j [ "UserNamesCol" ] );
			if ( j.contains( "WeaponNameCol" ) ) Core::Config::ESP::WeaponNameCol = g_Config.JsonToImCol( j [ "WeaponNameCol" ] );
			if ( j.contains( "SkeletonCol" ) ) Core::Config::ESP::SkeletonCol = g_Config.JsonToImCol( j [ "SkeletonCol" ] );
			if ( j.contains( "BoxCol" ) ) Core::Config::ESP::BoxCol = g_Config.JsonToImCol( j [ "BoxCol" ] );
			if ( j.contains( "FilledBoxCol" ) ) Core::Config::ESP::FilledBoxCol = g_Config.JsonToImCol( j [ "FilledBoxCol" ] );
			if ( j.contains( "SnapLinesCol" ) ) Core::Config::ESP::SnapLinesCol = g_Config.JsonToImCol( j [ "SnapLinesCol" ] );
			if ( j.contains( "FriendCol" ) ) Core::Config::ESP::FriendCol = g_Config.JsonToImCol( j [ "FriendCol" ] );
			Core::Config::ESP::KeyBind = j.value( "KeyBind" , Core::Config::ESP::KeyBind );
		}

		// --- VehicleESP ---
		if ( cfg.contains( "VehicleESP" ) ) {
			auto j = cfg [ "VehicleESP" ];
			Core::Config::VehicleESP::Enabled = j.value( "Enabled" , Core::Config::VehicleESP::Enabled );
			Core::Config::VehicleESP::SnapLines = j.value( "SnapLines" , Core::Config::VehicleESP::SnapLines );
			Core::Config::VehicleESP::ShowLockUnlock = j.value( "ShowLockUnlock" , Core::Config::VehicleESP::ShowLockUnlock );
			Core::Config::VehicleESP::VehName = j.value( "VehName" , Core::Config::VehicleESP::VehName );
			Core::Config::VehicleESP::DistanceFromMe = j.value( "DistanceFromMe" , Core::Config::VehicleESP::DistanceFromMe );
			Core::Config::VehicleESP::MaxDistance = j.value( "MaxDistance" , Core::Config::VehicleESP::MaxDistance );
			if ( j.contains( "SnapLinesCol" ) ) Core::Config::VehicleESP::SnapLinesCol = g_Config.JsonToImCol( j [ "SnapLinesCol" ] );
		}

		// --- Player ---
		if ( cfg.contains( "Player" ) ) {
			auto j = cfg [ "Player" ];
			Core::Config::Player::CurrentHealthValue = j.value( "CurrentHealthValue" , Core::Config::Player::CurrentHealthValue );
			Core::Config::Player::CurrentArmorValue = j.value( "CurrentArmorValue" , Core::Config::Player::CurrentArmorValue );
			Core::Config::Player::FastRun = j.value( "FastRun" , Core::Config::Player::FastRun );
			Core::Config::Player::TpwayBypass = j.value( "TpwayBypass" , Core::Config::Player::TpwayBypass );
			Core::Config::Player::RunSpeed = j.value( "RunSpeed" , Core::Config::Player::RunSpeed );
			Core::Config::Player::InfiniteStamina = j.value( "InfiniteStamina" , Core::Config::Player::InfiniteStamina );
			Core::Config::Player::WeaponOptions = j.value( "WeaponOptions" , Core::Config::Player::WeaponOptions );
			Core::Config::Player::NoRecoilEnabled = j.value( "NoRecoilEnabled" , Core::Config::Player::NoRecoilEnabled );
			Core::Config::Player::RecoilValue = j.value( "RecoilValue" , Core::Config::Player::RecoilValue );
			Core::Config::Player::NoSpreadEnabled = j.value( "NoSpreadEnabled" , Core::Config::Player::NoSpreadEnabled );
			Core::Config::Player::SpreadValue = j.value( "SpreadValue" , Core::Config::Player::SpreadValue );
			Core::Config::Player::InfiniteAmmoEnabled = j.value( "InfiniteAmmoEnabled" , Core::Config::Player::InfiniteAmmoEnabled );
			Core::Config::Player::NoReloadEnabled = j.value( "NoReloadEnabled" , Core::Config::Player::NoReloadEnabled );
			Core::Config::Player::NoClipEnabled = j.value( "NoClipEnabled" , Core::Config::Player::NoClipEnabled );
			Core::Config::Player::HandlingEditor = j.value( "HandlingEditor" , Core::Config::Player::HandlingEditor );
			Core::Config::Player::NoClipKey = j.value( "NoClipKey" , Core::Config::Player::NoClipKey );
			Core::Config::Player::NoClipSpeed = j.value( "NoClipSpeed" , Core::Config::Player::NoClipSpeed );
			Core::Config::Player::InfiniteCombatRoll = j.value( "InfiniteCombatRoll" , Core::Config::Player::InfiniteCombatRoll );
			Core::Config::Player::EnableGodMode = j.value( "EnableGodMode" , Core::Config::Player::EnableGodMode );
			Core::Config::Player::VehicleGodMode = j.value( "VehicleGodMode" , Core::Config::Player::VehicleGodMode );
			Core::Config::Player::SeatBelt = j.value( "SeatBelt" , Core::Config::Player::SeatBelt );
			Core::Config::Player::ForceWeaponWheel = j.value( "ForceWeaponWheel" , Core::Config::Player::ForceWeaponWheel );
			Core::Config::Player::ShrinkEnabled = j.value( "ShrinkEnabled" , Core::Config::Player::ShrinkEnabled );
			Core::Config::Player::NoRagDollEnabled = j.value( "NoRagDollEnabled" , Core::Config::Player::NoRagDollEnabled );
			Core::Config::Player::AntiHSEnabled = j.value( "AntiHSEnabled" , Core::Config::Player::AntiHSEnabled );
			Core::Config::Player::StealCarEnabled = j.value( "StealCarEnabled" , Core::Config::Player::StealCarEnabled );
			Core::Config::Player::GodModeKey = j.value( "GodModeKey" , Core::Config::Player::GodModeKey );
		}

		std::cout << "[ConfigManager] Config '" << name << "' carregada com sucesso.\n";
	}

	void DeleteConfigs( const std::string& name ) {
		json j = ReadFileDecrypted( );
		if ( !j.contains( name ) ) {
			std::cout << "[ConfigManager] Config '" << name << "' não encontrada para deletar.\n";
			return;
		}

		j.erase( name );
		WriteFileEncrypted( j );

		std::cout << "[ConfigManager] Config '" << name << "' deletada com sucesso.\n";
	}


	std::vector<std::string> GetConfigNames( ) {
		std::vector<std::string> out;
		json root = ReadFileDecrypted( );
		if ( !root.is_object( ) ) return out;
		for ( auto& it : root.items( ) ) {
			out.push_back( it.key( ) );
		}
		return out;
	}

}

