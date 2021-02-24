//   rdbusydialog.h
//
//   A 'progress dialog' widget that shows busy state.
//
//   (C) Copyright 2010-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDBUSYDIALOG_H
#define RDBUSYDIALOG_H

#include <rdbusybar.h>
#include <rddialog.h>

class RDBusyDialog : public RDDialog
{
  Q_OBJECT;
 public:
  RDBusyDialog(QWidget *parent=0,Qt::WindowFlags f=0);
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
