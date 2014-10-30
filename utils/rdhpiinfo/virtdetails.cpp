// virtdetails.cpp
//
// Show profile details for an AudioScience adapter.
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include "virtdetails.h"

VirtDetails::VirtDetails(uint16_t card,hpi_handle_t profile,
			 uint16_t profile_quan,QWidget *parent)
  : QDialog(parent,"",true)
{
  char name[200];

  virt_card=card;
  virt_profile=profile;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("RdHPIInfo - Profile Details"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Adapter Mode
  //
  virt_profile_box=new QComboBox(this);
  virt_profile_box->setGeometry(75,10,sizeHint().width()-85,20);
  virt_profile_box->setFont(font);
  virt_profile_label=new QLabel(virt_profile_box,tr("Profile")+":",this);
  virt_profile_label->setGeometry(10,10,60,20);
  virt_profile_label->setFont(label_font);
  virt_profile_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  for(uint16_t i=0;i<profile_quan;i++) {
    if(HPI_ProfileGetName(NULL,profile,i,name,200)==0) {
      if(QString(name).lower()!="unassigned") {
	virt_profile_box->insertItem(name);
      }
    }
  }

  //
  // Utilization Counter
  //
  virt_utilization_label=new QLabel(tr("Overall DSP Utilization")+":",this);
  virt_utilization_label->setGeometry(10,35,160,20);
  virt_utilization_label->setFont(label_font);
  virt_utilization_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_utilization_edit=new QLineEdit(this);
  virt_utilization_edit->setGeometry(175,35,90,20);
  virt_utilization_edit->setFont(font);
  virt_utilization_edit->setReadOnly(true);

  //
  // Profile Interval Counter
  //
  virt_interval_label=new QLabel(tr("Profile Interval")+":",this);
  virt_interval_label->setGeometry(10,57,160,20);
  virt_interval_label->setFont(label_font);
  virt_interval_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_interval_edit=new QLineEdit(this);
  virt_interval_edit->setGeometry(175,57,90,20);
  virt_interval_edit->setFont(font);
  virt_interval_edit->setReadOnly(true);

  //
  // Total Tick Count Counter
  //
  virt_total_ticks_label=new QLabel(tr("Total Tick Count")+":",this);
  virt_total_ticks_label->setGeometry(10,79,160,20);
  virt_total_ticks_label->setFont(label_font);
  virt_total_ticks_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_total_ticks_edit=new QLineEdit(this);
  virt_total_ticks_edit->setGeometry(175,79,90,20);
  virt_total_ticks_edit->setFont(font);
  virt_total_ticks_edit->setReadOnly(true);

  //
  // Call Count Counter
  //
  virt_call_count_label=new QLabel(tr("Call Count")+":",this);
  virt_call_count_label->setGeometry(10,101,160,20);
  virt_call_count_label->setFont(label_font);
  virt_call_count_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_call_count_edit=new QLineEdit(this);
  virt_call_count_edit->setGeometry(175,101,90,20);
  virt_call_count_edit->setFont(font);
  virt_call_count_edit->setReadOnly(true);

  //
  // Maximum Ticks Counter
  //
  virt_max_ticks_label=new QLabel(tr("Maximum Ticks / Pass")+":",this);
  virt_max_ticks_label->setGeometry(10,123,160,20);
  virt_max_ticks_label->setFont(label_font);
  virt_max_ticks_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_max_ticks_edit=new QLineEdit(this);
  virt_max_ticks_edit->setGeometry(175,123,90,20);
  virt_max_ticks_edit->setFont(font);
  virt_max_ticks_edit->setReadOnly(true);

  //
  // Ticks per Millisecond Counter
  //
  virt_ticks_per_ms_label=new QLabel(tr("Ticks / mS")+":",this);
  virt_ticks_per_ms_label->setGeometry(10,145,160,20);
  virt_ticks_per_ms_label->setFont(label_font);
  virt_ticks_per_ms_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  virt_ticks_per_ms_edit=new QLineEdit(this);
  virt_ticks_per_ms_edit->setGeometry(175,145,90,20);
  virt_ticks_per_ms_edit->setFont(font);
  virt_ticks_per_ms_edit->setReadOnly(true);

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
  close_button->setGeometry(sizeHint().width()-70,sizeHint().height()-40,
			     60,30);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(updateProfileData()));
  timer->start(1000);
}


QSize VirtDetails::sizeHint() const
{
  return QSize(280,222);
} 


QSizePolicy VirtDetails::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void VirtDetails::updateProfileData()
{
  uint32_t utilization;
  uint16_t interval;
  uint32_t total_ticks;
  uint32_t call_count;
  uint32_t max_ticks;
  uint32_t ticks_per_ms;

  if(HPI_ProfileGetUtilization(NULL,virt_profile,&utilization)==0) {
    virt_utilization_edit->
      setText(QString().sprintf("%5.1lf%%",(double)utilization/100.0));
  }
  if(HPI_ProfileGet(NULL,virt_profile,virt_profile_box->currentItem(),
		    &interval,&total_ticks,&call_count,&max_ticks,
		    &ticks_per_ms)==0) {
    virt_interval_edit->setText(QString().sprintf("%u sec",interval));
    virt_total_ticks_edit->setText(QString().sprintf("%u",total_ticks));
    virt_call_count_edit->setText(QString().sprintf("%u",call_count));
    virt_max_ticks_edit->setText(QString().sprintf("%u",max_ticks));
    virt_ticks_per_ms_edit->setText(QString().sprintf("%u",ticks_per_ms));
  }
}


void VirtDetails::closeData()
{
  done(0);
}
