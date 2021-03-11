// wavefactory_test.cpp
//
// Test harness for RDWavefactory
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QGraphicsTextItem>
#include <QMessageBox>

#include <rdapplication.h>
#include <rdpeaksexport.h>

#include "wavefactory_test.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;
  bool ok=false;

  d_cart_number=0;
  d_cut_number=-1;
  d_scene=NULL;
  bool track_mode_set=false;

  //
  // Open the database
  //
  rda=new RDApplication("wavefactory_test","wavefactory_test",WAVEFACTORY_TEST_USAGE,
			this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"wavefactory_test - "+tr("Error"),err_msg);
    exit(RDApplication::ExitNoDb);
  }
  d_font_engine=new RDFontEngine(font(),rda->config());

  //
  // Get Command Switches
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--cart-number") {
      d_cart_number=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(d_cart_number>RD_MAX_CART_NUMBER)) {
	fprintf(stderr,"wavefactory_test: invalid cart number\n");
	exit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cut-number") {
      d_cut_number=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(d_cut_number>RD_MAX_CUT_NUMBER)||(d_cut_number<1)) {
	fprintf(stderr,"wavefactory_test: invalid cut number\n");
	exit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--track-mode") {
      if(rda->cmdSwitch()->value(i)=="single") {
	d_track_mode=RDWaveFactory::SingleTrack;
	track_mode_set=true;
      }
      if(rda->cmdSwitch()->value(i)=="multi") {
	d_track_mode=RDWaveFactory::MultiTrack;
	track_mode_set=true;
      }
      if(!track_mode_set) {
	fprintf(stderr,"wavefactory_test: invalid --track-mode argument\n");
	exit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }

    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"wavefactory_test: unknown option \"%s\"\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(RDApplication::ExitInvalidOption);
    }
  }
  if(d_cart_number==0) {
    fprintf(stderr,"wavefactory_test: you must specify a cart number\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(d_cut_number==-1) {
    fprintf(stderr,"wavefactory_test: you must specify a cut number\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(!track_mode_set) {
    fprintf(stderr,"wavefactory_test: you must specify a track mode\n");
    exit(RDApplication::ExitInvalidOption);
  }
    
  //
  // Wave Display
  //
  d_factory=new RDWaveFactory(d_track_mode);
  d_view=new QGraphicsView(this);

  //
  // X Shrink Factor
  //
  d_shrink_factor_group=new QGroupBox(tr("X Shrink Factor"),this);
  d_shrink_factor_group->setFont(d_font_engine->labelFont());
  d_up_button=
    new RDTransportButton(RDTransportButton::Up,d_shrink_factor_group);
  connect(d_up_button,SIGNAL(clicked()),this,SLOT(upShrinkData()));
  d_down_button=
    new RDTransportButton(RDTransportButton::Down,d_shrink_factor_group);
  connect(d_down_button,SIGNAL(clicked()),this,SLOT(downShrinkData()));
  d_shrink_factor_edit=new QLineEdit(d_shrink_factor_group);
  d_shrink_factor_edit->setReadOnly(true);
  d_shrink_factor_edit->setText("1");

  //
  // Audio Gain
  //
  d_audio_gain_label=new QLabel(tr("Audio Gain")+":",this);
  d_audio_gain_label->setFont(d_font_engine->labelFont());
  d_audio_gain_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_audio_gain_spin=new QSpinBox(this);
  d_audio_gain_spin->setRange(0,100);
  d_audio_gain_unit=new QLabel(tr("dB")+":",this);
  d_audio_gain_unit->setFont(d_font_engine->labelFont());
  d_audio_gain_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(d_audio_gain_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(gainChangedData(int)));

  //
  // Connect to ripcd(8)
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());
}


QSize MainWidget::sizeHint() const
{
  return QSize(1000,375);
}


void MainWidget::userData()
{
  QString err_msg;

  setWindowTitle("wavefactory_test User: "+rda->user()->name());
  if(!d_factory->setCut(&err_msg,d_cart_number,d_cut_number)) {
    QMessageBox::critical(this,"wavefactory_test - "+tr("Error"),err_msg);
    exit(RDApplication::ExitInvalidOption);
  }
  UpdateWave();
}


void MainWidget::upShrinkData()
{
  int x_shrink=d_shrink_factor_edit->text().toInt();

  x_shrink=x_shrink*2;
  d_shrink_factor_edit->setText(QString().sprintf("%d",x_shrink));
  UpdateWave();
}


void MainWidget::downShrinkData()
{
  int x_shrink=d_shrink_factor_edit->text().toInt();

  if(x_shrink>1) {
    x_shrink=x_shrink/2;
    d_shrink_factor_edit->setText(QString().sprintf("%d",x_shrink));
    UpdateWave();
  }
}


void MainWidget::gainChangedData(int db)
{
  UpdateWave();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  d_view->setGeometry(0,0,w,20+d_view->sizeHint().height());

  d_shrink_factor_group->setGeometry(10,h-150,155,145);
  d_up_button->setGeometry(10,25,80,50);
  d_down_button->setGeometry(10,80,80,50);
  d_shrink_factor_edit->setGeometry(100,25,50,20);

  d_audio_gain_label->setGeometry(200,h-120,100,20);
  d_audio_gain_spin->setGeometry(305,h-120,50,20);
  d_audio_gain_unit->setGeometry(360,h-120,50,20);
}


void MainWidget::UpdateWave()
{
  QPixmap pix=d_factory->generate(200,d_shrink_factor_edit->text().toInt(),
				  100*d_audio_gain_spin->value());

  if(d_scene!=NULL) {
    d_scene->deleteLater();
  }
  d_scene=new QGraphicsScene(0,0,pix.width(),pix.height());
  d_scene->addPixmap(pix);
  d_view->setScene(d_scene);
  d_view->setGeometry(0,0,size().width(),20+d_view->sizeHint().height());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
