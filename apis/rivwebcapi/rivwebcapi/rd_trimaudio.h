/* rd_trimaudio.h
 *
 * Header for the Trim Audio  Rivendell Access Library
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

#ifndef RD_TRIMAUDIO_H
#define RD_TRIMAUDIO_H

struct rd_trimaudio {
  unsigned cart_number;
  unsigned cut_number;
  int trimlevel;
  int starttrimpoint;
  int endtrimpoint;
};

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

int RD_TrimAudio(struct rd_trimaudio *trimaudio[],
		 const char hostname[],
		 const char username[],
		 const char passwd[],
		 const char ticket[],
		 const unsigned cartnumber,
		 const unsigned cutnumber,
		 const int trimlevel,
		 const char user_agent[],
		 unsigned *numrecs);

_MYRIVLIB_FINI_DECL


#endif  // RD_TRIMAUDIO_H
