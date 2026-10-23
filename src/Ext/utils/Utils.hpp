#pragma once
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <Winhttp.h>
#include <iphlpapi.h>
#include "fstream"
#include <codecvt>
#include "sddl.h"
#include "locale"
#include <vector>
#include <Wbemidl.h>
#include <comdef.h>
#include <setupapi.h>
#include <devguid.h>
#include <cfgmgr32.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib") 
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

#include <curl/curl.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")

namespace util
{
    namespace info
    {
        size_t WriteCallback( char *contents, size_t size, size_t nmemb, std::string *output )
        {
            size_t totalSize = size * nmemb;
            output->append( contents, totalSize );
            return totalSize;
        }

        std::string get_local_ip( )
        {
            std::string ipAddress = "";

            CURL *curl = curl_easy_init( );
            if ( curl )
            {
                curl_easy_setopt( curl, CURLOPT_URL, "https://api.ipify.org" );
                curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
                curl_easy_setopt( curl, CURLOPT_WRITEDATA, &ipAddress );

                CURLcode res = curl_easy_perform( curl );
                if ( res != CURLE_OK )
                {
                    return "Failed to retrieve IP address: " + std::string( curl_easy_strerror( res ) );
                }

                curl_easy_cleanup( curl );
            }
            return ipAddress;
        }

        std::string get_local_username( )
        {
            TCHAR s[ UNLEN + 1 ];
            DWORD sizethis = UNLEN + 1;

            if ( !GetUserName( s, &sizethis ) )
            {
                return "";
            }

#ifdef UNICODE
            int size_needed = WideCharToMultiByte( CP_UTF8, 0, s, -1, nullptr, 0, nullptr, nullptr );
            std::vector<char> buffer( size_needed );
            WideCharToMultiByte( CP_UTF8, 0, s, -1, buffer.data( ), size_needed, nullptr, nullptr );
            return std::string( buffer.data( ) );
#else
            return std::string( s );
#endif
        }

        std::string get_sid( )
        {
            HANDLE tokenHandle = nullptr;
            if ( !OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &tokenHandle ) )
            {
                return "Failed to get token";
            }

            DWORD tokenInfoLength = 0;
            GetTokenInformation( tokenHandle, TokenUser, nullptr, 0, &tokenInfoLength );

            TOKEN_USER *tokenUser = ( TOKEN_USER * ) malloc( tokenInfoLength );
            if ( GetTokenInformation( tokenHandle, TokenUser, tokenUser, tokenInfoLength, &tokenInfoLength ) )
            {
                LPSTR sidString = nullptr;
                ConvertSidToStringSidA( tokenUser->User.Sid, &sidString );

                std::string sidResult( sidString );

                LocalFree( sidString );
                free( tokenUser );
                CloseHandle( tokenHandle );

                return sidResult;
            }

            free( tokenUser );
            CloseHandle( tokenHandle );
            return "SID Not Found";
        }
    }

    namespace local
    {
        void add_fileds( std::string &fields, const std::string &name, const std::string &value )
        {
            if ( !fields.empty( ) )
            {
                fields += ",";
            }
            fields += "{\"name\": \"" + name + "\", \"value\": \"" + value + "\", \"inline\": false}";
        }
    }

    namespace extern_use
    {
        static size_t WriteCallback( void *contents, size_t size, size_t nmemb, void *userp )
        {
            ( ( std::string * ) userp )->append( ( char * ) contents, size * nmemb );
            return size * nmemb;
        }
    }
}