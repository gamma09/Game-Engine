#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <atomic>
#include <stdarg.h>
#include <thread>

#include <GameAssert.h>
#include <MemoryEngine.h>

#include "Logger.h"

using namespace std;



#ifdef DONT_OVERRIDE_NEW
#include <stdlib.h>
#define new(heap, align) new
#define newArray( type, count, heap, align ) ((type)*) malloc( sizeof( type ) * count )
#define Allocate( heap, align, size ) malloc( size )
#else
#define new(heap, align) new((heap), (align), __FILE__, __LINE__)
#define newArray( type, count, heap, align )  ( (type*) Allocate( sizeof( type ) * (count), heap, align, __FILE__, __LINE__ ) )
#define newPtr( heap, align, size )  ( Allocate( (size), (heap), (align), __FILE__, __LINE__ ) )
#endif




class SynchronousLogger : private Logger
{
private:
	SynchronousLogger( const FileHandle& logFile ) : Logger( logFile ) {}
	virtual ~SynchronousLogger() override {}

	virtual void Write_To_Log( char* str ) override
	{
		this->WriteString( str );
		delete str;
	}



	SynchronousLogger( const SynchronousLogger& ) = delete;
	SynchronousLogger& operator=( const SynchronousLogger& ) = delete;

	friend class Logger;
};

class AsynchronousLogger : private Logger
{
private:

	struct LogEntry
	{

		LogEntry* next;
		char* entry;
	};


private:
	AsynchronousLogger( const FileHandle& logFile );
	virtual ~AsynchronousLogger() override;

	virtual void Write_To_Log( char* str ) override;

	void run();

	static DWORD WINAPI IOThreadProc( LPVOID lpParameter );


private:
	Heap* logEntryHeap;
	atomic_flag pendingEntriesLocked;
	LogEntry* pendingEntriesHead;
	LogEntry* pendingEntriesTail;
	HANDLE pendingSemaphore;
	HANDLE stopEvent;
	HANDLE ioThread;



	AsynchronousLogger( const AsynchronousLogger& ) = delete;
	AsynchronousLogger& operator=( const AsynchronousLogger& ) = delete;

	friend class Logger;
};



// would rather not use this many, but if the logging thread for asynchronous logger gets put on pause, we might need a whole lot of blocks
#define MAX_BUFFER_COUNT 62

AsynchronousLogger::AsynchronousLogger( const FileHandle& logFile )
	: Logger( logFile ),
	pendingEntriesHead( nullptr ),
	pendingEntriesTail( nullptr ),
	pendingEntriesLocked()
{
	this->pendingEntriesLocked.clear( memory_order_relaxed );

	this->pendingSemaphore = CreateSemaphore( nullptr, 0, MAX_BUFFER_COUNT - 1, nullptr );
	if( !this->pendingSemaphore )
	{
		char buffer[64];
		sprintf_s( buffer, "Could not create semaphore for logger - Error: %lu", GetLastError() );
		GameCheckFatal( this->pendingSemaphore, buffer );
	}

	this->stopEvent = CreateEvent( nullptr, false, false, nullptr );
	if( !this->stopEvent )
	{
		char buffer[64];
		sprintf_s( buffer, "Could not create event for logger - Error: %lu", GetLastError() );
		GameCheckFatal( this->stopEvent, buffer );
	}

	Mem::createFixBlockHeap( this->logEntryHeap, MAX_BUFFER_COUNT - 1, sizeof( LogEntry ) );

	this->ioThread = CreateThread( nullptr, 0, AsynchronousLogger::IOThreadProc, this, 0, nullptr );
	if( this->ioThread == nullptr )
	{
		char buffer[64];
		sprintf_s( buffer, "Could not create thread for logger - Error: %lu", GetLastError() );
		GameCheckFatal( this->stopEvent, buffer );
	}
}

AsynchronousLogger::~AsynchronousLogger()
{
	if( !SetEvent( this->stopEvent ) )
	{
		char buffer[64];
		sprintf_s( buffer, "Async log SetEvent failure - Error: %lu", GetLastError() );
		GameCheckFatal( false, buffer );
	}

	if( WAIT_FAILED == WaitForSingleObject( this->ioThread, INFINITE ) )
	{
		char buffer[64];
		sprintf_s( buffer, "Async log WaitForSingleObject failure - Error: %lu", GetLastError() );
		GameCheckFatal( false, buffer );
	}

	CloseHandle( this->ioThread );
	CloseHandle( this->pendingSemaphore );
	CloseHandle( this->stopEvent );

	Mem::destroyHeap( this->logEntryHeap );
}

void AsynchronousLogger::Write_To_Log( char* str )
{
	LogEntry* entry = new( this->logEntryHeap, ALIGN_4 ) LogEntry();
	entry->next = nullptr;
	entry->entry = str;

	while( this->pendingEntriesLocked.test_and_set( memory_order_acq_rel ) )
	{
		this_thread::yield();
	}

	if( this->pendingEntriesTail )
	{
		this->pendingEntriesTail->next = entry;
	}
	else
	{
		this->pendingEntriesHead = entry;
	}

	this->pendingEntriesTail = entry;

	this->pendingEntriesLocked.clear( memory_order_release );
	ReleaseSemaphore( this->pendingSemaphore, 1, nullptr );
}

