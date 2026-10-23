#include "Memory.hpp"
#include <filesystem>
#include <winternl.h>
#include <Auth/lazyimporter.hpp>
#include <Globals.hpp>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

namespace Core {

	uintptr_t MemoryClass::PatternScan( std::vector<uint8_t> Pattern , int InstructionLength ) {
		uintptr_t Address = FindSignature( Pattern );

		if ( InstructionLength != 0 ) {
			Address = ResolveRelativeAddress( Address , InstructionLength );
		}

		return Address;
	}

	uintptr_t MemoryClass::FindSignature( std::vector<uint8_t> Signature , uintptr_t ModuleBase , uintptr_t ModuleBaseSize ) {
		if ( g_Config.General->BattleFive )
			return 0;
		const size_t blockSize = ( 4096 * 4 );
		std::unique_ptr<uint8_t [ ]> data = std::make_unique<uint8_t [ ]>( blockSize );

		DWORD oldProtect;
		size_t signatureSize = Signature.size( );

		uintptr_t ModulBase;
		uintptr_t ModulBaseSize;

		if ( ModuleBase != 0 && ModuleBaseSize != 0 ) {
			ModulBase = ModuleBase;
			ModulBaseSize = ModuleBaseSize;
		}
		else {
			ModulBase = ModBase;
			ModulBaseSize = ModBaseSize;
		}

		for ( uintptr_t address = ModulBase; address < ModulBase + ModulBaseSize; address += blockSize ) {
			if ( !VirtualProtectEx( ProcHandle , ( LPVOID ) address , blockSize , PAGE_EXECUTE_READWRITE , &oldProtect ) ) { continue; }

			SIZE_T bytesRead;
			if ( !ReadProcessMemory( ProcHandle , ( void* ) address , data.get( ) , blockSize , &bytesRead ) ) {
				VirtualProtectEx( ProcHandle , ( LPVOID ) address , blockSize , oldProtect , NULL );
				continue;
			}

			VirtualProtectEx( ProcHandle , ( LPVOID ) address , blockSize , oldProtect , NULL );

			for ( uintptr_t i = 0; i < bytesRead; i++ ) {
				for ( uintptr_t j = 0; j < signatureSize; j++ ) {
					if ( Signature [ j ] == 0x00 )
						continue;

					if ( data [ i + j ] != Signature [ j ] )
						break;

					if ( j == signatureSize - 1 )
						return ( address + i );
				}
			}
		}

		return 0x0;
	}

	uintptr_t MemoryClass::FindSignatureBypass( std::vector<uint8_t> Signature , uintptr_t ModuleBase , uintptr_t ModuleBaseSize ) {
		const size_t blockSize = ( 4096 * 4 );
		std::unique_ptr<uint8_t [ ]> data = std::make_unique<uint8_t [ ]>( blockSize );

		DWORD oldProtect;
		size_t signatureSize = Signature.size( );

		uintptr_t ModulBase;
		uintptr_t ModulBaseSize;

		if ( ModuleBase != 0 && ModuleBaseSize != 0 ) {
			ModulBase = ModuleBase;
			ModulBaseSize = ModuleBaseSize;
		}
		else {
			ModulBase = ModBase;
			ModulBaseSize = ModBaseSize;
		}

		for ( uintptr_t address = ModulBase; address < ModulBase + ModulBaseSize; address += blockSize ) {
			//if (!VirtualProtectEx(ProcHandle, (LPVOID)address, blockSize, PAGE_EXECUTE_READWRITE, &oldProtect)) { continue; }

			SIZE_T bytesRead;
			if ( !ReadProcessMemory( ProcHandle , ( void* ) address , data.get( ) , blockSize , &bytesRead ) ) {
				//VirtualProtectEx(ProcHandle, (LPVOID)address, blockSize, oldProtect, NULL);
				continue;
			}

			//VirtualProtectEx(ProcHandle, (LPVOID)address, blockSize, oldProtect, NULL);

			for ( uintptr_t i = 0; i < bytesRead; i++ ) {
				for ( uintptr_t j = 0; j < signatureSize; j++ ) {
					if ( Signature [ j ] == 0x00 )
						continue;

					if ( data [ i + j ] != Signature [ j ] )
						break;

					if ( j == signatureSize - 1 )
						return ( address + i );
				}
			}
		}

		return 0x0;
	}

	uintptr_t MemoryClass::FindSignatureStr( std::string Pattern , uintptr_t ModuleBase , uintptr_t ModuleBaseSize ) {
		std::vector<uint8_t> signature = Pattern2Vector( Pattern );
		if ( ModuleBase != 0 && ModuleBaseSize != 0 ) {
			return FindSignature( signature , ModuleBase , ModuleBaseSize );
		}
		else {
			return FindSignature( signature ); // Find Sig in module from open proc.
		}
	}

