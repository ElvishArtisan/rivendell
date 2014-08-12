//   rdbusydialog.h
//
//   A 'progress dialog' widget that shows busy state.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdbusydialog.h,v 1.1.2.1 2012/11/26 20:19:36 cvs Exp $
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

#ifndef RDBUSYDIALOG_H
#define RDBUSYDIALOG_H

#include <qdialog.h>
#include <qlabel.h>

#include <rdbusybar.h>

class RDBusyDialog : public QDialog
{
  Q_OBJECT;
 public:
  RDBusyDialog(QWidget *parent=0,const char *name=0,WFlags f=0);
  ~RDBusyDialog();
  QSize sizeHint() const;
  void show(const QString &caption,const QString &label);
  void hide();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *bar_label;
  RDBusyBar *bar_bar;
};


#endif  // RDBUSYDIALOG_H
