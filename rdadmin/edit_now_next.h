// edit_now_next.h
//
// Edit the Now & Next Configuration for a Rivendell Workstation
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_now_next.h,v 1.7 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_NOW_NEXT_H
#define EDIT_NOW_NEXT_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdlistview.h>
#include <rdairplay_conf.h>
#include <rdripc.h>


class EditNowNext : public QDialog
{
  Q_OBJECT
  public:
   EditNowNext(RDAirPlayConf *conf,QWidget *parent=0,const char *name=0);
   ~EditNowNext();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void addPluginData();
   void editPluginData();
   void deletePluginData();
   void pluginDoubleClickedData(QListViewItem *item,const QPoint &pt,int col);
   void editNowcartData(int lognum);
   void editNextcartData(int lognum);
   void okData();
   void cancelData();

  private:
   QLineEdit *nownext_address_edit[3];
   QSpinBox *nownext_port_spin[3];
   QLineEdit *nownext_string_edit[3];
   QLineEdit *nownext_rml_edit[3];
   QLineEdit *nownext_nowcart_edit[3];
   QLineEdit *nownext_nextcart_edit[3];
   RDListView *nownext_plugin_list;
   QPushButton *nownext_add_button;
   QPushButton *nownext_edit_button;
   QPushButton *nownext_delete_button;
   RDAirPlayConf *nownext_conf;
};


#endif

