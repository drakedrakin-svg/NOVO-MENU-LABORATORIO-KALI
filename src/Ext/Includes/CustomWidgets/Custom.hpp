#pragma once
#include <Includes/includes.hpp>
#include <Includes/Utils.hpp>
#include <map>


namespace Custom {

	using namespace ImGui;

	const char* const KeyNames [ ] =
	{
		"None",
		"Left Button",
		"Right Button",
		"Cancel",
		"Mouse Button",
		"Mouse 5",
		"Mouse 6",
		"Unknown",
		"Backspace",
		"TAB",
		"Unknown",
		"Unknown",
		"Clear",
		"Enter",
		"Unknown",
		"Unknown",
		"Shift",
		"Control",
		"ALT",
		"Pause",
		"Capital",
		"Kana",
		"Unknown",
		"Junja",
		"Final",
		"Kanji",
		"Unknown",
		"Escape",
		"Convert",
		"Nonconvert",
		"Accpet",
		"Mode Changer",
		"Space",
		"Prior",
		"Next",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"Select",
		"Print",
		"Execute",
		"Snapshot",
		"Insert",
		"Delete",
		"Help",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"Left Winndows",
		"Right Windows",
		"Apps",
		"Unknown",
		"Sleep",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"Separator",
		"Subtract",
		"Decimal",
		"Divided",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"NumLock",
		"Scroll",
		"VK_OEM_NEC_EQUAL",
		"VK_OEM_FJ_MASSHOU",
		"VK_OEM_FJ_TOUROKU",
		"VK_OEM_FJ_LOYA",
		"VK_OEM_FJ_ROYA",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Left Shift",
		"Right Shift",
		"Left Control",
		"Right Control",
		"Left Menu",
		"Right Menu"
	};


	inline double EaseInOutCirc( double t ) {
		if ( t < 0.5 )
			return ( 1 - std::sqrt( 1 - 2 * t ) ) * 0.5;
		else
			return ( 1 + std::sqrt( 2 * t - 1 ) ) * 0.5;
	}

	inline int rotation_start_index;
	inline void ImRotateStart( ) {
		rotation_start_index = ImGui::GetWindowDrawList( )->VtxBuffer.Size;
	}

	inline ImVec2 ImRotationCenter( ) {
		ImVec2 l( FLT_MAX , FLT_MAX ) , u( -FLT_MAX , -FLT_MAX ); // bounds

		const auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
		for ( int i = rotation_start_index; i < buf.Size; i++ )
			l = ImMin( l , buf [ i ].pos ) , u = ImMax( u , buf [ i ].pos );

		return ImVec2( ( l.x + u.x ) / 2 , ( l.y + u.y ) / 2 ); // or use _ClipRectStack?
	}

	inline void ImRotateEnd( float rad , ImVec2 center = ImRotationCenter( ) ) {
		float s = sin( rad ) , c = cos( rad );
		center = ImRotate( center , s , c ) - center;

		auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
		for ( int i = rotation_start_index; i < buf.Size; i++ )
			buf [ i ].pos = ImRotate( buf [ i ].pos , s , c ) - center;
	}

	inline void ProfileBar( ) {
		ImVec2 WindowPos = ImGui::GetWindowPos( );
		ImDrawList* DrawList = ImGui::GetWindowDrawList( );

		std::string Username = g_Variables.UserName;
		std::string Role = g_Variables.Role;
		std::string PrimeiraLetra = Username.substr( 0 , 1 );
		std::transform( PrimeiraLetra.begin( ) , PrimeiraLetra.end( ) , PrimeiraLetra.begin( ) , [ ] ( unsigned char c ) { return std::toupper( c ); } );

		if ( Username.length( ) > 10 ) {
			Username = Username.substr( 0 , 7 ) + xorstr( "..." );
		}

		ImVec2 PrimeiraLetraNameTextSize = Utils::CalcTextSize( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , PrimeiraLetra.c_str( ) );
		ImVec2 NameTextSize = Utils::CalcTextSize( g_Variables.m_FontSecundary , g_Variables.m_FontSecundary->FontSize , Username.c_str( ) );
		ImVec2 RoleTextSize = Utils::CalcTextSize( g_Variables.m_FontSmaller , g_Variables.m_FontSmaller->FontSize , Role.c_str( ) );

		ImVec2 ProfilePos = { WindowPos.x + g_MenuInfo.MenuSize.x - 30, WindowPos.y + 30 };
		DrawList->AddCircleFilled( ProfilePos , 20 , ImGui::GetColorU32( ( ImVec4 ) ImColor( 17 , 17 , 20 ) ) , 100 );
		DrawList->AddCircle( ProfilePos , 20 , ImGui::GetColorU32( ( ImVec4 ) ( g_Col.BorderCol ) ) , 100 );


		ImVec2 ProfileLetra = { ProfilePos.x - PrimeiraLetraNameTextSize.x / 2, ProfilePos.y - PrimeiraLetraNameTextSize.y / 2 - 1 };
		DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ProfileLetra , ImGui::GetColorU32( ( ImVec4 ) ImColor( 163 , 163 , 163 ) ) , PrimeiraLetra.c_str( ) );

		ImVec2 UsernamePos = { ProfilePos.x - 32 - NameTextSize.x, ProfileLetra.y - 6 };
		DrawList->AddText( g_Variables.m_FontSecundary , g_Variables.m_FontSecundary->FontSize , UsernamePos , ImGui::GetColorU32( ( ImVec4 ) ImColor( 163 , 163 , 163 ) ) , Username.c_str( ) );

