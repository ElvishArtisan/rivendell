// edit_playout.cpp
//
// Edit a Rivendell RDCatch Playout
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

#include <QGroupBox>

#include <rdcut_path.h>
#include <rdtextvalidator.h>

#include "edit_playout.h"
#include "globals.h"

EditPlayout::EditPlayout(int id,std::vector<int> *adds,QString *filter,
			 QWidget *parent)
  : RDDialog(parent)
{
  QString temp;

  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setWindowTitle("RDCatch - "+tr("Edit Playout"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);

  //
  // Dialogs
  //
  edit_cut_dialog=new RDCutDialog(edit_filter,NULL,NULL,false,false,false,
				  "RDCatch",false,this);

  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::PlayEvent,this);
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());
  connect(edit_event_widget,SIGNAL(locationChanged(const QString &,int)),
	  this,SLOT(locationChangedData(const QString &,int)));

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(105,43,sizeHint().width()-115,20);
  edit_description_edit->setValidator(validator);
  QLabel *label=new QLabel(tr("Description:"),this);
  label->setGeometry(10,43,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this);
  edit_destination_edit->setGeometry(105,70,sizeHint().width()-185,20);
  edit_destination_edit->setReadOnly(true);
  label=new QLabel(tr("Destination:"),this);
  label->setGeometry(10,70,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-70,65,60,30);
  button->setFont(subButtonFont());
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCutData()));

  //
  // DOW Selector
  //
  edit_dow_selector=new DowSelector(this);
  edit_dow_selector->setGeometry(10,104,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this);
  edit_oneshot_box->setGeometry(20,180,15,15);
  label=new QLabel(tr("Make OneShot"),this);
  label->setGeometry(40,178,115,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  //  Save As Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-300,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Save As\nNew"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    button->hide();
  }

  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  edit_cutname=edit_recording->cutName();
  edit_destination_edit->setText(RDCutPath(edit_cutname));
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  locationChangedData(edit_event_widget->stationName(),
		      edit_event_widget->deckNumber());
}


EditPlayout::~EditPlayout()
{
  delete edit_cut_dialog;
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditPlayout::sizeHint() const
{
  return QSize(540,245);
} 


QSizePolicy EditPlayout::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditPlayout::locationChangedData(const QString &station,int decknum)
{
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
  edit_deck=new RDDeck(station,decknum);
}


void EditPlayout::selectCutData()
{
  if(edit_cut_dialog->exec(&edit_cutname)) {
    edit_destination_edit->setText(edit_cutname);
    edit_description_edit->setText(RDCutPath(edit_cutname));
  }
}


void EditPlayout::saveasData()
{
  delete edit_recording;
  edit_recording=new RDRecording(-1,true);
  edit_added_events->push_back(edit_recording->id());
  Save();
}


void EditPlayout::okData()
{
  Save();
  done(true);
}


void EditPlayout::cancelData()
{
  done(false);
}


void EditPlayout::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
  case Qt::Key_Escape:
    e->accept();
    cancelData();
    break;

  default:
    QDialog::keyPressEvent(e);
    break;
  }
}


void EditPlayout::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditPlayout::Save()
{
  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setType(RDRecording::Playout);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_cutname);
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}
