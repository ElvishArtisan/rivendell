// rdcollect.h
//
// Collect and combine log exports into a single file.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id:
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


#ifndef RDCOLLECT_H
#define RDCOLLECT_H

#include <vector>

#include <qobject.h>
#include <qstringlist.h>

#define RDCOLLECT_USAGE "[options]\n\nCollect and combine log exports from a set of directories into a single file.\n\n--source-file=<file-name>\n     Name of source file.  The path part of this value will be taken as the\n     top of the directory tree to recurse, while the name part will be used as\n     the name of the source file(s) to search for.  This option may be given\n     multiple times.\n\n--destination-file=<file-name>\n     Name of file to which to send output.\n\n--hours-offset=<offset>\n     Start position of the hours field on a data line\n\n--minutes-offset=<offset>\n     Start position of the minutes field on a data line\n\n--seconds-offset=<offset>\n     Start position of the seconds field on a data line\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  QStringList GetDirectoryList(const QString &src_file);
  void LoadSourceFiles(const QString &src_name,const QStringList &dirs,
		       QStringList *lines);
  void LoadSourceFile(const QString &filename,QStringList *lines);
  void SortLines(QStringList *lines,std::vector<unsigned> *index);
  int WriteOutputFile(const QString &filename,const QStringList &lines,
		      std::vector<unsigned> *index);
  void AddDirs(const QString &path,QStringList *dirs);
  QTime ReadTime(const QString &line);
  QStringList source_files;
  QString destination_file;
  unsigned hours_offset;
  unsigned minutes_offset;
  unsigned seconds_offset;
};


#endif  // RDCOLLECT_H