		ImVec2 RolePos = { ProfilePos.x - 32 - RoleTextSize.x, ProfileLetra.y + 8 };
		DrawList->AddText( g_Variables.m_FontSmaller , g_Variables.m_FontSmaller->FontSize , RolePos , ImGui::GetColorU32( ( ImVec4 ) ImColor( 80 , 80 , 80 ) ) , Role.c_str( ) );
	}

	inline void DrawBackground( bool Logged ) {
		const ImVec2 pos = ImGui::GetWindowPos( );
		ImDrawList* draw = ImGui::GetWindowDrawList( );
		ImDrawList* draw_list = GetWindowDrawList( );
		const ImVec2 size = GetWindowSize( );

		//Brackground
		draw->AddRectFilled( ImVec2( pos.x + 0 , pos.y + 0 ) , ImVec2( pos.x + g_MenuInfo.MenuSize.x , pos.y + g_MenuInfo.MenuSize.y ) , ImGui::GetColorU32( g_Col.BackgroundCol ) , 6.f );

		if ( Logged ) {
			ImVec2 topStart = ImVec2( pos.x , pos.y + 60 );
			ImVec2 topEnd = ImVec2( pos.x + g_MenuInfo.MenuSize.x , pos.y + 60 );

			ImDrawList* DrawList = ImGui::GetWindowDrawList( );
			ImVec2 Pos = ImGui::GetWindowPos( );
			ImVec2 Size = ImGui::GetWindowSize( );

			////Background Tabs - Vertical
			DrawList->AddRectFilled( Pos , Pos + ImVec2( Size.x / 12.0f , Size.y ) , ImColor( 15 , 15 , 15 , ( int ) ( ImGui::GetStyle( ).Alpha * 255 ) ) , ImGui::GetStyle( ).WindowRounding , ImDrawFlags_RoundCornersLeft );
			DrawList->AddLine( Pos + ImVec2( Size.x / 12.0f , 0 ) , Pos + ImVec2( Size.x / 12.0f , Size.y ) , ImGui::GetColorU32( ImGuiCol_Border ) );

			//Background SubTabs - Horizontal
			DrawList->AddRectFilled( Pos , Pos + ImVec2( Size.x , Size.y / 10.0f ) , ImColor( 15 , 15 , 15 , ( int ) ( ImGui::GetStyle( ).Alpha * 255 ) ) , ImGui::GetStyle( ).WindowRounding , ImDrawFlags_RoundCornersTop );
			DrawList->AddLine( Pos + ImVec2( 0 , Size.y / 10.0f ) , Pos + ImVec2( Size.x , Size.y / 10.0f ) , ImGui::GetColorU32( ImGuiCol_Border ) , 0.5f );

			//Background SubTabs - Horizontal Inferior
			ImVec2 barStart = Pos + ImVec2( 0 , Size.y - ( Size.y / 10.0f ) );
			ImVec2 barEnd = Pos + ImVec2( Size.x , Size.y );

			DrawList->AddRectFilled( barStart , barEnd , ImColor( 15 , 15 , 15 , ( int ) ( ImGui::GetStyle( ).Alpha * 255 ) ) , ImGui::GetStyle( ).WindowRounding , ImDrawFlags_RoundCornersBottom );
			DrawList->AddLine( barStart , barStart + ImVec2( Size.x , 0 ) , ImGui::GetColorU32( ImGuiCol_Border ) , 0.5f );
		}


		//Window Border
		draw->AddRect( ImVec2( pos.x + 0 , pos.y + 0 ) , ImVec2( pos.x + g_MenuInfo.MenuSize.x , pos.y + g_MenuInfo.MenuSize.y ) , ImGui::GetColorU32( g_Col.BorderCol ) , 6.f );

		if ( g_MenuInfo.IsOpen ) {
			//draw->add_image_rounded(draw_list, g_Variables.decor_background, { pos.x + 80.0f, pos.y }, { pos.x + size.x, pos.y + size.y }, { 0, 0 }, { 1, 1 }, ImColor(176, 63, 155), 6.0f, ImDrawFlags_RoundCornersRight);
			static int Pad = 20;

			ImGui::SetCursorPos( ImVec2( Pad + 490 , Pad - 25 ) );
			ImGui::Image( g_Variables.decor_background , ImVec2( 85 , 70 ) );
		}

		if ( !Logged ) {
			static int Pad = 20;
			ImGui::SetCursorPos( ImVec2( Pad - 6 - 36 + 170 , Pad + 5 - 10 ) );
			ImGui::Image( g_Variables.Logo , ImVec2( 160 + Pad * 2 , 90 + Pad * 2 ) );
		}
	}

	struct tab_state {
		float arrow_opticaly;
		float text_offset;
		ImVec4 text;
	};

	inline bool Tab( const char* icon , const char* label , bool active ) {
		struct Tab_t {
			ImVec4 BackgroundColor;
			ImVec2 BackgroundGrow;
			ImVec4 IconColor;
			ImVec4 TextColor;
			float UnSelectedAnim;
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		ImDrawList* DrawList = window->DrawList;

		if ( window->SkipItems ) {
			return false;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImGuiIO IO = g.IO;

		const ImVec2 pos = window->DC.CursorPos;
		const ImRect rect( pos , pos + ImVec2( 134 , 34 ) );
		ImGui::ItemSize( rect , style.FramePadding.y );
		if ( !ImGui::ItemAdd( rect , id ) ) {
			return false;
		}

		bool hovered , held;
		bool pressed = ImGui::ButtonBehavior( rect , id , &hovered , &held );
		if ( pressed ) { ImGui::MarkItemEdited( id ); }

		static std::map<ImGuiID , Tab_t> anim;
		auto TabAnim = anim.find( id );

		if ( TabAnim == anim.end( ) ) {
			anim.insert( { id, Tab_t( ) } );
			TabAnim = anim.find( id );
		}

		float NormalizedTime = ImClamp( IO.DeltaTime * 10.f , 0.0f , 1.0f );
		float NormalizedTime2 = ImClamp( IO.DeltaTime * 8.f , 0.0f , 1.0f );
		TabAnim->second.BackgroundColor = ImLerp( TabAnim->second.BackgroundColor , active ? ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 30.f / 255.f ) : ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.f ) , Custom::EaseInOutCirc( NormalizedTime ) );
		TabAnim->second.IconColor = ImLerp( TabAnim->second.IconColor , active ? ImColor( g_Col.Base ) : ImColor( 60 , 60 , 60 ) , Custom::EaseInOutCirc( NormalizedTime ) );
		TabAnim->second.TextColor = ImLerp( TabAnim->second.TextColor , active ? ImColor( 200 , 200 , 200 ) : ImColor( 60 , 60 , 60 ) , Custom::EaseInOutCirc( NormalizedTime ) );
		TabAnim->second.BackgroundGrow = ImLerp( TabAnim->second.BackgroundGrow , active ? ImVec2( 4 , 4 ) : ImVec2( 0 , 0 ) , Custom::EaseInOutCirc( NormalizedTime2 ) );
		TabAnim->second.UnSelectedAnim = ImLerp( TabAnim->second.UnSelectedAnim , hovered && !active ? 4.f : 0.f , IO.DeltaTime * 8.f );

		const float rounding = 6.f;
		DrawList->AddRectFilled( ImVec2( rect.Min.x - TabAnim->second.BackgroundGrow.x , rect.Min.y - TabAnim->second.BackgroundGrow.x ) , ImVec2( rect.Max.x + TabAnim->second.BackgroundGrow.x , rect.Max.y + TabAnim->second.BackgroundGrow.x ) , ImGui::GetColorU32( TabAnim->second.BackgroundColor ) , rounding , 0 );

		auto IconTextSize = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , icon );
		DrawList->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , ImVec2( rect.Min.x + 8 + TabAnim->second.UnSelectedAnim , pos.y + ( ( rect.GetHeight( ) - IconTextSize.y ) / 2 ) ) , ImGui::GetColorU32( TabAnim->second.IconColor ) , icon );

		auto TextSize = Utils::CalcTextSize( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , label );
		DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ImVec2( rect.Min.x + IconTextSize.x + 20 + TabAnim->second.UnSelectedAnim , pos.y + ( ( rect.GetHeight( ) - TextSize.y ) / 2 ) ) , ImGui::GetColorU32( TabAnim->second.TextColor ) , label );


		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags );
		return pressed;
	}

	inline ImVec4 accent_color = ImColor( 176 , 63 , 155 , 255 );

	inline bool TabBeta( bool selectable , const char* icon , const char* label , const ImVec2& size_arg ) {
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems ) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = CalcTextSize( label , NULL , true );
		const ImVec2 icon_size = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , icon );
		const ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = CalcItemSize( size_arg , label_size.x + icon_size.x + 8.0f , ImMax( label_size.y , icon_size.y ) );
		const ImRect bb( pos , pos + size );

		ImGui::ItemSize( size , 0.f );
		if ( !ImGui::ItemAdd( bb , id ) ) return false;

		bool hovered , held , pressed = ImGui::ButtonBehavior( bb , id , &hovered , &held , NULL );

		static std::map<ImGuiID , tab_state> anim;
		auto it_anim = anim.find( id );
		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, tab_state( ) } );
			it_anim = anim.find( id );
		}

		it_anim->second.text = ImLerp( it_anim->second.text ,
			selectable ? ImColor( 255 , 255 , 255 , 255 ) :
			hovered ? ImColor( 176 , 63 , 155 , 255 ) :
			ImColor( 59 , 62 , 72 , 255 ) ,
			g.IO.DeltaTime * 7.5f );

		it_anim->second.text_offset = ImClamp( it_anim->second.text_offset + ( 6.f * g.IO.DeltaTime * ( selectable ? 10.f : -10.f ) ) , 0.f , 24.f );
		it_anim->second.arrow_opticaly = ImLerp( it_anim->second.arrow_opticaly , selectable ? 1.f : 0.f , g.IO.DeltaTime * 7.5f );

		GetWindowDrawList( )->AddCircleFilled( ImVec2( bb.Min.x + 8.f , bb.Min.y + 10.f ) , 3.f , GetColorU32( accent_color , it_anim->second.arrow_opticaly ) , 100.f );

		ImVec2 icon_pos = ImVec2( bb.Min.x + it_anim->second.text_offset , bb.Max.y - icon_size.y - ( size.y - icon_size.y ) / 2 );
		GetWindowDrawList( )->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , icon_pos , GetColorU32( it_anim->second.text ) , icon );

		ImVec2 label_pos = ImVec2( icon_pos.x + icon_size.x + 6.f , bb.Max.y - label_size.y - ( size.y - label_size.y ) / 2 );
		GetWindowDrawList( )->AddText( label_pos , GetColorU32( it_anim->second.text ) , label );

		return pressed;
	}

	inline bool SubTabV2( const char* label , bool active ) {
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems ) return false;

		ImDrawList* DrawList = window->DrawList;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImGuiIO& IO = g.IO;

		static std::unordered_map<ImGuiID , float> alphaMap;

		const float fadeSpeed = 8.0f;

		float& alpha = alphaMap [ id ];

		if ( active )
			alpha = ImMin( alpha + IO.DeltaTime * fadeSpeed , 1.0f );
		else
			alpha = ImMax( alpha - IO.DeltaTime * fadeSpeed , 0.0f );

		const ImVec2 pos = window->DC.CursorPos;

		const float fixedWidth = 80.f;
		const float fixedHeight = 30.f;
		const float rounding = 5.f;

		const ImRect rect( pos , pos + ImVec2( fixedWidth , fixedHeight ) );
		ImGui::ItemSize( rect , style.FramePadding.y );
		if ( !ImGui::ItemAdd( rect , id ) ) return false;

		bool hovered , held;
		bool pressed = ImGui::ButtonBehavior( rect , id , &hovered , &held );
		if ( pressed ) ImGui::MarkItemEdited( id );

		const ImVec2 rectMin = rect.Min;
		const ImVec2 rectMax = rect.Max;

		ImVec4 baseColor = g_Col.Base;
		ImVec4 darkColor = g_Col.DarkenColor( baseColor , 0.5f );

		ImVec4 colTop = baseColor;
		colTop.w *= alpha;
		ImVec4 colBot = darkColor;
		colBot.w *= alpha;

		if ( alpha > 0.01f )
			DrawList->AddRectFilledMultiColor( rectMin , rectMax , ImGui::ColorConvertFloat4ToU32( colTop ) , ImGui::ColorConvertFloat4ToU32( colTop ) , ImGui::ColorConvertFloat4ToU32( colBot ) , ImGui::ColorConvertFloat4ToU32( colBot ) , rounding );
		else {
			ImU32 bg = ImColor( 5 , 5 , 5 , 255 );
			DrawList->AddRectFilled( rectMin , rectMax , bg , rounding );
		}

		ImVec4 textColorActive = ImVec4( 0 , 0 , 0 , alpha );
		ImVec4 textColorInactive = ImVec4( 0.50f , 0.50f , 0.50f , 1.0f - alpha );
		ImVec4 textColor = ImLerp( textColorInactive , textColorActive , alpha );

		auto textSize = Utils::CalcTextSize( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , label );
		ImVec2 textPos = ImVec2( pos.x + ( ( fixedWidth - textSize.x ) / 2.f ) , pos.y + ( ( fixedHeight - textSize.y ) / 2.f ) - 1.f );

		DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , textPos , ImGui::GetColorU32( textColor ) , label );

		return pressed;
	}

	inline bool SubTabBeta( bool selectable , const char* icon , const char* label , const ImVec2& size_arg ) {
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems ) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = CalcTextSize( label , NULL , true );
		const ImVec2 icon_size = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , icon );
		const ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = CalcItemSize( size_arg , label_size.x + icon_size.x + 8.0f , ImMax( label_size.y , icon_size.y ) );
		const ImRect bb( pos , pos + size );

		ImGui::ItemSize( size , 0.f );
		if ( !ImGui::ItemAdd( bb , id ) ) return false;

		bool hovered , held , pressed = ImGui::ButtonBehavior( bb , id , &hovered , &held , NULL );

		static std::map<ImGuiID , tab_state> anim;
		auto it_anim = anim.find( id );
		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, tab_state( ) } );
			it_anim = anim.find( id );
		}

		it_anim->second.text = ImLerp( it_anim->second.text ,
			selectable ? ImColor( 135 , 131 , 142 , 255 ) :
			hovered ? ImColor( 176 , 63 , 155 , 255 ) :
			ImColor( 59 , 62 , 72 , 255 ) ,
			g.IO.DeltaTime * 7.5f );

		it_anim->second.text_offset = ImClamp( it_anim->second.text_offset + ( 6.f * g.IO.DeltaTime * ( selectable ? 10.f : -10.f ) ) , 0.f , 24.f );
		it_anim->second.arrow_opticaly = ImLerp( it_anim->second.arrow_opticaly , selectable ? 1.f : 0.f , g.IO.DeltaTime * 7.5f );

		ImVec2 arrow_center = ImVec2( bb.Min.x + 8.f , bb.Min.y + 10.f );
		float size_arrow = 5.0f;
		ImVec2 p1 = ImVec2( arrow_center.x - size_arrow * 0.5f , arrow_center.y - size_arrow * 0.5f );
		ImVec2 p2 = ImVec2( arrow_center.x - size_arrow * 0.5f , arrow_center.y + size_arrow * 0.5f );
		ImVec2 p3 = ImVec2( arrow_center.x + size_arrow * 0.5f , arrow_center.y );
		GetWindowDrawList( )->AddTriangleFilled( p1 , p2 , p3 , GetColorU32( accent_color , it_anim->second.arrow_opticaly ) );


		ImVec2 icon_pos = ImVec2( bb.Min.x + it_anim->second.text_offset , bb.Max.y - icon_size.y - ( size.y - icon_size.y ) / 2 );
		GetWindowDrawList( )->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , icon_pos , GetColorU32( it_anim->second.text ) , icon );

		ImVec2 label_pos = ImVec2( icon_pos.x + icon_size.x + 6.f , bb.Max.y - label_size.y - ( size.y - label_size.y ) / 2 );
		GetWindowDrawList( )->AddText( label_pos , GetColorU32( it_anim->second.text ) , label );

		return pressed;
	}

	inline bool SubTab( const char* label , bool active ) {
		struct SubTab_t {
			ImVec4 BackgroundColor;
			ImVec2 BackgroundGrow;
			ImVec4 TextColor;
			float UnSelectedAnim;
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		ImDrawList* DrawList = window->DrawList;

		if ( window->SkipItems ) {
			return false;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		std::string IdStr = std::string( label );
		const ImGuiID id = window->GetID( IdStr.c_str( ) );
		const ImGuiIO IO = g.IO;

		const ImVec2 pos = window->DC.CursorPos;

		auto TextSize = Utils::CalcTextSize( g_Variables.m_FontSecundary , g_Variables.m_FontSecundary->FontSize , label );

		const ImRect rect( pos , pos + ImVec2( TextSize.x + 16 , 30 ) );
		ImGui::ItemSize( rect , style.FramePadding.y );
		if ( !ImGui::ItemAdd( rect , id ) ) {
			return false;
		}

		bool hovered , held;
		bool pressed = ImGui::ButtonBehavior( rect , id , &hovered , &held );
		if ( pressed ) { ImGui::MarkItemEdited( id ); }

		static std::map<ImGuiID , SubTab_t> anim;
		auto SubTabAnim = anim.find( id );

		if ( SubTabAnim == anim.end( ) ) {
			anim.insert( { id, SubTab_t( ) } );
			SubTabAnim = anim.find( id );
		}

		float NormalizedTime = ImClamp( IO.DeltaTime * 10.f , 0.0f , 1.0f );
		float NormalizedTime2 = ImClamp( IO.DeltaTime * 8.f , 0.0f , 1.0f );
		SubTabAnim->second.BackgroundColor = ImLerp( SubTabAnim->second.BackgroundColor , active ? ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 30.f / 255.f ) : ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.f ) , Custom::EaseInOutCirc( NormalizedTime ) );
		SubTabAnim->second.TextColor = ImLerp( SubTabAnim->second.TextColor , active ? ImColor( /*g_Col.Base*/ 255 , 255 , 255 ) : ImColor( 60 , 60 , 60 ) , Custom::EaseInOutCirc( NormalizedTime ) );
		SubTabAnim->second.BackgroundGrow = ImLerp( SubTabAnim->second.BackgroundGrow , active ? ImVec2( 4 , 4 ) : ImVec2( 0 , 0 ) , Custom::EaseInOutCirc( NormalizedTime2 ) );
		SubTabAnim->second.UnSelectedAnim = ImLerp( SubTabAnim->second.UnSelectedAnim , hovered && !active ? 2.f : 0.f , IO.DeltaTime * 4.f );

		const float rounding = 12.f;
		DrawList->AddRectFilled( ImVec2( rect.Min.x - SubTabAnim->second.BackgroundGrow.x , rect.Min.y - SubTabAnim->second.BackgroundGrow.x ) , ImVec2( rect.Max.x + SubTabAnim->second.BackgroundGrow.x , rect.Max.y + SubTabAnim->second.BackgroundGrow.x ) , ImGui::GetColorU32( SubTabAnim->second.BackgroundColor ) , rounding , 0 );

		DrawList->AddText( g_Variables.m_FontSecundary , g_Variables.m_FontSecundary->FontSize , ImVec2( pos.x + ( ( rect.GetWidth( ) - TextSize.x ) / 2 ) , pos.y + ( ( rect.GetHeight( ) - TextSize.y ) / 2 ) - 1 ) , ImGui::GetColorU32( SubTabAnim->second.TextColor ) , label );


		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags );
		return pressed;
	}

	namespace c {
		inline ImVec4 accent = ImColor( 118 , 187 , 117 );
		inline ImVec4 separator = ImColor( 22 , 23 , 26 );

		namespace bg {
			inline ImVec4 background = ImColor( 15 , 15 , 15 );
			inline ImVec2 size = ImVec2( 650 , 570 );
			inline float rounding = 8.f;
		}

		namespace child {
			inline ImVec4 background = ImColor( 17 , 17 , 18 );
			inline ImVec4 cap = ImColor( 20 , 21 , 23 );
			inline float rounding = 8.f;
		}

		namespace page {
			inline ImVec4 background_active = ImColor( 31 , 33 , 38 );
			inline ImVec4 background = ImColor( 22 , 23 , 25 );

			inline ImVec4 text_hov = ImColor( 69 , 74 , 95 );
			inline ImVec4 text = ImColor( 68 , 71 , 85 );

			inline float rounding = 4.f;
		}

		namespace elements {
			inline ImVec4 background_hovered = ImColor( 31 , 33 , 38 );
			inline ImVec4 background = ImColor( 22 , 23 , 25 );
			inline float rounding = 2.f;
		}

		namespace checkbox {
			inline ImVec4 mark = ImColor( 0 , 0 , 0 , 255 );
		}

		namespace text {
			inline ImVec4 text_active = ImColor( 255 , 255 , 255 );
			inline ImVec4 text_hov = ImColor( 69 , 74 , 95 );
			inline ImVec4 text = ImColor( 68 , 71 , 85 );
		}
	}

	inline bool ChildEx( const char* name , ImGuiID id , const ImVec2& size_arg , bool cap , ImGuiWindowFlags flags ) {
		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
		flags |= ( parent_window->Flags & ImGuiWindowFlags_NoMove );

		const ImVec2 content_avail = GetContentRegionAvail( );
		ImVec2 size = ImTrunc( size_arg );
		const int auto_fit_axises = ( ( size.x == 0.0f ) ? ( 1 << ImGuiAxis_X ) : 0x00 ) | ( ( size.y == 0.0f ) ? ( 1 << ImGuiAxis_Y ) : 0x00 );
		if ( size.x <= 0.0f ) size.x = ImMax( content_avail.x + size.x , 4.0f );
		if ( size.y <= 0.0f ) size.y = ImMax( content_avail.y + size.y , 4.0f );

		SetNextWindowPos( ImVec2( parent_window->DC.CursorPos + ImVec2( 0 , cap ? 45 : 0 ) ) );
		SetNextWindowSize( size - ImVec2( 0 , cap ? 45 : 0 ) );

		GetWindowDrawList( )->AddRectFilled( parent_window->DC.CursorPos + ImVec2( 0 , cap ? 45 : 0 ) , parent_window->DC.CursorPos + size_arg , GetColorU32( c::child::background ) , c::child::rounding , cap ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersAll );

		if ( cap ) {

			GetWindowDrawList( )->AddRectFilledMultiColor( parent_window->DC.CursorPos + ImVec2( ( size_arg.x / 2 ) , 44 ) , parent_window->DC.CursorPos + ImVec2( ( size_arg.x - 50 ) , 45 ) , GetColorU32( c::accent ) , GetColorU32( c::accent , 0.f ) , GetColorU32( c::accent , 0.f ) , GetColorU32( c::accent ) , c::child::rounding );
			GetWindowDrawList( )->AddRectFilledMultiColor( parent_window->DC.CursorPos + ImVec2( 50 , 44 ) , parent_window->DC.CursorPos + ImVec2( size_arg.x / 2 , 45 ) , GetColorU32( c::accent , 0.f ) , GetColorU32( c::accent ) , GetColorU32( c::accent ) , GetColorU32( c::accent , 0.f ) , c::child::rounding );

			GetWindowDrawList( )->AddRectFilled( parent_window->DC.CursorPos , parent_window->DC.CursorPos + ImVec2( size_arg.x , 35 ) , GetColorU32( c::child::cap ) , c::child::rounding , ImDrawFlags_RoundCornersTop );
			GetWindowDrawList( )->AddText( parent_window->DC.CursorPos + ImVec2( size_arg.x - CalcTextSize( name ).x , 35 - CalcTextSize( name ).y ) / 2 , GetColorU32( c::text::text_active ) , name );
		}

		const char* temp_window_name;

		if ( name ) ImFormatStringToTempBuffer( &temp_window_name , NULL , "%s/%s_%08X" , parent_window->Name , name , id );

		else ImFormatStringToTempBuffer( &temp_window_name , NULL , "%s/%08X" , parent_window->Name , id );

		const float backup_border_size = g.Style.ChildBorderSize;

		bool ret = Begin( temp_window_name , NULL , flags | ImGuiWindowFlags_NoBackground );

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;
		child_window->AutoFitChildAxises = ( ImS8 ) auto_fit_axises;

		if ( child_window->BeginCount == 1 ) parent_window->DC.CursorPos = child_window->Pos;

		const ImGuiID temp_id_for_activation = ImHashStr( "##Child" , 0 , id );
		if ( g.ActiveId == temp_id_for_activation ) ClearActiveID( );

		if ( g.NavActivateId == id && !( flags & ImGuiWindowFlags_NavFlattened ) && ( child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY ) ) {
			FocusWindow( child_window );
			NavInitWindow( child_window , false );
			SetActiveID( temp_id_for_activation , child_window );
			g.ActiveIdSource = g.NavInputSource;
		}
		return ret;
	}

	inline bool Child( const char* str_id , const ImVec2& size_arg , bool cap , ImGuiWindowFlags extra_flags ) {
		ImGuiWindow* window = GetCurrentWindow( );

		PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 13 , 13 ) );
		PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 13 , 13 ) );

		return ChildEx( str_id , window->GetID( str_id ) , size_arg , cap , extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding );
	}

	inline void EndChild2( ) {
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		PopStyleVar( 2 );

		IM_ASSERT( g.WithinEndChild == false );
		IM_ASSERT( window->Flags & ImGuiWindowFlags_ChildWindow );

		g.WithinEndChild = true;
		if ( window->BeginCount > 1 ) {
			End( );
		}
		else {
			ImVec2 sz = window->Size;

			if ( window->AutoFitChildAxises & ( 1 << ImGuiAxis_X ) ) sz.x = ImMax( 4.0f , sz.x );
			if ( window->AutoFitChildAxises & ( 1 << ImGuiAxis_Y ) ) sz.y = ImMax( 4.0f , sz.y );

			End( );

			ImGuiWindow* parent_window = g.CurrentWindow;
			ImRect bb( parent_window->DC.CursorPos , parent_window->DC.CursorPos + sz );
			ItemSize( sz );
			if ( ( window->DC.NavLayersActiveMask != 0 || window->DC.NavWindowHasScrollY ) && !( window->Flags & ImGuiWindowFlags_NavFlattened ) ) {
				ItemAdd( bb , window->ChildId );
			}
			else {
				ItemAdd( bb , 0 );

				if ( window->Flags & ImGuiWindowFlags_NavFlattened ) parent_window->DC.NavLayersActiveMaskNext |= window->DC.NavLayersActiveMaskNext;
			}
			if ( g.HoveredWindow == window ) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
		}
		g.WithinEndChild = false;
		g.LogLinePosY = -FLT_MAX;
	}

	inline void InvisibleShadow( const float alpha , const ImVec2 pos_min , const ImVec2 pos_max , ImU32 color , float shadow_tickness ) {
		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , alpha );
		ImGui::PushClipRect( pos_min , pos_max , true );
		ImGui::GetWindowDrawList( )->AddShadowCircle( ImVec2( ImGui::GetMousePos( ) ) , 1.f , ImGui::GetColorU32( color ) , shadow_tickness , ImVec2( 0 , 0 ) , 1000.f );
		ImGui::PopClipRect( );
		ImGui::PopStyleVar( );
	}

	inline void ToolTip( const char* Label , const char* Desc , const char* Icon , bool State ) {
		//bool Check = CheckBox( Label, Checked );
		bool Hovered = State;

		ImVec2 Pos = ImGui::GetMousePos( ) + ImVec2( 20 , 0 );
		auto DrawList = ImGui::GetForegroundDrawList( );
		float Padding = 12;
		float Rounding = 4;
		float Spacing = 6;

		struct ToolTip_t {
			ImVec4 BackGroundColor = ImColor( 20 , 20 , 22 );
			ImVec4 TextColor = ImColor( 200 , 200 , 200 , 0 );
			ImVec4 IconColor = ImColor( 245 , 158 , 66 , 0 );

			float GlobalAlpha = 0.f;
			float SlideX = 0.f;
		};

		static std::map<std::string , ToolTip_t> anim;
		std::string a1 = std::string( Label ) + std::string( Desc );
		auto ToolTipAnim = anim.find( a1 );

		if ( ToolTipAnim == anim.end( ) ) {
			anim.insert( { a1, ToolTip_t( ) } );
		}

		ImVec2 TextSize = Utils::CalcTextSize( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , Desc );
		ImVec2 IconTextSize = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , Icon );

		ToolTipAnim->second.GlobalAlpha = ImLerp( ToolTipAnim->second.GlobalAlpha , Hovered ? 1.f : 0.f , ImGui::GetIO( ).DeltaTime * 8 );
		ToolTipAnim->second.SlideX = ImLerp( ToolTipAnim->second.SlideX , Hovered ? ( IconTextSize.x + TextSize.x + Padding + Spacing ) : 0.f , ImGui::GetIO( ).DeltaTime * 8 );
		ToolTipAnim->second.TextColor = ImLerp( ToolTipAnim->second.TextColor , Hovered ? ImColor( 180 , 180 , 180 ) : ImColor( 180 , 180 , 180 , 0 ) , ImGui::GetIO( ).DeltaTime * 12 );
		ToolTipAnim->second.IconColor = ImLerp( ToolTipAnim->second.IconColor , Hovered ? ImVec4( g_Col.Base ) : ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.f / 255.f ) , ImGui::GetIO( ).DeltaTime * 8 );

		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , ToolTipAnim->second.GlobalAlpha );


		ImVec2 RectEndPos = ImVec2( Pos.x + ToolTipAnim->second.SlideX , Pos.y + TextSize.y + Padding );
		DrawList->AddRectFilled( Pos , RectEndPos , ImGui::GetColorU32( ImVec4( ImColor( 16 , 16 , 18 ) ) ) , Rounding );
		DrawList->AddRect( Pos , RectEndPos , ImGui::GetColorU32( ImVec4( ImColor( 22 , 22 , 24 ) ) ) , Rounding );


		DrawList->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize , ImVec2( Pos.x + Padding / 2 , Pos.y + ( ( TextSize.y + Padding ) / 2 - IconTextSize.y / 2 ) ) , ImGui::GetColorU32( ToolTipAnim->second.IconColor ) , Icon );

		if ( ToolTipAnim->second.SlideX < TextSize.x + Padding ) {

			//ChatGPT patrocina
			std::string ClippedDesc = Desc;
			while ( ImGui::CalcTextSize( ClippedDesc.c_str( ) ).x > ToolTipAnim->second.SlideX - Padding && !ClippedDesc.empty( ) ) {
				ClippedDesc.pop_back( );
			}

			DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ImVec2( Pos.x + Padding / 2 + ( IconTextSize.x + Spacing ) , Pos.y + Padding / 2 ) , ImGui::GetColorU32( ToolTipAnim->second.TextColor ) , ClippedDesc.c_str( ) );
		}
		else {
			DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ImVec2( Pos.x + Padding / 2 + ( IconTextSize.x + Spacing ) , Pos.y + Padding / 2 ) , ImGui::GetColorU32( ToolTipAnim->second.TextColor ) , Desc );
		}

		ImGui::PopStyleVar( );

		//return Check;
	}

	inline bool CheckBox( const char* label , bool* v ) {
		if ( g_MenuInfo.IsOpen ) {
			ImGuiWindow* window = GetCurrentWindow( );
			if ( window->SkipItems )
				return false;

			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID( label );
			const ImVec2 label_size = CalcTextSize( label );

			float toggle_width = 34.0f;
			float toggle_height = 16.0f;
			float radius = toggle_height * 0.5f;

			float spacing_between_toggle_and_label = style.ItemInnerSpacing.x + 8.0f;

			ImVec2 pos = window->DC.CursorPos;

			ImVec2 total_size = ImVec2( toggle_width + spacing_between_toggle_and_label + label_size.x , toggle_height );
			ImRect bb( pos , pos + total_size );
			ItemSize( bb , style.FramePadding.y );

			if ( !ItemAdd( bb , id ) )
				return false;

			ImRect toggle_bb( pos , pos + ImVec2( toggle_width , toggle_height ) );

			bool hovered , held;
			bool pressed = ButtonBehavior( bb , id , &hovered , &held );
			if ( pressed ) {
				*v = !( *v );
				MarkItemEdited( id );
			}

			ImVec4 baseColor = g_Col.Base;
			ImVec4 darkColor = g_Col.DarkenColor( baseColor , 0.5f );

			ImU32 colTop = ImGui::ColorConvertFloat4ToU32( baseColor );
			ImU32 colBot = ImGui::ColorConvertFloat4ToU32( darkColor );

			if ( *v ) {
				window->DrawList->AddRectFilledMultiColor( toggle_bb.Min , toggle_bb.Max , colTop , colTop , colBot , colBot , radius );
			}
			else {
				ImU32 bgOff = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.08f , 0.08f , 0.08f , 1.0f ) );
				window->DrawList->AddRectFilled( toggle_bb.Min , toggle_bb.Max , bgOff , radius );
			}

			ImVec4 knobColor = *v ? g_Col.DarkenColor( baseColor , 0.15f ) : ImVec4( 0.6f , 0.6f , 0.62f , 1.0f );
			ImU32 knob_col = ImGui::ColorConvertFloat4ToU32( knobColor );

			static std::unordered_map<ImGuiID , float> anims;
			float& anim = anims [ id ];
			anim = ImLerp( anim , *v ? 1.0f : 0.0f , 0.1f );

			float knob_x = ImLerp( toggle_bb.Min.x + 2.0f , toggle_bb.Max.x - radius * 2.0f - 2.0f , anim );

			PushFont( g_Variables.m_FontNormal );

			window->DrawList->AddCircleFilled( ImVec2( knob_x + radius , toggle_bb.Min.y + radius ) , 4.5f , knob_col );

			if ( label_size.x > 0.0f ) {
				ImVec2 text_pos = ImVec2( toggle_bb.Max.x + spacing_between_toggle_and_label , toggle_bb.Min.y + ( toggle_height - label_size.y ) * 0.5f );
				ImU32 text_color = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.85f , 0.85f , 0.85f , 1.0f ) );
				window->DrawList->AddText( text_pos , text_color , label );
			}

			PopFont( );

			return pressed;
		}
	}

	struct CheckBoxAnim {
		float a;
	};

	inline bool CheckboxWithKey( const char* label , bool* v , bool bShowGearIcon , std::function<void( )> Components ) {
		if ( g_MenuInfo.IsOpen ) {
			ImGuiWindow* window = ImGui::GetCurrentWindow( );
			if ( window->SkipItems ) return false;

			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID( label );
			const ImVec2 label_size = CalcTextSize( label );

			float toggle_width = 34.0f;
			float toggle_height = 16.0f;
			float radius = toggle_height * 0.5f;
			float icon_size = 14.0f;
			float icon_spacing = 10.0f;

			float spacing_between_toggle_and_label = style.ItemInnerSpacing.x + 8.0f;

			ImVec2 pos = window->DC.CursorPos;

			float available_width = GetContentRegionAvail( ).x;

			float gear_offset = icon_size + icon_spacing;
			float total_width = toggle_width + spacing_between_toggle_and_label + label_size.x + ( bShowGearIcon ? gear_offset : 0.f );

			ImVec2 total_size = ImVec2( available_width , toggle_height );
			ImRect bb( pos , pos + total_size );
			ItemSize( bb , style.FramePadding.y );

			if ( !ItemAdd( bb , id ) ) return false;

			ImVec2 toggle_pos = pos;
			ImRect toggle_bb( toggle_pos , toggle_pos + ImVec2( toggle_width , toggle_height ) );

			ImVec2 label_pos = ImVec2( toggle_bb.Max.x + spacing_between_toggle_and_label , toggle_bb.Min.y + ( toggle_height - label_size.y ) * 0.5f );

			ImVec2 gear_pos = ImVec2( bb.Max.x - icon_size - 6.0f , bb.Min.y + ( toggle_height - icon_size ) * 0.5f );
			ImVec2 gear_max = gear_pos + ImVec2( icon_size , icon_size );

			bool hovered , held;
			bool pressed = ButtonBehavior( bb , id , &hovered , &held );
			bool gear_hovered = bShowGearIcon && ImGui::IsMouseHoveringRect( gear_pos , gear_max );

			if ( pressed && !gear_hovered ) {
				*v = !( *v );
				MarkItemEdited( id );
			}

			ImVec4 baseColor = g_Col.Base;
			ImVec4 darkColor = g_Col.DarkenColor( baseColor , 0.5f );
			ImU32 colTop = ImGui::ColorConvertFloat4ToU32( baseColor );
			ImU32 colBot = ImGui::ColorConvertFloat4ToU32( darkColor );
			ImU32 bgOff = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.08f , 0.08f , 0.08f , 1.0f ) );

			if ( *v )
				window->DrawList->AddRectFilledMultiColor( toggle_bb.Min , toggle_bb.Max , colTop , colTop , colBot , colBot , radius );
			else
				window->DrawList->AddRectFilled( toggle_bb.Min , toggle_bb.Max , bgOff , radius );

			ImVec4 knobColor = *v ? g_Col.DarkenColor( baseColor , 0.15f ) : ImVec4( 0.6f , 0.6f , 0.62f , 1.0f );
			ImU32 knob_col = ImGui::ColorConvertFloat4ToU32( knobColor );

			static std::unordered_map<ImGuiID , float> anims;
			float& anim = anims [ id ];
			anim = ImLerp( anim , *v ? 1.0f : 0.0f , 0.1f );

			float knob_x = ImLerp( toggle_bb.Min.x + 2.0f , toggle_bb.Max.x - radius * 2.0f - 2.0f , anim );

			PushFont( g_Variables.m_FontNormal );
			window->DrawList->AddCircleFilled( ImVec2( knob_x + radius , toggle_bb.Min.y + radius ) , 4.5f , knob_col );
			RenderText( label_pos , label );
			PopFont( );

			static std::map<ImGuiID , bool> popupOpen;
			static std::map<ImGuiID , float> gearRotation;
			static std::map<ImGuiID , float> popupAlpha;

			if ( bShowGearIcon && Components ) {
				if ( gear_hovered && IsMouseClicked( 0 ) )
					popupOpen [ id ] = !popupOpen [ id ];

				gearRotation [ id ] = ImLerp( gearRotation [ id ] , popupOpen [ id ] ? 1.0f : -1.0f , g.IO.DeltaTime * 10.0f );
				popupAlpha [ id ] = ImClamp( popupAlpha [ id ] + ( popupOpen [ id ] ? 1.0f : -1.0f ) * g.IO.DeltaTime * 6.0f , 0.0f , 1.0f );

				window->DrawList->PushClipRect( gear_pos , gear_max , true );
				ImRotateStart( );
				ImVec4 gearColor = ImVec4( 0.75f , 0.75f , 0.78f , 0.9f );
				window->DrawList->AddText( g_Variables.FontAwesomeSolid , icon_size , gear_pos , ImGui::GetColorU32( gearColor ) , ICON_FA_GEAR );
				ImRotateEnd( gearRotation [ id ] * 1.57f );
				window->DrawList->PopClipRect( );

				if ( popupOpen [ id ] ) {
					ImGui::CaptureMouseFromApp( true );
					ImGui::CaptureKeyboardFromApp( true );
				}

				if ( popupAlpha [ id ] > 0.001f ) {
					PushStyleVar( ImGuiStyleVar_Alpha , popupAlpha [ id ] );
					PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 10 , 10 ) );
					PushStyleVar( ImGuiStyleVar_FramePadding , ImVec2( 8 , 6 ) );
					PushStyleVar( ImGuiStyleVar_WindowRounding , 4.f );
					PushStyleColor( ImGuiCol_PopupBg , ImColor( 20 , 20 , 22 , 255 ).Value );

					char popupName [ 64 ];
					sprintf_s( popupName , "##popup_%X" , id );

					SetNextWindowPos( gear_pos + ImVec2( 0 , icon_size + 6 ) );
					SetNextWindowSizeConstraints( ImVec2( 180 , 40 ) , ImVec2( FLT_MAX , FLT_MAX ) );

					if ( Begin( popupName , nullptr , ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) ) {
						ImDrawList* draw = GetWindowDrawList( );
						ImVec2 winPos = GetWindowPos( );
						ImVec2 winSize = GetWindowSize( );

						ImVec2 titlePos = winPos + ImVec2( 10.0f , 4.0f );
						PushFont( g_Variables.m_FontNormal );
						draw->AddText( titlePos , ImGui::GetColorU32( ImVec4( 0.85f , 0.85f , 0.88f , 1.0f ) ) , label );
						PopFont( );

						ImVec2 xSize = ImVec2( 14.0f , 1.0f );
						ImVec2 xPos = winPos + winSize - xSize - ImVec2( 6.0f , winSize.y - 6.0f );
						ImRect xRect( xPos , xPos + xSize );
						bool xHovered = ImGui::IsMouseHoveringRect( xRect.Min , xRect.Max );
						ImU32 xColor = ImGui::GetColorU32( xHovered ? ImVec4( 1.0f , 0.4f , 0.4f , 1.0f ) : ImVec4( 0.8f , 0.3f , 0.3f , 1.0f ) );
						PushFont( g_Variables.m_FontSecundary );
						draw->AddText( xPos , xColor , "X" );
						PopFont( );

						if ( xHovered && IsMouseClicked( 0 ) )
							popupOpen [ id ] = false;

						ImGui::Dummy( ImVec2( 0 , 24.0f ) );

						Components( );
						End( );
					}

					PopStyleColor( );
					PopStyleVar( 4 );
				}

				if ( IsMouseClicked( 0 ) && popupOpen [ id ] && !gear_hovered ) {
					ImVec2 mp = GetMousePos( );
					ImVec2 pmin = gear_pos + ImVec2( 0 , icon_size + 6 );
					ImVec2 pmax = pmin + ImVec2( 300 , 200 );
					if ( !( mp.x >= pmin.x && mp.x <= pmax.x && mp.y >= pmin.y && mp.y <= pmax.y ) )
						popupOpen [ id ] = false;
				}
			}

			return pressed && !gear_hovered;
		}
	}

	struct KeyBindAnim {
		float a;
		ImVec4 b;
	};

	inline bool KeyBind( const char* label , int* Key , int* KeyState ) {
		ImGuiWindow* window = GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGui::PushFont( g_Variables.m_FontNormal );

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID( label );

		static std::unordered_map<ImGuiID , KeyBindAnim> anim;
		auto it_anim = anim.find( id );

		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, KeyBindAnim( ) } );
			it_anim = anim.find( id );
		}

		const ImRect frame_bb( window->DC.CursorPos , window->DC.CursorPos + ImVec2( window->Size.x - 20 , 25 ) );
		const ImRect total_bb( window->DC.CursorPos , window->DC.CursorPos + ImVec2( window->Size.x - 20 , 25 ) );

		ImGui::PopFont( );

		ItemSize( total_bb , style.FramePadding.y );
		if ( !ItemAdd( total_bb , id ) )
			return false;

		const bool hovered = ImGui::ItemHoverable( frame_bb , id , ImGuiWindowFlags_None );
		if ( hovered ) {
			SetHoveredID( id );
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}

		char buf_display [ 64 ] = "None";
		if ( *Key != 0 && g.ActiveId != id ) {
			strcpy( buf_display , KeyNames [ *Key ] );
		}
		else if ( g.ActiveId == id ) {
			strcpy( buf_display , "Press" );
		}

		char* output = strstr( buf_display , "None" );
		char* output2 = strstr( buf_display , "Press" );

		it_anim->second.b = ImLerp( it_anim->second.b , ( output || output2 ) ? ImColor( 1.f , 1.f , 1.f , 0.6f * GetStyle( ).Alpha ) : ImColor( 1.f , 1.f , 1.f , 0.8f * GetStyle( ).Alpha ) , g.IO.DeltaTime * 10.f );

		ImGui::PushFont( g_Variables.m_FontNormal );
		window->DrawList->AddRectFilled( frame_bb.Min , frame_bb.Max , ImColor( 14 , 14 , 14 ) , 5 );

		PushClipRect( frame_bb.Min , frame_bb.Max - ImVec2( 27 , 0 ) , true );
		window->DrawList->AddText( ImVec2( frame_bb.Min.x + 7 , frame_bb.Min.y + frame_bb.GetHeight( ) / 2 - CalcTextSize( buf_display ).y / 2 ) , GetColorU32( it_anim->second.b ) , buf_display );
		PopClipRect( );

		PushFont( g_Variables.FontAwesomeSolid );
		window->DrawList->AddText( ImVec2( frame_bb.Max.x - 5 - CalcTextSize( "\xef\x84\x9c" ).x , frame_bb.Min.y + frame_bb.GetHeight( ) / 2 - CalcTextSize( "\xef\x84\x9c" ).y / 2 ) , GetColorU32( it_anim->second.b ) , "\xef\x84\x9c" );
		PopFont( );

		window->DrawList->AddRect( frame_bb.Min , frame_bb.Max , GetColorU32( ImGuiCol_Border ) , 5 );
		ImGui::PopFont( );

		const bool user_clicked = hovered && io.MouseClicked [ 0 ];

		if ( user_clicked ) {
			if ( g.ActiveId != id ) {
				memset( io.MouseDown , 0 , sizeof( io.MouseDown ) );
				memset( io.KeysDown , 0 , sizeof( io.KeysDown ) );
				*Key = 0;
			}
			SetActiveID( id , window );
			FocusWindow( window );
		}
		else if ( io.MouseClicked [ 0 ] ) {
			if ( g.ActiveId == id )
				ClearActiveID( );
		}

		bool value_changed = false;
		int key = *Key;

		if ( g.ActiveId == id ) {
			for ( auto i = 0; i < 5; i++ ) {
				if ( io.MouseDown [ i ] ) {
					switch ( i ) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ClearActiveID( );
				}
			}
			if ( !value_changed ) {
				for ( auto i = VK_BACK; i <= VK_RMENU; i++ ) {
					if ( io.KeysDown [ i ] ) {
						key = i;
						value_changed = true;
						ClearActiveID( );
					}
				}
			}

			if ( IsKeyPressedMap( ImGuiKey_Escape ) ) {
				*Key = 0;
				ClearActiveID( );
			}
			else {
				*Key = key;
			}
		}

		return value_changed;
	}

	inline bool CheckBoxCfg( const char* Label , bool* Checked , std::function<void( )> Components , bool bToolTip = false , const char* ToolTipMsg = "" , const char* ToolTipIcon = "" ) {
		ImGuiWindow* Window = ImGui::GetCurrentWindow( );
		if ( Window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		std::string UniqueID = std::string( Label ) + std::to_string( ( int ) Checked );
		const ImGuiID id = Window->GetID( UniqueID.c_str( ) );

		const float CheckBoxSizeVal = 22.f;
		const ImVec2 CheckBoxSize( CheckBoxSizeVal , CheckBoxSizeVal );
		const float IconSize = 16.f;
		const float IconSpacing = 6.f;

		ImVec2 TextSize = ImGui::CalcTextSize( Label );
		float TotalWidth = ImGui::GetContentRegionAvail( ).x;

		float RightBlockWidth = CheckBoxSize.x +
			( bToolTip ? IconSize + IconSpacing : 0 ) +
			( Components ? IconSize + IconSpacing : 0 );

		ImVec2 Pos = Window->DC.CursorPos;
		ImVec2 LabelPos = Pos;

		ImVec2 CheckBoxPos = ImVec2( Pos.x + TotalWidth - CheckBoxSize.x , Pos.y );
		ImVec2 GearPos = ImVec2( CheckBoxPos.x - IconSpacing - IconSize , Pos.y + ( CheckBoxSize.y - IconSize ) / 2 );
		ImVec2 WarnPos = ImVec2( GearPos.x - IconSpacing - IconSize , Pos.y + ( CheckBoxSize.y - IconSize ) / 2 );

		const ImRect Rect( CheckBoxPos , CheckBoxPos + CheckBoxSize );
		const ImRect Clickable = Rect;

		ImGui::ItemSize( Rect , style.FramePadding.y );

		if ( !ImGui::ItemAdd( Rect , id , &Clickable ) ) {
			IMGUI_TEST_ENGINE_ITEM_INFO( id , Label , g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *Checked ? ImGuiItemStatusFlags_Checked : 0 ) );
			return false;
		}

		bool Hovered , Held;
		bool Pressed = ImGui::ButtonBehavior( Clickable , id , &Hovered , &Held );
		if ( Pressed ) {
			*Checked = !( *Checked );
			ImGui::MarkItemEdited( id );
		}

		struct WidCheckBox_t {
			ImVec4 BackGroundColor = ImColor( 18 , 18 , 20 );
			ImVec4 UnCheckedBackGroundColor = ImColor( 18 , 18 , 20 );
			ImVec4 CheckColor = ImColor( 18 , 18 , 20 );
			ImVec4 LabelColor = g_Col.SecundaryText;

			ImVec2 BackGroundSize = ImVec2( 0 , 0 );
			float CheckUp = 0.f;
			float AnimKeyBind = 0.f;
			float PopupAlpha = 0.f;

			float SlideX = 0.f;
			ImVec4 IconColor = ImColor( 245 , 158 , 66 , 0 );
			float GearRotation = 0.f;
			bool PopupActive = false;
		};

		static std::map<ImGuiID , WidCheckBox_t> anim;
		auto& animRef = anim [ id ];

		animRef.LabelColor = ImLerp( animRef.LabelColor , *Checked ? ImColor( g_Col.FeaturesText ) : ImColor( g_Col.SecundaryFeaturesText ) , g.IO.DeltaTime * 8.f );
		animRef.BackGroundColor = ImLerp( animRef.BackGroundColor , *Checked ? ImColor( g_Col.Base ) : Hovered ? ImColor( 20 , 20 , 22 ) : ImColor( 18 , 18 , 20 ) , g.IO.DeltaTime * 10.f );
		animRef.UnCheckedBackGroundColor = ImLerp( animRef.UnCheckedBackGroundColor , Hovered ? ImColor( 20 , 20 , 22 ) : ImColor( 18 , 18 , 20 ) , g.IO.DeltaTime * 6.f );
		animRef.BackGroundSize = ImLerp( animRef.BackGroundSize , ( *Checked ) ? ImVec2( CheckBoxSize / 2 ) : ImVec2( 0 , 0 ) , g.IO.DeltaTime * 8.f );

		if ( animRef.BackGroundSize.x > ( CheckBoxSize.x / 2 ) - 2.f && ( *Checked ) ) {
			animRef.CheckColor = ImLerp( animRef.CheckColor , ImColor( 21 , 21 , 23 , 255 ) , g.IO.DeltaTime * 14.f );
			animRef.CheckUp = ImLerp( animRef.CheckUp , CheckBoxSize.x / 4 , g.IO.DeltaTime * 6.f );
		}
		else {
			animRef.CheckColor = ImLerp( animRef.CheckColor , ImColor( 21 , 21 , 23 , 0 ) , g.IO.DeltaTime * 14.f );
			animRef.CheckUp = ImLerp( animRef.CheckUp , 0.f , g.IO.DeltaTime * 6.f );
		}

		Window->DrawList->AddText( LabelPos , ImGui::GetColorU32( animRef.LabelColor ) , Label );

		ImVec2 CheckBoxCenter = CheckBoxPos + CheckBoxSize / 2;
		Window->DrawList->AddRectFilled( CheckBoxPos , CheckBoxPos + CheckBoxSize , ImGui::GetColorU32( animRef.UnCheckedBackGroundColor ) , 4.f );
		Window->DrawList->AddRect( CheckBoxPos , CheckBoxPos + CheckBoxSize , ImGui::GetColorU32( ImVec4( ImColor( 24 , 24 , 26 ) ) ) , 4.f );
		Window->DrawList->AddRectFilled( CheckBoxCenter - animRef.BackGroundSize , CheckBoxCenter + animRef.BackGroundSize , ImGui::GetColorU32( animRef.BackGroundColor ) , 4.f );
		ImGui::RenderCheckMark( Window->DrawList , ImVec2( CheckBoxPos.x + CheckBoxSize.x / 4 , CheckBoxPos.y + CheckBoxSize.x / 2 - animRef.CheckUp ) , ImGui::GetColorU32( animRef.CheckColor ) , CheckBoxSize.x / 2 );

		if ( bToolTip && g_MenuInfo.IsOpen && ImGui::GetStyle( ).Alpha > 0.0f ) {
			ImColor color = ImColor( 245 , 158 , 66 , static_cast< int >( ImGui::GetStyle( ).Alpha * 255.0f ) );
			Window->DrawList->AddText( g_Variables.FontAwesomeSolid , IconSize , WarnPos , color , ICON_FA_TRIANGLE_EXCLAMATION );

			if ( Hovered && g_MenuInfo.IsOpen )
				ToolTip( Label , ToolTipMsg , ToolTipIcon , true );
		}


		static bool IconHovered = false;
		ImVec2 MousePos = ImGui::GetMousePos( );
		ImVec2 IconTextSize = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , IconSize , ICON_FA_GEAR );

		ImVec2 GearIconMax = GearPos + IconTextSize;
		IconHovered = MousePos.x > GearPos.x && MousePos.x < GearIconMax.x &&
			MousePos.y > GearPos.y && MousePos.y < GearIconMax.y;

		if ( IconHovered && g.IO.MouseClicked [ 0 ] ) {
			animRef.PopupActive = !animRef.PopupActive;
		}
		else if ( !IconHovered && g.IO.MouseClicked [ 0 ] && animRef.PopupActive ) {
			ImVec2 PopupSize( 180 , 90 );
			ImVec2 PopupMin = GearPos + ImVec2( 0 , IconSize + 4 );
			if ( !( MousePos.x > PopupMin.x && MousePos.x < PopupMin.x + PopupSize.x &&
				MousePos.y > PopupMin.y && MousePos.y < PopupMin.y + PopupSize.y ) ) {
				animRef.PopupActive = false;
			}
		}

		animRef.GearRotation = ImLerp( animRef.GearRotation , animRef.PopupActive ? 1.f : -1.f , g.IO.DeltaTime * 10.f );
		animRef.IconColor = ImLerp( animRef.IconColor , animRef.PopupActive ? ImColor( g_Col.Base ) : ImColor( 60 , 60 , 60 ) , g.IO.DeltaTime * 10.f );

		ImRotateStart( );
		Window->DrawList->AddText( g_Variables.FontAwesomeSolid , IconSize , GearPos , ImGui::GetColorU32( animRef.IconColor ) , ICON_FA_GEAR );
		ImRotateEnd( 1.57f * animRef.GearRotation );

		animRef.PopupAlpha = ImClamp( animRef.PopupAlpha + ( 6.f * g.IO.DeltaTime * ( animRef.PopupActive && g_MenuInfo.IsOpen ? 1.f : -1.f ) ) , 0.f , 1.f );
		if ( animRef.PopupAlpha >= 0.001f ) {
			PushStyleVar( ImGuiStyleVar_Alpha , animRef.PopupAlpha );
			PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_FramePadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_CellPadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_WindowRounding , 4.f );
			PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 15 , 15 ) );
			PushStyleVar( ImGuiStyleVar_PopupBorderSize , 1 );
			PushStyleColor( ImGuiCol_Border , GetColorU32( ImVec4( ImColor( 20 , 20 , 22 ) ) ) );
			PushStyleColor( ImGuiCol_PopupBg , GetColorU32( ImVec4( ImColor( 14 , 14 , 16 ) ) ) );

			ImVec2 PopupSize( 180 , 90 );
			ImVec2 PopupMin = GearPos + ImVec2( 0 , IconSize + 4 );
			SetNextWindowSize( PopupSize );
			SetNextWindowPos( PopupMin );

			Begin( Label , NULL , ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_WithoutScrollClamp | ImGuiWindowFlags_Tooltip );
			{
				PopupSize = ImGui::GetWindowSize( );
				Components( );
			}

			PopStyleVar( 7 );
			PopStyleColor( 2 );
			End( );
		}

		if ( g_MenuInfo.IsOpen ) {
			float lineOffset = 6.0f;
			float lineY = Rect.Max.y + lineOffset;

			float contentMinX = Window->Pos.x + ImGui::GetWindowContentRegionMin( ).x;
			float contentMaxX = Window->Pos.x + ImGui::GetWindowContentRegionMax( ).x;

			Window->DrawList->AddLine( ImVec2( contentMinX , lineY ) , ImVec2( contentMaxX , lineY ) , ImColor( 30 , 30 , 30 ) , 1.0f );
		}

		IMGUI_TEST_ENGINE_ITEM_INFO( id , Label , g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *Checked ? ImGuiItemStatusFlags_Checked : 0 ) );
		return Pressed;
	}


	inline bool CustomChild( const char* str_id , const ImVec2& size , const char* icon ) {
		ImDrawList* draw = GetWindowDrawList( );
		ImVec2 pos = GetWindowPos( ) + GetCursorPos( );
		ImVec2 header_size( size.x , 35 );

		if ( g_MenuInfo.IsOpen ) {
			PushStyleColor( ImGuiCol_ChildBg , ImColor( 13 , 13 , 13 , ( int ) ( GetStyle( ).Alpha * 255 ) ).Value );
			PushStyleVar( ImGuiStyleVar_ChildRounding , 5 );

			draw->AddRectFilled( pos , pos + header_size , ImColor( 17 , 17 , 17 , ( int ) ( GetStyle( ).Alpha * 255 ) ) , 5 , ImDrawFlags_RoundCornersTop );

			PushFont( g_Variables.FontAwesomeSolid );
			draw->AddText( pos + ImVec2( 10 , 9 ) , ImColor( g_Col.Base ) , icon );
			PopFont( );

			PushFont( g_Variables.m_FontNormal );
			draw->AddText( pos + ImVec2( icon ? 32 : 10 , 10 ) , ImColor( 255 , 255 , 255 , ( int ) ( 0.8f * GetStyle( ).Alpha * 255 ) ) , str_id );
			PopFont( );

			SetCursorPosY( GetCursorPosY( ) + 35 );
			bool ret = BeginChild( str_id , size - ImVec2( 0 , 35 ) , false , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
			SetCursorPos( ImVec2( 10 , 10 ) );
			BeginGroup( );

			if ( IsWindowHovered( ImGuiHoveredFlags_ChildWindows ) )
				draw->AddRect( pos , pos + size , ImColor( 0 , 0 , 0 , 0 ) , 5 , ImDrawFlags_RoundCornersAll );

			return ret;
		}
	}

	inline void EndCustomChild( ) {
		if ( g_MenuInfo.IsOpen ) {
			if ( GetCurrentWindow( )->ScrollbarY > 0 )
				SetCursorPosY( GetCursorPosY( ) + 10 );

			EndGroup( );
			GetWindowDrawList( )->AddRect( GetWindowPos( ) , GetWindowPos( ) + GetWindowSize( ) , GetColorU32( ImGuiCol_Border ) , GetStyle( ).ChildRounding , ImDrawFlags_RoundCornersBottom );
			EndChild( );
			PopStyleVar( );
			PopStyleColor( );
			SetCursorPosY( GetCursorPosY( ) + 12.5f );
		}
	}

	inline bool CfgButton( const char* Label , std::function<void( )> Components ) {
		ImGuiWindow* Window = ImGui::GetCurrentWindow( );
		if ( Window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		std::string IdStr = ( std::string ) Label;
		const ImGuiID id = Window->GetID( IdStr.c_str( ) );

		ImVec2 IconTextSize = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize - 4 , ICON_FA_GEAR ); //ICON_FA_ELLIPSIS
		ImVec2 MousePos = ImGui::GetMousePos( );

		const ImVec2 Pos = Window->DC.CursorPos;
		const ImRect Rect( Pos , Pos + ImVec2( IconTextSize.x , IconTextSize.y ) );

		ImGui::ItemSize( Rect , style.FramePadding.y );

		if ( !ImGui::ItemAdd( Rect , id ) ) {
			return false;
		}

		bool Hovered , Held;
		bool Pressed = ImGui::ButtonBehavior( Rect , id , &Hovered , &Held );
		if ( Pressed ) {
			ImGui::MarkItemEdited( id );
		}

		struct CfgBtn_t {
			ImVec4 IconColor = ImColor( 245 , 158 , 66 , 0 );
			float GearRotation = 0.f;
			float PopupAlpha = 0.f;
			bool PopupActive = false;
		};

		static std::map<ImGuiID , CfgBtn_t> anim;
		auto CfgAnim = anim.find( id );

		if ( CfgAnim == anim.end( ) ) {
			anim.insert( { id, CfgBtn_t( ) } );
			CfgAnim = anim.find( id );
		}

		static ImVec2 PopupSize( 180 , 90 );
		ImVec2 PopupMin = Pos + ImVec2( IconTextSize.x + 4 , IconTextSize.y + 4 );

		if ( Hovered && g.IO.MouseClicked [ 0 ] ) {
			CfgAnim->second.PopupActive = !CfgAnim->second.PopupActive;
		}
		else if ( !Hovered && g.IO.MouseClicked [ 0 ] && CfgAnim->second.PopupActive ) {
			if ( !( MousePos.x > PopupMin.x && MousePos.x < PopupMin.x + PopupSize.x && MousePos.y > PopupMin.y && MousePos.y < PopupMin.y + PopupSize.y ) ) {
				CfgAnim->second.PopupActive = false;
			}
		}

		CfgAnim->second.GearRotation = ImLerp( CfgAnim->second.GearRotation , CfgAnim->second.PopupActive ? 1.f : -1.f , g.IO.DeltaTime * 10.f );
		CfgAnim->second.IconColor = ImLerp( CfgAnim->second.IconColor , CfgAnim->second.PopupActive ? ImColor( g_Col.Base ) : ImColor( 60 , 60 , 60 ) , g.IO.DeltaTime * 10.f );

		ImRotateStart( );
		Window->DrawList->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize - 4 , Pos , ImGui::GetColorU32( CfgAnim->second.IconColor ) , ICON_FA_GEAR ); //ICON_FA_ELLIPSIS
		ImRotateEnd( 1.57f * CfgAnim->second.GearRotation );

		CfgAnim->second.PopupAlpha = ImClamp( CfgAnim->second.PopupAlpha + ( 6.f * g.IO.DeltaTime * ( CfgAnim->second.PopupActive && g_MenuInfo.IsOpen ? 1.f : -1.f ) ) , 0.f , 1.f );

		if ( CfgAnim->second.PopupAlpha >= 0.001f ) {
			PushStyleVar( ImGuiStyleVar_Alpha , CfgAnim->second.PopupAlpha );
			PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_FramePadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_CellPadding , ImVec2( 10 , 10 ) );
			PushStyleVar( ImGuiStyleVar_WindowRounding , 4.f );
			PushStyleVar( ImGuiStyleVar_ItemSpacing , ImVec2( 15 , 15 ) );
			PushStyleVar( ImGuiStyleVar_PopupBorderSize , 1 );
			PushStyleColor( ImGuiCol_Border , GetColorU32( ImVec4( ImColor( 20 , 20 , 22 ) ) ) );
			PushStyleColor( ImGuiCol_PopupBg , GetColorU32( ImVec4( ImColor( 14 , 14 , 16 ) ) ) );

			SetNextWindowSize( ImVec2( PopupSize.x , 0 ) );
			SetNextWindowPos( PopupMin );

			Begin( Label , NULL , ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_WithoutScrollClamp | ImGuiWindowFlags_Tooltip );
			{
				PopupSize = ImGui::GetWindowSize( );
				Components( );
			}

			PopStyleVar( 7 );
			PopStyleColor( 2 );

			End( );
		}
	}

	inline bool CheckBoxPage( const char* Label , bool* Checked , std::function<void( )> Code , bool bToolTip = false , const char* ToolTipMsg = "" ) {
		ImGuiWindow* Window = ImGui::GetCurrentWindow( );
		if ( Window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		std::string UniqueID = ( std::string ) Label + std::to_string( ( int ) Checked );
		const ImGuiID id = Window->GetID( UniqueID.c_str( ) );

		float Width = ImGui::GetWindowContentRegionMax( ).x - ImGui::GetWindowContentRegionMin( ).x - ( Window->ScrollbarY ? 5.f : 0.f );

		ImVec2 TextSize = ImGui::CalcTextSize( Label );
		const ImVec2 CheckBoxSize( 22 , 22 );

		const ImVec2 Pos = Window->DC.CursorPos;
		const ImRect Rect( Pos , Pos + ImVec2( Width , CheckBoxSize.y - 4 ) );
		const ImRect Clickable( Pos , Pos + ImVec2( CheckBoxSize.x + TextSize.x + 8 , CheckBoxSize.y ) );

		ImGui::ItemSize( Rect , style.FramePadding.y );

		if ( !ImGui::ItemAdd( Rect , id , &Clickable ) ) {
			IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *v ? ImGuiItemStatusFlags_Checked : 0 ) );
			return false;
		}

		bool Hovered , Held;
		bool Pressed = ImGui::ButtonBehavior( Clickable , id , &Hovered , &Held );
		if ( Pressed ) {
			*Checked = !( *Checked );
			ImGui::MarkItemEdited( id );
		}

		struct WidCheckBox_t {
			ImVec4 BackGroundColor = ImColor( 18 , 18 , 20 );
			ImVec4 UnCheckedBackGroundColor = ImColor( 18 , 18 , 20 );
			ImVec4 CheckColor = ImColor( 18 , 18 , 20 );
			ImVec4 LabelColor = g_Col.SecundaryText;

			ImVec2 BackGroundSize = ImVec2( 0 , 0 );

			float CheckUp = 0.f;
			float AnimKeyBind = 0.f;

			float SlideUp = 0.f;
			ImVec4 IconColor = ImColor( 245 , 158 , 66 , 0 );
		};

		static std::map<ImGuiID , WidCheckBox_t> anim;
		auto CheckBoxAnim = anim.find( id );

		if ( CheckBoxAnim == anim.end( ) ) {
			anim.insert( { id, WidCheckBox_t( ) } );
			CheckBoxAnim = anim.find( id );
		}

		CheckBoxAnim->second.LabelColor = ImLerp( CheckBoxAnim->second.LabelColor , *Checked ? ImColor( g_Col.FeaturesText ) : ImColor( g_Col.SecundaryFeaturesText ) , g.IO.DeltaTime * 8.f );
		CheckBoxAnim->second.BackGroundColor = ImLerp( CheckBoxAnim->second.BackGroundColor , *Checked ? ImColor( g_Col.Base ) : Hovered ? ImColor( 20 , 20 , 22 ) : ImColor( 18 , 18 , 20 ) , g.IO.DeltaTime * 10.f );
		CheckBoxAnim->second.UnCheckedBackGroundColor = ImLerp( CheckBoxAnim->second.UnCheckedBackGroundColor , Hovered ? ImColor( 20 , 20 , 22 ) : ImColor( 18 , 18 , 20 ) , g.IO.DeltaTime * 6.f );
		CheckBoxAnim->second.BackGroundSize = ImLerp( CheckBoxAnim->second.BackGroundSize , ( *Checked ) ? ImVec2( CheckBoxSize / 2 ) : ImVec2( 0 , 0 ) , g.IO.DeltaTime * 8.f );

		if ( CheckBoxAnim->second.BackGroundSize.x > ( CheckBoxSize.x / 2 ) - 2.f && ( *Checked ) ) {
			CheckBoxAnim->second.CheckColor = ImLerp( CheckBoxAnim->second.CheckColor , ImColor( 21 , 21 , 23 , 255 ) , g.IO.DeltaTime * 14.f );
			CheckBoxAnim->second.CheckUp = ImLerp( CheckBoxAnim->second.CheckUp , CheckBoxSize.x / 4 , g.IO.DeltaTime * 6.f );
		}
		else {
			CheckBoxAnim->second.CheckColor = ImLerp( CheckBoxAnim->second.CheckColor , ImColor( 21 , 21 , 23 , 0 ) , g.IO.DeltaTime * 14.f );
			CheckBoxAnim->second.CheckUp = ImLerp( CheckBoxAnim->second.CheckUp , 0.f , g.IO.DeltaTime * 6.f );
		}

		ImVec2 CheckBoxCenter( Rect.Min + CheckBoxSize / 2 );

		Window->DrawList->AddRectFilled( Rect.Min , Rect.Min + CheckBoxSize , ImGui::GetColorU32( CheckBoxAnim->second.UnCheckedBackGroundColor ) , 4.f );
		Window->DrawList->AddRect( Rect.Min , Rect.Min + CheckBoxSize , ImGui::GetColorU32( ImVec4( ImColor( 24 , 24 , 26 ) ) ) , 4.f );


		Window->DrawList->AddRectFilled( CheckBoxCenter - CheckBoxAnim->second.BackGroundSize , CheckBoxCenter + CheckBoxAnim->second.BackGroundSize , ImGui::GetColorU32( CheckBoxAnim->second.BackGroundColor ) , 4.f );

		ImGui::RenderCheckMark( Window->DrawList , ImVec2( Pos.x + CheckBoxSize.x / 4 , Pos.y + CheckBoxSize.x / 2 - CheckBoxAnim->second.CheckUp ) , ImGui::GetColorU32( CheckBoxAnim->second.CheckColor ) , CheckBoxSize.x / 2 );


		ImVec2 TextPos = ImVec2( Rect.Min.x + CheckBoxSize.x + 8.f , Pos.y + Rect.Max.y / 2 - ( Pos.y + TextSize.y ) / 2 );
		Window->DrawList->AddText( TextPos , ImGui::GetColorU32( CheckBoxAnim->second.LabelColor ) , Label );

		////////////////////////////
		// Link Obj
		////////////////////////////
		static bool IconHovered = false;
		ImVec2 IconTextSize = Utils::CalcTextSize( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize - 4 , ICON_FA_SHARE );
		ImVec2 MousePos = ImGui::GetMousePos( );
		ImVec2 MinPos = TextPos + ImVec2( TextSize.x , ( CheckBoxSize.y / 2 - TextSize.y / 2 ) - 4 );
		ImVec2 MaxPos = TextPos + ImVec2( TextSize.x + ( 10 * 2 ) + IconTextSize.x , ( MinPos.y - TextPos.y ) + IconTextSize.y + 4 );

		if ( MousePos.x > MinPos.x && MousePos.x < MaxPos.x && MousePos.y > MinPos.y && MousePos.y < MaxPos.y ) {
			IconHovered = true;
		}
		else {
			IconHovered = false;
		}

		if ( bToolTip ) {
			ToolTip( Label , ToolTipMsg , ICON_FA_SHARE , IconHovered && g_MenuInfo.IsOpen );
		}

		CheckBoxAnim->second.SlideUp = ImLerp( CheckBoxAnim->second.SlideUp , IconHovered ? 2.f : 0.f ,
			g.IO.DeltaTime * 10.f );
		CheckBoxAnim->second.IconColor = ImLerp( CheckBoxAnim->second.IconColor , IconHovered
			? ImColor( g_Col.Base ) : ImColor( 60 , 60 , 60 ) , g.IO.DeltaTime * 10.f );

		Window->DrawList->AddText( g_Variables.FontAwesomeSolid , g_Variables.FontAwesomeSolid->FontSize - 4
			, TextPos + ImVec2( TextSize.x + 10 , ( CheckBoxSize.y / 2 - TextSize.y / 2 ) - CheckBoxAnim->second.SlideUp ) ,
			ImGui::GetColorU32( CheckBoxAnim->second.IconColor ) , ICON_FA_SHARE );

		if ( IconHovered && g.IO.MouseClicked [ 0 ] ) {
			Code( );
		}

		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *Checked ? ImGuiItemStatusFlags_Checked : 0 ) );
		return Pressed;
	}

	inline void TextCentered( const char* text , int m ) {

		ImVec2 textSize = ImGui::CalcTextSize( text );
		float posX = ( g_MenuInfo.MenuSize.x - textSize.x ) * 0.5f;
		float posY = ( g_MenuInfo.MenuSize.y - textSize.y ) * 0.5f;

		switch ( m ) {

		case 0:
			ImGui::SetCursorPos( { posX, posY } );
			ImGui::Text( text );
			break;
		case 1:
			ImGui::SetCursorPosX( posX );
			ImGui::Text( text );
			break;
		case 2:
			ImGui::SetCursorPosY( posY );
			ImGui::Text( text );
			break;
		default:
			ImGui::SetCursorPos( { posX, posY } );
			ImGui::Text( text );
			break;

		}
	}


	inline bool ButtonWithIcon( const char* icon , const char* label , const ImVec2& size_arg , ImGuiButtonFlags flags ) {
		struct button_struct {
			ImVec4 BorderCol;
			ImVec4 background;
			ImVec4 LabelColor;
			float UpBackground;
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = g_Variables.m_FontNormal->CalcTextSizeA( g_Variables.m_FontNormal->FontSize , FLT_MAX , 0 , label );
		const ImVec2 IconTextSize = g_Variables.FontAwesomeSolid->CalcTextSizeA( g_Variables.FontAwesomeSolid->FontSize - 4 , FLT_MAX , 0 , icon );
		const ImVec2 pos = window->DC.CursorPos;

		static std::map<ImGuiID , button_struct> anim;
		auto it_anim = anim.find( id );

		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, button_struct( ) } );
			it_anim = anim.find( id );
		}

		ImVec2 size = ImGui::CalcItemSize( size_arg , label_size.x + IconTextSize.x + style.FramePadding.x * 3.0f , label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos , ImVec2( pos.x + size.x , pos.y + size.y ) );
		ImGui::ItemSize( size , 0.f );

		if ( !ImGui::ItemAdd( bb , id ) )
			return false;

		bool Hovered , Held , Pressed = ImGui::ButtonBehavior( bb , id , &Hovered , &Held , flags );

		it_anim->second.BorderCol = ImLerp( it_anim->second.BorderCol , Hovered ? ImVec4( g_Col.Base ) : ImVec4( ImColor( 18 , 18 , 20 ) ) , g.IO.DeltaTime * 6.f );
		it_anim->second.background = ImLerp( it_anim->second.background , Hovered ? ImVec4( ImColor( 14 , 14 , 16 ) ) : ImVec4( ImColor( 14 , 14 , 16 ) ) , g.IO.DeltaTime * 6.f );
		it_anim->second.LabelColor = ImLerp( it_anim->second.LabelColor , Hovered ? ImVec4( ImColor( 225 , 225 , 225 ) ) : ImVec4( ImColor( 225 , 225 , 225 ) ) , g.IO.DeltaTime * 6.f );
		it_anim->second.UpBackground = ImLerp( it_anim->second.UpBackground , Hovered ? ( bb.Min.y - pos.y ) - ( bb.Max.y - pos.y ) : 0.f , g.IO.DeltaTime * 6.f );

		float totalWidth = label_size.x + IconTextSize.x + style.FramePadding.x;
		ImVec2 TextPos = { pos.x + ( size.x - totalWidth ) * 0.5f + IconTextSize.x + style.FramePadding.x, pos.y + ( size.y - label_size.y ) * 0.5f - 0.5f };
		ImVec2 TextWithoutIconPos = { pos.x + ( size.x - label_size.x ) * 0.5f + style.FramePadding.x, pos.y + ( size.y - label_size.y ) * 0.5f - 0.5f };
		ImVec2 IconPos = { pos.x + ( size.x - totalWidth ) * 0.5f, pos.y + ( size.y - IconTextSize.y ) * 0.5f - 0.5f };

		window->DrawList->AddRectFilled( bb.Min , bb.Max , ImGui::GetColorU32( it_anim->second.background ) , 4 );
		window->DrawList->AddRect( bb.Min , bb.Max , ImGui::GetColorU32( it_anim->second.BorderCol ) , 4 );

		window->DrawList->AddRectFilled( ImVec2( bb.Min.x , bb.Max.y ) , ImVec2( bb.Max.x , bb.Max.y + it_anim->second.UpBackground ) , ImGui::GetColorU32( g_Col.Base ) , 4 );

		//window->DrawList->AddText( g_Variables.FontAwesomeSolid, g_Variables.FontAwesomeSolid->FontSize - 4, IconPos, ImGui::GetColorU32( it_anim->second.LabelColor ), icon );
		window->DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , TextWithoutIconPos , ImGui::GetColorU32( it_anim->second.LabelColor ) , label );

		return Pressed;
	}

	inline bool Button( const char* label , const ImVec2& size_arg , ImGuiButtonFlags flags , bool bToolTip = false , const char* ToolTipMsg = "" , const char* ToolTipIcon = "" ) {
		struct button13Anims {
			float closing_anim;
			float closing_alpha;
			float label_alpha;
			bool animation_complete;
		};

		ImGuiWindow* window = GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = CalcTextSize( label , NULL , true );
		ImDrawList* draw = GetWindowDrawList( );

		static std::map<ImGuiID , button13Anims> anim; // #include <map> on top of this file, where is "imgui.h" located.
		auto it_anim = anim.find( id );
		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, button13Anims( ) } );
			it_anim = anim.find( id );
		}

		ImVec2 pos = window->DC.CursorPos;
		if ( ( flags & ImGuiButtonFlags_AlignTextBaseLine ) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset ) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize( size_arg , label_size.x + style.FramePadding.x * 2.0f , label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos , pos + size );
		ItemSize( size , style.FramePadding.y );
		if ( !ItemAdd( bb , id ) )
			return false;

		if ( g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat )
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered , held;
		bool pressed = ButtonBehavior( bb , id , &hovered , &held , flags );

		if ( bToolTip ) {
			ToolTip( label , ToolTipMsg , ToolTipIcon , hovered && g_MenuInfo.IsOpen );
		}

		it_anim->second.closing_anim = ImLerp( it_anim->second.closing_anim , ( hovered ? size.y : 0 ) , g.IO.DeltaTime * 8.f );

		if ( hovered || pressed ) {
			if ( it_anim->second.label_alpha < 255.f )
				it_anim->second.label_alpha += 5.f / GetIO( ).Framerate * 160.f;

			if ( it_anim->second.closing_alpha < 255.f )
				it_anim->second.closing_alpha += 15.f / GetIO( ).Framerate * 160.f;
		}
		else {
			if ( it_anim->second.label_alpha > 0.f )
				it_anim->second.label_alpha -= 5.f / GetIO( ).Framerate * 160.f;

			if ( it_anim->second.closing_alpha > 0.f )
				it_anim->second.closing_alpha -= 10.f / GetIO( ).Framerate * 160.f;
		}

		// Render
		const ImU32 inside_solid_col = GetColorU32( ImVec4( ImColor( 16 , 16 , 18 ) ) );
		const ImU32 outside_solid_col = GetColorU32( ImVec4( ImColor( 24 , 24 , 26 ) ) );
		const ImU32 inside_hover_col = GetColorU32( ImVec4( ImColor( 176 , 63 , 155 , ( int ) it_anim->second.closing_alpha ) ) );

		draw->AddRectFilled( ImVec2( bb.Min.x , bb.Min.y ) , ImVec2( bb.Max.x , bb.Max.y ) , inside_solid_col , 6 );
		draw->AddRect( bb.Min , bb.Max , outside_solid_col , 6 );
		draw->AddRectFilled( ImVec2( bb.Min.x , bb.Max.y - it_anim->second.closing_anim ) , ImVec2( bb.Max.x , bb.Max.y ) , inside_hover_col , 6 );

		PushStyleColor( ImGuiCol_Text , ColorConvertFloat4ToU32( ImColor( 140 , 140 , 140 , 255 - ( int ) it_anim->second.label_alpha ) ) );
		RenderTextClipped( bb.Min + style.FramePadding , bb.Max - style.FramePadding , label , NULL , &label_size , style.ButtonTextAlign , &bb );
		PopStyleColor( );

		PushStyleColor( ImGuiCol_Text , ColorConvertFloat4ToU32( ImColor( 20 , 20 , 20 , ( int ) it_anim->second.label_alpha ) ) );
		RenderTextClipped( bb.Min + style.FramePadding , bb.Max - style.FramePadding , label , NULL , &label_size , style.ButtonTextAlign , &bb );
		PopStyleColor( );

		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags );

		return pressed;
	}

	inline bool ButtonHeld( const char* label , const ImVec2& size_arg , ImGuiButtonFlags flags ) {
		struct button13Anims {
			float closing_anim;
			float closing_alpha;
			float label_alpha;
			bool animation_complete;
		};

		ImGuiWindow* window = GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = CalcTextSize( label , NULL , true );
		ImDrawList* draw = GetWindowDrawList( );

		static std::map<ImGuiID , button13Anims> anim; // #include <map> on top of this file, where is "imgui.h" located.
		auto it_anim = anim.find( id );
		if ( it_anim == anim.end( ) ) {
			anim.insert( { id, button13Anims( ) } );
			it_anim = anim.find( id );
		}

		ImVec2 pos = window->DC.CursorPos;
		if ( ( flags & ImGuiButtonFlags_AlignTextBaseLine ) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset ) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize( size_arg , label_size.x + style.FramePadding.x * 2.0f , label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos , pos + size );
		ItemSize( size , style.FramePadding.y );
		if ( !ItemAdd( bb , id ) )
			return false;

		if ( g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat )
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered , held;
		bool pressed = ButtonBehavior( bb , id , &hovered , &held , flags );

		it_anim->second.closing_anim = ImLerp( it_anim->second.closing_anim , ( held ? size.y : 0 ) , g.IO.DeltaTime * 8.f );

		if ( held || pressed ) {
			if ( it_anim->second.label_alpha < 255.f )
				it_anim->second.label_alpha += 5.f / GetIO( ).Framerate * 160.f;

			if ( it_anim->second.closing_alpha < 255.f )
				it_anim->second.closing_alpha += 15.f / GetIO( ).Framerate * 160.f;
		}
		else {
			if ( it_anim->second.label_alpha > 0.f )
				it_anim->second.label_alpha -= 5.f / GetIO( ).Framerate * 160.f;

			if ( it_anim->second.closing_alpha > 0.f )
				it_anim->second.closing_alpha -= 10.f / GetIO( ).Framerate * 160.f;
		}

		// Render
		const ImU32 inside_solid_col = GetColorU32( ImVec4( ImColor( 16 , 16 , 18 ) ) );
		const ImU32 outside_solid_col = GetColorU32( ImVec4( ImColor( 24 , 24 , 26 ) ) );
		const ImU32 inside_hover_col = ImColor( 107 , 105 , 255 , ( int ) it_anim->second.closing_alpha );

		draw->AddRectFilled( ImVec2( bb.Min.x , bb.Min.y ) , ImVec2( bb.Max.x , bb.Max.y ) , inside_solid_col , 6 );
		draw->AddRect( bb.Min , bb.Max , outside_solid_col , 6 );
		draw->AddRectFilled( ImVec2( bb.Min.x , bb.Max.y - it_anim->second.closing_anim ) , ImVec2( bb.Max.x , bb.Max.y ) , inside_hover_col , 6 );

		PushStyleColor( ImGuiCol_Text , ColorConvertFloat4ToU32( ImColor( 140 , 140 , 140 , 255 - ( int ) it_anim->second.label_alpha ) ) );
		RenderTextClipped( bb.Min + style.FramePadding , bb.Max - style.FramePadding , label , NULL , &label_size , style.ButtonTextAlign , &bb );
		PopStyleColor( );

		PushStyleColor( ImGuiCol_Text , ColorConvertFloat4ToU32( ImColor( 225 , 225 , 225 , ( int ) it_anim->second.label_alpha ) ) );
		RenderTextClipped( bb.Min + style.FramePadding , bb.Max - style.FramePadding , label , NULL , &label_size , style.ButtonTextAlign , &bb );
		PopStyleColor( );

		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags );

		bool completed = it_anim->second.closing_anim >= ( size.y - 1.f );
		if ( completed && !it_anim->second.animation_complete ) {
			it_anim->second.animation_complete = true;
			return true;
		}

		if ( pressed ) {
			it_anim->second.animation_complete = false;
		}

		return false;
	}

	inline bool WeaponButtonHeld( ImTextureID Icon , const char* label , ImGuiButtonFlags flags ) {
		ImGuiWindow* window = GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImVec2 LabelSize = Utils::CalcTextSize( g_Variables.m_FontSecundary , g_Variables.m_FontSecundary->FontSize , label );
		const ImGuiID id = window->GetID( label );
		ImDrawList* Draw = window->DrawList;

		struct WeaponButtonHeld_t {
			ImVec4 ProgressCol = ImColor( );
			ImVec4 ShadowProgressCol = ImColor( );
			ImVec4 SlideCol = ImColor( );
			float Alpha = 0.f;
			float SlideX = 0.f;
			float SlideXShadow = 0.f;
			float IconSize = 0.f;
			bool Completed = false;
		};

		static std::map<ImGuiID , WeaponButtonHeld_t> anim;
		auto WeaponButtonAnim = anim.find( id );
		if ( WeaponButtonAnim == anim.end( ) ) {
			anim.insert( { id, WeaponButtonHeld_t( ) } );
			WeaponButtonAnim = anim.find( id );
		}

		const float Width = 100;
		const float Height = Width;
		ImVec2 Pos = window->DC.CursorPos;
		const ImRect Rect( Pos , Pos + ImVec2( Width , Height ) );
		ItemSize( ImVec2( Width , Height ) , style.FramePadding.y );
		if ( !ItemAdd( Rect , id ) )
			return false;

		if ( g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat )
			flags |= ImGuiButtonFlags_Repeat;

		bool Hovered , Held;
		bool Pressed = ButtonBehavior( Rect , id , &Hovered , &Held , flags );

		bool Condition = WeaponButtonAnim->second.Completed ? false : Held;

		WeaponButtonAnim->second.Alpha = ImLerp( WeaponButtonAnim->second.Alpha , Condition ? 0.8f : Hovered ? 1.f : 0.6f , g.IO.DeltaTime * 8.f );
		WeaponButtonAnim->second.IconSize = ImLerp( WeaponButtonAnim->second.IconSize , Condition ? 4.f : 0.f , g.IO.DeltaTime * 8.f );
		WeaponButtonAnim->second.SlideX = ImLerp( WeaponButtonAnim->second.SlideX , Condition ? Width : 0.f , g.IO.DeltaTime * 2.2f );
		WeaponButtonAnim->second.SlideXShadow = ImLerp( WeaponButtonAnim->second.SlideXShadow , WeaponButtonAnim->second.SlideX >= 1.f ? WeaponButtonAnim->second.SlideX + 1.f : WeaponButtonAnim->second.SlideX , g.IO.DeltaTime * 3.f );
		WeaponButtonAnim->second.ProgressCol = ImLerp( WeaponButtonAnim->second.ProgressCol , WeaponButtonAnim->second.SlideX >= 0.6f ? ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 255.f / 255.f ) : ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.f ) , g.IO.DeltaTime * 18.f );
		WeaponButtonAnim->second.ShadowProgressCol = ImLerp( WeaponButtonAnim->second.ShadowProgressCol , WeaponButtonAnim->second.SlideXShadow >= 0.6f ? ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.4f ) : ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.f ) , g.IO.DeltaTime * 18.f );

		float Size = ( 10 + WeaponButtonAnim->second.IconSize );
		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , g.Style.Alpha * WeaponButtonAnim->second.Alpha );
		{
			Draw->AddRectFilled( Rect.Min , Rect.Max , GetColorU32( ImVec4( ImColor( 20 , 20 , 22 ) ) ) , 8 );
			//Draw->AddRectFilledMultiColor( Rect.Min, Rect.Max, GetColorU32( ImVec4( ImColor( g_Col.Base ) ), WeaponButtonAnim->second.Alpha ), GetColorU32( ImVec4( ImColor( g_Col.Base ) ), WeaponButtonAnim->second.Alpha ), GetColorU32( ImVec4( ImColor( 20, 20, 22, 0 ) ) ), GetColorU32( ImVec4( ImColor( 20, 20, 22, 0 ) ) ), 8 );
			Draw->AddRectFilled( Pos + ImVec2( 0 , Height - 4 ) , Pos + ImVec2( WeaponButtonAnim->second.SlideXShadow , Height ) , GetColorU32( WeaponButtonAnim->second.ShadowProgressCol ) , 8 );
			Draw->AddRectFilled( Pos + ImVec2( 0 , Height - 4 ) , Pos + ImVec2( WeaponButtonAnim->second.SlideX , Height ) , GetColorU32( WeaponButtonAnim->second.ProgressCol ) , 8 );
			Draw->AddImage( Icon , ImVec2( Rect.Min.x + Size , Rect.Min.y + Size ) , ImVec2( Rect.Max.x - Size , Rect.Max.y - Size ) , { 0,0 } , { 1,1 } , GetColorU32( ImVec4( ImColor( 255 , 255 , 255 ) ) ) );
		}
		ImGui::PopStyleVar( );

		Draw->AddRect( Rect.Min - ImVec2( 4 , 4 ) , Rect.Max + ImVec2( 4 , 4 ) , GetColorU32( g_Col.BackgroundCol ) , 8 , 0 , 8.f );

		IMGUI_TEST_ENGINE_ITEM_INFO( id , label , g.LastItemData.StatusFlags );

		bool Completed = WeaponButtonAnim->second.SlideX >= ( Width - 1.f );

		if ( WeaponButtonAnim->second.Completed && Held ) {
			return false;
		}


		if ( Completed ) {
			WeaponButtonAnim->second.Completed = true;
			return true;
		}

		if ( !Held ) {
			WeaponButtonAnim->second.Completed = false;
		}

		return false;
	}

	inline bool ResourceListButton( const char* Label , uint32_t ResourceState , ImVec2 SizeArg , ImGuiButtonFlags flags ) {


		ImGuiWindow* Window = ImGui::GetCurrentWindow( );
		if ( Window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = Window->GetID( Label );
		const ImVec2 LabelSize = g_Variables.m_FontNormal->CalcTextSizeA( g_Variables.m_FontNormal->FontSize , FLT_MAX , 0 , Label );
		const ImVec2 TwoTextSize = g_Variables.m_FontSecundary->CalcTextSizeA( g_Variables.m_FontSecundary->FontSize , FLT_MAX , 0 , xorstr( "Stop" ) );
		const ImVec2 IconTextSize = g_Variables.FontAwesomeSolid->CalcTextSizeA( g_Variables.FontAwesomeSolid->FontSize , FLT_MAX , 0 , ICON_FA_CIRCLE_STOP );
		const ImVec2 pos = Window->DC.CursorPos;


		float Width = ImGui::GetWindowContentRegionMax( ).x - ImGui::GetWindowContentRegionMin( ).x - ( Window->ScrollbarY ? 12.f : 0.f );
		SizeArg.x = Width;
		ImVec2 size = ImGui::CalcItemSize( SizeArg , LabelSize.x + IconTextSize.x + style.FramePadding.x * 3.0f , LabelSize.y + style.FramePadding.y * 2.0f );

		const ImRect Rect( pos , ImVec2( pos.x + size.x , pos.y + size.y ) );
		ImGui::ItemSize( size , 0.f );

		if ( !ImGui::ItemAdd( Rect , id ) )
			return false;

		bool Hovered , Held , Pressed = ImGui::ButtonBehavior( Rect , id , &Hovered , &Held , flags );

		struct ResourceBtn_t {
			float Alpha = 0.f;
			float SlideHeld = 0.f;
			float Size = 0.f;
			bool Completed = false;
		};

		static std::map<ImGuiID , ResourceBtn_t> anim;
		auto ResourceBtnAnim = anim.find( id );

		if ( ResourceBtnAnim == anim.end( ) ) {
			anim.insert( { id, ResourceBtn_t( ) } );
			ResourceBtnAnim = anim.find( id );
		}

		auto TextPos = ImVec2( Rect.Min.x + 8 , pos.y + ( size.y / 2 - LabelSize.y / 2 ) );

		ResourceBtnAnim->second.Alpha = ImLerp( ResourceBtnAnim->second.Alpha , ResourceState == 3 ? 1.f : 0.4f , g.IO.DeltaTime * 8 );
		ResourceBtnAnim->second.SlideHeld = ImLerp( ResourceBtnAnim->second.SlideHeld , Held && ResourceState == 3 ? Rect.Max.x - pos.x : TextPos.x - pos.x + LabelSize.x + 8 , g.IO.DeltaTime * 4 );
		ResourceBtnAnim->second.Size = ImLerp( ResourceBtnAnim->second.Size , ResourceState == 3 && Held ? 4.f : 3.f , g.IO.DeltaTime * 4 );

		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , ImGui::GetStyle( ).Alpha * ResourceBtnAnim->second.Alpha );

		Window->DrawList->AddRectFilled( Rect.Min , Rect.Max , GetColorU32( ( ImVec4 ) ImColor( 16 , 16 , 18 ) ) , 6 );
		Window->DrawList->AddRectFilled( Rect.Min , ImVec2( pos.x + ResourceBtnAnim->second.SlideHeld , Rect.Max.y ) , GetColorU32( ( ImVec4 ) ImColor( 24 , 24 , 26 ) ) , 6 );
		Window->DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , TextPos , GetColorU32( ImVec4( ImColor( g_Col.FeaturesText ) ) ) , Label );

		ImVec2 CirclePos( Rect.Max.x - 16 , pos.y + ( size.y ) / 2 );

		if ( ResourceState == 3 ) {
			Window->DrawList->AddCircleFilled( CirclePos , ResourceBtnAnim->second.Size , GetColorU32( ImVec4( ImColor( 55 , 237 , 125 , 180 ) ) ) , 99 );
		}
		else {
			Window->DrawList->AddCircleFilled( CirclePos , ResourceBtnAnim->second.Size , GetColorU32( ImVec4( ImColor( 237 , 55 , 55 , 180 ) ) ) , 99 );
		}

		ImGui::PopStyleVar( );

		if ( ResourceState == 3 ) {
			if ( Held && ResourceBtnAnim->second.SlideHeld >= ( Rect.Max.x - pos.x ) - 1.f ) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	inline bool ListSelectableCustom( const char* Label , bool* Selected , int Type ) {
		if ( Type == 0 ) //Players
		{
			//if ( SelectableCustomResources( Label, *Selected, 3 ) ) {
				//*Selected = !*Selected;
				//return true;
			//}
		}
		else if ( Type == 1 ) //Vehicles
		{
			//if ( SelectableCustomResources( Label, *Selected, 3 ) ) {
				//*Selected = !*Selected;
				//return true;
			//}
		}

		return false;
	}

}