	std::vector<uint8_t> MemoryClass::ReadBytes( uintptr_t Addr , size_t Size ) {
		std::vector<uint8_t> bytes( Size );
		size_t bytesRead = 0;
		if ( g_Config.General->BattleFive ) {
			driver::read_physical( reinterpret_cast< PVOID >( Addr ) ,
				reinterpret_cast< PVOID >( bytes.data( ) ) ,
				static_cast< DWORD >( Size ) );
		}
		else {
			ZwReadVirtualMemory( ProcHandle , ( LPCVOID ) Addr , bytes.data( ) , Size , &bytesRead );
		}
		bytes.resize( bytesRead );
		return bytes;
	}

	BOOL MemoryClass::WriteBytes( uintptr_t Addr , std::vector<uint8_t> Bytes ) {
		if ( g_Config.General->BattleFive ) {

			driver::write_physical( reinterpret_cast< PVOID >( Addr ) ,
				reinterpret_cast< PVOID >( Bytes.data( ) ) ,
				static_cast< DWORD >( Bytes.size( ) ) );

		}
		else {
			NTSTATUS status = ZwWriteVirtualMemory( ProcHandle , ( LPVOID ) Addr , Bytes.data( ) , Bytes.size( ) , NULL );
		}
		return TRUE;
	}

	BOOL MemoryClass::PatchFunc( uintptr_t Addr , int NopCount ) {
		if ( !NopCount ) return false;

		std::vector<uint8_t> PatchTable = { };
		for ( int i = 0; i < NopCount; i++ ) {
			PatchTable.push_back( { 0x90 } );
		}

		return WriteBytes( Addr , PatchTable );
	}


	BOOL MemoryClass::GetMaxPrivileges( HANDLE hProc ) {
		HANDLE h_Token;
		DWORD dw_TokenLength;
		if ( OpenProcessToken( hProc , TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES , &h_Token ) ) {
			TOKEN_PRIVILEGES* privilages = new TOKEN_PRIVILEGES [ 100 ];
			if ( GetTokenInformation( h_Token , TokenPrivileges , privilages , sizeof( TOKEN_PRIVILEGES ) * 100 , &dw_TokenLength ) ) {
				for ( int i = 0; i < privilages->PrivilegeCount; i++ ) {
					privilages->Privileges [ i ].Attributes = SE_PRIVILEGE_ENABLED;
				}
				if ( AdjustTokenPrivileges( h_Token , false , privilages , sizeof( TOKEN_PRIVILEGES ) * 100 , NULL , NULL ) ) {
					delete [ ] privilages;
					return true;
				}
			}
			delete [ ] privilages;
		}

		return false;
	}

	std::string MemoryClass::ReadString( uintptr_t Addr ) {
		const int bufferSize = 256;
		char buffer [ bufferSize ];
		int bytesRead = 0;
		bool success = true;

		while ( bytesRead < bufferSize ) {
			char character;
			ZwReadVirtualMemory( ProcHandle , ( LPVOID ) ( Addr + bytesRead ) , &character , sizeof( char ) , NULL );
			buffer [ bytesRead ] = character;

			if ( character == '\0' ) { break; }

			bytesRead++;
		}

		if ( bytesRead == bufferSize ) { success = false; }

		if ( !success ) { return ""; }

		return std::string( buffer );
	}

	DWORD MemoryClass::GetPidByName( const char* ProcName ) {
		HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , 0 );
		if ( hSnapshot == INVALID_HANDLE_VALUE ) {
			return 0;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		if ( !Process32First( hSnapshot , &pe32 ) ) {
			CloseHandle( hSnapshot );
			return 0;
		}

		DWORD pid = 0;
		do {
			if ( _stricmp( pe32.szExeFile , ProcName ) == 0 ) {
				pid = pe32.th32ProcessID;
				break;
			}
		} while ( Process32Next( hSnapshot , &pe32 ) );

		CloseHandle( hSnapshot );
		return pid;
	}

	std::string MemoryClass::GetNameByPid( DWORD Pid ) {
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE , Pid );
		if ( !hProcess ) { return ""; }

		char processName [ MAX_PATH ];
		if ( !GetModuleFileNameEx( hProcess , NULL , processName , MAX_PATH ) ) { return ""; }

		CloseHandle( hProcess );

