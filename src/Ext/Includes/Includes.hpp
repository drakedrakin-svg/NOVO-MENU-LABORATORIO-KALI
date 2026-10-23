#pragma once

#define CURL_STATICLIB
#define IMGUI_DEFINE_MATH_OPERATORS

#define M_PI 3.14159265358979323846

#include <Includes/ImGui/Files/imgui.h>
#include <Includes/ImGui/Files/imgui_internal.h>
#include <Includes/ImGui/Files/imgui_impl_win32.h>
#include <Includes/ImGui/Files/imgui_impl_dx11.h>

#include <Includes/ImGui/FontAwesome/FontAwesome.hpp>
#include <Includes/ImGui/FontAwesome/FontAwesomeBrands.hpp>
#include <Includes/ImGui/FontAwesome/RawAwesome6.hpp>

#include <Security/xorstr.hpp>
#include <json.hpp>

#include <curl/curl.h>
#include <Globals.hpp>

#include <Core/Config.hpp>

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <d3d11.h>


// Substitui <D3dx9math.h>. Fica aqui porque 38 arquivos incluem Includes.hpp e
// esperam D3DXVECTOR3/D3DXMATRIX disponiveis, como era antes.
#include <Includes/D3DXMath.hpp>
// Carrega PNG da memoria por WIC, no lugar do D3DX11CreateShaderResourceViewFromMemory.
#include <Includes/Texture.hpp>

#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "Normaliz.lib" )
#pragma comment( lib, "Crypt32.lib" )
#pragma comment( lib, "Wldap32.lib" )
#pragma comment( lib, "libcurl.lib" )

#pragma comment( lib, "d3d11.lib" )

#pragma warning( disable : 4996 )

namespace Core {

}

using namespace Core;
