// rdcmd_cache.cpp
//
// A low-level container class for a CAE command.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcmd_cache.cpp,v 1.6 2010/07/29 19:32:33 cvs Exp $
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

#include <stdio.h>
#include <string.h>

#include <rdcmd_cache.h>


RDCmdCache::RDCmdCache()
{
  clear();
}


RDCmdCache::RDCmdCache(const RDCmdCache &cmd)
{
  clear();
  cache_argnum=cmd.argNum();
  cache_argptr=cmd.argPtr();
  for(int i=0;i<cache_argnum;i++) {
    strcpy(cache_args[i],cmd.arg(i));
  }
}


void RDCmdCache::clear()
{
  for(int i=0;i<CAE_MAX_ARGS;i++) {
    cache_args[i][0]=0;
  }
  cache_argnum=0;
  cache_argptr=0;
}


const char *RDCmdCache::arg(int n) const
{
  return cache_args[n];
}


void RDCmdCache::setArg(int n,char *arg)
{
  strcpy(cache_args[n],arg);
}


int RDCmdCache::argNum() const
{
  return cache_argnum;
}


void RDCmdCache::setArgNum(int num)
{
  cache_argnum=num;
}


int RDCmdCache::argPtr() const
{
  return cache_argptr;
}


void RDCmdCache::setArgPtr(int ptr)
{
  cache_argptr=ptr;
}


void RDCmdCache::load(char args[CAE_MAX_ARGS][CAE_MAX_LENGTH],
		      int argnum,int argptr)
{
  for(int i=0;i<argnum;i++) {
    int j=0;
    while(args[i][j]!=0) {
      cache_args[i][j]=args[i][j];
      j++;
    }
    cache_args[i][j]=0;
  }
  cache_argnum=argnum;
  cache_argptr=argptr;
}
