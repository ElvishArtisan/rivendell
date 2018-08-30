/* rd_listcuts.h
 *
 * Header for the ListCuts  Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>
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

#ifndef RD_LISTCUTS_H
#define RD_LISTCUTS_H

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

#include <rivwebcapi/rd_cut.h>

int RD_ListCuts(struct rd_cut *cuts[],
		const char hostname[],
		const char username[],
		const char passwd[],
		const char ticket[],
		const unsigned cartnumber,
		const char user_agent[],
		unsigned *numrecs);

_MYRIVLIB_FINI_DECL


#endif  // RD_LISTCUTS_H
