// ww1ipump.h
//
// Replicator implementation for the Westwood One iPump Replicator
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WW1IPUMP_H
#define WW1IPUMP_H

#include "replfactory.h"

class Ww1Ipump : public ReplFactory
{
 public:
  Ww1Ipump(ReplConfig *repl_config);
  void startProcess();
  bool processCart(const unsigned cartnum);

 private:
  void CheckIsciXreference();
  bool LoadIsciXreference(const QString &filename);
  bool ValidateFilename(const QString &filename);
  void CheckCarts();
  bool PostCut(const QString &cutname,const QString &filename);
  void PurgeCuts();
  QDateTime xds_isci_datetime;
};


#endif  // WW1IPUMP_H
