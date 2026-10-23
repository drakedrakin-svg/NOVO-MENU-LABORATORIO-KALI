#pragma once
#include <Includes/includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

namespace World {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;


	void Render( ) {
		ImGui::SetCursorPos( ImVec2( 105 , 13 ) );
		if ( g_MenuInfo.IsOpen ) {
			if ( Custom::SubTabV2( "Players" , iSubTabCount == 0 ) ) iSubTabCount = 0; ImGui::SameLine( );
			if ( Custom::SubTabV2( "Vehicles" , iSubTabCount == 1 ) ) iSubTabCount = 1; ImGui::SameLine( );
		}

		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing , ImVec2( 6 , 0 ) );

		SubTabAlpha = ImClamp( SubTabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( iSubTabCount == iSubTab ? 1.f : -1.f ) ) , 0.f , 1.f );

		if ( SubTabAlpha == 0.f )
			iSubTab = iSubTabCount;


		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , SubTabAlpha * ImGui::GetStyle( ).Alpha );

		ImGui::SetCursorPos( ImVec2( 105 , 76 ) );
		ImGui::BeginGroup( );
		{
			switch ( iSubTab ) {
			case 0: //Players
			{
				static int SeletedIndex = -1;
				bool IsSelected;

				ImGui::BeginGroup( );
				{

					Custom::CustomChild( "List" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 430 ) , ICON_FA_LIST );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 8 ) );
						for ( int i = 0; i < Core::SDK::Game::EntityList.size( ); i++ ) {
							if ( Core::SDK::Game::EntityList [ i ].Ped == Core::SDK::Pointers::pLocalPlayer )
								continue;

							if ( Core::SDK::Game::EntityList [ i ].PedType != 2 )
								continue;

							IsSelected = SeletedIndex == i;
							if ( ImGui::ListSelectable( Core::SDK::Game::EntityList [ i ].NetworkInfo.UserName.c_str( ) , &IsSelected ) ) SeletedIndex = i;
						}
						ImGui::PopStyleVar( );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Info" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 200 ) , ICON_FA_FILE_CODE );
					{
						if ( SeletedIndex == -1 ) {
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , xorstr( "Select Player" ) );
						}
						else {
							auto SelectedPed = Core::SDK::Game::EntityList [ SeletedIndex ];
							ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 4 ) );

							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Name:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , SelectedPed.NetworkInfo.UserName.c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Id:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , std::to_string( SelectedPed.Id ).c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Distance:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , std::to_string( SelectedPed.Distance ).c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Weapon:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , SelectedPed.WeaponName.c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Friend:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , SelectedPed.IsFriend ? xorstr( "True" ) : xorstr( "False" ) );

							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Invisible:" ) ); ImGui::SameLine( );
							//ImGui::TextColored( ImColor( g_Col.SecundaryText ) , SelectedPed.ThisInvisible == 0x01 ? xorstr( "True" ) : xorstr( "False" ) );


							ImGui::PopStyleVar( );
						}
					}
					Custom::EndCustomChild( );

					Custom::CustomChild( "Actions" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 210 ) , ICON_FA_ROCKET );
					{
						if ( SeletedIndex == -1 ) {
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , xorstr( "Select Player" ) );
						}
						else {
							auto& SelectedPed = Core::SDK::Game::EntityList [ SeletedIndex ];

							ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 8 ) );
							bool Friend = SelectedPed.IsFriend;
							if ( Custom::CheckBox( xorstr( "Friend" ) , &Friend ) ) {
								Core::SDK::Game::FriendMap [ SelectedPed.Ped ] = Friend;
							}

							ImGui::Spacing( );

							if ( Custom::Button( xorstr( "Teleport" ) , ImVec2( -1 , 30 ) , 0 ) ) {
								Core::SDK::Pointers::pLocalPlayer->SetPos( SelectedPed.Pos );
							}

							if ( Custom::Button( xorstr( "Copy Clothes" ) , ImVec2( -1 , 30 ) , 0 ) ) {

								if ( SeletedIndex >= 0 && SeletedIndex < Core::SDK::Game::EntityList.size( ) ) {
									CPed* selectedPed = Core::SDK::Game::EntityList [ SeletedIndex ].Ped;
									if ( !selectedPed ) return;

									uintptr_t drawhandler = Mem.Read<uintptr_t>( reinterpret_cast< uintptr_t >( selectedPed ) + 0x48 );
									uintptr_t local_drawhandler = Mem.Read<uintptr_t>( reinterpret_cast< uintptr_t >( Core::SDK::Pointers::pLocalPlayer ) + 0x48 );
									if ( !drawhandler || !local_drawhandler ) return;

									constexpr std::array<std::pair<int , int> , 9> offsets = { {{0x100, 0x100}, {0xF8, 0xF8}, {0x114, 0x114}, {0x108, 0x108}, {0xEC, 0xEC}, {0xF4, 0xF4}, {0xFC, 0xFC}, {0x10C, 0x10C}, {0xAC, 0xAC}} };

									for ( const auto& [sourceOffset , targetOffset] : offsets ) {
										BYTE value = Mem.Read<BYTE>( drawhandler + sourceOffset );
										Mem.Write<BYTE>( local_drawhandler + targetOffset , value );
									}
								}
							}

							ImGui::PopStyleVar( );
						}
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
			}
			break;
			case 1: //Vehicles
			{

				static int SeletedIndex = -1;
				bool IsSelected;


				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "List" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 450 ) , ICON_FA_FILE_CODE );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 8 ) );
						{
							std::lock_guard<std::mutex> lock( Core::Threads::g_VehicleList.vehicleListMutex );

							for ( int i = 0; i < Core::SDK::Game::VehicleList.size( ); i++ ) {
								IsSelected = SeletedIndex == i;
								std::string Name = Core::SDK::Game::VehicleList [ i ].Name + xorstr( " (" ) + std::to_string( ( int ) Core::SDK::Game::VehicleList [ i ].Dist ) + xorstr( "m)" );
								if ( ImGui::ListSelectable( Name.c_str( ) , &IsSelected ) ) SeletedIndex = i;
							}
						}

						ImGui::PopStyleVar( );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Informations" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 200 ) , ICON_FA_FILE_CODE );
					{
						if ( SeletedIndex == -1 ) {
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , xorstr( "Select Vehicle" ) );
						}
						else {
							ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 4 ) );

							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Name:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , Core::SDK::Game::VehicleList [ SeletedIndex ].Name.c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Distance:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , ( std::to_string( ( int ) Core::SDK::Game::VehicleList [ SeletedIndex ].Dist ) + xorstr( "m" ) ).c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Driver:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , Core::SDK::Game::VehicleList [ SeletedIndex ].Driver == 0 ? xorstr( "Unknown" ) : Core::SDK::Game::VehicleList [ SeletedIndex ].Driver->GetPedType( ) != 2 ? xorstr( "NPC" ) : Core::SDK::Game::GetPedName( Core::SDK::Game::VehicleList [ SeletedIndex ].Driver ).c_str( ) );
							ImGui::TextColored( ImColor( g_Col.FeaturesText ) , xorstr( "Doors Locked:" ) ); ImGui::SameLine( );
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , Core::SDK::Game::VehicleList [ SeletedIndex ].IsLocked ? xorstr( "Yes" ) : xorstr( "No" ) );

							ImGui::PopStyleVar( );
						}
					}
					Custom::EndCustomChild( );

					Custom::CustomChild( "Actions" , ImVec2( ImGui::GetWindowSize( ).x / 2 - 65 , 230 ) , ICON_FA_ROCKET );
					{
						if ( SeletedIndex == -1 ) {
							ImGui::TextColored( ImColor( g_Col.SecundaryText ) , xorstr( "Select Vehicle" ) );
						}
						else {
							auto Veh = Core::SDK::Game::VehicleList [ SeletedIndex ].Pointer;
							ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 8 ) );

							if ( Core::SDK::Game::VehicleList [ SeletedIndex ].Pointer->IsLocked( ) ) {
								if ( Custom::Button( xorstr( "Unlock" ) , ImVec2( -1 , 30 ) , 0 ) ) {
									Veh->DoorState( true );
								}
							}
							else {
								if ( Custom::Button( xorstr( "Lock" ) , ImVec2( -1 , 30 ) , 0 ) ) {
									Veh->DoorState( false );
								}
							}

							if ( Custom::Button( xorstr( "Teleport" ) , ImVec2( -1 , 30 ) , 0 ) ) {
								Core::SDK::Pointers::pLocalPlayer->SetPos( Veh->GetPos( ) );
							}

							if ( Custom::Button( xorstr( "Fix Vehicle" ) , ImVec2( -1 , 30 ) , 0 ) ) {

								Veh->Fix( );

								std::thread( [ ] ( ) { NotifyManager::Send( xorstr( "Vehicle Has Been Repaired!" ) , 4000 ); } ).detach( );
							}

							if ( Custom::Button( xorstr( "Request Control (beta)" ) , ImVec2( -1 , 30 ) , 0 ) ) {

								Core::SDK::Pointers::pLocalPlayer == nullptr;

								Veh->Fix( );


								//std::thread([]() { NotifyManager::Send(xorstr("Vehicle Has Been Repaired!"), 4000); }).detach();
							}

							ImGui::PopStyleVar( );
						}
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
			}
			break;
			default:
				break;
			}

		}
		ImGui::EndGroup( );
		ImGui::PopStyleVar( );

	}
}