// edit_nownextplugin.cpp
//
// Edit a Rivendell Now & Next Plugin Configuration
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_nownextplugin.cpp,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>

#include <rd.h>
#include <rdpaths.h>

#include <edit_nownextplugin.h>


EditNowNextPlugin::EditNowNextPlugin(QString *path,QString *arg,
				     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  plugin_path=path;
  plugin_arg=arg;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Edit Plugin"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont user_font=QFont("Helvetica",12,QFont::Normal);
  user_font.setPixelSize(12);

  //
  // Path
  //
  plugin_path_edit=new QLineEdit(this);
  plugin_path_edit->setGeometry(110,11,sizeHint().width()-180,19);
  plugin_path_edit->setMaxLength(255);
  QLabel *label=new QLabel(plugin_path_edit,tr("Plugin Path:"),this);
  label->setGeometry(10,11,95,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(tr("Select"),this);
  button->setGeometry(sizeHint().width()-60,10,50,22);
  button->setFont(user_font);
  connect(button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Argument
  //
  plugin_arg_edit=new QLineEdit(this);
  plugin_arg_edit->setGeometry(110,38,sizeHint().width()-120,19);
  plugin_arg_edit->setMaxLength(255);
  label=new QLabel(plugin_arg_edit,tr("Argument:"),this);
  label->setGeometry(10,38,95,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  plugin_path_edit->setText(*path);
  plugin_arg_edit->setText(*arg);
}


EditNowNextPlugin::~EditNowNextPlugin()
{
  delete plugin_path_edit;
  delete plugin_arg_edit;
}


QSize EditNowNextPlugin::sizeHint() const
{
  return QSize(400,130);
} 


QSizePolicy EditNowNextPlugin::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditNowNextPlugin::selectData()
{
  QString filename=RD_MODULES_DIR;
  if(!plugin_path_edit->text().isEmpty()) {
    filename=plugin_path_edit->text();
  }
  filename=QFileDialog::getOpenFileName(filename,RD_MODULE_FILE_FILTER,
					this,"",tr("Select plugin"));
  if(!filename.isNull()) {
    plugin_path_edit->setText(filename);
  }
}


void EditNowNextPlugin::okData()
{
  *plugin_path=plugin_path_edit->text();
  *plugin_arg=plugin_arg_edit->text();
  done(0);
}


void EditNowNextPlugin::cancelData()
{
  done(-1);
}
