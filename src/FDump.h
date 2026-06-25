#ifndef FDUMP_H
#define FDUMP_H

#ifndef US_TYPEDEFS
#include "typedefs.h"
#endif

#undef DXDEC
#ifdef BUILD_DACOM
#define DXDEC __declspec(dllexport)
#else
#define DXDEC __declspec(dllimport)
#endif

#define __SEV_FATAL 0
#define __SEV_ERROR 1
#define __SEV_WARNING 2
#define __SEV_NOTICE 3
#define __SEV_TRACE_1 4
#define __SEV_TRACE_2 5
#define __SEV_TRACE_3 6
#define __SEV_TRACE_4 7
#define __SEV_TRACE_5 8

#ifndef DA_ERROR_LEVEL
#ifdef DA_LIBS
#pragma message("WARNING: DA_ERROR_LEVEL not defined. Setting to maximum verbosity (default).")
#endif
#define DA_ERROR_LEVEL __SEV_TRACE_5
#endif

#ifdef __cplusplus

enum SeverityLevel
{
	SEV_FATAL = __SEV_FATAL,
	SEV_ERROR = __SEV_ERROR,
	SEV_WARNING = __SEV_WARNING,
	SEV_NOTICE = __SEV_NOTICE,
	SEV_TRACE_1 = __SEV_TRACE_1,
	SEV_TRACE_2 = __SEV_TRACE_2,
	SEV_TRACE_3 = __SEV_TRACE_3,
	SEV_TRACE_4 = __SEV_TRACE_4,
	SEV_TRACE_5 = __SEV_TRACE_5,
};

#define ERR_SEV_BITS 4
#define ERR_KIND_BITS (32 - ERR_SEV_BITS)
#define ERR_LIB_BASE 65536

#ifndef ERRORKIND_DEFINED
#define ERRORKIND_DEFINED

enum ErrorKind
{
	ERR_GENERAL = ERR_LIB_BASE,
	ERR_FILE,
	ERR_MEMORY,
	ERR_CORRUPT,
	ERR_ASSERT,
	ERR_BADPARAM,
	ERR_PERFORMANCE,
	ERR_PARSER
};
#endif

struct ErrorCode
{
	SeverityLevel severity : ERR_SEV_BITS;
	ErrorKind kind : ERR_KIND_BITS;

	ErrorCode() {}
	ErrorCode(ErrorKind _kind, SeverityLevel _severity)
	{
		severity = _severity;
		kind = _kind;
	}
	ErrorCode(unsigned int _kind, SeverityLevel _severity)
	{
		severity = _severity;
		kind = static_cast<ErrorKind>(_kind);
	}
};

typedef int(__cdecl *DA_ERROR_HANDLER)(ErrorCode code, const C8 *fmt, ...);

extern "C"
{
	extern DXDEC DA_ERROR_HANDLER FDUMP;
}

#define IDE_ERR_FMT "%s(%d) : "

#define GENERAL_FMT "General:%s"

#define GENERAL_FATAL(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_FATAL), IDE_ERR_FMT "FATAL:" GENERAL_FMT, __FILE__, __LINE__, reason)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define GENERAL_ERROR(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_ERROR), IDE_ERR_FMT "ERROR:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_ERROR(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define GENERAL_WARNING(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_WARNING), IDE_ERR_FMT "WARNING:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_WARNING(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_NOTICE
#define GENERAL_NOTICE(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_NOTICE), IDE_ERR_FMT "NOTICE:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_NOTICE(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define GENERAL_TRACE_1(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_TRACE_1), IDE_ERR_FMT "TRACE_1:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_TRACE_1(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define GENERAL_TRACE_2(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_TRACE_2), IDE_ERR_FMT "TRACE_2:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_TRACE_2(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define GENERAL_TRACE_3(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_TRACE_3), IDE_ERR_FMT "TRACE_3:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_TRACE_3(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define GENERAL_TRACE_4(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_TRACE_4), IDE_ERR_FMT "TRACE_4:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_TRACE_4(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define GENERAL_TRACE_5(reason) \
	(*FDUMP)(ErrorCode(ERR_GENERAL, SEV_TRACE_5), IDE_ERR_FMT "TRACE_5:" GENERAL_FMT, __FILE__, __LINE__, reason)
#else
#define GENERAL_TRACE_5(reason)
#endif

#define FILE_FMT "File: on file \"%s\": failed on %s"

