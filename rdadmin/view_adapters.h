// view_adapter.h
//
// Display Audio Adapter Information
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef VIEW_ADAPTER_H
#define VIEW_ADAPTER_H

#include <QPushButton>
#include <QTextEdit>

#include <rddialog.h>
#include <rdstation.h>
#include <rdtty.h>

class ViewAdapters : public RDDialog
{
 Q_OBJECT
 public:
  ViewAdapters(RDStation *station,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void closeData();

 protected:
  void resizeEvent (QResizeEvent *e);

 private:
  QLabel *view_title_label;
  QTextEdit *view_text_edit;
  QPushButton *view_close_button;
};


#endif  // VIEW_ADAPTER_H
