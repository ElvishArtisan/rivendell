// edit_nownextplugin.h
//
// Edit a Rivendell Now & Next Plugin Configuration
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_nownextplugin.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_NOWNEXTPLUGIN_H
#define EDIT_NOWNEXTPLUGIN_H

#include <qdialog.h>
#include <qlineedit.h>


class EditNowNextPlugin : public QDialog
{
  Q_OBJECT
 public:
  EditNowNextPlugin(QString *path,QString *arg,
		    QWidget *parent=0,const char *name=0);
  ~EditNowNextPlugin();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void selectData();
  void okData();
  void cancelData();

 private:
  QLineEdit *plugin_path_edit;
  QLineEdit *plugin_arg_edit;
  QString *plugin_path;
  QString *plugin_arg;
};


#endif  // EDIT_NOWNEXTPLUGIN_H