static DWORD WaitForUpdate( DWORD handleCount, HANDLE* handles )
{
	DWORD waitResult = WaitForMultipleObjects( handleCount, handles, false, INFINITE );
	if( waitResult == WAIT_FAILED )
	{
		char buffer[80];
		sprintf_s( buffer, "Async log WaitForMultipleObjects failure - Error: %lu", GetLastError() );
		GameCheckFatal( false, buffer );
	}

	return waitResult - WAIT_OBJECT_0;
}


#define STOP_EVENT_INDEX 0
#define PENDING_SEMAPHORE_INDEX 1

void AsynchronousLogger::run()
{
	HANDLE updateEvents[2];

	updateEvents[STOP_EVENT_INDEX] = this->stopEvent;
	updateEvents[PENDING_SEMAPHORE_INDEX] = this->pendingSemaphore;

	DWORD updateIndex = WaitForUpdate( 2, updateEvents );
	while( updateIndex != STOP_EVENT_INDEX )
	{
		// exchange head with head's next (atomically)
		while( this->pendingEntriesLocked.test_and_set( memory_order_acq_rel ) )
		{
			this_thread::yield();
		}

		LogEntry* curr = this->pendingEntriesHead;
		this->pendingEntriesHead = curr->next;
		if( !this->pendingEntriesHead )
		{
			this->pendingEntriesTail = nullptr;
		}

		this->pendingEntriesLocked.clear( memory_order_release );
		this->WriteString( curr->entry );

		delete curr->entry;
		delete curr;

		updateIndex = WaitForUpdate( 2, updateEvents );
	}
}

DWORD WINAPI AsynchronousLogger::IOThreadProc( LPVOID lpParameter )
{
	AsynchronousLogger* asyncLogger = (AsynchronousLogger*) lpParameter;
	asyncLogger->run();
	return 0;
}



Logger* Logger::logger = nullptr;

void Logger::Start_Log( const char* logFile, bool bAsync )
{
	FileHandle fh;

	GameCheckFatal( FileError::FILE_SUCCESS == File::open( fh, logFile, FileMode::FILE_WRITE ), "Could not create log file." );
	GameAssert( logger == nullptr );

	if( bAsync )
	{
		logger = new AsynchronousLogger( fh );
	}
	else
	{
		logger = new SynchronousLogger( fh );
	}

	atomic_thread_fence( memory_order_release );
}

#define BUFFER_SIZE 256

void Logger::Log_Debug( const char* format, ... )
{
	GameAssert( logger );

	format;

#ifdef _DEBUG

	char* formatBuffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );
	sprintf_s( formatBuffer, BUFFER_SIZE, "[DEBUG][Thread %lu] %s\n", GetCurrentThreadId(), format );
	char* buffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );

	va_list ap;
	va_start( ap, format );

	vsprintf_s( buffer, BUFFER_SIZE, formatBuffer, ap );

	va_end( ap );

	delete formatBuffer;

	// Write_To_Log is responsible for freeing the buffer
	logger->Write_To_Log( buffer );

#endif
}

void Logger::Log_Info( const char* format, ... )
{
	GameAssert( logger );

	char* formatBuffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );
	sprintf_s( formatBuffer, BUFFER_SIZE, "[INFO][Thread %lu] %s\n", GetCurrentThreadId(), format );
	char* buffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );

	va_list ap;
	va_start( ap, format );

	vsprintf_s( buffer, BUFFER_SIZE, formatBuffer, ap );

	va_end( ap );

	delete formatBuffer;

	// Write_To_Log is responsible for freeing the buffer
	logger->Write_To_Log( buffer );
}

void Logger::Log_Warning( const char* format, ... )
{
	GameAssert( logger );

	char* formatBuffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );
	sprintf_s( formatBuffer, BUFFER_SIZE, "[WARNING][Thread %lu] %s\n", GetCurrentThreadId(), format );
	char* buffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );

	va_list ap;
	va_start( ap, format );

	vsprintf_s( buffer, BUFFER_SIZE, formatBuffer, ap );

	va_end( ap );

	delete formatBuffer;

	// Write_To_Log is responsible for freeing the buffer
	logger->Write_To_Log( buffer );
}

void Logger::Log_Error( const char* format, ... )
{
	GameAssert( logger );

	char* formatBuffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );
	sprintf_s( formatBuffer, BUFFER_SIZE, "[ERROR][Thread %lu] %s\n", GetCurrentThreadId(), format );
	char* buffer = newArray( char, BUFFER_SIZE, logger->loggingBufferHeap, ALIGN_4 );

	va_list ap;
	va_start( ap, format );

	vsprintf_s( buffer, BUFFER_SIZE, formatBuffer, ap );

	va_end( ap );

	delete formatBuffer;

	// Write_To_Log is responsible for freeing the buffer
	logger->Write_To_Log( buffer );
}

void Logger::Stop_Log()
{
	GameAssert( logger );

	delete logger;
}



Logger::Logger( const FileHandle& inLogFile )
	: logFile( inLogFile )
{
	Mem::createFixBlockHeap( this->loggingBufferHeap, MAX_BUFFER_COUNT, BUFFER_SIZE );
}

Logger::~Logger()
{
	GameCheckFatal( FileError::FILE_SUCCESS == File::close( this->logFile ), "Could not close the log file." );

	Mem::destroyHeap( this->loggingBufferHeap );
}

void Logger::WriteString( const char* str ) const
{
	GameVerify( FileError::FILE_SUCCESS == File::write( this->logFile, str, strlen( str ) ) );
}