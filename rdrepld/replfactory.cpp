// replfactory.cpp
//
// Virtual base class for replicator methods
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: replfactory.cpp,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

#include <replfactory.h>

ReplFactory::ReplFactory(ReplConfig *config)
{
  repl_config=config;
}


ReplFactory::~ReplFactory()
{
  delete repl_config;
}


ReplConfig *ReplFactory::config() const
{
  return repl_config;
}
