// edit_now_next.h
//
// Edit the Now & Next Configuration for a Rivendell Workstation
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

#include <rdairplay_conf.h>
#include <rdsqltablemodel.h>
#include <rdtableview.h>

class EditNowNext : public QDialog
{
  Q_OBJECT
  public:
   EditNowNext(RDAirPlayConf *conf,QWidget *parent=0);
   ~EditNowNext();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void addPluginData();
   void editPluginData();
   void deletePluginData();
   void pluginDoubleClickedData(const QModelIndex &index);
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
   RDSqlTableModel *nownext_model;
   RDTableView *nownext_view;
   QPushButton *nownext_add_button;
   QPushButton *nownext_edit_button;
   QPushButton *nownext_delete_button;
   RDAirPlayConf *nownext_conf;
};


#endif

