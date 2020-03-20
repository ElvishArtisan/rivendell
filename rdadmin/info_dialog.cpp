// info_dialog.cpp
//
// Display System Information for Rivendell
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <dbversion.h>
#include <rd.h>

#include "info_dialog.h"
#include "license.h"

//
// This is a kludge, but apparently needed to get the bitmap data
// for the info banners, as Automake refuses to process the cwrap
// dependency correctly.
//
#include "xpm_info_banner1.cpp"
#include "xpm_info_banner2.cpp"

InfoDialog::InfoDialog(QWidget *parent)
  : RDDialog(parent)
{
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
  label=new QLabel(tr("Version")+" "+VERSION,this);
  label->setGeometry(10,73,200,14);
  label->setFont(subLabelFont());

  label=new QLabel(tr("Database Schema")+
		   QString().sprintf(" %d",RD_VERSION_DATABASE),this);
  label->setGeometry(210,73,sizeHint().width()-220,14);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label->setFont(subLabelFont());

  //
  // Signature
  //
  label=new QLabel(QString::fromUtf8(RD_COPYRIGHT_NOTICE),this);
  label->setGeometry(10,87,sizeHint().width()-20,14);
  label->setFont(subLabelFont());

  //
  // Disclaimer
  //
  label=new QLabel(this);
  label->setGeometry(10,104,sizeHint().width()-20,60);
  label->setFont(subLabelFont());
  label->setWordWrap(true);
  label->setText(tr("This program is free software, and comes with ABSOLUTELY NO WARRANTY, not even the implied warranties of MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. Touch the \"View License\" button for details."));
  //  label->setText(tr("This program is free software, and comes with ABSOLUTELY NO WARRANTY,\nnot even the implied warranties of MERCHANTIBILITY or FITNESS FOR A\nPARTICULAR PURPOSE.  Touch the \"View License\" button for details."));

  //
  // Credits Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()/2-145,174,80,50);
  button->setFont(buttonFont());
  button->setText(tr("View\n&Credits"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewCreditsData()));

  //
  // License Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()/2-45,174,80,50);
  button->setFont(buttonFont());
  button->setText(tr("View\n&License"));
  connect(button,SIGNAL(clicked()),this,SLOT(viewLicenseData()));

  //
  //  Close Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-90,80,50);
  button->setFont(buttonFont());
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


void InfoDialog::viewCreditsData()
{
  License *lic=new License(this);
  lic->exec(License::Credits);
  delete lic;
}


void InfoDialog::viewLicenseData()
{
  License *lic=new License(this);
  lic->exec(License::GplV2);
  delete lic;
}


void InfoDialog::closeData()
{
  done(0);
}
