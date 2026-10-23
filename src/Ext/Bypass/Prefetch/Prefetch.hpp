#include <filesystem>

std::wstring StringToWString( const std::string& str ) {
	return std::wstring( str.begin( ) , str.end( ) );
}

static std::string wstring_to_utf8( const std::wstring& w ) {
	if ( w.empty( ) ) return {};
	int size_needed = WideCharToMultiByte( CP_UTF8 , 0 , w.data( ) , ( int ) w.size( ) , nullptr , 0 , nullptr , nullptr );
	std::string out( size_needed , '\0' );
	WideCharToMultiByte( CP_UTF8 , 0 , w.data( ) , ( int ) w.size( ) , &out [ 0 ] , size_needed , nullptr , nullptr );
	return out;
}

void ResetProcess( const std::string& path ) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si , sizeof( si ) );
	si.cb = sizeof( si );
	ZeroMemory( &pi , sizeof( pi ) );

	if ( CreateProcessA( path.c_str( ) , NULL , NULL , NULL , FALSE , 0 , NULL , NULL , &si , &pi ) ) {
		WaitForSingleObject( pi.hProcess , 100 );
		TerminateProcess( pi.hProcess , 0 );
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
}
void PrefetchFile( std::string file_name ) {
	std::string prefetchPath = "C:\\Windows\\Prefetch\\";

	try {
		for ( const auto& entry : std::filesystem::directory_iterator( prefetchPath ) ) {
			if ( entry.is_regular_file( ) ) {
				std::string filename = entry.path( ).filename( ).string( );
				std::transform( filename.begin( ) , filename.end( ) , filename.begin( ) , ::toupper );

				if ( filename.find( file_name ) != std::string::npos ||
					filename.find( file_name ) != std::string::npos ) {

					HANDLE hFile = CreateFileA( entry.path( ).string( ).c_str( ) ,
						GENERIC_WRITE ,
						0 ,
						NULL ,
						TRUNCATE_EXISTING ,
						FILE_ATTRIBUTE_NORMAL ,
						NULL );

					if ( hFile != INVALID_HANDLE_VALUE ) {
						CloseHandle( hFile );
					}
				}
			}
		}
	}
	catch ( ... ) { }
}
inline void PrefetchSecondary( ) {

	while ( 1 ) {
		printf( "another timez\n" );

		PrefetchFile( ( "ctfmon.exe" ) );
		ResetProcess( ( "C:\\Windows\\System32\\ctfmon.exe" ) );

		std::this_thread::sleep_for( std::chrono::seconds( 20 ) );
	}

}