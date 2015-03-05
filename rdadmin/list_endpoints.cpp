// list_endpoints.cpp
//
// List a Rivendell Endpoints
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_endpoints.cpp,v 1.18.8.1 2013/11/17 04:27:05 cvs Exp $
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
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdpasswd.h>
#include <rddb.h>
#include <edit_user.h>
#include <list_endpoints.h>
#include <edit_endpoint.h>


ListEndpoints::ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,
			     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QListViewItem *l;
  QString str;

  list_matrix=matrix;
  list_endpoint=endpoint;
  switch(list_endpoint) {
      case RDMatrix::Input:
	list_size=list_matrix->inputs();
	list_table="INPUTS";
	setCaption(tr("List Inputs"));
	break;

      case RDMatrix::Output:
	list_size=list_matrix->outputs();
	list_table="OUTPUTS";
	setCaption(tr("List Outputs"));
	break;
  }

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Endpoints List Box
  //
  list_list_view=new QListView(this,"list_box");
  list_list_view->
    setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  QLabel *label=
    new QLabel(list_list_view,list_table,this,"list_list_view_label");
  label->setFont(bold_font);
  label->setGeometry(14,5,85,19);
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  switch(list_endpoint) {
      case RDMatrix::Input:
	list_list_view->addColumn(tr("INPUT"));
	break;

      case RDMatrix::Output:
	list_list_view->addColumn(tr("OUTPUT"));
	break;
  }
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_list_view->addColumn(tr("LABEL"));
  list_list_view->setColumnAlignment(1,Qt::AlignLeft);
  switch(matrix->type()) {
    case RDMatrix::Unity4000:
      list_readonly=false;
      if(list_endpoint==RDMatrix::Input) {
	list_list_view->addColumn(tr("SOURCE"));
	list_list_view->setColumnAlignment(2,Qt::AlignHCenter);
	list_list_view->addColumn(tr("MODE"));
	list_list_view->setColumnAlignment(3,Qt::AlignHCenter);
      }
      break;
      
    case RDMatrix::LogitekVguest:
      list_readonly=false;
      list_list_view->addColumn(tr("ENGINE (Hex)"));
      list_list_view->setColumnAlignment(2,Qt::AlignHCenter);	
      list_list_view->addColumn(tr("DEVICE (Hex)"));
      list_list_view->setColumnAlignment(2,Qt::AlignHCenter);	
      break;
      
    case RDMatrix::StarGuideIII:
      list_readonly=false;
      if(list_endpoint==RDMatrix::Input) {
	list_list_view->addColumn(tr("PROVIDER ID"));
	list_list_view->setColumnAlignment(2,Qt::AlignHCenter);
	list_list_view->addColumn(tr("SERVICE ID"));
	list_list_view->setColumnAlignment(3,Qt::AlignHCenter);
	list_list_view->addColumn(tr("MODE"));
	list_list_view->setColumnAlignment(4,Qt::AlignHCenter);
      }
      break;
      
    case RDMatrix::LiveWireLwrpAudio:
      list_readonly=true;
      list_list_view->addColumn(tr("NODE"));
      list_list_view->setColumnAlignment(2,Qt::AlignHCenter);
      list_list_view->addColumn(tr("#"));
      list_list_view->setColumnAlignment(3,Qt::AlignHCenter);
      break;

    case RDMatrix::SasUsi:
      list_readonly=true;
      break;

    default:
      list_readonly=false;
      break;
  }
  if(!list_readonly) {
    connect(list_list_view,
	    SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	    this,
	    SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));
  }

  //
  //  Edit Button
  //
  QPushButton *button=new QPushButton(this,"edit_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Edit"));
  connect(button,SIGNAL(clicked()),this,SLOT(editData()));
  button->setDisabled(list_readonly);

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  switch(list_matrix->type()) {
      case RDMatrix::Unity4000:
	if(list_endpoint==RDMatrix::Input) {
	  sql=QString().sprintf("select NUMBER,NAME,FEED_NAME,CHANNEL_MODE\
                                 from %s where STATION_NAME=\"%s\" && \
                                 MATRIX=%d order by NUMBER",
				(const char *)list_table,
				(const char *)list_matrix->station(),
				list_matrix->matrix());
	}
	else {
	  sql=QString().sprintf("select NUMBER,NAME from %s\
                                 where STATION_NAME=\"%s\" && \
                                 MATRIX=%d order by NUMBER",
				(const char *)list_table,
				(const char *)list_matrix->station(),
				list_matrix->matrix());
	}
	break;

      case RDMatrix::LogitekVguest:
	sql=QString().sprintf("select NUMBER,NAME,ENGINE_NUM,DEVICE_NUM\
                               from %s where (STATION_NAME=\"%s\")&&\
                               MATRIX=%d order by NUMBER",
			      (const char *)list_table,
			      (const char *)list_matrix->station(),
			      list_matrix->matrix());
	break;

      case RDMatrix::StarGuideIII:
	if(list_endpoint==RDMatrix::Input) {
	  sql=QString().sprintf("select NUMBER,NAME,ENGINE_NUM,DEVICE_NUM,\
                                 CHANNEL_MODE\
                                 from %s where (STATION_NAME=\"%s\")&&\
                                 MATRIX=%d order by NUMBER",
				(const char *)list_table,
				(const char *)list_matrix->station(),
				list_matrix->matrix());
	}
	else {
	  sql=QString().sprintf("select NUMBER,NAME from %s\
                                 where STATION_NAME=\"%s\" && \
                                 MATRIX=%d order by NUMBER",
				(const char *)list_table,
				(const char *)list_matrix->station(),
				list_matrix->matrix());
	}
	break;

    case RDMatrix::LiveWireLwrpAudio:
	sql=QString().sprintf("select NUMBER,NAME,NODE_HOSTNAME,NODE_SLOT \
                               from %s where STATION_NAME=\"%s\" && \
                               MATRIX=%d order by NUMBER",
			      (const char *)list_table,
			      (const char *)list_matrix->station(),
			      list_matrix->matrix());
	break;

      default:
	sql=QString().sprintf("select NUMBER,NAME from %s\
                               where STATION_NAME=\"%s\" && \
                               MATRIX=%d order by NUMBER",
			      (const char *)list_table,
			      (const char *)list_matrix->station(),
			      list_matrix->matrix());
	break;
  }
  q=new RDSqlQuery(sql);
  if(list_matrix->type()==RDMatrix::LiveWireLwrpAudio) {
    while(q->next()) {
      l=new QListViewItem(list_list_view); 
      l->setText(0,QString().sprintf("%05d",q->value(0).toInt()));
      l->setText(1,q->value(1).toString());
      l->setText(2,q->value(2).toString());
      l->setText(3,QString().sprintf("%d",q->value(3).toInt()));
    }
  }
  else {
    q->first();
    for(int i=0;i<list_size;i++) {
      l=new QListViewItem(list_list_view); 
      l->setText(0,QString().sprintf("%03d",i+1));
      if(q->isValid()&&(q->value(0).toInt()==(i+1))){
	l->setText(1,q->value(1).toString());
	switch(list_matrix->type()) {
	  case RDMatrix::Unity4000:
	    if(list_endpoint==RDMatrix::Input) {
	      l->setText(2,q->value(2).toString());
	      switch((RDMatrix::Mode)q->value(3).toInt()) {
		case RDMatrix::Stereo:
		  l->setText(3,tr("Stereo"));
		  break;
		  
		case RDMatrix::Left:
		  l->setText(3,tr("Left"));
		  break;
		  
		case RDMatrix::Right:
		  l->setText(3,tr("Right"));
		  break;
	      }
	    }
	    break;
	    
	  case RDMatrix::LogitekVguest:
	    if(q->value(2).toInt()>=0) {
	      l->setText(2,QString().sprintf("%04X",q->value(2).toInt()));
	    }
	    else {
	      l->setText(2,"");
	    }
	    if(q->value(3).toInt()>=0) {
	      l->setText(3,QString().sprintf("%04X",q->value(3).toInt()));
	    }
	    else {
	      l->setText(3,"");
	    }
	    break;
	    
	  case RDMatrix::StarGuideIII:
	    if(list_endpoint==RDMatrix::Input) {
	      if(q->value(2).toInt()>=0) {
		l->setText(2,q->value(2).toString());
	      }
	      if(q->value(3).toInt()>=0) {
		l->setText(3,q->value(3).toString());
	      }
	      switch((RDMatrix::Mode)q->value(4).toInt()) {
		case RDMatrix::Stereo:
		  l->setText(4,tr("Stereo"));
		  break;
		  
		case RDMatrix::Left:
		  l->setText(4,tr("Left"));
		  break;
		  
		case RDMatrix::Right:
		  l->setText(4,tr("Right"));
		  break;
	      }
	    }
	    break;
	    
	  default:
	    break;
	}
	q->next();
      }
      else {
	switch(list_endpoint) {
	  case RDMatrix::Input:
	    str=QString(tr("Input"));
	    l->setText(1,QString().sprintf("%s %03d",(const char *)str,i+1));
	    switch(list_matrix->type()) {
	      case RDMatrix::Unity4000:
		l->setText(3,tr("Stereo"));
		break;
		
	      case RDMatrix::StarGuideIII:
		l->setText(4,tr("Stereo"));
		break;
		
	      default:
		break;
	    }
	    break;
	    
	  case RDMatrix::Output:
	    str=QString(tr("Output"));
	    l->setText(1,QString().sprintf("%s %03d",(const char *)str,i+1));
	    break;
	}
      }
    }
  }
  delete q;
}


