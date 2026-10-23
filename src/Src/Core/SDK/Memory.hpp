#pragma once
#include <Core/SDK/Structs/Structs.hpp>
#include <Security/xorstr.hpp>
#include <tlhelp32.h>
#include <windows.h>
#include <iostream>
#include <psapi.h>
#include <sstream>
#include <cstring>
#include <vector>
#include <regex>
#include <Core/Config.hpp>
#pragma comment(lib, "ntdll.lib")

typedef struct _MEMORY_REGION {
	DWORD_PTR dwBaseAddr;
	DWORD_PTR dwMemorySize;
}MEMORY_REGION;


namespace Core {


	class MemoryClass {
	public:
		DWORD ProcId;
		HANDLE ProcHandle;
		std::wstring ProcName;
		uintptr_t ModBase, ModBaseSize;

		DWORD GetPidByName( const char * ProcName );
		std::string GetNameByPid( DWORD Pid );
		uintptr_t GetModuleBaseAddr(DWORD ProcId, const char* ModuleName, uintptr_t* ModSize);

		uint64_t GetModuleBaseByName(DWORD Pid, std::wstring ModuleName);

		uint64_t GetModuleBaseSizeByName(DWORD Pid, std::wstring ModuleName);

		BOOL OpenProcByPid();
		BOOL GetMaxPrivileges( HANDLE hProc );

		std::string ReadString( uintptr_t Addr );
		std::vector<uint8_t> ReadBytes( uintptr_t Addr, size_t Size );
		BOOL WriteBytes( uintptr_t Addr, std::vector<uint8_t> Bytes );
		BOOL PatchFunc( uintptr_t Addr, int NopCount );

		uintptr_t FindSignature( std::vector<uint8_t> Signature, uintptr_t ModuleBase = 0, uintptr_t ModuleBaseSize = 0 );
		uintptr_t FindSignatureStr( std::string Pattern, uintptr_t ModuleBase = 0, uintptr_t ModuleBaseSize = 0 );
		uintptr_t PatternScan( std::vector<uint8_t> Pattern, int InstructionLength );
		uintptr_t FindSignatureBypass(std::vector<uint8_t> Signature, uintptr_t ModuleBase, uintptr_t ModuleBaseSize);

		template <typename T>
		T Read( uintptr_t Addr ) {
			T value = { };
			if ( g_Config.General->BattleFive ) {
				driver::read_physical( reinterpret_cast< PVOID >( Addr ) , &value , sizeof( T ) );
			}
			else {
				ZwReadVirtualMemory( ProcHandle , ( LPCVOID ) Addr , &value , sizeof( value ) , NULL );
			}
			return value;
		}

		template <typename T>
		BOOL Write( uintptr_t Addr , T Value ) {
			if ( g_Config.General->BattleFive ) {
				driver::write_physical( reinterpret_cast< PVOID >( Addr ) , &Value , sizeof( T ) );
				return TRUE;
			}
			return ZwWriteVirtualMemory( ProcHandle , ( LPVOID ) Addr , &Value , sizeof( Value ) , NULL );
		}


		uintptr_t CreateCodeCave( size_t Size ) {
			LPVOID CaveAddress = VirtualAllocEx( ProcHandle, NULL, Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
			return ( uintptr_t ) CaveAddress;
		}

		BOOL FreeCave( uintptr_t Addr ) {
			return VirtualFreeEx( ProcHandle, ( LPVOID ) Addr, 0, MEM_RELEASE );
		}

		BOOL HookJMP( uintptr_t HookAddress, uintptr_t JmpToAddress ) {
			BYTE jmpPatch[ ] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
			for ( int x = 0; x < sizeof( jmpPatch ); x++ ) {
				Write<BYTE>( HookAddress + x, jmpPatch[ x ] );
			}

			Write<uintptr_t>( HookAddress + 6, JmpToAddress );

			return true;
		}

		uintptr_t ResolveRelativeAddress( uintptr_t Addr, int InstructionLength )
		{
			uintptr_t Offset = Read<uintptr_t>( Addr + ( InstructionLength - 4 ) );
			return Addr + InstructionLength + Offset;
		}

		std::vector<uint8_t> Pattern2Vector( std::string Pattern ) {
			std::vector<uint8_t> result;
			std::stringstream ss( Pattern );
			std::string token;

			while ( ss >> token ) {
				if ( token == xorstr( "??" ) ) {
					result.push_back( 0x00 );
				}
				else if ( token == xorstr( "?" ) ) {
					result.push_back( 0x00 );
				}
				else {
					uint8_t value = std::stoul( token, nullptr, 16 );
					result.push_back( value );
				}
			}

			return result;
		}
		std::optional<std::string> ReadRemoteString(uint64_t address) {
			uint64_t chunk0 = Read<uint64_t>(address + 0x00);
			uint64_t chunk1 = Read<uint64_t>(address + 0x08);
			uint64_t length = Read<uint64_t>(address + 0x10);

			if (length <= 15) {
				std::string result;
				result.reserve(length);

				for (size_t i = 0; i < length; ++i) {
					char c;
					if (i < 8) {
						c = reinterpret_cast<char*>(&chunk0)[i];
					}
					else {
						c = reinterpret_cast<char*>(&chunk1)[i - 8];
					}
					result.push_back(c);
				}
				return result;
			}
			else {
				uint64_t heap_ptr = chunk0;

				std::string result;
				result.reserve(length);

				for (size_t i = 0; i < length; ++i) {
					char c = Read<char>(heap_ptr + i);
					result.push_back(c);
				}

				return result;
			}
		}
	};

	inline MemoryClass Mem;
}