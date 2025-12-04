// dropbox_copy_test.h
//
// Generate multiple slow file transfers for testing dropboxes
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef DROPBOX_COPY_TEST_H
#define DROPBOX_COPY_TEST_H

#include <QDir>
#include <QObject>
#include <QStringList>

#define DROPBOX_COPY_TEST_USAGE "[options]\n\nPerform multiple slow file transfer for testing dropboxes\n\nOptions are:\n--in=<filename>\n\n--out=<dirname>\n\n--xfer-size=<blocks>\n\n--gap-size=<seconds>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  void CopyFile(const QString &filename) const;
  QStringList d_input_filenames;
  QDir d_output_dir;
  int d_bytes_size;
  int d_gap_size;
};


#endif  // DROPBOX_COPY_TEST_H
