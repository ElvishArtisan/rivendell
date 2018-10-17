/* rd_listcartcuts.h
 *
 * Header for the ListCartCuts  Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>
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

#ifndef RD_LISTCARTCUTS_H
#define RD_LISTCARTCUTS_H

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

#include <rivwebcapi/rd_cart.h>

int RD_ListCartCuts(struct rd_cart *carts[],
		    const char hostname[],
		    const char username[],
		    const char passwd[],
		    const char ticket[],
		    const unsigned cartnumber,
		    const char user_agent[],
		    unsigned *numrecs);
struct rd_cut *RD_ListCartCuts_GetCut(struct rd_cart *cart,int pos);
void RD_ListCartCuts_Free(struct rd_cart *cart);

_MYRIVLIB_FINI_DECL


#endif  // RD_LISTCARTCUTS_H
