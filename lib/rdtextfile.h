// rdtextfile.h
//
// Spawn an external text file viewer.
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTEXTFILE_H
#define RDTEXTFILE_H

#include <qstring.h>

bool RDTextFile(const QString &data,bool delete_on_exit=true);
bool RDTextViewer(const QString &filename);
bool RDWebBrowser(const QString &url);


#endif  // RDTEXTFILE_H