#define FILE_FATAL(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_FATAL), IDE_ERR_FMT "FATAL:" FILE_FMT, __FILE__, __LINE__, file, operation)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define FILE_ERROR(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_ERROR), IDE_ERR_FMT "ERROR:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_ERROR(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define FILE_WARNING(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_WARNING), IDE_ERR_FMT "WARNING:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_WARNING(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define FILE_TRACE_1(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_TRACE_1), IDE_ERR_FMT "TRACE_1:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_TRACE_1(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define FILE_TRACE_2(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_TRACE_2), IDE_ERR_FMT "TRACE_2:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_TRACE_2(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define FILE_TRACE_3(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_TRACE_3), IDE_ERR_FMT "TRACE_3:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_TRACE_3(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define FILE_TRACE_4(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_TRACE_4), IDE_ERR_FMT "TRACE_4:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_TRACE_4(file, operation)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define FILE_TRACE_5(file, operation) \
	(*FDUMP)(ErrorCode(ERR_FILE, SEV_TRACE_5), IDE_ERR_FMT "TRACE_5:" FILE_FMT, __FILE__, __LINE__, file, operation)
#else
#define FILE_TRACE_5(file, operation)
#endif

#define MEMORY_FMT "Memory: %s"

#define MEMORY_FATAL(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_FATAL), IDE_ERR_FMT "FATAL:" MEMORY_FMT, __FILE__, __LINE__, reason)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define MEMORY_ERROR(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_ERROR), IDE_ERR_FMT "ERROR:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_ERROR(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define MEMORY_WARNING(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_WARNING), IDE_ERR_FMT "WARNING:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_WARNING(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define MEMORY_TRACE_1(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_TRACE_1), IDE_ERR_FMT "TRACE_1:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_TRACE_1(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define MEMORY_TRACE_2(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_TRACE_2), IDE_ERR_FMT "TRACE_2:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_TRACE_2(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define MEMORY_TRACE_3(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_TRACE_3), IDE_ERR_FMT "TRACE_3:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_TRACE_3(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define MEMORY_TRACE_4(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_TRACE_4), IDE_ERR_FMT "TRACE_4:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_TRACE_4(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define MEMORY_TRACE_5(reason) \
	(*FDUMP)(ErrorCode(ERR_MEMORY, SEV_TRACE_5), IDE_ERR_FMT "TRACE_5:" MEMORY_FMT, __FILE__, __LINE__, reason)
#else
#define MEMORY_TRACE_5(reason)
#endif

#define CORRUPT_FMT "Corrupt Data: %s"

#define CORRUPT_FATAL(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_FATAL), IDE_ERR_FMT "FATAL:" CORRUPT_FMT, __FILE__, __LINE__, reason)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define CORRUPT_ERROR(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_ERROR), IDE_ERR_FMT "ERROR:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_ERROR(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define CORRUPT_WARNING(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_WARNING), IDE_ERR_FMT "WARNING:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_WARNING(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define CORRUPT_TRACE_1(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_TRACE_1), IDE_ERR_FMT "TRACE_1:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_TRACE_1(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define CORRUPT_TRACE_2(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_TRACE_2), IDE_ERR_FMT "TRACE_2:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_TRACE_2(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define CORRUPT_TRACE_3(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_TRACE_3), IDE_ERR_FMT "TRACE_3:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_TRACE_3(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define CORRUPT_TRACE_4(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_TRACE_4), IDE_ERR_FMT "TRACE_4:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_TRACE_4(reason)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define CORRUPT_TRACE_5(reason) \
	(*FDUMP)(ErrorCode(ERR_CORRUPT, SEV_TRACE_5), IDE_ERR_FMT "TRACE_5:" CORRUPT_FMT, __FILE__, __LINE__, reason)
#else
#define CORRUPT_TRACE_5(reason)
#endif

#include <assert.h>

#define ASSERT_FMT "Assertion failed: %s"

