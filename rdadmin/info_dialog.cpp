// info_dialog.cpp
//
// Display System Information for Rivendell
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

#include <QPushButton>

#include <dbversion.h>
#include <rd.h>
#include <rdlabel.h>
#include <rdlicense.h>

#include "info_dialog.h"

//
// This is a kludge, but apparently needed to get the bitmap data
// for the info banners, as Automake refuses to process the cwrap
// dependency correctly.
//
#include <xpm_info_banner1.cpp>
#include <xpm_info_banner2.cpp>

InfoDialog::InfoDialog(QWidget *parent)
  : QDialog(parent)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("System Information"));

  //
  // Create Fonts
  //
  QFont title_font=QFont("Helvetica",24,QFont::DemiBold);
  title_font.setPixelSize(22);
  QFont slogan_font=QFont("Helvetica",14,QFont::Normal);
  slogan_font.setPixelSize(14);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont bold_font=QFont("Helvetica",10,QFont::Bold);
  bold_font.setPixelSize(10);
  QFont font=QFont("Helvetica",10,QFont::Normal);
  font.setPixelSize(10);

  //
  // Banners
  //
  QImage *image=new QImage(460,35,16);
  image->loadFromData(xpm_info_banner1,strlen((const char *)xpm_info_banner1),
		      "XPM");
  QLabel *label=new QLabel(this);
  QPixmap pix(460,35);
  pix.convertFromImage(*image);
  label->setGeometry(0,0,460,35);
  label->setPixmap(pix);

  image->loadFromData(xpm_info_banner2,strlen((const char *)xpm_info_banner2),
		      "XPM");
  label=new QLabel(this);
  pix.convertFromImage(*image);
  label->setGeometry(0,sizeHint().height()-35,460,35);
  label->setPixmap(pix);

  //
  // Title
  //
  label=new QLabel(tr("Rivendell"),this);
  label->setGeometry(10,41,120,36);
  label->setFont(title_font);

  //
  // Slogan
  //
  label=new QLabel(tr("A Radio Automation System"),this);
  label->setGeometry(130,52,200,18);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label->setFont(slogan_font);

  //
  // Version
  //
  str=QString(tr("Version"));
  label=new QLabel(QString().sprintf("%s %s",(const char *)str,VERSION),this);
  label->setGeometry(10,73,200,14);
  label->setFont(font);

  str=QString(tr("Database Schema"));
  label=new QLabel(QString().sprintf("%s %d",(const char *)str,
				     RD_VERSION_DATABASE),this);
  label->setGeometry(210,73,120,14);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label->setFont(font);

  //
  // Signature
  //
  str=QString(tr("Copyright 2002-2016"));
  label=new QLabel(QString().sprintf("%s %s",(const char *)str,
				     PACKAGE_BUGREPORT),this);
  label->setGeometry(10,87,sizeHint().width()-20,14);
  label->setFont(font);

  //
  // Disclaimer
  //
  label=new RDLabel(this);
  label->setGeometry(10,104,sizeHint().width()-20,60);
  label->setFont(font);
  label->setText(tr("This program is free software, and comes with ABSOLUTELY NO WARRANTY,\nnot even the implied warranties of MERCHANTIBILITY or FITNESS FOR A\nPARTICULAR PURPOSE.  Touch the \"View License\" button for details."));

  //
  // License Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()/2-45,174,80,50);
  button->setFont(button_font);
  button->setText(tr("View\n&License"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewLicenseData()));

  //
  //  Close Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-90,
			    80,50);
  button->setFont(button_font);
  button->setText(tr("&Close"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize InfoDialog::sizeHint() const
{
  return QSize(460,310);
} 


QSizePolicy InfoDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void InfoDialog::viewLicenseData()
{
  RDLicense *lic=new RDLicense(this);
  lic->exec(RDLicense::GplV2);
  delete lic;
}


void InfoDialog::closeData()
{
  done(0);
}
