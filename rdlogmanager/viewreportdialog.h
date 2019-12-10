// viewreportdialog.h
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

#ifndef VIEWREPORTDIALOG_H
#define VIEWREPORTDIALOG_H

#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qpushbutton.h>

#include <rddialog.h>

class ViewReportDialog : public RDDialog
{
  Q_OBJECT
 public:
  ViewReportDialog(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(const QString &rpt_filename);
  
 private slots:
  void viewData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *view_message_label;
  QPushButton *view_view_button;
  QPushButton *view_close_button;
  QString view_report_filename;
};


#endif  // VIEWREPORTDIALOG_H
