// rdchunk.cpp
//
// A Qt-based application for playing Microsoft WAV files. 
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdchunk.cpp,v 1.3.8.3 2014/01/21 21:59:33 cvs Exp $
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


#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <math.h>

#include <rd.h>
#include <rdconf.h>

#include <rdchunk.h>


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  y_chunk_button=40;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont font("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  wave_loaded=false;
  wavefile=NULL;
  fmt_button=NULL;
  data_button=NULL;
  fact_button=NULL;
  cart_button=NULL;
  bext_button=NULL;
  mext_button=NULL;
  wave_base="RDChunk";
  setCaption(wave_base);

  //
  // Load Button
  //
  QPushButton *button=new QPushButton(tr("Load"),this,"load_button");
  button->setGeometry(10,10,sizeHint().width()-20,30);
  connect(button,SIGNAL(clicked()),this,SLOT(loadWaveFile()));

  //
  // Chunk Label
  //
  wave_chunk_label=new QLabel(tr("Chunks"),this,"wave_chunk_label");
  wave_chunk_label->setGeometry(25,55,sizeHint().width()-50,20);
  wave_chunk_label->setAlignment(AlignCenter);
  wave_chunk_label->setFont(label_font);
  wave_chunk_label->hide();
}


QSize MainWidget::sizeHint() const
{
  return QSize(120,y_chunk_button+10);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::createChunkButtons()
{
  int xptr=20;
  int yptr=78;

  //
  // Create chunk buttons
  //
  if(wavefile->getFormatChunk()) {
    fmt_button=new QPushButton(this,"fmt");
    fmt_button->setText("Format");
    fmt_button->setGeometry(xptr,yptr,80,25);
    fmt_button->show();
    yptr+=30;
    connect(fmt_button,SIGNAL(clicked()),this,SLOT(displayFmt()));
  }
  if(wavefile->getDataChunk()) {
    data_button=new QPushButton(this,"data");
    data_button->setText("Data");
    data_button->setGeometry(xptr,yptr,80,25);
    data_button->show();
    yptr+=30;
    connect(data_button,SIGNAL(clicked()),this,SLOT(displayData()));
  }
  if(wavefile->getFactChunk()) {
    fact_button=new QPushButton(this,"fact");
    fact_button->setText("Fact");
    fact_button->setGeometry(xptr,yptr,80,25);
    fact_button->show();
    yptr+=30;
    connect(fact_button,SIGNAL(clicked()),this,SLOT(displayFact()));
  }
  if(wavefile->getCartChunk()) {
    cart_button=new QPushButton(this,"cart");
    cart_button->setText("Cart");
    cart_button->setGeometry(xptr,yptr,80,25); 
    cart_button->show();
    yptr+=30;
    connect(cart_button,SIGNAL(clicked()),this,SLOT(displayCart()));
  }
  if(wavefile->getBextChunk()) {
    bext_button=new QPushButton(this,"bext");
    bext_button->setText("Bext");
    bext_button->setGeometry(xptr,yptr,80,25);
    bext_button->show();
    yptr+=30;
    connect(bext_button,SIGNAL(clicked()),this,SLOT(displayBext()));
  }
  if(wavefile->getMextChunk()) {
    mext_button=new QPushButton(this,"mext");
    mext_button->setText("Mext");
    mext_button->setGeometry(xptr,yptr,80,25);
    mext_button->show();
    yptr+=30;
    connect(mext_button,SIGNAL(clicked()),this,SLOT(displayMext()));
  }
  if(wavefile->getLevlChunk()) {
    levl_button=new QPushButton(this,"levl");
    levl_button->setText("Levl");
    levl_button->setGeometry(xptr,yptr,80,25);
    levl_button->show();
    yptr+=30;
    connect(levl_button,SIGNAL(clicked()),this,SLOT(displayLevl()));
  }
  if(wavefile->getAIR1Chunk()) {
    AIR1_button=new QPushButton(this,"AIR1");
    AIR1_button->setText("AIR1");
    AIR1_button->setGeometry(xptr,yptr,80,25);
    AIR1_button->show();
    yptr+=30;
    connect(AIR1_button,SIGNAL(clicked()),this,SLOT(displayAIR1()));
  }
  y_chunk_button=yptr;
  wave_chunk_label->show();
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
  setGeometry(geometry().x(),geometry().y(),sizeHint().width(),
	      sizeHint().height());
}


void MainWidget::destroyChunkButtons()
{
  if(fmt_button!=NULL) {
    fmt_button->hide();
    delete fmt_button;
    fmt_button=NULL;
  }

  if(data_button!=NULL) {
    data_button->hide();
    delete data_button;
    data_button=NULL;
  }

  if(fact_button!=NULL) {
    fact_button->hide();
    delete fact_button;
    fact_button=NULL;
  }

  if(cart_button!=NULL) {
    cart_button->hide();
    delete cart_button;
    cart_button=NULL; 
  }

  if(bext_button!=NULL) {
    bext_button->hide();
    delete bext_button;
    bext_button=NULL;
  }

  if(mext_button!=NULL) {
    mext_button->hide();
    delete mext_button;
    mext_button=NULL;
  }
  wave_chunk_label->hide();
  y_chunk_button=40;
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
  setGeometry(geometry().x(),geometry().y(),sizeHint().width(),
	      sizeHint().height());
}


void MainWidget::loadWaveFile()
{
  QString str;

  delete wavefile;
  wavefile=NULL;
  destroyChunkButtons();
  wave_name=QFileDialog::getOpenFileName(wave_path,RD_AUDIO_FILE_FILTER,this);
  if(wave_name.isEmpty()) {
    wave_loaded=false;
    wave_path=getenv("HOME");
    wave_base=tr("RHPIPlay");
    update();
    return;
  }
  wavefile=new RDWaveFile(wave_name);
  if(wavefile->openWave()) {
    wave_loaded=true;
    wave_path=RDGetPathPart(wave_name);
    wave_base="RHPIPlay - ";
    if(wavefile->getCartChunk()) {
      wave_base+=wavefile->getCartTitle();
    }
    else {
      wave_base+=RDGetBasePart(wave_name);
    }
    createChunkButtons();
    update();
  }
  else {
    QMessageBox::warning(this,tr("RDChunk"),tr("Unable to open file!"));
    delete wavefile;
    wavefile=NULL;
  }
}


void MainWidget::quitMainWidget()
{
  qApp->quit();
}


void MainWidget::paintEvent(QPaintEvent *paintevent)
{
  QPainter *p=new QPainter(this);
  p->setFont(QFont("arial",12,QFont::Bold));

  if(wave_loaded) {
    p->moveTo(10,65);
    p->lineTo(sizeHint().width()-10,65);
    p->lineTo(sizeHint().width()-10,y_chunk_button);
    p->lineTo(10,y_chunk_button);
    p->lineTo(10,65);
  }    
}


void MainWidget::displayFmt()
{
  char string[256];
  QString output;
  QString str1;
  QString str2;

  //
  // Basic Data, common to all valid WAV files
  //
  switch(wavefile->getFormatTag()) {
  case WAVE_FORMAT_PCM:
    str1=QString(tr("Format:  PCM"));
    str2=QString(tr("Linear"));
    sprintf(string,"%s%d %s\n",(const char *)str1,
	    wavefile->getBitsPerSample(),(const char *)str2);
    output=output.append(string);
    break;
  case WAVE_FORMAT_MPEG:
    str1=QString(tr("Format:  MPEG Layer"));
    sprintf(string,"%s %d\n",(const char *)str1,wavefile->getHeadLayer());
    output=output.append(string);
    break;
  case WAVE_FORMAT_VORBIS:
    str1=QString(tr("Format:  OggVorbis"));
    sprintf(string,"%s\n",(const char *)str1);
    output=output.append(string);
    break;
  default:
    str1=QString(tr("Format:  Unknown"));
    sprintf(string,"Format:  %s\n",(const char *)str1);
    output=output.append(string);
    break;
  }

  str1=QString(tr("Channels:"));
  sprintf(string,"%s %d\n",(const char *)str1,wavefile->getChannels());
  output=output.append(string);

  str1=QString(tr("Sample Rate:"));
  str2=QString(tr("samples/sec"));
  sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getSamplesPerSec(),
	  (const char *)str2);
  output=output.append(string);

  str1=QString(tr("Average Data Rate:"));
  str2=QString(tr("bytes/sec"));
  sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getAvgBytesPerSec(),
	  (const char *)str2);
  output=output.append(string);

  str1=QString(tr("Frame Size:"));
  str2=QString(tr("byte3s"));
  sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getBlockAlign(),
	  (const char *)str2);
  output=output.append(string);

  //
  // PCM Linear specific data
  //
  if(wavefile->getFormatTag()==WAVE_FORMAT_PCM) {
    str1=QString(tr("Sample Size:"));
    str2=QString(tr("bits/chan/sample"));
    sprintf(string,"%s %d %s\n",(const char *)str1,
	    wavefile->getBitsPerSample(),
	    (const char *)str2);
    output=output.append(string);
  }

  //
  // MPEG-1 specific data
  //
  if(wavefile->getFormatTag()==WAVE_FORMAT_MPEG) {
    str1=QString(tr("Bit Rate:"));
    str2=QString(tr("bits/sec"));
    sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getHeadBitRate(),
	    (const char *)str2);
    output=output.append(string);
    sprintf(string,tr("Codec Mode(s): "));
    if((wavefile->getHeadMode()&ACM_MPEG_STEREO)!=0) {
      strcat(string,tr("Stereo "));
    }
    if((wavefile->getHeadMode()&ACM_MPEG_JOINTSTEREO)!=0) {
      strcat(string,tr("JointStereo "));
    }
    if((wavefile->getHeadMode()&ACM_MPEG_DUALCHANNEL)!=0) {
      strcat(string,tr("DualChannel "));
    }
    if((wavefile->getHeadMode()&ACM_MPEG_SINGLECHANNEL)!=0) {
      strcat(string,tr("SingleChannel "));
    }
    strcat(string,"\n");
    output=output.append(string);

    switch(wavefile->getHeadEmphasis()) {
    case 1:
      str1=QString(tr("None"));
      sprintf(string,"Emphasis: %s\n",(const char *)str1);
      output=output.append(string);
      break;
    case 2:
      str1=QString(tr("50/15 ms"));
      sprintf(string,"Emphasis: %s\n",(const char *)str1);
      output=output.append(string);
      break;
    case 3:
      str1=QString(tr("Reserved"));
      sprintf(string,"Emphasis: %s\n",(const char *)str1);
      output=output.append(string);
      break;
    case 4:
      str1=QString(tr("CCITT J.17"));
      sprintf(string,"Emphasis: %s\n",(const char *)str1);
      output=output.append(string);
      break;
    default:
      str1=QString(tr("Unknown"));
      sprintf(string,"Emphasis: %s\n",(const char *)str1);
      output=output.append(string);
      break;
    }
    sprintf(string,tr("Flags: "));
    if((wavefile->getHeadFlags()&ACM_MPEG_PRIVATEBIT)!=0) {
      strcat(string,tr("Private "));
    }
    if((wavefile->getHeadFlags()&ACM_MPEG_COPYRIGHT)!=0) {
      strcat(string,tr("Copyright "));
    }
    if((wavefile->getHeadFlags()&ACM_MPEG_ORIGINALHOME)!=0) {
      strcat(string,tr("Home "));
    }
    if((wavefile->getHeadFlags()&ACM_MPEG_PROTECTIONBIT)!=0) {
      strcat(string,"Protect ");
    }
    if((wavefile->getHeadFlags()&ACM_MPEG_ID_MPEG1)!=0) {
      strcat(string,tr("MPEG "));
    }
    strcat(string,"\n");
    output=output.append(string);
  }
  QMessageBox::information(this,tr("FMT Chunk"),output);

}

