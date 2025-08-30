// dropboxlist.cpp
//
// List of dropbox file entries
//
//   (C) Copyright 2002-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <QFile>

#include "dropboxlist.h"

DropboxList::DropboxList(const QString &fname)
{
  QFile *file=new QFile(fname);
  filename=fname;
  size=file->size();
  pass=0;
  checked=true;
  failed=false;
  delete file;
}


QString DropboxList::dump() const
{
  return QString::asprintf("%s:  size: %ld  pass: %d  checked: %d  failed: %d",
			   filename.toUtf8().constData(),
			   size,pass,checked,failed);
}
