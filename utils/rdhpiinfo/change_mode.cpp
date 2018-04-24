// change_mode.cpp
//
// Change the mode of an AudioScience Adapter.
//
//   (C) Copyright 2002-2014,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <change_mode.h>

ChangeMode::ChangeMode(unsigned short card,unsigned short type,int mode,
		       QWidget *parent)
  : QDialog(parent,"",true)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("RdHPIInfo - Change Mode"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Adapter Type
  //
  QLabel *label=new QLabel(QString().sprintf("AudioScience %X",type),this);
  label->setGeometry(10,10,sizeHint().width()-20,15);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignCenter);

  //
  // Adapter Mode
  //
  change_mode_box=new QComboBox(this);
  change_mode_box->setGeometry(10,35,sizeHint().width()-20,22);
  change_mode_box->setFont(font);
  int index=0;
  for(uint32_t i=1;i<14;i++) {
    if(HPI_AdapterSetModeEx(NULL,card,i,HPI_ADAPTER_MODE_QUERY)==0) {
      switch(i) {
	  case HPI_ADAPTER_MODE_4OSTREAM:
	    change_mode_box->insertItem(tr("Four Output Streams"));
	    break;
	    
	  case HPI_ADAPTER_MODE_6OSTREAM:
	    change_mode_box->insertItem(tr("Six Output Streams"));
	    break;
	    
	  case HPI_ADAPTER_MODE_8OSTREAM:
	    change_mode_box->insertItem(tr("Eight Output Streams"));
	    break;
	    
	  case HPI_ADAPTER_MODE_12OSTREAM:
	    change_mode_box->insertItem(tr("Twelve Output Streams"));
	    break;
	    
	  case HPI_ADAPTER_MODE_16OSTREAM:
	    change_mode_box->insertItem(tr("Sixteen Output Streams"));
	    break;
	    
	  case HPI_ADAPTER_MODE_1OSTREAM:
	    change_mode_box->insertItem(tr("One Output Stream"));
	    break;
	    
	  case HPI_ADAPTER_MODE_1:
	    change_mode_box->insertItem(tr("Mode 1"));
	    break;
	    
	  case HPI_ADAPTER_MODE_2:
	    change_mode_box->insertItem(tr("Mode 2"));
	    break;
	    
	  case HPI_ADAPTER_MODE_3:
	    change_mode_box->insertItem(tr("Mode 3"));
	    break;
	    
	  case HPI_ADAPTER_MODE_MULTICHANNEL:
	    change_mode_box->insertItem(tr("Surround Sound [SSX]"));
	    break;
	    
	  case HPI_ADAPTER_MODE_9OSTREAM:
	    change_mode_box->insertItem(tr("Nine Output Stream"));
	    break;
	    
	  case HPI_ADAPTER_MODE_MONO:
	    change_mode_box->insertItem(tr("Mono Mode"));
	    break;
	    
	  case HPI_ADAPTER_MODE_LOW_LATENCY:
	    change_mode_box->insertItem(tr("Low Latency Mode"));
	    break;
	    
	  default:
	    str=QString(tr("Unknown"));
	    change_mode_box->
	      insertItem(QString().sprintf("  %s [MODE=%u]\n",
					   (const char *)str,(unsigned)i));
	    break;
      }
      hpi_mode_map[index]=i;
      if(mode==(int)i) {
	change_mode_box->setCurrentItem(index);
	change_index=index;
      }
      index++;
    }
  }

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-140,sizeHint().height()-40,60,30);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-70,sizeHint().height()-40,
			     60,30);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize ChangeMode::sizeHint() const
{
  return QSize(280,110);
} 


QSizePolicy ChangeMode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ChangeMode::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void ChangeMode::okData()
{
  if(change_mode_box->currentItem()==change_index) {
    done(-1);
    return;
  }
  if(QMessageBox::warning(this,"RDHPIInfo",tr("The HPI driver will need to be restarted\nafter changing the mode.  Continue?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
    return;
  }
  done(hpi_mode_map[change_mode_box->currentItem()]);
  return;
}


void ChangeMode::cancelData()
{
  done(-1);
}
