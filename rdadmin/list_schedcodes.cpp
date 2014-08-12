// List_schedcodes.cpp
//
// The scheduler codes dialog for rdadmin
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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


#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <rdcart.h>
#include <rdescape_string.h>
#include <rdtextfile.h>

#include <edit_schedcodes.h>
#include <add_schedcodes.h>
#include <list_schedcodes.h>

ListSchedCodes::ListSchedCodes(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Rivendell Scheduler Codes List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Group List
  //
  list_schedCodes_view=new QListView(this);
  list_schedCodes_view->setAllColumnsShowFocus(true);
  list_schedCodes_view->addColumn(tr("CODE"));
  list_schedCodes_view->addColumn(tr("DESCRIPTION"));
  QLabel *list_box_label=
    new QLabel(list_schedCodes_view,tr("Scheduler Codes:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,200,19);
  connect(list_schedCodes_view,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  RefreshList();
}


ListSchedCodes::~ListSchedCodes()
{
}


QSize ListSchedCodes::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListSchedCodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSchedCodes::addData()
{
  QString schedCode;

  AddSchedCode *add_schedCode=new AddSchedCode(&schedCode,this);
  if(add_schedCode->exec()<0) {
    delete add_schedCode;
    return;
  }
  delete add_schedCode;
  add_schedCode=NULL;
  QListViewItem *item=new QListViewItem(list_schedCodes_view);
  item->setText(0,schedCode);
  RefreshItem(item);
  item->setSelected(true);
  list_schedCodes_view->setCurrentItem(item);
  list_schedCodes_view->ensureItemVisible(item);
}


void ListSchedCodes::editData()
{
  QListViewItem *item=list_schedCodes_view->selectedItem();
  if(item==NULL) {
    return;
  }
  EditSchedCode *edit_schedCode=
    new EditSchedCode(item->text(0),item->text(1),this);
  edit_schedCode->exec();
  delete edit_schedCode;
  edit_schedCode=NULL;
  RefreshItem(item);
}


void ListSchedCodes::deleteData()
{
  QListViewItem *item=list_schedCodes_view->selectedItem();
  if(item==NULL) {
    return;
  }

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;

  QString codename=item->text(0);
  if(codename.isEmpty()) {
    return;
  }
  if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Scheduler Code"),
			   tr("This operation will delete the selected scheduler code and")+
			   "\n"+tr("all of its associated data.")+" "+
			   tr("This operation cannot be undone.")+"\n\n"+
			   tr("Delete scheduler code")+" \""+codename+"\"?",
			   QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

  sql=QString("delete from DROPBOX_SCHED_CODES where ")+
    "SCHED_CODE=\""+RDEscapeString(codename)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from SCHED_CODES where ")+
    "CODE=\""+RDEscapeString(codename)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  item->setSelected(false);
  delete item;
}


void ListSchedCodes::doubleClickedData(QListViewItem *item,const QPoint &pt,
				   int col)
{
  editData();
}


void ListSchedCodes::closeData()
{
  done(0);
}


void ListSchedCodes::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,210,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_schedCodes_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListSchedCodes::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  QListViewItem *item;

  list_schedCodes_view->clear();
  q=new RDSqlQuery("select CODE,DESCRIPTION from SCHED_CODES",0);
  while (q->next()) {
    item=new QListViewItem(list_schedCodes_view);
    WriteItem(item,q);
  }
  delete q;
}


void ListSchedCodes::RefreshItem(QListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select CODE,DESCRIPTION from SCHED_CODES where CODE=\"%s\"",
			(const char *)item->text(0));
  q=new RDSqlQuery(sql);
  if(q->next()) {
    WriteItem(item,q);
  }
  delete q;
}


void ListSchedCodes::WriteItem(QListViewItem *item,RDSqlQuery *q)
{
  item->setText(0,q->value(0).toString());
  item->setText(1,q->value(1).toString());
}