void MainWidget::displayData()
{
  QString str1;
  QString str2;
  char string[256];
  QString output;

  str1=QString(tr("Data Size:"));
  str2=QString(tr("bytes"));
  sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getDataLength(),
	  (const char *)str2);
  output=output.append(string);

  QMessageBox::information(this,tr("DATA Chunk"),output);
}


void MainWidget::displayFact()
{
  QString str1;
  QString str2;
  char string[256];
  QString output;

  str1=QString(tr("Sample Size:"));
  str2=QString(tr("samples"));
  sprintf(string,"%s %d %s\n",(const char *)str1,wavefile->getSampleLength(),
	  (const char *)str2);
  output=output.append(string);

  QMessageBox::information(this,"FACT Chunk",output);
}


void MainWidget::displayCart()
{
  QString str1;
  QString str2;
  char string[256];
  QString output;
  bool timers_found=false;

/*
  sprintf(string,"Cart Chunk Version: %c.%c.%c\n",
	  (wavefile->getCartVersion()>>24)&255,
	  (wavefile->getCartVersion()>>16)&255,
	  (wavefile->getCartVersion()>>8)&255);
  output=output.append(string);
*/
  output=output.append(tr("TITLE: "));
  output=output.append(wavefile->getCartTitle());
  output=output.append("\n");
  output=output.append(tr("ARTIST: "));
  output=output.append(wavefile->getCartArtist());
  output=output.append("\n");
  output=output.append(tr("CUT ID: "));
  output=output.append(wavefile->getCartCutID());
  output=output.append("\n");
  output=output.append(tr("CLIENT ID: "));
  output=output.append(wavefile->getCartClientID());
  output=output.append("\n");
  output=output.append(tr("CATEGORY: "));
  output=output.append(wavefile->getCartCategory());
  output=output.append("\n");
  output=output.append(tr("CLASSIFICATION: "));
  output=output.append(wavefile->getCartClassification());
  output=output.append("\n");
  output=output.append(tr("OUT CUE: "));
  output=output.append(wavefile->getCartOutCue());
  output=output.append("\n");
  output=output.append(tr("START DATE: "));
  output=output.append(wavefile->getCartStartDate().toString());
  output=output.append("\n");
  output=output.append(tr("START TIME: "));
  output=output.append(wavefile->getCartStartTime().toString());
  output=output.append("\n");
  output=output.append(tr("END DATE: "));
  output=output.append(wavefile->getCartEndDate().toString());
  output=output.append("\n");
  output=output.append(tr("END TIME: "));
  output=output.append(wavefile->getCartEndTime().toString());
  output=output.append("\n");
  output=output.append(tr("PRODUCER ID: "));
  output=output.append(wavefile->getCartProducerAppID());
  output=output.append("\n");
  output=output.append(tr("PRODUCER VERSION: "));
  output=output.append(wavefile->getCartProducerAppVer());
  output=output.append("\n");
  output=output.append(tr("USER DEFINED: "));
  output=output.append(wavefile->getCartUserDef());
  output=output.append("\n");
  if(wavefile->getCartLevelRef()>0) {
    str1=QString(tr("LEVEL REFERENCE:"));
    str2=QString(tr("dB"));
    sprintf(string,"%s %5.1lf %s\n",(const char *)str1,
	    20*log10((float)wavefile->getCartLevelRef()/32767),
	    (const char *)str2);
  }
  else {
    str1=QString(tr("LEVEL REFERENCE:"));
    str2=QString(tr("Undefined"));
    sprintf(string,"%s %s\n",(const char *)str1,(const char *)str2);
  }
  output=output.append(string);
  output=output.append(tr("URL: "));
  output=output.append(wavefile->getCartURL());
  output=output.append("\n");
  output=output.append(tr("CART TIMER(S): "));
  for(int i=0;i<MAX_TIMERS;i++) {
    if(!wavefile->getCartTimerLabel(i).isEmpty()) {
      output=output.append(wavefile->getCartTimerLabel(i));
      sprintf(string,": %u\n",wavefile->getCartTimerSample(i));
      output=output.append(string);
      timers_found=true;
    }
  }
  if(!timers_found) {
    output=output.append("\n");
  }
  output=output.append(tr("TAG TEXT: "));
  output=output.append(wavefile->getCartTagText());
  output=output.append("\n");

  QMessageBox::information(this,tr("CART Chunk"),output);
}



