// logdialog.cpp
//
// Read-only log lister dialog for Rivendell
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QModelIndexList>

#include "logdialog.h"

LogDialog::LogDialog(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDCastManager - "+tr("List Log Events"));

  d_log_view=new RDTableView(this);
  d_log_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  d_model=NULL;

  d_ok_button=new QPushButton(tr("OK"),this);
  d_ok_button->setFont(buttonFont());
  connect(d_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  d_cancel_button=new QPushButton(tr("Cancel"),this);
  d_cancel_button->setFont(buttonFont());
  connect(d_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


LogDialog::~LogDialog()
{
}


QSize LogDialog::sizeHint() const
{
  return QSize(640,480);
}


int LogDialog::exec(RDLogModel *model,int *start_line,int *end_line)
{
  QItemSelection item_sel;
  QItemSelectionModel *sel=NULL;

  d_model=model;
  d_model->setFont(font());
  d_log_view->setModel(d_model);

  d_log_view->resizeColumnsToContents();

  sel=d_log_view->selectionModel();
  sel->reset();
  item_sel.select(d_model->index(*start_line,0),
  		  d_model->index(*end_line,d_model->columnCount()-1));
  sel->select(item_sel,QItemSelectionModel::SelectCurrent);

  d_start_line=start_line;
  d_end_line=end_line;

  return QDialog::exec();
}


void LogDialog::okData()
{
  QModelIndexList list=d_log_view->selectionModel()->selectedRows();
  if(list.size()==0) {
    QMessageBox::information(this,"RDCastManager - "+tr("List Log"),
			     tr("At least one log event must be selected!"));
    return;
  }
  *d_start_line=list.first().row();
  *d_end_line=list.last().row();

  done(true);
}


void LogDialog::cancelData()
{
  done(false);
}


void LogDialog::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  d_log_view->setGeometry(10,2,w-20,h-72);

  d_ok_button->setGeometry(w-180,h-60,80,50);
  d_cancel_button->setGeometry(w-90,h-60,80,50);
}