QSize ListEndpoints::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListEndpoints::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListEndpoints::editData()
{
  QListViewItem *item;
  RDMatrix::Mode mode=RDMatrix::Stereo;
  bool ok;
  QString feedname;
  int enginenum=-1;
  int devicenum=-1;

  if((item=list_list_view->selectedItem())==NULL) {
    return;
  }
  int pointnum=item->text(0).toInt()-1;
  QString pointname=item->text(1);
  switch(list_matrix->type()) {
      case RDMatrix::Unity4000:
	feedname=item->text(2);
	if(item->text(3).lower()==QString(tr("stereo"))) {
	  mode=RDMatrix::Stereo;
	}
	if(item->text(3).lower()==QString(tr("left"))) {
	  mode=RDMatrix::Left;
	}
	if(item->text(3).lower()==QString(tr("right"))) {
	  mode=RDMatrix::Right;
	}
	break;

      case RDMatrix::LogitekVguest:
	enginenum=item->text(2).toInt(&ok,16);
	if(!ok) {
	  enginenum=-1;
	}
	devicenum=item->text(3).toInt(&ok,16);
	if(!ok) {
	  devicenum=-1;
	}
	break;

      case RDMatrix::StarGuideIII:
	enginenum=item->text(2).toInt(&ok);
	if(!ok) {
	  enginenum=-1;
	}
	devicenum=item->text(3).toInt(&ok);
	if(!ok) {
	  devicenum=-1;
	}
	if(item->text(4).lower()==QString(tr("stereo"))) {
	  mode=RDMatrix::Stereo;
	}
	if(item->text(4).lower()==QString(tr("left"))) {
	  mode=RDMatrix::Left;
	}
	if(item->text(4).lower()==QString(tr("right"))) {
	  mode=RDMatrix::Right;
	}
	break;

      default:
	break;
  }
  EditEndpoint *edit=new EditEndpoint(list_matrix->type(),list_endpoint,
				      pointnum,&pointname,&feedname,&mode,
				      &enginenum,&devicenum,this);
  if(edit->exec()==0) {
    item->setText(1,pointname);
    item->setText(2,feedname);
    switch(list_matrix->type()) {
	case RDMatrix::Unity4000:
	  if(list_endpoint==RDMatrix::Input) {
	    switch(mode) {
		case RDMatrix::Stereo:
		  item->setText(3,tr("Stereo"));
		  break;
		  
		case RDMatrix::Left:
		  item->setText(3,tr("Left"));
		  break;
		  
		case RDMatrix::Right:
		  item->setText(3,tr("Right"));
		  break;
	    }
	  }
	  break;

	case RDMatrix::LogitekVguest:
	  if(enginenum>=0) {
	    item->setText(2,QString().sprintf("%04X",enginenum));
	  }
	  else {
	    item->setText(2,"");
	  }
	  if(devicenum>=0) {
	    item->setText(3,QString().sprintf("%04X",devicenum));
	  }
	  else {
	    item->setText(3,"");
	  }
	  break;

	case RDMatrix::StarGuideIII:
	  if(enginenum>=0) {
	    item->setText(2,QString().sprintf("%d",enginenum));
	  }
	  else {
	    item->setText(2,"");
	  }
	  if(devicenum>=0) {
	    item->setText(3,QString().sprintf("%d",devicenum));
	  }
	  else {
	    item->setText(3,"");
	  }
	  if(list_endpoint==RDMatrix::Input) {
	    switch(mode) {
		case RDMatrix::Stereo:
		  item->setText(4,tr("Stereo"));
		  break;
		  
		case RDMatrix::Left:
		  item->setText(4,tr("Left"));
		  break;
		  
		case RDMatrix::Right:
		  item->setText(4,tr("Right"));
		  break;
	    }
	  }
	  break;

	default:
	  break;
    }
  }
  delete edit;
}


