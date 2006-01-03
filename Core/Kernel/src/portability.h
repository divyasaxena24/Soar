#ifndef PORTABILITY_H
#define PORTABILITY_H

/* portability.h
 * Author: Jonathan Voigt (voigtjr@gmail.com)
 * Date: November 2005
 *
 * SYNOPSIS
 *
 * The purpose of this file is to deal with portability issues when compiling the Soar Kernel
 * on different systems. There are two specific environments that this file deals with:
 *   * SoarKernel is compiled in Microsoft Visual Studio
 *   * SoarKernel is compiled with autoconf
 *
 * This file can be viewed as containing four different parts:
 *   * Part 1: Unconditional "bootstrap"
 *   * Part 2: Autoconf section
 *   * Part 3: Visual Studio section
 *   * Part 4: Unconditional "wrap-up"
 *
 * Parts 1 and 4 are parsed no matter what environment SoarKernel is compiled with.
 * Part 2 is only processed when autoconf is used.
 * Part 3 is only processed with autoconf is not used (assuming Visual Studio).
 *
 * USAGE
 * 
 * In order for both environments to compile correctly, the following four lines must be 
 * included in every non-header source file:

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H
#include "portability.h"

 * The presence of the HAVE_CONFIG_H symbol means that the environment used autoconf and
 * related tools.  The absence of this symbol means that these tools weren't used, and
 * this is the case with Visual Studio.
 *
 * When autoconf and related tools are used, there is a config.h file generated by these
 * tools that needs to be included before this file.  This file needs to be included before
 * any other file.
 * 
 * TODO:
 * It is likely the case that only this file needs to be included by all source files
 * and the HAVE_CONFIG_H block moved to this file, but this is a change that will be
 * made at a later date.
 */

/*
 * Part 1: Unconditional 'bootstrap'
 */
#include <stdio.h>

#ifdef HAVE_CONFIG_H
/*
 * Part 2: Autoconf section
 */

#if HAVE_UTIME_H
# include <utime.h>
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
# include <sys/time.h>
# else
# include <time.h>
# endif
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
# include <stdlib.h>
# endif
#endif

#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
# include <memory.h>
# endif
# include <string.h>
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
# include <stdint.h>
# endif
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif

#if HAVE_ASSERT_H
#include <assert.h>
#else // not HAVE_ASSERT_H
typedef void assert;
#endif // not HAVE_ASSERT_H

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#else // not HAVE_CONFIG_H
/*
 * Part 3: Visual Studio section
 */


#include <windows.h>
#include <direct.h>
#include <time.h>
#include <assert.h>

#endif // not HAVE_CONFIG_H
/*
 * Part 4: Unconditional "wrap-up"
 */

#include <ctype.h>
#include <math.h>
#include <signal.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024   /* AGR 536  - from sys/param.h */
#endif

#endif // PORTABILITY_H
