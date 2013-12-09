#include "CrashHandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QString>
 
#include "exception_handler.h"

#ifndef WIN32
#include <unistd.h>
#endif

class CrashHandlerPrivate
{
public:
    CrashHandlerPrivate()
    {
        pHandler = NULL;
    }
 
    ~CrashHandlerPrivate()
    {
        delete pHandler;
    }
 
    void InitCrashHandler(const QString& dumpPath);
    static google_breakpad::ExceptionHandler* pHandler;
    static bool bReportCrashesToSystem;
};
 
google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;
bool CrashHandlerPrivate::bReportCrashesToSystem = false;
 
/************************************************************************/
/* DumpCallback                                                         */
/************************************************************************/
#if defined(Q_OS_WIN32)
bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(Q_OS_LINUX)
bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success)
#elif defined(Q_OS_MAC)
bool DumpCallback(const char* _dump_dir,const char* _minidump_id,void *context, bool success)
#endif
{
    Q_UNUSED(context);
#if defined(Q_OS_WIN32)
    Q_UNUSED(_dump_dir);
    Q_UNUSED(_minidump_id);
    Q_UNUSED(assertion);
    Q_UNUSED(exinfo);
#endif
    /*
    NO STACK USE, NO HEAP USE THERE !!!
    Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */

#ifdef WIN32
	wchar_t command[MAX_PATH * 3 + 6];
	wcscpy( command, L"crashreporter ");
	wcscat( command, _dump_dir );
	wcscat( command, L"\\" );
	wcscat( command, _minidump_id );

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof( si ) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;
	ZeroMemory( &pi, sizeof(pi) );

	if ( CreateProcess( NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		TerminateProcess( GetCurrentProcess(), 1 );
	}
#else

#ifdef Q_OS_LINUX

#else
	pid_t pid = fork();
	if ( pid == -1 ) // fork failed
		return false;
	if ( pid == 0 )
	{
		// we are the fork
		execl( crashReporter,
			crashReporter,
			_dump_dir,
			minidump_id,
			minidump_id,
			(char*) 0 );

		// execl replaces this process, so no more code will be executed
		// unless it failed. If it failed, then we should return false.
		printf( "Error: Can't launch CrashReporter!\n" );
		return false;
	}
#endif
#endif



    return CrashHandlerPrivate::bReportCrashesToSystem ? success : true;
}
 
void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath)
{
    if ( pHandler != NULL )
        return;
 
#if defined(Q_OS_WIN32)
    std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
    pHandler = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
        DumpCallback,
        /*context*/
        0,
        true
        );
#elif defined(Q_OS_LINUX)
    std::string pathAsStr = dumpPath.toStdString();
    google_breakpad::MinidumpDescriptor md(pathAsStr);
    pHandler = new google_breakpad::ExceptionHandler(
        md,
        /*FilterCallback*/ 0,
        DumpCallback,
        /*context*/ 0,
        true,
        -1
        );
#elif defined(Q_OS_MAC)
    std::string pathAsStr = dumpPath.toStdString();
    pHandler = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
        DumpCallback,
        /*context*/
        0,
        true,
        NULL
        );
#endif
}
 
/************************************************************************/
/* CrashHandler                                                         */
/************************************************************************/
CrashHandler* CrashHandler::instance()
{
    static CrashHandler globalHandler;
    return &globalHandler;
}
 
CrashHandler::CrashHandler()
{
    d = new CrashHandlerPrivate();
}
 
CrashHandler::~CrashHandler()
{
    delete d;
}
 
void CrashHandler::setReportCrashesToSystem(bool report)
{
    d->bReportCrashesToSystem = report;
}
 
bool CrashHandler::writeMinidump()
{
    bool res = d->pHandler->WriteMinidump();
    if (res) {
        qDebug("BreakpadQt: writeMinidump() successed.");
    } else {
        qWarning("BreakpadQt: writeMinidump() failed.");
    }
    return res;
}
 
void CrashHandler::Init( const QString& reportPath )
{
    d->InitCrashHandler(reportPath);
}
