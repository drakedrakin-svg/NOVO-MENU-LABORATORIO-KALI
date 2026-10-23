#pragma once
#include <Includes/Includes.hpp>
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

namespace Combat {

	static int iSubTabCount = 0;
	static float SubTabAlpha = 0.f;
	static int iSubTab = 0;

	void Render( ) {

		ImGui::SetCursorPos( ImVec2( 105 , 13 ) );
		SubTabAlpha = ImClamp( SubTabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( iSubTabCount == iSubTab ? 1.f : -1.f ) ) , 0.f , 1.f );

		if ( SubTabAlpha == 0.f )
			iSubTab = iSubTabCount;

		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , SubTabAlpha * ImGui::GetStyle( ).Alpha );

		ImGui::SetCursorPos( ImVec2( 105 , 76 ) );
		ImGui::BeginGroup( );
		{
			switch ( iSubTab ) {
			case 0: //Aimbot
				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Aimbot" , ImVec2( 320 , 430 ) , ICON_FA_FILE_CODE );
					{
						Custom::CheckboxWithKey( "Activate Aimbot" , &g_Config.Aimbot->Enabled , true , [ & ] ( ) {
							static int KeyMode = 1;
							Custom::KeyBind( "Key" , &g_Config.Aimbot->KeyBind , &KeyMode );

							} );
						Custom::CheckBox( xorstr( "Visualize Fov" ) , &g_Config.Aimbot->ShowFov );
						Custom::CheckBox( xorstr( "Visibility Filter" ) , &g_Config.Aimbot->OnlyVisible );
						Custom::CheckBox( xorstr( "Skip NPCs" ) , &g_Config.Aimbot->IgnoreNPCs );

						ImGui::SliderInt( xorstr( "Field of View" ) , &g_Config.Aimbot->FOV , 0 , 400 , xorstr( "%d" ) , 0 );
						ImGui::SliderInt( xorstr( "Smooth" ) , &g_Config.Aimbot->AimbotSpeed , 0 , 100 , xorstr( "%d" ) , 0 );
						ImGui::SliderInt( xorstr( "Max Distance" ) , &g_Config.Aimbot->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				ImGui::SameLine( );

				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "Silent" , ImVec2( 320 , 430 ) , ICON_FA_COMPUTER_MOUSE );
					{
						Custom::CheckboxWithKey( "Activate Silent" , &g_Config.SilentAim->Enabled , true , [ & ] ( ) { static int KeyMode = 1; Custom::KeyBind( "Key" , &g_Config.SilentAim->KeyBind , &KeyMode ); } );
						if ( g_Config.SilentAim->Enabled )
						{
							Custom::CheckBox( xorstr( "Visualize Fov" ) , &g_Config.SilentAim->ShowFov );
							Custom::CheckBox( xorstr( "Visibility Filter" ) , &g_Config.SilentAim->OnlyVisible );
							Custom::CheckBox( xorstr( "Skip NPCs" ) , &g_Config.SilentAim->IgnoreNPCs );
							ImGui::SliderInt( xorstr( "Field of View" ) , &g_Config.SilentAim->FOV , 0 , 400 , xorstr( "%d" ) , 0 );
							ImGui::SliderInt( xorstr( "Miss Chance" ) , &g_Config.SilentAim->MissChance , 0 , 100 , xorstr( "%dx" ) , 0 );
							ImGui::SliderInt( xorstr( "Max Distance" ) , &g_Config.SilentAim->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );
						}
						Custom::CheckBox( xorstr( "Activate MagicBullets" ) , &g_Config.MagicBullet->Enabled );
						if ( g_Config.MagicBullet->Enabled ) {
							ImGui::SliderInt( xorstr( "Miss Chance" ) , &g_Config.MagicBullet->MissChance , 0 , 400 , xorstr( "%d" ) , 0 );
							static int KeyMode = 1;
							Custom::KeyBind( "Key" , &g_Config.MagicBullet->KeyBind , &KeyMode );
						}

					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );

				ImGui::SameLine( );

				ImGui::BeginGroup( );
				{
					Custom::CustomChild( "TriggerBot" , ImVec2( 320 , 430 ) , ICON_FA_COMPUTER_MOUSE );
					{
						Custom::CheckboxWithKey( "Activate TriggerBot" , &g_Config.TriggerBot->Enabled , true , [ & ] ( ) { static int KeyMode = 1; Custom::KeyBind( "Key" , &g_Config.TriggerBot->KeyBind , &KeyMode ); } );
						Custom::CheckBox( xorstr( "Smart Triggerbot" ) , &g_Config.TriggerBot->SmartTrigger );
						Custom::CheckBox( xorstr( "Skip NPCs" ) , &g_Config.TriggerBot->IgnoreNPCs );
						if ( !g_Config.TriggerBot->SmartTrigger ) {
							Custom::CheckBox( xorstr( "Visualize Fov" ) , &g_Config.TriggerBot->ShowFov );
							ImGui::SliderInt( xorstr( "Field of View" ) , &g_Config.TriggerBot->FOV , 0 , 400 , xorstr( "%d" ) , 0 );
							ImGui::SliderInt( xorstr( "Max Distance" ) , &g_Config.TriggerBot->MaxDistance , 0 , 1000 , xorstr( "%dm" ) , 0 );
						}
						ImGui::SliderInt( xorstr( "Delay" ) , &g_Config.TriggerBot->Delay , 0 , 10 , xorstr( "%d" ) , 0 );
					}
					Custom::EndCustomChild( );
				}
				ImGui::EndGroup( );
			default:
				break;
			}

		}
		ImGui::EndGroup( );

		ImGui::PopStyleVar( );
	}
}