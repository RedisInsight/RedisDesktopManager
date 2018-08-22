#include "crashhandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QString>
#include <QDebug>
#include <QFileInfo>

#if defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#elif defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#endif

#ifndef WIN32
#include <unistd.h>
#endif
#include <wchar.h>

#define CRASHANDLER_MAX_PATH 2000

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
 
    void InitCrashHandler(const QString& dumpPath, const QString &appPath, const QString &appDir);
    static google_breakpad::ExceptionHandler* pHandler;
    static bool bReportCrashesToSystem;
    
};
 
google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;
bool CrashHandlerPrivate::bReportCrashesToSystem = false;
wchar_t crashReporterPath[CRASHANDLER_MAX_PATH];
wchar_t applicationPath[CRASHANDLER_MAX_PATH];

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
#elif defined(Q_OS_LINUX)
    Q_UNUSED(md);
#endif
    /*
    NO STACK USE, NO HEAP USE THERE !!!
    Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */

#if defined(WIN32)    
    wcscat_s( crashReporterPath, L" \"");
    wcscat_s( crashReporterPath, _dump_dir );
    wcscat_s( crashReporterPath, L"/" );
    wcscat_s( crashReporterPath, _minidump_id );
    wcscat_s( crashReporterPath, L".dmp\"" );
    wcscat_s( crashReporterPath, L" \"");
    wcscat_s( crashReporterPath, applicationPath );
    wcscat_s( crashReporterPath, L"\"");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ZeroMemory( &pi, sizeof(pi) );

    if ( CreateProcess( NULL, crashReporterPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
    {
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        TerminateProcess( GetCurrentProcess(), 1 );
    }
#else // OSX & LINUX

   char reporterPath[CRASHANDLER_MAX_PATH];
   wcstombs(reporterPath, crashReporterPath, CRASHANDLER_MAX_PATH);

   char aPath[CRASHANDLER_MAX_PATH];
   wcstombs(aPath, applicationPath, CRASHANDLER_MAX_PATH);

   char command[CRASHANDLER_MAX_PATH];

#if defined(Q_OS_LINUX)
   strcpy(command, md.path());
#elif defined(Q_OS_MAC)
    strcpy(command, _dump_dir);
    strcat(command, "/");
    strcat(command, _minidump_id);
    strcat(command, ".dmp");
#endif

    pid_t pid = fork();
    if ( pid == -1 ) // fork failed
        return false;
    if ( pid == 0 )
    {
        // we are the fork
        execl( reporterPath,
            reporterPath,
            command,
            aPath,
            (char*) 0 );

        printf( "Error: Can't launch CrashReporter!\n" );
        return false;
    }
#endif

    return CrashHandlerPrivate::bReportCrashesToSystem ? success : true;
}
 
void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath, const QString& appPath, const QString& crashReporterFullPath)
{
    if ( pHandler != NULL )
        return;

    wcscpy(applicationPath, appPath.toStdWString().c_str());
    wcscpy(crashReporterPath, crashReporterFullPath.toStdWString().c_str());
 
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
 
void CrashHandler::Init( const QString& reportPath,  const QString& appPath, const QString& crashReporterFullPath )
{
    d->InitCrashHandler(reportPath, appPath, crashReporterFullPath);
}
