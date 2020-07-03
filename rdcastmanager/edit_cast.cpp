// edit_cast.cpp
//
// Edit a Rivendell Cast
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

#include <rdconf.h>
#include <rddatedialog.h>

#include "edit_cast.h"
#include "globals.h"
#include "pick_report_dates.h"

EditCast::EditCast(unsigned cast_id,QWidget *parent)
  : RDDialog(parent)
{
  cast_cast=new RDPodcast(rda->config(),cast_id);
  cast_feed=new RDFeed(cast_cast->feedId(),rda->config());
  cast_status=cast_cast->status();
  setWindowTitle("RDCastManager - "+tr("Editing PodCast"));

  //
  // Item Media Link
  //
  cast_item_medialink_edit=new QLineEdit(this);
  cast_item_medialink_edit->setReadOnly(true);
  cast_item_medialink_label=
    new QLabel(cast_item_medialink_edit,tr("Media Link:"),this);
  cast_item_medialink_label->setFont(labelFont());
  cast_item_medialink_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  if(cast_feed->mediaLinkMode()==RDFeed::LinkNone) {
    cast_item_medialink_edit->hide();
    cast_item_medialink_label->hide();
  }

  //
  // Item Title
  //
  cast_item_title_edit=new QLineEdit(this);
  cast_item_title_edit->setMaxLength(255);
  cast_item_title_label=
    new QLabel(cast_item_title_edit,tr("Title:"),this);
  cast_item_title_label->setFont(labelFont());
  cast_item_title_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Author
  //
  cast_item_author_edit=new QLineEdit(this);
  cast_item_author_edit->setMaxLength(255);
  cast_item_author_label=
    new QLabel(cast_item_author_edit,tr("Author E-Mail:"),this);
  cast_item_author_label->setFont(labelFont());
  cast_item_author_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Category
  //
  cast_item_category_edit=new QLineEdit(this);
  cast_item_category_edit->setMaxLength(64);
  cast_item_category_label=
    new QLabel(cast_item_category_edit,tr("Category:"),this);
  cast_item_category_label->setFont(labelFont());
  cast_item_category_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Link
  //
  cast_item_link_edit=new QLineEdit(this);
  cast_item_link_edit->setMaxLength(255);
  cast_item_link_label=
    new QLabel(cast_item_link_edit,tr("Link URL:"),this);
  cast_item_link_label->setFont(labelFont());
  cast_item_link_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Description
  //
  cast_item_description_edit=new QTextEdit(this);
  cast_item_description_label=
    new QLabel(cast_item_description_edit,tr("Description:"),this);
  cast_item_description_label->setFont(labelFont());
  cast_item_description_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Explicit
  //
  cast_item_explicit_check=new QCheckBox(this);
  cast_item_explicit_label=new QLabel(cast_item_explicit_check,
				     tr("Post contains explicit content"),this);
  cast_item_explicit_label->setFont(labelFont());
  cast_item_explicit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  cast_item_image_box=new RDImagePickerBox("FEED_IMAGES","FEED_ID","ID",this);
  cast_item_image_label=new QLabel(cast_item_image_box,tr("Image")+":",this);
  cast_item_image_label->setFont(labelFont());
  cast_item_image_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Comments
  //
  cast_item_comments_edit=new QLineEdit(this);
  cast_item_comments_edit->setMaxLength(64);
  cast_item_comments_label=
    new QLabel(cast_item_comments_edit,tr("Comments URL:"),this);
  cast_item_comments_label->setFont(labelFont());
  cast_item_comments_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Effective DateTime
  //
  cast_item_effective_edit=new QDateTimeEdit(this);
  cast_item_effective_label=
    new QLabel(cast_item_effective_edit,tr("Air Date/Time:"),this);
  cast_item_effective_label->setFont(labelFont());
  cast_item_effective_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_effective_button=new QPushButton(this);
  cast_item_effective_button->setFont(subButtonFont());
  cast_item_effective_button->setText(tr("&Select"));
  connect(cast_item_effective_button,SIGNAL(clicked()),
	  this,SLOT(effectiveSelectData()));

  //
  // Item Origin
  //
  cast_item_origin_edit=new QLineEdit(this);
  cast_item_origin_edit->setReadOnly(true);
  cast_item_origin_edit->setMaxLength(64);
  cast_item_origin_label=
    new QLabel(cast_item_origin_edit,tr("Posted At:"),this);
  cast_item_origin_label->setFont(labelFont());
  cast_item_origin_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item Expiration
  //
  cast_item_expiration_box=new QComboBox(this);
  cast_item_expiration_box->insertItem(tr("No"));
  cast_item_expiration_box->insertItem(tr("Yes"));
  connect(cast_item_expiration_box,SIGNAL(activated(int)),
	  this,SLOT(expirationSelectedData(int)));
  cast_item_expiration_box_label=
    new QLabel(cast_item_expiration_box,tr("Cast Expires:"),this);
  cast_item_expiration_box_label->setFont(labelFont());
  cast_item_expiration_box_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_expiration_box->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_box_label->setEnabled(cast_status!=RDPodcast::StatusExpired);

  cast_item_expiration_edit=new QDateEdit(this);
  cast_item_expiration_label=
    new QLabel(cast_item_expiration_edit,tr("Expires On:"),this);
  cast_item_expiration_label->setFont(labelFont());
  cast_item_expiration_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_expiration_button=new QPushButton(this);
  cast_item_expiration_button->setFont(subButtonFont());
  cast_item_expiration_button->setText(tr("&Select"));
  connect(cast_item_expiration_button,SIGNAL(clicked()),
	  this,SLOT(expirationSelectData()));
  cast_item_expiration_edit->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_label->
    setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_button->
    setEnabled(cast_status!=RDPodcast::StatusExpired);

  //
  // Cast Status
  //
  cast_item_status_group=new QButtonGroup(this);
  cast_item_status_group->setExclusive(true);

  cast_hold_rbutton=new QRadioButton(this);
  cast_item_status_group->addButton(cast_hold_rbutton,0);
  cast_hold_label=new QLabel(cast_hold_rbutton,tr("Hold"),this);
  cast_hold_label->setFont(subButtonFont());
  cast_hold_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  cast_hold_rbutton->setChecked(true);
  cast_hold_label->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_hold_rbutton->setEnabled(cast_status!=RDPodcast::StatusExpired);

  cast_active_rbutton=new QRadioButton(this);
  cast_item_status_group->addButton(cast_active_rbutton,1);
  cast_active_label=new QLabel(cast_active_rbutton,tr("Active"),this);
  cast_active_label->setFont(subLabelFont());
  cast_active_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  cast_active_label->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_status_label=new QLabel(tr("Posting Status:"),this);
  cast_status_label->setFont(labelFont());
  cast_status_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_status_label->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_active_rbutton->setEnabled(cast_status!=RDPodcast::StatusExpired);

  //
  //  Report Button
  //
  cast_report_button=new QPushButton(this);
  cast_report_button->setFont(buttonFont());
  cast_report_button->setText(tr("Episode\n&Report"));
  cast_report_button->setEnabled(cast_feed->audienceMetrics());
  connect(cast_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  //  Ok Button
  //
  cast_ok_button=new QPushButton(this);
  cast_ok_button->setDefault(true);
  cast_ok_button->setFont(buttonFont());
  cast_ok_button->setText(tr("&OK"));
  connect(cast_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  cast_cancel_button=new QPushButton(this);
  cast_cancel_button->setFont(buttonFont());
  cast_cancel_button->setText(tr("&Cancel"));
  connect(cast_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //******** END ***********
  //
  // Populate Values
  //
  cast_item_medialink_edit->
    setText(cast_feed->audioUrl(cast_feed->mediaLinkMode(),
				"[web-hostname]",cast_cast->id()));
  cast_item_title_edit->setText(cast_cast->itemTitle());
  cast_item_author_edit->setText(cast_cast->itemAuthor());
  cast_item_origin_edit->setText(RDUtcToLocal(cast_cast->originDateTime()).
				 toString("MM/dd/yyyy - hh:mm:ss"));
  cast_item_category_edit->setText(cast_cast->itemCategory());
  cast_item_category_label->
    setVisible(rda->rssSchemas()->
	       supportsItemCategories(cast_feed->rssSchema()));
  cast_item_category_edit->
    setVisible(rda->rssSchemas()->
	       supportsItemCategories(cast_feed->rssSchema()));
  cast_item_link_edit->setText(cast_cast->itemLink());
  cast_item_description_edit->setText(cast_cast->itemDescription());
  cast_item_explicit_check->setChecked(cast_cast->itemExplicit());
  cast_item_image_box->setCategoryId(cast_feed->id());
  cast_item_image_box->setCurrentImageId(cast_cast->itemImageId());
  cast_item_comments_edit->setText(cast_cast->itemComments());
  cast_item_effective_edit->
    setDateTime(RDUtcToLocal(cast_cast->effectiveDateTime()));
  if(cast_cast->shelfLife()>0) {
    cast_item_expiration_box->setCurrentItem(1);
  }
  cast_item_expiration_edit->
    setDate(RDUtcToLocal(cast_cast->originDateTime()).date().
	    addDays(cast_cast->shelfLife()));
  expirationSelectedData(cast_item_expiration_box->currentItem());
  switch(cast_status) {
  case RDPodcast::StatusActive:
    cast_item_status_group->button(1)->setChecked(true);
    break;

  case RDPodcast::StatusPending:
    cast_item_status_group->button(0)->setChecked(true);
    break;

  case RDPodcast::StatusExpired:
    cast_item_status_group->button(0)->setDisabled(true);
    cast_item_status_group->button(1)->setDisabled(true);
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
}


EditCast::~EditCast()
{
  delete cast_feed;
  delete cast_cast;
}


QSize EditCast::sizeHint() const
{
  return QSize(640,442);
} 


QSizePolicy EditCast::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditCast::expirationSelectedData(int state)
{
  state=state&&(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_edit->setEnabled(state);
  cast_item_expiration_button->setEnabled(state);
  cast_item_expiration_label->setEnabled(state);
}


void EditCast::effectiveSelectData()
{
  QDate current_date=QDate::currentDate();
  QDateTime datetime=cast_item_effective_edit->dateTime();
  QDate date=datetime.date();

  RDDateDialog *dd=
    new RDDateDialog(current_date.year()-5,current_date.year()+5,this);
  if(dd->exec(&date)==0) {
    datetime.setDate(date);
    cast_item_effective_edit->setDateTime(datetime);
  }
  delete dd;
}


void EditCast::expirationSelectData()
{
  QDate current_date=QDate::currentDate();
  QDate date=cast_item_expiration_edit->date();
    
  RDDateDialog *dd=
    new RDDateDialog(current_date.year(),current_date.year()+10,this);
  if(dd->exec(&date)==0) {
    cast_item_expiration_edit->setDate(date);
  }
  delete dd;
}


void EditCast::reportData()
{
  PickReportDates *rd=new PickReportDates(cast_cast->feedId(),cast_cast->id());
  rd->exec();
  delete rd;
}


void EditCast::okData()
{
  QString err_msg;

  cast_cast->setItemTitle(cast_item_title_edit->text());
  cast_cast->setItemAuthor(cast_item_author_edit->text());
  cast_cast->setItemCategory(cast_item_category_edit->text());
  cast_cast->setItemLink(cast_item_link_edit->text());
  cast_cast->setItemDescription(cast_item_description_edit->text());
  cast_cast->setItemExplicit(cast_item_explicit_check->isChecked());
  cast_cast->setItemImageId(cast_item_image_box->currentImageId());
  cast_cast->setItemComments(cast_item_comments_edit->text());
  cast_cast->
    setEffectiveDateTime(RDLocalToUtc(cast_item_effective_edit->dateTime()));
  if(cast_item_status_group->button(0)->isEnabled()) {
    if(cast_item_expiration_box->currentItem()) {
      int shelf_life=RDUtcToLocal(cast_cast->originDateTime()).date().
	daysTo(cast_item_expiration_edit->date());
      if(shelf_life<1) {
	shelf_life=1;
      }
      cast_cast->setShelfLife(shelf_life);
    }
    else {
      cast_cast->setShelfLife(0);
    }
    switch(cast_item_status_group->checkedId()) {
      case 0:
	cast_cast->setStatus(RDPodcast::StatusPending);
	break;
	
      case 1:
	cast_cast->setStatus(RDPodcast::StatusActive);
	break;
    }
  }

  cast_feed->
    setLastBuildDateTime(RDLocalToUtc(QDateTime(QDate::currentDate(),
						QTime::currentTime())));

  if(!cast_feed->postXmlConditional("RDCastManager",this)) {
    return;
  }
  done(0);
}


void EditCast::cancelData()
{
  done(-1);
}


void EditCast::resizeEvent(QResizeEvent *e)
{
  int ypos=0;
  int h=size().height();

  cast_item_medialink_edit->setGeometry(135,10,size().width()-145,20);
  cast_item_medialink_label->setGeometry(20,10,110,20);
  if(cast_feed->mediaLinkMode()==RDFeed::LinkNone) {
    ypos=10;
  }
  else {
    ypos=42;
  }

  //
  // Title
  //
  cast_item_title_edit->setGeometry(135,ypos,size().width()-145,20);
  cast_item_title_label->setGeometry(20,ypos,110,20);
  ypos+=22;

  //
  // Author E-Mail
  //
  cast_item_author_edit->setGeometry(135,ypos,size().width()-145,20);
  cast_item_author_label->setGeometry(20,ypos,110,20);
  ypos+=22;

  //
  // Category
  //
  if(rda->rssSchemas()->supportsItemCategories(cast_feed->rssSchema())) {
    cast_item_category_edit->setGeometry(135,ypos,size().width()-145,20);
    cast_item_category_label->setGeometry(20,ypos,110,20);
    ypos+=22;
  }

  //
  // Link URL
  //
  cast_item_link_edit->setGeometry(135,ypos,size().width()-145,20);
  cast_item_link_label->setGeometry(20,ypos,110,20);
  ypos+=22;

  //
  // Comments URL
  //
  cast_item_comments_edit->setGeometry(135,ypos,size().width()-145,20);
  cast_item_comments_label->setGeometry(10,ypos,120,20);
  ypos+=22;

  //
  // Description
  //
  cast_item_description_label->setGeometry(20,ypos,110,20);
  cast_item_description_edit->
    setGeometry(135,ypos,size().width()-145,h-ypos-271);

  //
  // Explicit Content
  //
  cast_item_explicit_check->setGeometry(140,h-266,15,15);
  cast_item_explicit_label->setGeometry(160,h-269,size().width()-145,20);

  //
  // Image
  //
  cast_item_image_label->setGeometry(20,h-247,110,20);
  cast_item_image_box->setIconSize(QSize(36,36));
  cast_item_image_box->setGeometry(135,h-247,300,38);

  //
  // Air Date/Time
  //
  cast_item_effective_edit->setGeometry(135,h-207,165,20);
  cast_item_effective_label->setGeometry(20,h-207,110,20);
  cast_item_effective_button->setGeometry(310,h-207,50,20);

  //
  // Posted At
  //
  cast_item_origin_edit->setGeometry(135,h-185,165,20);
  cast_item_origin_label->setGeometry(20,h-185,110,20);

  //
  // Cast Expires
  //
  cast_item_expiration_box->setGeometry(135,h-163,50,20);
  cast_item_expiration_box_label->setGeometry(20,h-163,110,20);

  //
  // Expires On
  //
  cast_item_expiration_edit->setGeometry(135,h-141,95,20);
  cast_item_expiration_label->setGeometry(20,h-141,110,20);
  cast_item_expiration_button->setGeometry(240,h-141,50,20);

  //
  // Posting Status
  //
  cast_hold_rbutton->setGeometry(140,h-114,15,15);
  cast_hold_label->setGeometry(160,h-114,30,15);
  cast_active_rbutton->setGeometry(210,h-114,15,15);
  cast_active_label->setGeometry(230,h-114,80,15);
  cast_status_label->setGeometry(20,h-114,110,20);

  //
  // Buttons
  //
  cast_report_button->setGeometry(10,size().height()-60,80,50);
  cast_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  cast_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