void ListEndpoints::doubleClickedData(QListViewItem *item,const QPoint &pt,
				      int col)
{
  editData();
}


void ListEndpoints::okData()
{
  if(!list_readonly) {
    QListViewItem *item;
    QString sql;
    RDSqlQuery *q;
    RDMatrix::Mode mode=RDMatrix::Stereo;
    int enginenum;
    int devicenum;
    int modecol=3;
    
    if(list_matrix->type()==RDMatrix::StarGuideIII) {
      modecol=4;
    }
    for(int i=0;i<list_size;i++) {
      item=list_list_view->findItem(QString().sprintf("%03d",i+1),0);
      if(item->text(modecol).lower()==QString(tr("stereo"))) {
	mode=RDMatrix::Stereo;
      }
      if(item->text(modecol).lower()==QString(tr("left"))) {
	mode=RDMatrix::Left;
      }
      if(item->text(modecol).lower()==QString(tr("right"))) {
	mode=RDMatrix::Right;
      }
      sql=QString().sprintf("select ID from %s where\
                             STATION_NAME=\"%s\" && \
                             MATRIX=%d && \
                             NUMBER=%d",
			    (const char *)list_table,
			    (const char *)list_matrix->station(),
			    list_matrix->matrix(),
			    i+1);
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	delete q;
	sql=QString().sprintf("insert into %s set STATION_NAME=\"%s\",\
                               MATRIX=%d,\
                               NUMBER=%d,\
                               NAME=\"%s\"",
			      (const char *)list_table,
			      (const char *)list_matrix->station(),
			      list_matrix->matrix(),
			      i+1,
			      (const char *)item->text(1));
	switch(list_matrix->type()) {
	  case RDMatrix::Unity4000:
	    if(list_endpoint==RDMatrix::Input) {
	      sql+=QString().sprintf(",FEED_NAME=\"%s\",CHANNEL_MODE=%d",
				     (const char *)item->text(2),
				     mode);
	    }
	    break;
	    
	  case RDMatrix::LogitekVguest:
	    if(item->text(2).isEmpty()) {
	      sql+=",ENGINE_NUM=-1";
	    }
	    else {
	      sql+=QString().sprintf(",ENGINE_NUM=%d",
				     item->text(2).toInt(NULL,16));
	    }
	    if(item->text(3).isEmpty()) {
	      sql+=",DEVICE_NUM=-1";
	    }
	    else {
	      sql+=QString().sprintf(",DEVICE_NUM=%d",
				     item->text(3).toInt(NULL,16));
	    }
	    break;
	    
	  case RDMatrix::StarGuideIII:
	    if(item->text(2).isEmpty()) {
	      sql+=",ENGINE_NUM=-1";
	    }
	    else {
	      sql+=QString().sprintf(",ENGINE_NUM=%d",
				     item->text(2).toInt());
	    }
	    if(item->text(3).isEmpty()) {
	      sql+=",DEVICE_NUM=-1";
	    }
	    else {
	      sql+=QString().sprintf(",DEVICE_NUM=%d",
				     item->text(3).toInt());
	    }
	    if(list_endpoint==RDMatrix::Input) {
	      sql+=QString().sprintf(",CHANNEL_MODE=%d",mode);
	    }
	    break;
	    
	  default:
	    break;
	}
	q=new RDSqlQuery(sql);
	delete q;
      }
      else {
	delete q;
	switch(list_matrix->type()) {
	  case RDMatrix::Unity4000:
	    if(list_endpoint==RDMatrix::Input) {
	      sql=QString().sprintf("update %s set NAME=\"%s\",\
                                     FEED_NAME=\"%s\",\
                                     CHANNEL_MODE=%d where\
                                     STATION_NAME=\"%s\" && \
                                     MATRIX=%d && \
                                     NUMBER=%d",
				    (const char *)list_table,
				    (const char *)item->text(1),
				    (const char *)item->text(2),
				    mode,
				    (const char *)list_matrix->station(),
				    list_matrix->matrix(),
				    i+1);
	    }
	    else {
	      sql=QString().sprintf("update %s set NAME=\"%s\" where \
                                     STATION_NAME=\"%s\" && \
                                     MATRIX=%d && \
                                     NUMBER=%d",
				    (const char *)list_table,
				    (const char *)item->text(1),
				    (const char *)list_matrix->station(),
				    list_matrix->matrix(),
				    i+1);
	    }
	    break;
	    
	  case RDMatrix::LogitekVguest:
	    if(item->text(2).isEmpty()) {
	      enginenum=-1;
	    }
	    else {
	      enginenum=item->text(2).toInt(NULL,16);
	    }
	    if(item->text(3).isEmpty()) {
	      devicenum=-1;
	    }
	    else {
	      devicenum=item->text(3).toInt(NULL,16);
	    }
	    sql=QString().sprintf("update %s set NAME=\"%s\",\
                                   ENGINE_NUM=%d,DEVICE_NUM=%d where\
                                   STATION_NAME=\"%s\" && \
                                   MATRIX=%d && \
                                   NUMBER=%d",
				  (const char *)list_table,
				  (const char *)item->text(1),
				  enginenum,
				  devicenum,
				  (const char *)list_matrix->station(),
				  list_matrix->matrix(),
				  i+1);
	    break;
	    
	  case RDMatrix::StarGuideIII:
	    if(list_endpoint==RDMatrix::Input) {
	      if(item->text(2).isEmpty()) {
		enginenum=-1;
	      }
	      else {
		enginenum=item->text(2).toInt(NULL);
	      }
	      if(item->text(3).isEmpty()) {
		devicenum=-1;
	      }
	      else {
		devicenum=item->text(3).toInt(NULL);
	      }
	      sql=QString().sprintf("update %s set NAME=\"%s\",\
                                     ENGINE_NUM=%d,DEVICE_NUM=%d,\
                                     CHANNEL_MODE=%d where\
                                     STATION_NAME=\"%s\" && \
                                     MATRIX=%d && \
                                     NUMBER=%d",
				    (const char *)list_table,
				    (const char *)item->text(1),
				    enginenum,
				    devicenum,
				    mode,
				    (const char *)list_matrix->station(),
				    list_matrix->matrix(),
				    i+1);
	    }
	    break;
	    
	  default:
	    sql=QString().sprintf("update %s set NAME=\"%s\" where\
                                   STATION_NAME=\"%s\" && \
                                   MATRIX=%d && \
                                   NUMBER=%d",
				  (const char *)list_table,
				  (const char *)item->text(1),
				  (const char *)list_matrix->station(),
				  list_matrix->matrix(),
				  i+1);
	    break;
	}
	q=new RDSqlQuery(sql);
	delete q;
      }
    }
  }
  done(0);
}


void ListEndpoints::cancelData()
{
  done(1);
}
