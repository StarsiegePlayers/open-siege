#ifndef ENGINETYPES_HPP
#define ENGINETYPES_HPP

// Most Darkstar games have functions using Borland fastcall,
// so this is considered the standard calling convention for functions
#ifdef __GNUC__
    #define DARKCALL __attribute__((regparm(3)))
#else
    #define DARKCALL __fastcall
#endif

#define APICALL __cdecl

#include <cstdint>

namespace Core
{
	struct GameRoot;
	struct GameConsole;
	struct GameManager;
	struct GameObject;

   	using ConsoleCallbackFunc = const char* (DARKCALL*)(GameConsole*, std::int32_t, std::int32_t, const char**);
	using ManagedString = const char*;

	template<typename T>
	struct DynamicArray
	{
		std::uint32_t size;
		std::uint32_t capacity;
		std::uint16_t chunkSize;
		std::uint16_t settings;
        T* data;
    };

	struct ConsoleConsumer
	{
		virtual void DARKCALL writeLine(GameConsole*, const char *consoleLine) = 0;
	};

	struct ConsoleCallback
	{
		virtual const char* DARKCALL executeCallback(GameConsole* console,
				std::int32_t callbackId,
				std::int32_t argc,
				const char** argv) = 0;
	};

	struct GamePlugin : ConsoleCallback
	{
		GameManager* manager;
		GameConsole* console;

        virtual void DARKCALL destroy() = 0;
        virtual void DARKCALL setManager(GameManager* manager) = 0;
		virtual void DARKCALL init() = 0;
		virtual void DARKCALL startFrame() = 0;
		virtual void DARKCALL endFrame() = 0;

        // unlike Borland C++ of old, destructors seem to go at the end of the vtable
        virtual DARKCALL ~GamePlugin() = default;
	};
}

#endif