		std::string fullPath( processName );
		size_t pos = fullPath.find_last_of( xorstr( "\\/" ) );
		if ( pos != std::string::npos ) {
			return fullPath.substr( pos + 1 );
		}
		else {
			return fullPath;
		}
	}

	uintptr_t MemoryClass::GetModuleBaseAddr( DWORD ProcId , const char* ModuleName , uintptr_t* ModSize ) {
		HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 , ProcId );

		if ( hSnapShot == INVALID_HANDLE_VALUE ) { return 0; }

		MODULEENTRY32 ModuleEntry;
		ModuleEntry.dwSize = sizeof( MODULEENTRY32 );
		if ( Module32First( hSnapShot , &ModuleEntry ) ) {
			do {
				if ( strcmp( ModuleEntry.szModule , ModuleName ) == 0 ) {
					break;
				}
			} while ( Module32Next( hSnapShot , &ModuleEntry ) );
		}
		CloseHandle( hSnapShot );

		*ModSize = ( uintptr_t ) ModuleEntry.modBaseSize;
		return ( uintptr_t ) ModuleEntry.modBaseAddr;
	}


	/*uint64_t MemoryClass::GetModuleBaseByName(DWORD Pid, std::wstring ModuleName)
	{
		HANDLE hSnapshot = SafeCall(CreateToolhelp32Snapshot)(TH32CS_SNAPMODULE, Pid);
		if (!hSnapshot || hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == ((HANDLE)(LONG_PTR)ERROR_BAD_LENGTH))
		{
			return 0;
		}

		uint64_t ModuleBase;
		MODULEENTRY32 ModuleEntry;
		ModuleEntry.dwSize = sizeof(ModuleEntry);
		if (SafeCall(Module32First)(hSnapshot, &ModuleEntry))
		{
			while (_wcsicmp(ModuleEntry.szModule, ModuleName.c_str()))
			{
				if (!SafeCall(Module32Next)(hSnapshot, &ModuleEntry))
				{
					SafeCall(CloseHandle)(hSnapshot);
					return 0;
				}
			}

			ModuleBase = (uint64_t)ModuleEntry.modBaseAddr;
		}
		else
		{
			SafeCall(CloseHandle)(hSnapshot);
			return 0;
		}

		SafeCall(CloseHandle)(hSnapshot);
		return ModuleBase;
	}

	uint64_t MemoryClass::GetModuleBaseSizeByName(DWORD Pid, std::wstring ModuleName)
	{
		HANDLE hSnapshot = SafeCall(CreateToolhelp32Snapshot)(TH32CS_SNAPMODULE, Pid);
		if (!hSnapshot || hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == ((HANDLE)(LONG_PTR)ERROR_BAD_LENGTH))
		{
			return 0;
		}

		uint64_t ModuleBase;
		MODULEENTRY32 ModuleEntry;
		ModuleEntry.dwSize = sizeof(ModuleEntry);
		if (SafeCall(Module32First)(hSnapshot, &ModuleEntry))
		{
			while (_wcsicmp(ModuleEntry.szModule, ModuleName.c_str()))
			{
				if (!SafeCall(Module32Next)(hSnapshot, &ModuleEntry))
				{
					SafeCall(CloseHandle)(hSnapshot);
					return 0;
				}
			}

			ModuleBase = (uint64_t)ModuleEntry.modBaseSize;
		}
		else
		{
			SafeCall(CloseHandle)(hSnapshot);
			return 0;
		}

		SafeCall(CloseHandle)(hSnapshot);

		return ModuleBase;
	}*/


	// Antes esta funcao baixava um .sys de uma URL no catbox.moe, criava um servico de
	// kernel e iniciava. Tres problemas: o servico criado era "4ware", mas o device que
	// driver::init() abre e "GN6Qn-554SFf" (Driver.hpp:21), entao mesmo carregando o
	// init falharia; o arquivo vinha de terceiros sem hash e sem verificacao de
	// assinatura; e todo caminho de falha era um return silencioso, sem dizer o motivo.
	//
	// O driver_data de 42136 bytes em Driver.hpp NAO serve como substituto: a analise do
	// PE mostra Subsystem 523 (driver de kernel usa 1 = NATIVE), flag DLL setada, e zero
	// simbolos de kernel (DriverEntry, IoCreateDevice, MmCopyVirtualMemory). E uma DLL
	// x64, nao um .sys.
	//
	// Nao ha driver de kernel neste projeto. Em vez de instalar um arquivo desconhecido
	// na maquina do cliente, a funcao reporta que o driver nao esta disponivel e diz o
	// que fazer. Se um .sys legitimo for adicionado ao projeto, troque o corpo por um
	// carregamento local dele.
	void LoadDriver( ) {
		const std::string motivo =
			"Driver de kernel nao disponivel.\n\n"
			"Este menu espera um driver no device \\\\.\\GN6Qn-554SFf, mas nenhum\n"
			"arquivo de driver acompanha o projeto.\n\n"
			"Ligue o modo de leitura por handle em Config, ou instale o driver\n"
			"do device GN6Qn-554SFf antes de usar o modo kernel.";

		// Antes era cout: em build de janela (WinMain) nao ha console, a mensagem sumia.
		std::cout << "[-] LoadDriver: " << motivo << std::endl;
		MessageBoxA( NULL , motivo.c_str( ) , "HARX" , MB_OK | MB_ICONINFORMATION );
	}

	std::wstring string_to_wstring( const std::string& str ) {
		if ( str.empty( ) ) return std::wstring( );
		int size_needed = MultiByteToWideChar( CP_UTF8 , 0 , str.c_str( ) , ( int ) str.size( ) , nullptr , 0 );
		std::wstring wstr( size_needed , 0 );
		MultiByteToWideChar( CP_UTF8 , 0 , str.c_str( ) , ( int ) str.size( ) , &wstr [ 0 ] , size_needed );
		return wstr;
	}

	BOOL MemoryClass::OpenProcByPid( ) {
		{
			if ( g_Config.General->BattleFive ) {
				if ( !driver::init( ) ) {
					// Era "return 1". OpenProcByPid devolve BOOL, e 1 e TRUE: quem chamou
					// entendia SUCESSO depois de o driver nao abrir, e o menu seguia
					// tentando ler memoria com driver_handle = nullptr. Agora devolve
					// false e diz o motivo.
					std::cout << ( "[-] failed to init comunication.\n" );
					std::thread( LoadDriver ).detach( );
					return false;
				}
				std::string procNameStr( Mem.ProcName.begin( ) , Mem.ProcName.end( ) );
				driver::process_id = driver::get_process_id( procNameStr.c_str( ) );
				if ( driver::process_id == 0 ) {
					std::cout << ( "[-] processo alvo nao encontrado: " ) << procNameStr << "\n";
					return false;
				}
				driver::base_addrrs = driver::get_base_address( );
				if ( driver::base_addrrs == 0 ) {
					std::cout << ( "[-] base do modulo nao retornada pelo driver.\n" );
					return false;
				}
				auto cr3 = driver::CR3( );
				std::cout << ( "[+] CR3: " ) << std::hex << cr3 << std::dec << "\n";

				Mem.ModBase = driver::base_addrrs;
				// O original gravava base_addrrs aqui tambem, ou seja o TAMANHO do modulo
				// recebia o ENDERECO dele. Passava na checagem de Core.hpp:290
				// (!ModBaseSize -> return false) por acidente, mas o valor era falso.
				// Agora o tamanho vem do modulo de verdade, pelo mesmo caminho que o modo
				// usermode usa. Se nao vier, cai no endereco base para nao travar o menu.
				// O ProcIdFiveM precisa ser atribuido ANTES: GetModuleBaseAddr usa ele.
				g_Variables.ProcIdFiveM = driver::process_id;
				ModBaseSize = 0;
				Mem.GetModuleBaseAddr( g_Variables.ProcIdFiveM , procNameStr.c_str( ) , &ModBaseSize );
				Mem.ModBaseSize = ModBaseSize ? ModBaseSize : driver::base_addrrs;

				std::cout << ( "[+] ProcessID: " ) << driver::process_id << "\n";
				std::cout << ( "[+] BaseAddress: " ) << driver::base_addrrs << "\n";
				auto nigga = read<uintptr_t>( driver::base_addrrs ); // test read
				std::cout << ( "[+] read: " ) << nigga << "\n";
			}
			else {
				ProcHandle = OpenProcess( PROCESS_ALL_ACCESS , FALSE , g_Variables.ProcIdFiveM );
				std::cout << "Process Handle: " << ProcHandle << std::endl;
				if ( !ProcHandle ) { return false; };

				/*Mem.ModBase = Mem.GetModuleBaseByName(g_Variables.ProcIdFiveM, Mem.ProcName);
				Mem.ModBaseSize = Mem.GetModuleBaseSizeByName(g_Variables.ProcIdFiveM, Mem.ProcName);*/
				std::string procNameStr( Mem.ProcName.begin( ) , Mem.ProcName.end( ) );
				Mem.ModBase = Mem.GetModuleBaseAddr( g_Variables.ProcIdFiveM , procNameStr.c_str( ) , &ModBaseSize );
			}
		}

		return true;
	}

}