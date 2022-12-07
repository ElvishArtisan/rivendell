// rdpodcastfilter.cpp
//
// Filter widget for podcasts.
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QList>
#include <QResizeEvent>

#include "rdapplication.h"
#include "rdcart_search_text.h"
#include "rdescape_string.h"
#include "rdpodcast.h"
#include "rdpodcastfilter.h"

RDPodcastFilter::RDPodcastFilter(QWidget *parent)
  : RDWidget(parent)
{
  //
  // Filter Phrase
  //
  d_filter_edit=new QLineEdit(this);
  d_filter_label=new QLabel(tr("Filter:"),this);
  d_filter_label->setFont(labelFont());
  d_filter_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Show Audio Carts Checkbox
  //
  d_showactive_check=new QCheckBox(this);
  d_showactive_label=new QLabel(tr("Only Show Active Items"),this);
  d_showactive_label->setFont(labelFont());
  d_showactive_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showactive_check,SIGNAL(stateChanged(int)),
	  this,SLOT(checkChangedData(int)));
}


RDPodcastFilter::~RDPodcastFilter()
{
}


QSize RDPodcastFilter::sizeHint() const
{
  return QSize(640,57);
}


QSizePolicy RDPodcastFilter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}


QString RDPodcastFilter::filterSql() const
{
  return RDPodcastFilter::searchString(d_filter_edit->text(),
				       d_showactive_check->isChecked());
}


QString RDPodcastFilter::filterText() const
{
  return d_filter_edit->text();
}


void RDPodcastFilter::setFilterText(const QString &str)
{
  d_filter_edit->setText(str);
  filterChangedData(str);
}


QString RDPodcastFilter::searchString(const QString &filter,bool active_only)
{
  QString ret;
  if(!filter.trimmed().isEmpty()) {
    QString fil=RDEscapeString(filter);
    ret+=QString("&&((PODCASTS.ITEM_TITLE like \"%")+fil+"%\")||"+
      "(PODCASTS.ITEM_DESCRIPTION like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_CATEGORY like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_LINK like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_COMMENTS like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_AUTHOR like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_SOURCE_TEXT like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_SOURCE_URL like \"%"+fil+"%\"))";
  }
  if(active_only) {
    ret+=QString::asprintf("&&(PODCASTS.STATUS=%d)",RDPodcast::StatusActive);
  }
  return ret;
}


void RDPodcastFilter::filterChangedData(const QString &str)
{
  if(rda->config()->logSearchStrings()) {
    rda->syslog(rda->config()->logSearchStringsLevel(),
		"searching podcast items by string: \"%s\" [%s]",
		d_filter_edit->text().toUtf8().constData(),
		RDConfig::hexify(d_filter_edit->text()).toUtf8().constData());
  }
  emit filterChanged(filterSql());
}


void RDPodcastFilter::checkChangedData(int n)
{
  filterChangedData("");
}


void RDPodcastFilter::resizeEvent(QResizeEvent *e)
{
  d_filter_label->setGeometry(10,3,55,20);
  d_filter_edit->setGeometry(70,3,e->size().width()-80,20);
  d_showactive_check->setGeometry(75,28,15,15);
  d_showactive_label->setGeometry(95,26,size().width()-105,20);
}
