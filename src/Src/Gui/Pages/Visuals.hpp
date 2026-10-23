#pragma once
#include <Includes/includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>
#include <Includes/CustomWidgets/Preview.hpp>

#include <Core/Core.hpp>

using namespace std;

namespace Visuals {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;
	const char* barPositions [ ] = { "Top", "Right", "Bottom", "Left" };
	const char* textPositions [ ] = { "Top", "Center", "Bottom" };
	const char* namePositions [ ] = { "Top", "Test", "Bottom" };
	void Render( ) {

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
			case 0: //Players
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Enemy ESP" , ImVec2( 320 , 430 ) , ICON_FA_FILE_CODE );
					{
						Custom::CheckBox( xorstr( "Activate" ) , &g_Config.ESP->Enabled );
						Custom::CheckBox( xorstr( "Visibility Filter" ) , &g_Config.ESP->HighlightVisible );

						if ( Custom::CheckBox( xorstr( "Skeleton Overlay" ) , &g_Config.ESP->Skeleton ) ) {
						}
						Custom::CheckBox( xorstr( "Display Player Names" ) , &g_Config.ESP->UserNames );
						Custom::CheckBox( xorstr( "Display Weapon Info" ) , &g_Config.ESP->WeaponName );
						Custom::CheckBox( xorstr( "Display Distance" ) , &g_Config.ESP->DistanceFromMe );

						// Custom::CheckBox(xorstr("Head Circle"), &g_Config.ESP->HeadCircle);
						Custom::CheckBox( xorstr( "Health Indicator" ) , &g_Config.ESP->HealthBar );
						Custom::CheckBox( xorstr( "Armor Indicator" ) , &g_Config.ESP->ArmorBar );
						// Custom::CheckBox(xorstr("Snaplines"), &g_Config.ESP->SnapLines);

						Custom::CheckboxWithKey( xorstr( "Friend Identifier" ) , &g_Config.ESP->FriendsMarker , true , [ ] {
							static int KeyMode = 1;
							Custom::Keybind( xorstr( "Activation Key" ) , &g_Config.ESP->FriendsMarkerBind , &KeyMode );
							} );

						Custom::CheckBox( xorstr( "Show Local Player" ) , &g_Config.ESP->ShowLocalPlayer );
						Custom::CheckBox( xorstr( "Filter Dead Entities" ) , &g_Config.ESP->IgnoreDead );
						Custom::CheckBox( xorstr( "Exclude NPCs" ) , &g_Config.ESP->IgnoreNPCs );
						/*							Custom::CheckBox( xorstr( "Show LocalPlayer" ) , &g_Config.ESP->ShowLocalPlayer );
							Custom::CheckBox( xorstr( "Ignore Dead" ) , &g_Config.ESP->IgnoreDead );
							Custom::CheckBox( xorstr( "Ignore NPCs" ) , &g_Config.ESP->IgnoreNPCs );
							// Lista de opções

							ImGui::Combo( "Health Bar Position" , &g_Config.ESP->HealthBarState , barPositions , IM_ARRAYSIZE( barPositions ) );

							ImGui::Combo( "Armor Bar Position" , &g_Config.ESP->ArmorBarState , barPositions , IM_ARRAYSIZE( barPositions ) );

							ImGui::Combo( "UserName Position" , &g_Config.ESP->UserNamesState , namePositions , IM_ARRAYSIZE( namePositions ) );

							ImGui::SliderInt( xorstr( "Max Distance" ) , &g_Config.ESP->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );

							static int KeyMode = 1;
							ImGui::Keybind( xorstr( "Visuals KeyBind" ) , &g_Config.ESP->KeyBind , &KeyMode );*/
						ImGui::SliderInt( xorstr( "ESP Distance Limit" ) , &g_Config.ESP->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				ImGui::SameLine( );

				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Preview" , ImVec2( 320 , 430 ) , ICON_FA_EYE );
					{
						Custom::g_EspPreview.DragDropHandler( );
						Custom::g_EspPreview.Draw( );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				ImGui::SameLine( );

				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Vehicle ESP" , ImVec2( 320 , 430 ) , ICON_FA_EYE );
					{
						Custom::CheckBox( xorstr( "Activate" ) , &g_Config.VehicleESP->Enabled );
						Custom::CheckBox( xorstr( "Vehicle Names" ) , &g_Config.VehicleESP->VehName );
						Custom::CheckBox( xorstr( "Locked/Unlocked" ) , &g_Config.VehicleESP->ShowLockUnlock );
						Custom::CheckBox( xorstr( "SnapLines" ) , &g_Config.VehicleESP->SnapLines );
						Custom::CheckBox( xorstr( "Distance" ) , &g_Config.VehicleESP->DistanceFromMe );
						ImGui::SliderInt( xorstr( "Max Distance" ) , &g_Config.VehicleESP->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );
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