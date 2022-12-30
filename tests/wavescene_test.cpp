// wavescene_test.cpp
//
// Test harness for RDWaveScene
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "wavescene_test.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;
  bool ok=false;

  d_cart_number=0;
  d_cut_number=-1;
  d_scene=NULL;
  d_view=NULL;

  //
  // Open the database
  //
  rda=new RDApplication("wavescene_test","wavescene_test",WAVESCENE_TEST_USAGE,
			this);
  if(!rda->open(&err_msg,NULL,true)) {
    QMessageBox::critical(this,"wavescene_test - "+tr("Error"),err_msg);
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
	fprintf(stderr,"wavescene_test: invalid cart number\n");
	exit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cut-number") {
      d_cut_number=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(d_cut_number>RD_MAX_CUT_NUMBER)||(d_cut_number<1)) {
	fprintf(stderr,"wavescene_test: invalid cut number\n");
	exit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }

    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"wavescene_test: unknown option \"%s\"\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(RDApplication::ExitInvalidOption);
    }
  }
  if(d_cart_number==0) {
    fprintf(stderr,"wavescene_test: you must specify a cart number\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(d_cut_number==-1) {
    fprintf(stderr,"wavescene_test: you must specify a cut number\n");
    exit(RDApplication::ExitInvalidOption);
  }

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
  setWindowTitle("wavescene_test User: "+rda->user()->name());

  d_cut=new RDCut(d_cart_number,d_cut_number);
  if(!d_cut->exists()) {
    fprintf(stderr,"wavescene_test: no such cart/cut\n");
    exit(RDApplication::ExitInvalidCart);
  }
  d_channel=RDWaveScene::Left;

  LoadEnergy();

  LoadWave();
}


void MainWidget::upShrinkData()
{
  int x_shrink=d_shrink_factor_edit->text().toInt();

  x_shrink=x_shrink*2;
  d_shrink_factor_edit->setText(QString::asprintf("%d",x_shrink));
  LoadWave();
}


void MainWidget::downShrinkData()
{
  int x_shrink=d_shrink_factor_edit->text().toInt();

  if(x_shrink>1) {
    x_shrink=x_shrink/2;
    d_shrink_factor_edit->setText(QString::asprintf("%d",x_shrink));
    LoadWave();
  }
}


void MainWidget::gainChangedData(int db)
{
  LoadWave();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  //  int w=size().height();
  int h=size().height();

  if(d_view!=NULL) {
    d_view->setGeometry(0,0,size().width(),h-155);
  }

  d_shrink_factor_group->setGeometry(10,h-150,155,145);
  d_up_button->setGeometry(10,25,80,50);
  d_down_button->setGeometry(10,80,80,50);
  d_shrink_factor_edit->setGeometry(100,25,50,20);

  d_audio_gain_label->setGeometry(200,h-120,100,20);
  d_audio_gain_spin->setGeometry(305,h-120,50,20);
  d_audio_gain_unit->setGeometry(360,h-120,50,20);
}


void MainWidget::LoadWave()
{
  int x_shrink=d_shrink_factor_edit->text().toInt();

  if(d_scene!=NULL) {
    d_scene->deleteLater();
  }
  d_scene=new RDWaveScene(d_energy_data,x_shrink,
			  (qreal)d_audio_gain_spin->value(),size().height()-175,
			  this);

  QGraphicsTextItem *t_item=
    d_scene->addText(QString::asprintf("%d frames/pixel",1152*x_shrink),
		     d_font_engine->bigLabelFont());
  t_item->setDefaultTextColor(Qt::red);
  //  t_item->setPos(10.0,-35.0);
  d_view->setScene(d_scene);
  d_view->show();
  //  d_view->setGeometry(0,0,size().width(),size().height()-
  //		      175+d_view->scrollBarWidgets(Qt::AlignBottom).at(0)->size().height());
  d_view->setGeometry(0,0,size().width(),size().height()-155);
}


void MainWidget::LoadEnergy()
{
  RDPeaksExport::ErrorCode err_code;
  RDPeaksExport *conv=new RDPeaksExport(this);

  conv->setCartNumber(d_cut->cartNumber());
  conv->setCutNumber(d_cut->cutNumber());
  if((err_code=conv->runExport(rda->user()->name(),rda->user()->password()))!=RDPeaksExport::ErrorOk) {
    QMessageBox::critical(this,"wavescene_test - "+tr("Error"),
			  tr("Energy export failed")+": "+
			  RDPeaksExport::errorText(err_code));
    exit(255);
  }
  d_energy_data.clear();
  for(unsigned i=0;i<conv->energySize();i+=2) {
    //    d_energy_data.push_back(conv->energy(i)/256);
    d_energy_data.push_back(conv->energy(i));
  }

  delete conv;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
