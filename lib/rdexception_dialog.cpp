// rdexception_dialog.cpp
//
// A dialog for displaying exception reports.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdexception_dialog.cpp,v 1.8 2010/07/29 19:32:33 cvs Exp $
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
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <rdconf.h>
#include <rdexception_dialog.h>


RDExceptionDialog::RDExceptionDialog(QString report,
				     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  setCaption(tr("Rivendell Exception Report"));

  //
  // Generate Fonts
  //
  QFont button_font("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Report Viewer
  //
  report_view=new QTextView(this,"report_view");
  report_view->setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-80);
  report_view->setText(report);

  //
  // Save Button
  //
  QPushButton *button=new QPushButton(this,"save_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setText(tr("&Save"));
  button->setFont(button_font);
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  // Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setText(tr("&Close"));
  button->setFont(button_font);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


RDExceptionDialog::~RDExceptionDialog()
{
}


QSize RDExceptionDialog::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy RDExceptionDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDExceptionDialog::saveData()
{
  QString str1;
  QString str2;
  QString filename=QFileDialog::getSaveFileName(RDGetHomeDir(),
				   tr("Text (*.txt *.TXT)\nAll Files (*.*)"),
						this,tr("Export File"));
  if(filename.isEmpty()) {
    return;
  }
  if(filename.find(".")<0) {
    filename+=".txt";
  }
  QFile file(filename);
  if(file.exists()) {
    str1=QString(tr("The file"));
    str2=QString(tr("already exists!\nOverwrite?"));
    if(QMessageBox::question(this,tr("File Exists"),
			     QString().sprintf("%s \'%s\' %s",
					       (const char *)str1,
					       (const char *)filename,
					       (const char *)str2),
			     QMessageBox::Yes,QMessageBox::No)==
       QMessageBox::No) {
      return;
    }
  }
  FILE *fh=fopen((const char *)filename,"w");
  if(fh==NULL) {
    str1=QString(tr("Unable to open file"));
    str2=QString(tr("for writing!"));
    QMessageBox::
      warning(this,tr("File Error"),QString().
	      sprintf("%s \'%s\' %s!",(const char *)str1,
		      (const char *)filename,(const char *)str2));
    return;
  }
  fprintf(fh,(const char *)report_view->text());
  fclose(fh);
}


void RDExceptionDialog::closeData()
{
  done(0);
}
