// edit_cast.cpp
//
// Edit a Rivendell Cast
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

#include <qmessagebox.h>

#include <rdconf.h>
#include <rddatedialog.h>

#include "edit_cast.h"
#include "globals.h"

EditCast::EditCast(unsigned cast_id,QWidget *parent)
  : RDDialog(parent)
{
  cast_cast=new RDPodcast(rda->config(),cast_id);
  cast_feed=new RDFeed(cast_cast->feedId(),rda->config());
  cast_schema=cast_feed->rssSchema();
  cast_status=cast_cast->status();

  //
  // Active Checkbox
  //
  cast_active_check=new QCheckBox(this);
  cast_active_label=
    new QLabel(cast_active_check,tr("Item Active"),this);
  cast_active_label->setFont(labelFont());
  cast_active_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Item Origin
  //
  cast_item_origin_label=new QLabel(tr("Posted By")+":",this);
  cast_item_origin_label->setFont(labelFont());
  cast_item_origin_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_origin_edit=new QLineEdit(this);
  cast_item_origin_edit->setReadOnly(true);

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
  cast_item_link_edit->
    setVisible(rda->rssSchemas()->supportsItemLinks(cast_schema));
  cast_item_link_label=
    new QLabel(cast_item_link_edit,tr("Link URL:"),this);
  cast_item_link_label->setFont(labelFont());
  cast_item_link_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_link_label->
    setVisible(rda->rssSchemas()->supportsItemLinks(cast_schema));
  

  //
  // Item Comments
  //
  cast_item_comments_edit=new QLineEdit(this);
  cast_item_comments_edit->setMaxLength(64);
  cast_item_comments_edit->
    setVisible(rda->rssSchemas()->supportsItemComments(cast_schema));
  cast_item_comments_label=
    new QLabel(cast_item_comments_edit,tr("Comments URL:"),this);
  cast_item_comments_label->setFont(labelFont());
  cast_item_comments_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_comments_label->
    setVisible(rda->rssSchemas()->supportsItemComments(cast_schema));

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
				     tr("Item contains explicit content"),this);
  cast_item_explicit_label->setFont(labelFont());
  cast_item_explicit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  cast_item_image_box=new RDImagePickerBox("FEED_IMAGES","FEED_ID","ID",this);
  cast_item_image_label=new QLabel(cast_item_image_box,tr("Image")+":",this);
  cast_item_image_label->setFont(labelFont());
  cast_item_image_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Effective DateTime
  //
  cast_item_effective_edit=new QDateTimeEdit(this);
  cast_item_effective_edit->setDisplayFormat("MM/dd/yyyy hh:mm:ss");
  cast_item_effective_label=
    new QLabel(cast_item_effective_edit,tr("Air Date/Time:"),this);
  cast_item_effective_label->setFont(labelFont());
  cast_item_effective_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_effective_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_effective_button=new QPushButton(this);
  cast_item_effective_button->setFont(subButtonFont());
  cast_item_effective_button->setText(tr("&Select Date"));
  connect(cast_item_effective_button,SIGNAL(clicked()),
	  this,SLOT(effectiveSelectData()));

  //
  // Item Expiration
  //
  cast_item_expiration_box=new QComboBox(this);
  cast_item_expiration_box->insertItem(tr("No"));
  cast_item_expiration_box->insertItem(tr("Yes"));
  connect(cast_item_expiration_box,SIGNAL(activated(int)),
	  this,SLOT(expirationSelectedData(int)));
  cast_item_expiration_box_label=
    new QLabel(cast_item_expiration_box,tr("Item Expires")+":",this);
  cast_item_expiration_box_label->setFont(labelFont());
  cast_item_expiration_box_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cast_item_expiration_box->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_box_label->
    setEnabled(cast_status!=RDPodcast::StatusExpired);

  cast_item_expiration_edit=new QDateTimeEdit(this);
  cast_item_expiration_edit->setDisplayFormat("MM/dd/yyyy hh:mm:ss");
  cast_item_expiration_label=
    new QLabel(cast_item_expiration_edit,tr("at"),this);
  cast_item_expiration_label->setFont(labelFont());
  cast_item_expiration_label->setAlignment(Qt::AlignCenter);
  cast_item_expiration_button=new QPushButton(this);
  cast_item_expiration_button->setFont(subButtonFont());
  cast_item_expiration_button->setText(tr("&Select Date"));
  connect(cast_item_expiration_button,SIGNAL(clicked()),
	  this,SLOT(expirationSelectData()));
  cast_item_expiration_edit->setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_label->
    setEnabled(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_button->
    setEnabled(cast_status!=RDPodcast::StatusExpired);

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
  setWindowTitle("RDCastManager - "+tr("Editing Item")+
		 +"  [Cast ID: "+QString().sprintf("%u",cast_cast->id())+"]");
  cast_item_title_edit->setText(cast_cast->itemTitle());
  cast_item_author_edit->setText(cast_cast->itemAuthor());
  if(cast_cast->originLoginName().isEmpty()) {
    cast_item_origin_edit->
      setText(tr("unknown")+" "+tr("at")+" "+
	      cast_cast->originDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
  }
  else {
  cast_item_origin_edit->
    setText(cast_cast->originLoginName()+" "+tr("on")+" "+
	    cast_cast->originStation()+" "+tr("at")+" "+
	    cast_cast->originDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
  }

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
  cast_item_effective_edit->setDateTime(cast_cast->effectiveDateTime());
  if(!cast_cast->expirationDateTime().isNull()) {
    cast_item_expiration_box->setCurrentItem(1);
  }
  cast_item_expiration_edit->setDateTime(cast_cast->expirationDateTime());
  cast_item_expiration_edit->
    setEnabled(cast_item_expiration_box->currentItem());
  cast_item_expiration_button->
    setEnabled(cast_item_expiration_box->currentItem());
  cast_item_expiration_label->
    setEnabled(cast_item_expiration_box->currentItem());

  switch(cast_status) {
  case RDPodcast::StatusActive:
    cast_active_check->setChecked(true);
    break;

  case RDPodcast::StatusPending:
    cast_active_check->setChecked(false);
    break;

  case RDPodcast::StatusExpired:
    cast_active_check->setDisabled(true);
    cast_active_label->setDisabled(true);
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
  return QSize(640,430);
} 


QSizePolicy EditCast::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditCast::effectiveSelectData()
{
  QDate current_date=QDate::currentDate();
  QDate date=cast_item_effective_edit->date();

  RDDateDialog *dd=
    new RDDateDialog(current_date.year(),current_date.year()+10,this);
  if(dd->exec(&date)==0) {
    cast_item_effective_edit->setDate(date);
  }
  delete dd;
}


void EditCast::expirationSelectedData(int state)
{
  state=state&&(cast_status!=RDPodcast::StatusExpired);
  cast_item_expiration_edit->setEnabled(state);
  cast_item_expiration_button->setEnabled(state);
  cast_item_expiration_label->setEnabled(state);
  if(state) {
    cast_item_expiration_edit->setDate(QDate::currentDate().addDays(1));
  }
}


void EditCast::expirationSelectData()
{
  QDate current_date=QDate::currentDate();
  QDate date=cast_item_expiration_edit->date();

  RDDateDialog *dd=
    new RDDateDialog(1970,current_date.year()+10,this);
  if(dd->exec(&date)==0) {
    cast_item_expiration_edit->setDate(date);
  }
  delete dd;
}


void EditCast::okData()
{
  QString err_msg;

  //
  // Sanity Checks
  //
  if(cast_item_expiration_box->currentItem()) {
    if(cast_item_effective_edit->dateTime()>
      cast_item_expiration_edit->dateTime()) {
      QMessageBox::warning(this,"RDCastManager - "+tr("Error"),
		       tr("Item expiration cannot be prior to Air Date/Time!"));
      return;
    }
    if(cast_item_expiration_edit->dateTime()<QDateTime::currentDateTime()) {
      QMessageBox::warning(this,"RDCastManager - "+tr("Error"),
			   tr("Item expiration must be in the future!"));
      return;
    }
  }

  cast_cast->setItemTitle(cast_item_title_edit->text());
  cast_cast->setItemAuthor(cast_item_author_edit->text());
  cast_cast->setItemCategory(cast_item_category_edit->text());
  cast_cast->setItemLink(cast_item_link_edit->text());
  cast_cast->setItemDescription(cast_item_description_edit->text());
  cast_cast->setItemExplicit(cast_item_explicit_check->isChecked());
  cast_cast->setItemImageId(cast_item_image_box->currentImageId());
  cast_cast->setItemComments(cast_item_comments_edit->text());
  cast_cast->setEffectiveDateTime(cast_item_effective_edit->dateTime());
  if(cast_active_check->isEnabled()) {
    if(cast_item_expiration_box->currentItem()) {
      int shelf_life=cast_cast->originDateTime().date().
	daysTo(cast_item_expiration_edit->date());
      if(shelf_life<1) {
	shelf_life=1;
      }
      cast_cast->setExpirationDateTime(cast_item_expiration_edit->dateTime());
    }
    else {
      cast_cast->setExpirationDateTime(QDateTime());
    }
    if(cast_active_check->isChecked()) {
      cast_cast->setStatus(RDPodcast::StatusActive);
    }
    else {
      cast_cast->setStatus(RDPodcast::StatusPending);
    }
  }

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
  int w=size().width();
  int h=size().height();

  ypos=2;

  //
  // Posting Status
  //
  cast_active_check->setGeometry(10,ypos+2,15,15);
  cast_active_label->setGeometry(30,ypos,300,20);

  //
  // Posted At
  //
  cast_item_origin_label->setGeometry(w-445,ypos,110,20);
  cast_item_origin_edit->setGeometry(w-330,ypos,320,20);
  ypos+=27;

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
  if(rda->rssSchemas()->supportsItemLinks(cast_schema)) {
    cast_item_link_edit->setGeometry(135,ypos,size().width()-145,20);
    cast_item_link_label->setGeometry(20,ypos,110,20);
    ypos+=22;
  }

  //
  // Comments URL
  //
  if(rda->rssSchemas()->supportsItemComments(cast_schema)) {
    cast_item_comments_edit->setGeometry(135,ypos,size().width()-145,20);
    cast_item_comments_label->setGeometry(10,ypos,120,20);
    ypos+=22;
  }

  //
  // Description
  //
  cast_item_description_label->setGeometry(20,ypos,110,20);
  cast_item_description_edit->
    setGeometry(135,ypos,size().width()-145,h-ypos-221);

  //
  // Explicit Content
  //
  cast_item_explicit_check->setGeometry(140,h-216,15,15);
  cast_item_explicit_label->setGeometry(160,h-219,size().width()-145,20);

  //
  // Image
  //
  cast_item_image_label->setGeometry(20,h-197,110,20);
  cast_item_image_box->setIconSize(QSize(36,36));
  cast_item_image_box->setGeometry(135,h-197,300,38);

  //
  // Air Date/Time
  //
  cast_item_effective_label->setGeometry(20,h-154,110,20);
  cast_item_effective_edit->setGeometry(135,h-154,150,20);
  cast_item_effective_button->setGeometry(295,h-156,75,24);

  //
  // Cast Expiration
  //
  cast_item_expiration_box_label->setGeometry(20,h-126,110,20);
  cast_item_expiration_box->setGeometry(135,h-126,50,20);

  cast_item_expiration_label->setGeometry(190,h-126,20,20);
  cast_item_expiration_edit->setGeometry(215,h-126,150,20);
  cast_item_expiration_button->setGeometry(375,h-128,75,24);

  //
  // Buttons
  //
  cast_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  cast_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
