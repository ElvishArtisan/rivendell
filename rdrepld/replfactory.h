// replfactory.h
//
// Virtual base class for replicator methods
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: replfactory.h,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

#ifndef REPLFACTORY_H
#define REPLFACTORY_H

#include <globals.h>
#include <replconfig.h>

class ReplFactory
{
 public:
  ReplFactory(ReplConfig *config);
  virtual ~ReplFactory();
  ReplConfig *config() const;
  virtual void startProcess()=0;
  virtual bool processCart(const unsigned cartnum)=0;

 private:
  ReplConfig *repl_config;
};


#endif  // REPLCONFIG_H