#define ASSERT_DEF(sev, expr) \
	if (!(expr))              \
	(*FDUMP)(ErrorCode(ERR_ASSERT, SEV_##sev), IDE_ERR_FMT #sev ":" ASSERT_FMT, __FILE__, __LINE__, #expr)

#define ASSERT_FATAL(expr) ASSERT_DEF(FATAL, expr)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define ASSERT_ERROR(expr) ASSERT_DEF(ERROR, expr)
#else
#define ASSERT_ERROR(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define ASSERT_WARNING(expr) ASSERT_DEF(WARNING, expr)
#else
#define ASSERT_WARNING(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define ASSERT_TRACE_1(expr) ASSERT_DEF(TRACE_1, expr)
#else
#define ASSERT_TRACE_1(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define ASSERT_TRACE_2(expr) ASSERT_DEF(TRACE_2, expr)
#else
#define ASSERT_TRACE_2(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define ASSERT_TRACE_3(expr) ASSERT_DEF(TRACE_3, expr)
#else
#define ASSERT_TRACE_3(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define ASSERT_TRACE_4(expr) ASSERT_DEF(TRACE_4, expr)
#else
#define ASSERT_TRACE_4(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define ASSERT_TRACE_5(expr) ASSERT_DEF(TRACE_5, expr)
#else
#define ASSERT_TRACE_5(expr)
#endif

#define BADPARAM_FMT "Bad Parameter: %s"

#define BADPARAM_DEF(sev, expr)                                                                                         \
	{                                                                                                                   \
		if (!(expr))                                                                                                    \
		{                                                                                                               \
			(*FDUMP)(ErrorCode(ERR_BADPARAM, SEV_##sev), IDE_ERR_FMT #sev ":" BADPARAM_FMT, __FILE__, __LINE__, #expr); \
			assert(expr);                                                                                               \
		}                                                                                                               \
	}

#define BADPARAM_FATAL(expr) BADPARAM_DEF(FATAL, expr)

#if DA_ERROR_LEVEL >= __SEV_ERROR
#define BADPARAM_ERROR(expr) BADPARAM_DEF(ERROR, expr)
#else
#define BADPARAM_ERROR(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_WARNING
#define BADPARAM_WARNING(expr) BADPARAM_DEF(WARNING, expr)
#else
#define BADPARAM_WARNING(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define BADPARAM_TRACE_1(expr) BADPARAM_DEF(TRACE_1, expr)
#else
#define BADPARAM_TRACE_1(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define BADPARAM_TRACE_2(expr) BADPARAM_DEF(TRACE_2, expr)
#else
#define BADPARAM_TRACE_2(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define BADPARAM_TRACE_3(expr) BADPARAM_DEF(TRACE_3, expr)
#else
#define BADPARAM_TRACE_3(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define BADPARAM_TRACE_4(expr) BADPARAM_DEF(TRACE_4, expr)
#else
#define BADPARAM_TRACE_4(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define BADPARAM_TRACE_5(expr) BADPARAM_DEF(TRACE_5, expr)
#else
#define BADPARAM_TRACE_5(expr)
#endif

#define PERFORMANCE_FMT "Performance warning: %s"

#define PERFORMANCE_DEF(sev, expr) \
	(*FDUMP)(ErrorCode(ERR_PERFORMANCE, SEV_##sev), IDE_ERR_FMT #sev ":" PERFORMANCE_FMT, __FILE__, __LINE__, #expr)

#if DA_ERROR_LEVEL >= __SEV_TRACE_1
#define PERFORMANCE_TRACE_1(expr) PERFORMANCE_DEF(TRACE_1, expr)
#else
#define PERFORMANCE_TRACE_1(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_2
#define PERFORMANCE_TRACE_2(expr) PERFORMANCE_DEF(TRACE_2, expr)
#else
#define PERFORMANCE_TRACE_2(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_3
#define PERFORMANCE_TRACE_3(expr) PERFORMANCE_DEF(TRACE_3, expr)
#else
#define PERFORMANCE_TRACE_3(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_4
#define PERFORMANCE_TRACE_4(expr) PERFORMANCE_DEF(TRACE_4, expr)
#else
#define PERFORMANCE_TRACE_4(expr)
#endif

#if DA_ERROR_LEVEL >= __SEV_TRACE_5
#define PERFORMANCE_TRACE_5(expr) PERFORMANCE_DEF(TRACE_5, expr)
#else
#define PERFORMANCE_TRACE_5(expr)
#endif

#endif

#undef ASSERT
#undef ASSERT2

#ifdef FINAL_RELEASE

#define ASSERT(exp) ((void)0)
#define ASSERT2(exp) ((void)0)

#else

#define ASSERT(exp) ASSERT_FATAL(exp)
#define ASSERT2(sev, exp) ASSERT_##sev(exp)

#endif

#undef CHECKPARAM

#ifdef USE_CHECKPARAM

#define CHECKPARAM(sev, exp) ((void)0)

#else

#define CHECKPARAM(sev, exp) BADPARAM_##sev(exp)

#endif

#endif
