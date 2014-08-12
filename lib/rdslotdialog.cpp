// rdslotdialog.cpp
//
// Slot Editor for RDCartSlots.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdslotdialog.cpp,v 1.3.2.4 2012/11/28 01:57:38 cvs Exp $
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

#include <qcolordialog.h>

#include <rddb.h>
#include <rdcart.h>
#include <rdcart_dialog.h>
#include <rd.h>
#include <rdconf.h>

#include "rdslotdialog.h"

RDSlotDialog::RDSlotDialog(const QString &caption,QWidget *parent)
  : QDialog(parent)
{
  edit_caption=caption;
  edit_options=NULL;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(caption+" - "+tr("Edit Slot Options"));

  //
  // Create Fonts
  //
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);
  QFont counter_font=QFont("Helvetica",24,QFont::Bold);
  counter_font.setPixelSize(24);

  //
  // Slot Mode
  //
  edit_mode_box=new QComboBox(this);
  connect(edit_mode_box,SIGNAL(activated(int)),
	  this,SLOT(modeActivatedData(int)));
  for(int i=0;i<RDSlotOptions::LastMode;i++) {
    edit_mode_box->insertItem(RDSlotOptions::modeText((RDSlotOptions::Mode)i));
  }
  edit_mode_label=new QLabel(tr("Slot Mode:"),this);
  edit_mode_label->setFont(label_font);
  edit_mode_label->setAlignment(Qt::AlignRight);

  //
  // Hook Mode
  //
  edit_hook_box=new QComboBox(this);
  edit_hook_box->insertItem(tr("Full Cart"));
  edit_hook_box->insertItem(tr("Hook"));
  edit_hook_label=new QLabel(tr("Play Mode:"),this);
  edit_hook_label->setFont(label_font);
  edit_hook_label->setAlignment(Qt::AlignRight);

  //
  // Stop Action
  //
  edit_stop_action_box=new QComboBox(this);
  for(int i=0;i<RDSlotOptions::LastStop;i++) {
    edit_stop_action_box->
      insertItem(RDSlotOptions::stopActionText((RDSlotOptions::StopAction)i));
  }
  edit_stop_action_label=new QLabel(tr("At Playout End:"),this);
  edit_stop_action_label->setFont(label_font);
  edit_stop_action_label->setAlignment(Qt::AlignRight);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(label_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(label_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDSlotDialog::~RDSlotDialog()
{
}


QSize RDSlotDialog::sizeHint() const
{
  return QSize(350,142);
} 


QSizePolicy RDSlotDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDSlotDialog::exec(RDSlotOptions *opt)
{
  edit_options=opt;
  edit_mode_box->setCurrentItem(edit_options->mode());
  edit_hook_box->setCurrentItem(edit_options->hookMode());
  edit_stop_action_box->setCurrentItem(edit_options->stopAction());
  modeActivatedData(edit_mode_box->currentItem());
  return QDialog::exec();
}


void RDSlotDialog::modeActivatedData(int index)
{
  RDSlotOptions::Mode mode=(RDSlotOptions::Mode)index;
  edit_hook_label->setEnabled(mode==RDSlotOptions::CartDeckMode);
  edit_hook_box->setEnabled(mode==RDSlotOptions::CartDeckMode);
  edit_stop_action_label->setEnabled(mode==RDSlotOptions::CartDeckMode);
  edit_stop_action_box->setEnabled(mode==RDSlotOptions::CartDeckMode);
}


void RDSlotDialog::okData()
{
  edit_options->setMode((RDSlotOptions::Mode)edit_mode_box->currentItem());
  edit_options->setHookMode(edit_hook_box->currentItem());
  edit_options->setStopAction((RDSlotOptions::StopAction)edit_stop_action_box->
			      currentItem());
  done(0);
}


void RDSlotDialog::cancelData()
{
  done(-1);
}


void RDSlotDialog::resizeEvent(QResizeEvent *e)
{
  edit_mode_box->setGeometry(130,10,150,20);
  edit_mode_label->setGeometry(10,10,115,20);
  edit_hook_box->setGeometry(130,32,150,20);
  edit_hook_label->setGeometry(10,32,115,20);
  edit_stop_action_box->setGeometry(130,54,size().width()-140,20);
  edit_stop_action_label->setGeometry(10,54,115,20);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
