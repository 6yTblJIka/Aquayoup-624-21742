#include "GitRevision.h"
#include "revision_data.h"

char const* GitRevision::GetHash()
{
    return _HASH;
}

char const* GitRevision::GetDate()
{
    return _DATE;
}

char const* GitRevision::GetBranch()
{
    return _BRANCH;
}

char const* GitRevision::GetCMakeCommand()
{
    return _CMAKE_COMMAND;
}

char const* GitRevision::GetBuildDirectory()
{
    return _BUILD_DIRECTORY;
}

char const* GitRevision::GetSourceDirectory()
{
    return _SOURCE_DIRECTORY;
}

char const* GitRevision::GetMySQLExecutable()
{
    return _MYSQL_EXECUTABLE;
}

char const* GitRevision::GetFullDatabase()
{
    return _FULL_DATABASE;
}

char const* GitRevision::GetHotfixesDatabase()
{
    return _HOTFIXES_DATABASE;
}

#if PLATFORM == PLATFORM_WINDOWS
#  ifdef _WIN64
#    define TRINITY_PLATFORM_STR "Win64"
#  else
#    define TRINITY_PLATFORM_STR "Win32"
#  endif
#elif PLATFORM == PLATFORM_APPLE
#  define TRINITY_PLATFORM_STR "MacOSX"
#elif PLATFORM == PLATFORM_INTEL
#  define TRINITY_PLATFORM_STR "Intel"
#else // PLATFORM_UNIX
#  define TRINITY_PLATFORM_STR "Unix"
#endif

#ifndef TRINITY_API_USE_DYNAMIC_LINKING
#  define TRINITY_LINKAGE_TYPE_STR "Static"
#else
#  define TRINITY_LINKAGE_TYPE_STR "Dynamic"
#endif

char const* GitRevision::GetFullVersion()
{
	return "Core Aquayoup 6.2.4 21742 - rev. Juillet 2025 " "(" TRINITY_PLATFORM_STR ", "", " TRINITY_LINKAGE_TYPE_STR ")";
}