void MainWidget::displayBext()
{
  char string[256];
  QString output;
  QString str;

  output=output.append(tr("DESCRIPTION: "));
  output=output.append(wavefile->getBextDescription());
  output=output.append("\n");
  output=output.append(tr("ORIGINATOR: "));
  output=output.append(wavefile->getBextOriginator());
  output=output.append("\n");
  output=output.append(tr("ORIGINATOR REFERENCE: "));
  output=output.append(wavefile->getBextOriginatorRef());
  output=output.append("\n");
  output=output.append(tr("ORIGINATION DATE: "));
  output=output.append(wavefile->getBextOriginationDate().toString());
  output=output.append("\n");
  output=output.append(tr("ORIGINATION TIME: "));
  output=output.append(wavefile->getBextOriginationTime().toString());
  output=output.append("\n");
  str=QString(tr("VERSION:"));
  sprintf(string,"%s %d\n",(const char *)str,wavefile->getBextVersion());
  output=output.append(string);
  output=output.append(tr("CODING HISTORY: "));
  output=output.append(wavefile->getBextCodingHistory());
  output=output.append("\n");

  QMessageBox::information(this,tr("BEXT Chunk"),output);
}


void MainWidget::displayMext()
{
  QString output;
  QString str1;
  QString str2;

  output=output.append(tr("MPEG Data Composition: "));
  if(wavefile->getMextHomogenous()) {
    output=output.append(tr("Homogenous"));
    output=output.append("\n");
    if(wavefile->getMextPaddingUsed()) {
      output=output.append(tr("The padding bit is active."));
      output=output.append("\n");
    }
    else {
      output=output.append(tr("The padding bit is inactive."));
      output=output.append("\n");
    }
    if(wavefile->getMextHackedBitRate()) {
      output=output.append(tr("The bit rate is non-standard."));
      output=output.append("\n");
    }
  }
  else {
    output=output.append(tr("Non-homogenous"));
    output=output.append("\n");
  }
  if(wavefile->getMextFreeFormat()) {
    output=output.append(tr("MPEG Format: Free Format"));
    output=output.append("\n");
  }
  else {
    output=output.append(tr("MPEG Format: Constant Bit Rate"));
    output=output.append("\n");
  }
  str1=QString(tr("MPEG Frame Size:"));
  str2=QString(tr("bytes"));
  output=output.append(QString().sprintf("%s %d %s",(const char *)str1,
					 wavefile->getMextFrameSize(),
					 (const char *)str2));
  output=output.append("\n");
  str1=QString(tr("Ancillary Bytes:"));
  str2=QString(tr("bytes"));
  output=output.append(QString().sprintf("%s %d %s\n",(const char *)str1,
					 wavefile->getMextAncillaryLength(),
					 (const char *)str2));
  output=output.append("\n");
  if(wavefile->getMextAncillaryLength()>0) {
    output=output.append(tr("Ancillary Data: "));
    if(wavefile->getMextLeftEnergyPresent()) {
      output=output.append(tr(" LeftEnergy"));
    }
    if(wavefile->getMextRightEnergyPresent()) {
      output=output.append(tr(" RightEnergy"));
    }
    if(wavefile->getMextPrivateDataPresent()) {
      output=output.append(tr(" PrivateData"));
    }
    output=output.append("\n");
  }

  QMessageBox::information(this,tr("MEXT Chunk"),output);
}


void MainWidget::displayLevl()
{
  QString output;
  QString str;

  output=output.append(tr("Version: "));
  output=output.append(QString().sprintf("%d\n",wavefile->getLevlVersion()));
  output=output.append(tr("Samples per Peak: "));
  output=
    output.append(QString().sprintf("%d\n",wavefile->getLevlBlockSize()));
  output=output.append(tr("Sample Channels: "));
  output=output.append(QString().sprintf("%d\n",wavefile->getLevlChannels()));
  output=output.append(tr("Overall Peak: "));
  if(wavefile->getLevlPeak()==0) {
    output=output.append(tr("Unknown"));
    output=output.append("\n");
  }
  else {
    str=QString(tr("dBFS"));
    output=output.append(QString().sprintf("%5.1f %s\n",
			 20.0*log10((double)wavefile->getLevlPeak()/32768.0),
			 (const char *)str));
  }
  output=output.append(tr("Timestamp: "));
  output=
   output.append(wavefile->getLevlTimestamp().toString("MM-dd-yyyy hh:mm:ss"));

  QMessageBox::information(this,tr("LEVL Chunk"),output);
}


void MainWidget::displayAIR1()
{
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdutils_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}


