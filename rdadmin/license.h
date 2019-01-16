// license.h
//
// Display License Text.
//
// (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef LICENSE_H
#define LICENSE_H

#include <qdialog.h>
#include <qtextedit.h>

class License : public QDialog
{
 Q_OBJECT
 public:
  enum Text {GplV2=0,Credits=1};
  License(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void exec(License::Text lic);

 private slots:
  void closeData();

 private:
  QTextEdit *license_edit;
};


#endif  // LICENSE_H
