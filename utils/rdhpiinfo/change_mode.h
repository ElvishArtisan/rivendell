// change_mode.h
//
// Change the mode of an AudioScience Adapter
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: change_mode.h,v 1.5 2011/05/18 14:38:14 cvs Exp $
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

#ifndef CHANGE_MODE_H
#define CHANGE_MODE_H

#define MAX_HPI_MODES 11

#include <qdialog.h>
#include <qcombobox.h>

#include <asihpi/hpi.h>
#ifndef HPI_ADAPTER_MODE_MULTICHANNEL
#define HPI_ADAPTER_MODE_MULTICHANNEL (9)
#endif


class ChangeMode : public QDialog
{
 Q_OBJECT
 public:
  ChangeMode(unsigned short card,unsigned short type,int mode,
	     QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void closeEvent(QCloseEvent *e);

 private slots:
  void okData();
  void cancelData();

 private:
  int change_index;
  uint32_t hpi_mode_map[MAX_HPI_MODES];
  QComboBox *change_mode_box;
};


#endif

