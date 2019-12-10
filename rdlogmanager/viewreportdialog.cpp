// viewreportdialog.cpp
//
// Offer to display a generated report.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

#include <rdtextfile.h>

#include "viewreportdialog.h"

ViewReportDialog::ViewReportDialog(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMaximumSize(sizeHint());
  setMinimumSize(sizeHint());


  setWindowTitle("RDLogManager - "+tr("Report Complete"));
  view_message_label=new QLabel(this);
  view_message_label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
  view_message_label->setWordWrap(true);

  view_view_button=new QPushButton(tr("View in")+"\n"+tr("Report Editor"),this);
  view_view_button->setFont(buttonFont());
  connect(view_view_button,SIGNAL(clicked()),this,SLOT(viewData()));

  view_close_button=new QPushButton(tr("Close"),this);
  view_close_button->setFont(buttonFont());
  connect(view_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ViewReportDialog::sizeHint() const
{
  return QSize(300,120);
}


QSizePolicy ViewReportDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int ViewReportDialog::exec(const QString &rpt_filename)
{
  view_report_filename=rpt_filename;
  view_message_label->
    setText(tr("Report generated in")+" \""+rpt_filename+"\".");

  return QDialog::exec();
}
  

void ViewReportDialog::viewData()
{
  if(!RDTextViewer(view_report_filename)) {
    QMessageBox::information(this,"RDLogManager - "+tr("Error"),
			     tr("Unable to launce report viewer!"));
  }
}


void ViewReportDialog::closeData()
{
  done(true);
}


void ViewReportDialog::resizeEvent(QResizeEvent *e)
{
  view_message_label->setGeometry(10,10,size().width()-20,size().height()-80);

  view_view_button->setGeometry(10,size().height()-60,110,50);

  view_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
