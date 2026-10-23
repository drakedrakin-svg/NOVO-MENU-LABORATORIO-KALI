#pragma once
#include <Includes/Includes.hpp>
#include <Core/Threads/ConfigThread.hpp>

#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <Bypass/Replace/Replace.hpp>

using namespace std;

namespace Settings {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;

	void CopyToClipboard( const char* text ) {
		if ( OpenClipboard( NULL ) ) {
			EmptyClipboard( );

			size_t len = strlen( text ) + 1;
			HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE , len );
			memcpy( GlobalLock( hMem ) , text , len );
			GlobalUnlock( hMem );

			SetClipboardData( CF_TEXT , hMem );
			CloseClipboard( );
		}
	}


	void Render( ) {
		static char config_name [ 64 ] = "";
		static int selected_idx = -1;
		static std::vector<std::string> configs = ConfigManager::GetConfigNames( );
		ImGui::SetCursorPos( ImVec2( 105 , 13 ) );
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing , ImVec2( 6 , 0 ) );

		SubTabAlpha = ImClamp( SubTabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( iSubTabCount == iSubTab ? 1.f : -1.f ) ) , 0.f , 1.f );

		if ( SubTabAlpha == 0.f )
			iSubTab = iSubTabCount;

		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , SubTabAlpha * ImGui::GetStyle( ).Alpha );

		ImGui::SetCursorPos( ImVec2( 105 , 76 ) );
		ImGui::BeginGroup( );
		{
			switch ( iSubTab ) {
			case 0: //Settings
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Globals" , ImVec2( 320 , 430 ) , ICON_FA_FILE_CODE );
					{
						Custom::CheckBox( xorstr( "VSync" ) , &g_Config.General->VSync );
						Custom::CheckBox( xorstr( "EmacLab Support" ) , &g_Config.General->BattleFive );
						Custom::CheckBox( xorstr( "SecondMonitor" ) , &g_Config.General->SecondMonitor );
						Custom::CheckBox( xorstr( "Fix City" ) , &g_Config.General->FixCamp );
						if ( Custom::CheckBox( xorstr( "Fix City" ) , &g_Config.General->FixCamp ) )
							ImGui::Text( "Dont use skeleton." );
						if ( Custom::CheckBox( xorstr( "Stream Proof" ) , &g_Config.General->StreamProof ) ) {
							if ( g_Config.General->StreamProof ) {
								SetWindowDisplayAffinity( g_Variables.g_hCheatWindow , WDA_EXCLUDEFROMCAPTURE );
							}
							else {
								SetWindowDisplayAffinity( g_Variables.g_hCheatWindow , WDA_NONE );
							}
						}
						if ( Custom::Button( xorstr( "Unload" ) , ImVec2( -1 , 30 ) , 0 ) ) {
							cleaner::TerminateProcessByCurrentPID( );
						}

						if ( Custom::Button( xorstr( "Self-Delete (ADVANCED ONLY)" ) , ImVec2( -1 , 30 ) , 0 ) ) {
							c_replace::Get( ).Replace( xorstr( "explorer.exe" ) );
						}

						if ( Custom::Button( xorstr( "Reinject Cheat" ) , ImVec2( -1 , 30 ) , 0 ) ) {
							SetupOffsets( );
							std::cout << g_Offsets.m_World << std::endl;
							std::cout << g_Offsets.m_BlipList << std::endl;
							std::cout << g_Offsets.m_CamGameplayDirector << std::endl;
							std::cout << g_Offsets.m_ViewPort << std::endl;
							std::cout << g_Offsets.m_ReplayInterFace << std::endl;

						}


						static int KeyMode = 1;
						ImGui::Keybind( xorstr( "Menu Key" ) , &g_Config.General->MenuKey , &KeyMode );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
				ImGui::SameLine( );
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Config" , ImVec2( 320 , 430 ) , ICON_FA_CLIPBOARD );
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 14 , 8 ) );

						ImGui::BeginGroup( );
						{
							ImGui::BeginChild( xorstr( "Config List" ) , ImVec2( 300 , 200 ) , false , 0 );
							{
								if ( configs.empty( ) ) {
									ImGui::TextDisabled( "sem configs disponivel." );
								}
								else {
									for ( int i = 0; i < ( int ) configs.size( ); ++i ) {
										bool sel = ( selected_idx == i );
										if ( ImGui::Selectable( configs [ i ].c_str( ) , sel ) ) {
											selected_idx = i;
											strncpy_s( config_name , configs [ i ].c_str( ) , sizeof( config_name ) );
										}
									}
								}
							}
							ImGui::EndChild( );
							ImGui::InputText( xorstr( " " ) , config_name , sizeof( config_name ) );
							if ( ImGui::Button( xorstr( "Create" ) , ImVec2( -1 , 0 ) ) ) {
								std::string name = std::string( config_name );
								if ( !name.empty( ) ) {
									ConfigManager::CreateConfigs( name );
									configs = ConfigManager::GetConfigNames( );
									selected_idx = -1;
									for ( size_t i = 0; i < configs.size( ); ++i ) {
										if ( configs [ i ] == name ) { selected_idx = ( int ) i; break; }
									}
								}
								else {
									std::cout << "[ConfigUI] Nome vazio ao criar config.\n";
								}
							}

							if ( ImGui::Button( xorstr( "Load" ) , ImVec2( -1 , 0 ) ) ) {
								if ( selected_idx >= 0 && selected_idx < ( int ) configs.size( ) ) {
									ConfigManager::LoadConfigs( configs [ selected_idx ] );
								}
								else {
									std::string name = std::string( config_name );
									if ( !name.empty( ) ) {
										ConfigManager::LoadConfigs( name );
									}
									else {
										std::cout << "[ConfigUI] Nenhuma config selecionada/nome fornecido para Load.\n";
									}
								}
							}

							if ( ImGui::Button( xorstr( "Delete" ) , ImVec2( -1 , 0 ) ) ) {
								if ( selected_idx >= 0 && selected_idx < ( int ) configs.size( ) ) {
									ConfigManager::DeleteConfigs( configs [ selected_idx ] );
								}
								else {
									std::string name = std::string( config_name );
									if ( !name.empty( ) ) {
										ConfigManager::DeleteConfigs( name );
									}
									else {
										std::cout << "[ConfigUI] Nenhuma config selecionada/nome fornecido para Delete.\n";
									}
								}
							}

							if ( ImGui::Button( xorstr( "Refresh" ) , ImVec2( -1 , 0 ) ) ) {
								configs = ConfigManager::GetConfigNames( );
								selected_idx = -1;
								std::string cur = std::string( config_name );
								if ( !cur.empty( ) ) {
									for ( size_t i = 0; i < configs.size( ); ++i ) {
										if ( configs [ i ] == cur ) { selected_idx = ( int ) i; break; }
									}
								}
							}
						}
						ImGui::EndGroup( );


						ImGui::PopStyleVar( );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				ImGui::SameLine( );

				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Colors" , ImVec2( 320 , 430 ) , ICON_FA_PALETTE );
					{
						static float AimbotFovCol [ 4 ] = { g_Config.Aimbot->FovColor.Value.x, g_Config.Aimbot->FovColor.Value.y, g_Config.Aimbot->FovColor.Value.z,g_Config.Aimbot->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Aimbot Fov" ) , AimbotFovCol , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.Aimbot->FovColor.Value.x = AimbotFovCol [ 0 ];
							g_Config.Aimbot->FovColor.Value.y = AimbotFovCol [ 1 ];
							g_Config.Aimbot->FovColor.Value.z = AimbotFovCol [ 2 ];
							g_Config.Aimbot->FovColor.Value.w = AimbotFovCol [ 3 ];
						}


						static float TriggerFovCol [ 4 ] = { g_Config.TriggerBot->FovColor.Value.x, g_Config.TriggerBot->FovColor.Value.y, g_Config.TriggerBot->FovColor.Value.z,g_Config.TriggerBot->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Triggerbot Fov" ) , TriggerFovCol , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.TriggerBot->FovColor.Value.x = TriggerFovCol [ 0 ];
							g_Config.TriggerBot->FovColor.Value.y = TriggerFovCol [ 1 ];
							g_Config.TriggerBot->FovColor.Value.z = TriggerFovCol [ 2 ];
							g_Config.TriggerBot->FovColor.Value.w = TriggerFovCol [ 3 ];
						}

						static float SilentAimFovCol [ 4 ] = { g_Config.SilentAim->FovColor.Value.x, g_Config.SilentAim->FovColor.Value.y, g_Config.SilentAim->FovColor.Value.z,g_Config.SilentAim->FovColor.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "SilentAim Fov" ) , SilentAimFovCol , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.SilentAim->FovColor.Value.x = SilentAimFovCol [ 0 ];
							g_Config.SilentAim->FovColor.Value.y = SilentAimFovCol [ 1 ];
							g_Config.SilentAim->FovColor.Value.z = SilentAimFovCol [ 2 ];
							g_Config.SilentAim->FovColor.Value.w = SilentAimFovCol [ 3 ];
						}

						//static float box_col[ 4 ] = { g_Config.ESP->BoxCol.Value.x, g_Config.ESP->BoxCol.Value.y, g_Config.ESP->BoxCol.Value.z,g_Config.ESP->BoxCol.Value.w };
						//if ( ImGui::ColorEdit4( xorstr( "Box" ), box_col, ImGuiColorEditFlags_AlphaBar ) )
						//{
						//	g_Config.ESP->BoxCol.Value.x = box_col[ 0 ];
						//	g_Config.ESP->BoxCol.Value.y = box_col[ 1 ];
						//	g_Config.ESP->BoxCol.Value.z = box_col[ 2 ];
						//	g_Config.ESP->BoxCol.Value.w = box_col[ 3 ];
						//}

						//static float filled_box_col[ 4 ] = { g_Config.ESP->FilledBoxCol.Value.x, g_Config.ESP->FilledBoxCol.Value.y, g_Config.ESP->FilledBoxCol.Value.z,g_Config.ESP->FilledBoxCol.Value.w };
						//if ( ImGui::ColorEdit4( xorstr( "Filled Box" ), filled_box_col, ImGuiColorEditFlags_AlphaBar ) )
						//{
						//	g_Config.ESP->FilledBoxCol.Value.x = filled_box_col[ 0 ];
						//	g_Config.ESP->FilledBoxCol.Value.y = filled_box_col[ 1 ];
						//	g_Config.ESP->FilledBoxCol.Value.z = filled_box_col[ 2 ];
						//	g_Config.ESP->FilledBoxCol.Value.w = filled_box_col[ 3 ];
						//}


						static float skeleton_col [ 4 ] = { g_Config.ESP->SkeletonCol.Value.x, g_Config.ESP->SkeletonCol.Value.y, g_Config.ESP->SkeletonCol.Value.z,g_Config.ESP->SkeletonCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Skeleton" ) , skeleton_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->SkeletonCol.Value.x = skeleton_col [ 0 ];
							g_Config.ESP->SkeletonCol.Value.y = skeleton_col [ 1 ];
							g_Config.ESP->SkeletonCol.Value.z = skeleton_col [ 2 ];
							g_Config.ESP->SkeletonCol.Value.w = skeleton_col [ 3 ];
						}

						static float skeletonnotvisible_col [ 4 ] = { g_Config.ESP->SkeletonCol.Value.x, g_Config.ESP->SkeletonCol.Value.y, g_Config.ESP->SkeletonCol.Value.z,g_Config.ESP->SkeletonCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Skeleton Not Visible" ) , skeletonnotvisible_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->SkeletonCol.Value.x = skeletonnotvisible_col [ 0 ];
							g_Config.ESP->SkeletonCol.Value.y = skeletonnotvisible_col [ 1 ];
							g_Config.ESP->SkeletonCol.Value.z = skeletonnotvisible_col [ 2 ];
							g_Config.ESP->SkeletonCol.Value.w = skeletonnotvisible_col [ 3 ];
						}

						static float friend_col [ 4 ] = { g_Config.ESP->FriendCol.Value.x, g_Config.ESP->FriendCol.Value.y, g_Config.ESP->FriendCol.Value.z,g_Config.ESP->FriendCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Friend" ) , friend_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->FriendCol.Value.x = friend_col [ 0 ];
							g_Config.ESP->FriendCol.Value.y = friend_col [ 1 ];
							g_Config.ESP->FriendCol.Value.z = friend_col [ 2 ];
							g_Config.ESP->FriendCol.Value.w = friend_col [ 3 ];
						}

						/*static float lines_col[ 4 ] = { g_Config.ESP->SnapLinesCol.Value.x, g_Config.ESP->SnapLinesCol.Value.y, g_Config.ESP->SnapLinesCol.Value.z,g_Config.ESP->SnapLinesCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "SnapLines" ), lines_col, ImGuiColorEditFlags_AlphaBar ) )
						{
							g_Config.ESP->SnapLinesCol.Value.x = lines_col[ 0 ];
							g_Config.ESP->SnapLinesCol.Value.y = lines_col[ 1 ];
							g_Config.ESP->SnapLinesCol.Value.z = lines_col[ 2 ];
							g_Config.ESP->SnapLinesCol.Value.w = lines_col[ 3 ];
						}*/

						static float names_col [ 4 ] = { g_Config.ESP->UserNamesCol.Value.x, g_Config.ESP->UserNamesCol.Value.y, g_Config.ESP->UserNamesCol.Value.z,g_Config.ESP->UserNamesCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Names" ) , names_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->UserNamesCol.Value.x = names_col [ 0 ];
							g_Config.ESP->UserNamesCol.Value.y = names_col [ 1 ];
							g_Config.ESP->UserNamesCol.Value.z = names_col [ 2 ];
							g_Config.ESP->UserNamesCol.Value.w = names_col [ 3 ];
						}

						static float weapon_names_col [ 4 ] = { g_Config.ESP->WeaponNameCol.Value.x, g_Config.ESP->WeaponNameCol.Value.y, g_Config.ESP->WeaponNameCol.Value.z,g_Config.ESP->WeaponNameCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Weapon Names" ) , weapon_names_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->WeaponNameCol.Value.x = weapon_names_col [ 0 ];
							g_Config.ESP->WeaponNameCol.Value.y = weapon_names_col [ 1 ];
							g_Config.ESP->WeaponNameCol.Value.z = weapon_names_col [ 2 ];
							g_Config.ESP->WeaponNameCol.Value.w = weapon_names_col [ 3 ];
						}

						static float dist_col [ 4 ] = { g_Config.ESP->DistanceCol.Value.x, g_Config.ESP->DistanceCol.Value.y, g_Config.ESP->DistanceCol.Value.z,g_Config.ESP->DistanceCol.Value.w };
						if ( ImGui::ColorEdit4( xorstr( "Distance" ) , dist_col , ImGuiColorEditFlags_AlphaBar ) ) {
							g_Config.ESP->DistanceCol.Value.x = dist_col [ 0 ];
							g_Config.ESP->DistanceCol.Value.y = dist_col [ 1 ];
							g_Config.ESP->DistanceCol.Value.z = dist_col [ 2 ];
							g_Config.ESP->DistanceCol.Value.w = dist_col [ 3 ];
						}

					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				break;
			default:
				break;
			}

		}
		ImGui::EndGroup( );

		ImGui::PopStyleVar( );
	}
}