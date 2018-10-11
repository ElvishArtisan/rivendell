/* rd_listcartscuts.h
 *
 * Header for the ListCartsCuts Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker <bakert@rfa.org>
 * (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RD_LISTCARTSCUTS_H
#define RD_LISTCARTSCUTS_H

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

#include <rivwebcapi/rd_cart.h>

int RD_ListCartsCuts(struct rd_cart *carts[],
		     const char hostname[],
		     const char username[],
		     const char passwd[],
		     const char ticket[],
		     const char group_name[],
		     const char filter[],
		     const char type[],
		     const char user_agent[],
		     unsigned *numrecs);
struct rd_cut *RD_ListCartsCuts_GetCut(struct rd_cart carts[],int cart_rec,
				       int cut_rec);
void RD_ListCartsCuts_Free(struct rd_cart carts[],int numrecs);

_MYRIVLIB_FINI_DECL


#endif  // RD_LISTCARTSCUTS_H
