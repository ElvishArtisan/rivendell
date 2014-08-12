// rdcmd_cache.h
//
// A low-level container class for a CAE command.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcmd_cache.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef RDCMD_CACHE_H
#define RDCMD_CACHE_H

#include <rd.h>


class RDCmdCache
{
 public:
  RDCmdCache();
  RDCmdCache(const RDCmdCache &cmd);
  void clear();
  const char *arg(int n) const;
  void setArg(int n,char *arg);
  int argNum() const;
  void setArgNum(int num);
  int argPtr() const;
  void setArgPtr(int ptr);
  void load(char args[CAE_MAX_ARGS][CAE_MAX_LENGTH],int argnum,int argptr);

 private:
  char cache_args[CAE_MAX_ARGS][CAE_MAX_LENGTH];
  int cache_argnum;
  int cache_argptr;
};


#endif  // RDCMD_CACHE_H
