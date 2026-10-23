#include "SilentAim.hpp"

bool Core::Features::cSilentAim::SilentAimInitialized;
std::uintptr_t Core::Features::cSilentAim::StartAddy;

void Core::Features::cSilentAim::InitializeSilentAim( ) {
	StartAddy = g_Offsets.m_SilentAim;
	SilentAimHook = Mem.CreateCodeCave( 500 );

	if ( !SilentAimHook ) return;

	Mem.WriteBytes( SilentAimHook , SilentAimShell );
	OriginalFuncTable = Mem.ReadBytes( StartAddy , 38 );
	Mem.HookJMP( StartAddy , SilentAimHook );
}

void Core::Features::cSilentAim::RestoreSilent( ) {
	Mem.WriteBytes( StartAddy , OriginalFuncTable );
	Mem.FreeCave( SilentAimHook );
}

void Core::Features::cSilentAim::HookSilent( ) {
	uintptr_t IsPedShooting = Mem.FindSignature(
		{ 0x40, 0x53, 0x48, 0x83, 0xec, 0x00 , 0x8b, 0xd9, 0xe8, 0x00 , 0x00 , 0x00 , 0x00 , 0x8b, 0xcb, 0xe8, 0x00 , 0x00 , 0x00 , 0x00 , 0x48, 0x8b, 0xd8, 0x48, 0x85, 0xc0, 0x74, 0x00 , 0x8b, 0x0d, 0x00 , 0x00 , 0x00 , 0x00 , 0x4c, 0x8b, 0x00, 0x48, 0x8d, 0x54, 0x24, 0x00 , 0x89, 0x4c, 0x24, 0x00 , 0x48, 0x8b, 0xc8, 0x41, 0xff, 0x50, 0x00 , 0x84, 0xc0, 0x74, 0x00 , 0x48, 0x8b, 0xc3, 0xeb, 0x00 , 0x33, 0xc0, 0x48, 0x83, 0xc4, 0x00 , 0x5b, 0xc3, 0xcc, 0x0f, 0x48, 0x89 }
	);

	while ( true ) {
		if ( g_Config.SilentAim->Enabled && g_Config.SilentAim->KeyBind != 0 &&
			GetAsyncKeyState( g_Config.SilentAim->KeyBind ) & 0x8000 &&
			GetForegroundWindow( ) != g_Variables.g_hCheatWindow ) {
			CPed* LocalPlayer = Core::SDK::Pointers::pLocalPlayer;
			if ( !LocalPlayer ) { continue; }

			int LocalId = Mem.Read<int>( ( uintptr_t ) LocalPlayer + g_Offsets.m_PlayerId );
			if ( LocalId <= 0 ) { continue; }

			int ShooterId = Mem.Read<int>( ( uintptr_t ) LocalPlayer + g_Offsets.m_PlayerId );
			//if (ShooterId != LocalId) { continue; }

			CPed* Ped = Core::SDK::Game::GetClosestPed(
				g_Config.SilentAim->MaxDistance ,
				g_Config.SilentAim->IgnoreNPCs ,
				g_Config.SilentAim->OnlyVisible
			);
			if ( !Ped ) { continue; }

			D3DXVECTOR3 HeadPos = Ped->GetBonePosDefault( 0 );
			D3DXVECTOR2 HeadToScreen = Core::SDK::Game::WorldToScreen( HeadPos );

			if ( Core::SDK::Game::IsOnScreen( HeadToScreen ) ) {
				int Fov = std::hypot(
					HeadToScreen.x - g_Variables.g_vGameWindowCenter.x ,
					HeadToScreen.y - g_Variables.g_vGameWindowCenter.y
				);

				if ( Fov < g_Config.SilentAim->FOV ) {
					if ( !SilentAimInitialized ) {
						InitializeSilentAim( );
						SilentAimInitialized = true;
					}

					bool Miss = g_Config.SilentAim->MissChance >= Utils::GenRandomInt( 0 , 100 );
					auto FinalPos = Miss ? HeadPos + D3DXVECTOR3( 0.0 , 0.4 , 0 ) : HeadPos + D3DXVECTOR3( 0 , 0 , 0.08 );

					memcpy( SilentAimShell.data( ) + 3 , &FinalPos.x , sizeof( float ) );
					memcpy( SilentAimShell.data( ) + 10 , &FinalPos.y , sizeof( float ) );
					memcpy( SilentAimShell.data( ) + 17 , &FinalPos.z , sizeof( float ) );

					uintptr_t BackAddress = ( uintptr_t ) ( StartAddy + 0x26 );
					memcpy( SilentAimShell.data( ) + 27 , &BackAddress , sizeof( BackAddress ) );

					Mem.WriteBytes( SilentAimHook , SilentAimShell );
				}
				else {
					if ( SilentAimInitialized ) {
						RestoreSilent( );
						SilentAimInitialized = false;
					}
				}
			}
			else {
				if ( SilentAimInitialized ) {
					RestoreSilent( );
					SilentAimInitialized = false;
				}
			}
		}
		else {
			if ( SilentAimInitialized ) {
				RestoreSilent( );
				SilentAimInitialized = false;
			}
		}

		std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
	}
}

auto Core::Features::cCustomSilent::Inject( ) -> void { }
auto Core::Features::cCustomSilent::Toogle( bool toogle ) -> void { }
auto Core::Features::cCustomSilent::ChangeFov( int FovValue ) -> void { }
auto Core::Features::cCustomSilent::ChangeMissChance( int FovValue ) -> void { }

auto Core::Features::cCustomSilent::ChangeConfig( int Feature , int Value , bool toogle ) -> void {
	if ( Feature == 0 ) {
		Toogle( toogle );
	}
	else if ( Feature == 1 ) {
		ChangeFov( Value );
	}
	else if ( Feature == 2 ) {
		ChangeMissChance( Value );
	}
	std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
}
