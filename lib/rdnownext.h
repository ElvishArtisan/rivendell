// rdnownext.h
//
// Rivendell Now & Next Implementation
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdnownext.h,v 1.3.8.2 2014/01/07 18:18:29 cvs Exp $
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

#ifndef RDNOWNEXT_H
#define RDNOWNEXT_H

#include <rdlog_line.h>
#include "../rlm/rlm.h"

void RDResolveNowNext(QString *str,RDLogLine **loglines,int encoding);
QString RDResolveNowNext(const QString &pattern,RDLogLine *ll);


#endif  // RDNOWNEXT_H
