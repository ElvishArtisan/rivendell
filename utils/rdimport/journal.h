// journal.h
//
// E-mail file importation actions
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef JOURNAL_H
#define JOURNAL_H

#include <QList>
#include <QMultiMap>
#include <QString>
#include <QStringList>

class Journal
{
 public:
  Journal(bool send_immediately);
  void addSuccess(const QString &groupname,QString filename,
		  unsigned cartnum,const QString &title);
  void addFailure(const QString &groupname,QString filename,
		  const QString &err_msg);
  void sendAll();

 private:
  QMultiMap<QString,QString> GroupsByAddress(QStringList groups) const;
  bool c_send_immediately;
  QStringList c_good_groups;
  QStringList c_good_filenames;
  QList<unsigned> c_good_cart_numbers;
  QStringList c_good_titles;
  QStringList c_bad_groups;
  QStringList c_bad_filenames;
  QStringList c_bad_errors;
};


#endif  // JOURNAL_H
