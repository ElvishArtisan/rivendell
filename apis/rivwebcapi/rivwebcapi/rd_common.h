/* rd_common.h
 *
 * Common Utility Functions for the Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker <bakert@rfa.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RD_COMMON_H
#define RD_COMMON_H
#if ((defined(_WINDLL)) || (defined(_WIN32)))
#ifndef MINGW32
#define strcpy_s strcpy
#define strncpy_s strncpy
#define strcat_s strcat
#define snprintf _snprintf
#define strcasecmp _stricmp
#define sscanf_s sscanf
#endif  // MINGW32
#define VERSION "0.0.6"
#endif

/*   #define RIVC_DEBUG_OUT  Uncomment for stderr output */

#if defined(__cplusplus)
#define _MYRIVLIB_INIT_DECL extern "C" {
#define _MYRIVLIB_FINI_DECL }
#else
#define _MYRIVLIB_INIT_DECL
#define _MYRIVLIB_FINI_DECL
#endif

extern unsigned RD_ReadBool(const char *val);

extern struct tm RD_Cnv_DTString_to_tm( const char *datein);

extern size_t RD_Cnv_tm_to_DTString(struct tm *tmptr,char * dest);

extern size_t strlcpy(char * dest, const char* src, size_t bufsize);

extern double get_local_offset();

extern int validate_tm (struct tm *tmptr);

extern int RD_Cnv_TString_to_msec(const char *str);

extern size_t RD_Cnv_msec_to_TString(char *str, size_t len,int msec);


#endif  // RD_COMMON_H
