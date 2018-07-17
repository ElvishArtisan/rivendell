// view_adapters.cpp
//
// Display Audio Adapter Information
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qtextedit.h>
#include <qlabel.h>

#include <rd.h>
#include <rdencoderlist.h>

#include <view_adapters.h>


ViewAdapters::ViewAdapters(RDStation *rdstation,QWidget *parent)
  : QDialog(parent,"",true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption("RDADmin - "+tr("Audio Resource Information"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",16,QFont::Bold);
  font.setPixelSize(16);
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);

  //
  // Title
  //
  QLabel *label=new QLabel(tr("Audio Resources on")+" "+rdstation->name(),this);
  label->setGeometry(15,10,sizeHint().width()-20,16);
  label->setFont(font);

  //
  // Resource List
  //
  QTextEdit *text_edit=new QTextEdit(this,"adapter_edit");
  text_edit->setGeometry(10,28,sizeHint().width()-20,sizeHint().height()-98);
  text_edit->setReadOnly(true);
  QString text;
  if(rdstation->scanned()) {
    text+=tr("SUPPORTED AUDIO DRIVERS\n");
    if(!rdstation->driverVersion(RDStation::Hpi).isEmpty()) {
      text+=QString("  AudioScience HPI [")+
	rdstation->driverVersion(RDStation::Hpi)+"]\n";
    }
    if(!rdstation->driverVersion(RDStation::Jack).isEmpty()) {
      text+=QString("  JACK Audio Connection Kit [")+
	rdstation->driverVersion(RDStation::Jack)+"]\n";
    }
    if(!rdstation->driverVersion(RDStation::Alsa).isEmpty()) {
      text+=QString("  Advanced Linux Sound Architecture (ALSA) [")+
	rdstation->driverVersion(RDStation::Alsa)+"]\n";
    }
    text+="\n";
    
    text+=tr("SUPPORTED IMPORT FORMATS\n");
    if(rdstation->haveCapability(RDStation::HaveFlac)) {
      text+=tr("    Free Lossless Audio Codec (FLAC)\n");
    }
    if(rdstation->haveCapability(RDStation::HaveMpg321)) {
      text+=tr("    MPEG Layer 1\n");
      text+=tr("    MPEG Layer 2\n");
      text+=tr("    MPEG Layer 3\n");
    }
    if(rdstation->haveCapability(RDStation::HaveMp4Decode)) {
      text+=tr("    MP-4/AAC\n");
    }
    if(rdstation->haveCapability(RDStation::HaveOgg123)) {
      text+=tr("    OggVorbis\n");
    }
    text+=tr("    PCM16 Linear\n");
    text+=tr("    PCM24 Linear\n");
    text+="\n";
    text+=tr("SUPPORTED EXPORT FORMATS\n");
    if(rdstation->haveCapability(RDStation::HaveFlac)) {
      text+=tr("    Free Lossless Audio Codec (FLAC)\n");
    }
    if(rdstation->haveCapability(RDStation::HaveTwoLame)) {
      text+=tr("    MPEG Layer 2\n");
    }
    if(rdstation->haveCapability(RDStation::HaveLame)) {
      text+=tr("    MPEG Layer 3\n");
    }
    if(rdstation->haveCapability(RDStation::HaveOggenc)) {
      text+=tr("    OggVorbis\n");
    }
    text+=tr("    PCM16 Linear\n");
    text+=tr("    PCM24 Linear\n");
    RDEncoderList *encoders=new RDEncoderList(rdstation->name());
    for(unsigned i=0;i<encoders->encoderQuantity();i++) {
      text+="    "+encoders->encoder(i)->name()+" [Custom]\n";
    }
    delete encoders;

    text+="\n";
    
    text+=tr("AUDIO ADAPTERS\n");
    for(int i=0;i<RD_MAX_CARDS;i++) {
      if(rdstation->cardName(i).isEmpty()) {
	text+=QString("  ")+tr("Card")+QString().sprintf(" %d: ",i)+
	  tr("Not present")+"\n";
      }
      else {
	text+=QString("  ")+tr("Card")+QString().sprintf(" %d: ",i)+
	  rdstation->cardName(i)+"\n";
	switch(rdstation->cardDriver(i)) {
	case RDStation::Hpi:
	  text+=tr("      Driver: AudioScience HPI\n");
	  break;
	      
	case RDStation::Jack:
	  text+=tr("      Driver: JACK Audio Connection Kit\n");
	  break;
	      
	case RDStation::Alsa:
	  text+=tr("      Driver: Advanced Linux Sound Architecture (ALSA)\n");
	break;
	      
	case RDStation::None:
	  text+=tr("      Driver: UNKNOWN\n");
	  break;
	}
	text+=QString("      ")+tr("Inputs")+
	  QString().sprintf(" %d\n",rdstation->cardInputs(i));
	text+=QString("      ")+tr("Outputs")+
	  QString().sprintf(" %d\n",rdstation->cardOutputs(i));
      }
      text+="\n";
    }
  }
  else {
    text=tr("NO DATA AVAILABLE\n\n");
    text+=tr("Please start the Rivendell daemons on this host (by executing, as user 'root', the command \"/etc/init.d/rivendell start\") in order to populate the audio resources database.");
  }
  text_edit->setText(text);

  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  button->setFont(button_font);
  button->setText(tr("&Close"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ViewAdapters::sizeHint() const
{
  return QSize(460,290);
} 


QSizePolicy ViewAdapters::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ViewAdapters::closeData()
{
  done(0);
}
