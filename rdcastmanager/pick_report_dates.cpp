// pick_report_date.cpp
//
// Select a Set of Dates for a Rivendell Report
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pick_report_dates.cpp,v 1.5 2011/09/07 13:44:59 cvs Exp $
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
#include <qmessagebox.h>
#include <qfile.h>

#include <rddatedialog.h>
#include <rdreport.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdtextfile.h>
#include <rdfeed.h>
#include <rdpodcast.h>

#include <globals.h>
#include <pick_report_dates.h>


PickReportDates::PickReportDates(unsigned feed_id,unsigned cast_id,
				 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QDate yesterday_date=QDate::currentDate().addDays(-1);

  edit_cast_id=feed_id;
  edit_cast_id=cast_id;
  setCaption(tr("Select Report Dates"));
	     
  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%d",feed_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_keyname=q->value(0).toString();
  }
  delete q;

  //
  // Fix the Window Size
  //
  setMaximumWidth(sizeHint().width());
  setMaximumHeight(sizeHint().height());
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Start Date
  //
  edit_startdate_edit=new QDateEdit(this,"edit_startdate_edit");
  edit_startdate_edit->setGeometry(150,10,100,20);
  edit_startdate_edit->setDate(yesterday_date.addMonths(-1));
  QLabel *label=new QLabel(edit_startdate_edit,tr("&Start Date:"),
			   this,"edit_startdate_label");
  label->setGeometry(75,10,70,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"startdate_button");
  button->setGeometry(260,7,50,27);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectStartDateData()));

  //
  // End Date
  //
  edit_enddate_edit=new QDateEdit(this,"edit_enddate_edit");
  edit_enddate_edit->setGeometry(150,40,100,20);
  edit_enddate_edit->setDate(yesterday_date);
  label=new QLabel(edit_enddate_edit,tr("&End Date:"),
				      this,"edit_enddate_label");
  label->setGeometry(75,40,70,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"enddate_button");
  button->setGeometry(260,37,50,27);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectEndDateData()));

  //
  //  Generate Button
  //
  button=new QPushButton(this,"list_purge_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Generate\nReport"));
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("C&lose"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


PickReportDates::~PickReportDates()
{
}


QSize PickReportDates::sizeHint() const
{
  return QSize(400,134);
}


QSizePolicy PickReportDates::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

  
void PickReportDates::selectStartDateData()
{
  RDDateDialog *dialog=new RDDateDialog(2002,QDate::currentDate().year(),this);
  QDate date=edit_startdate_edit->date();
  if(dialog->exec(&date)<0) {
    delete dialog;
    return;
  }
  edit_startdate_edit->setDate(date);
  edit_enddate_edit->setDate(date);
  delete dialog;
}


void PickReportDates::selectEndDateData()
{
  RDDateDialog *dialog=new RDDateDialog(2002,QDate::currentDate().year(),this);
  QDate date=edit_enddate_edit->date();
  if(dialog->exec(&date)<0) {
    delete dialog;
    return;
  }
  edit_enddate_edit->setDate(date);
  delete dialog;
}


void PickReportDates::generateData()
{
  QString report;
  if(edit_cast_id>0) {
    GenerateEpisodeReport(edit_keyname,edit_cast_id,&report);
  }
  else {
    GenerateSubscriptionReport(edit_keyname,&report);
  }
  RDTextFile(report);
}


void PickReportDates::closeData()
{
  done(0);
}


void PickReportDates::GenerateSubscriptionReport(const QString &keyname,
						 QString *rpt)
{
  QString sql;
  RDSqlQuery *q;

  RDFeed *feed=new RDFeed(keyname,this);
  unsigned total=0;
  unsigned rss_total=0;
  unsigned audio_total=0;

  //
  // Header
  //
  *rpt+="                      Rivendell Podcast Subscription Report\n";
  QString title=feed->channelTitle();
  for(unsigned i=0;i<((80-title.length())/2);i++) {
    *rpt+=" ";
  }
  *rpt+=title;
  *rpt+="\n\n";
  *rpt+="                                          ----- Downloads -----\n";
  *rpt+="                  Date                    RSS             Audio\n";
  *rpt+="                  ---------------------------------------------\n";

  //
  // Data Rows
  //
  QString keyname_esc=keyname;
  keyname_esc.replace(" ","_");
  sql=QString().sprintf("select ACCESS_DATE,ACCESS_COUNT,CAST_ID from %s_FLG \
                         where (ACCESS_DATE>=\"%s\")&&(ACCESS_DATE<=\"%s\") \
                         order by ACCESS_DATE,CAST_ID desc",
			(const char *)keyname_esc,
			(const char *)edit_startdate_edit->date().
			toString("yyyy-MM-dd"),
			(const char *)edit_enddate_edit->date().
			toString("yyyy-MM-dd"));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(2).toUInt()==0) {
      *rpt+=QString().sprintf("                  %s         %9u        %9u\n",
			      (const char *)q->value(0).toDate().
			      toString("MM/dd/yyyy"),
			      q->value(1).toUInt(),total);
      total=0;
      rss_total+=q->value(1).toUInt();
    }
    else {
      total+=q->value(1).toUInt();
      audio_total+=q->value(1).toUInt();
    }
  }
  delete q;
  *rpt+=QString().sprintf("                                  ------------     ------------\n");
  *rpt+=QString().sprintf("                                     %9u        %9u\n",
			  rss_total,audio_total);

  delete feed;
}


void PickReportDates::GenerateEpisodeReport(const QString &keyname,
					    unsigned cast_id,QString *rpt)
{
  QString sql;
  RDSqlQuery *q;

  RDFeed *feed=new RDFeed(keyname,this);
  RDPodcast *cast=new RDPodcast(cast_id);

  //
  // Header
  //
  *rpt+="                       Rivendell Podcast Episode Report\n";
  QString channel_title=feed->channelTitle();
  for(unsigned i=0;i<((80-channel_title.length())/2);i++) {
    *rpt+=" ";
  }
  *rpt+=channel_title;
  *rpt+="\n";
  QString item_title=cast->itemTitle();
  for(unsigned i=0;i<((80-item_title.length())/2);i++) {
    *rpt+=" ";
  }
  *rpt+=item_title;
  *rpt+="\n";
  *rpt+=QString().sprintf("                       Posted on %s at %s\n\n",
			  (const char *)cast->originDateTime().
			  toString("MM/dd/yyyy"),
			  (const char *)cast->originDateTime().
			  toString("hh:mm:ss"));
  *rpt+="                       Date                   Downloads\n";
  *rpt+="                       --------------------------------\n";

  //
  // Data Rows
  //
  unsigned total=0;
  QString keyname_esc=keyname;
  keyname_esc.replace(" ","_");
  sql=QString().sprintf("select ACCESS_DATE,ACCESS_COUNT from %s_FLG \
                         where (ACCESS_DATE>=\"%s\")&&(ACCESS_DATE<=\"%s\")&& \
                         (CAST_ID=%u) order by ACCESS_DATE",
			(const char *)keyname_esc,
			(const char *)edit_startdate_edit->date().
			toString("yyyy-MM-dd"),
			(const char *)edit_enddate_edit->date().
			toString("yyyy-MM-dd"),
			cast_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    *rpt+=QString().sprintf("                       %s             %9u\n",
			    (const char *)q->value(0).toDate().
			    toString("MM/dd/yyyy"),
			    q->value(1).toUInt());
    total+=q->value(1).toUInt();
  }
  delete q;

  *rpt+=QString().
    sprintf("                                           ------------\n");
  *rpt+=QString().
    sprintf("                                              %9u\n",total);

  delete cast;
  delete feed;
}
