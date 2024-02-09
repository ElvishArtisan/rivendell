// messagewidget.cpp
//
// Message Widget for RDAirPlay Rivendell
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QEvent>
#include <QKeyEvent>
#include <QWebFrame>

#include <rdeventfilter.h>

#include "colors.h"
#include "messagewidget.h"

MessageWidget::MessageWidget(QWidget *parent)
  : QWidget(parent)
{
  setStyleSheet("background-color: "+
		QColor(LOGLINEBOX_BACKGROUND_COLOR).name());

  //
  // Generate Fonts
  //
  for(unsigned i=0;i<MESSAGE_FONT_QUANTITY;i++) {
    d_message_fonts[i]=QFont(font().family(),12+2*i,QFont::Normal);
    d_message_fonts[i].setPixelSize(12+2*i);
    d_message_metrics[i]=new QFontMetrics(d_message_fonts[i]);
  }

  //
  // Message Display
  //
  d_label=new QLabel(this);
  d_label->setWordWrap(true);
  d_label->setAlignment(Qt::AlignCenter);
  d_view=new QWebView(this);
  connect(d_view,SIGNAL(loadFinished(bool)),
	  this,SLOT(webLoadFinishedData(bool)));
  d_view->hide();
  RDEventFilter *filter=new RDEventFilter(this);
  filter->addFilter(QEvent::Enter);
  filter->addFilter(QEvent::Leave);
  filter->addFilter(QEvent::KeyPress);
  filter->addFilter(QEvent::KeyRelease);
  filter->addFilter(QEvent::MouseButtonPress);
  filter->addFilter(QEvent::MouseButtonRelease);
  filter->addFilter(QEvent::MouseButtonDblClick);
  filter->addFilter(QEvent::MouseMove);
  filter->addFilter(QEvent::Wheel);
  d_view->installEventFilter(filter);
}

  
void MessageWidget::setText(const QString &str,const QColor &col)
{
  QPalette pal=d_label->palette();

  pal.setColor(QPalette::Active,QPalette::Foreground,col);
  pal.setColor(QPalette::Inactive,QPalette::Foreground,col);
  d_label->setPalette(pal);
  d_label->setFont(MessageFont(str));
  d_label->setText(str);
  d_label->show();
  d_view->hide();
}


bool MessageWidget::setUrl(const QString &str)
{
  QUrl url(str);
  if(!url.isValid()) {
    setText(tr("invalid URL")+": "+str.toUtf8().constData(),Qt::black);
    return false;
  }
  if((url.scheme().toLower()!="http")&&
     (url.scheme().toLower()!="https")&&
     (url.scheme().toLower()!="file")) {
    setText(tr("unsupported URL scheme")+": "+str.toUtf8().constData(),
		   Qt::black);
    return false;
  }
  d_view->load(url);
  d_view->show();
  d_label->hide();

  return true;
}


void MessageWidget::clear()
{
  d_label->clear();
  d_label->show();
  d_view->hide();
}


void MessageWidget::webLoadFinishedData(bool state)
{
  d_view->page()->mainFrame()->
    setScrollBarPolicy(Qt::Horizontal,Qt::ScrollBarAlwaysOff);
  d_view->page()->mainFrame()->
    setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
}


void MessageWidget::resizeEvent(QResizeEvent *e)
{
  d_label->setGeometry(0,0,size().width(),size().height());
  d_view->setGeometry(0,0,size().width(),size().height());
}


QFont MessageWidget::MessageFont(QString str) const
{
  for(int i=(MESSAGE_FONT_QUANTITY-1);i>=0;i--) {
    if(d_message_metrics[i]->width(str)<MESSAGE_WIDGET_WIDTH) {
      return d_message_fonts[i];
    }
  }
  return d_message_fonts[0];
}
