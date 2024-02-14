// messagewidget.h
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

#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QWebView>

#define MESSAGE_FONT_QUANTITY 8
#define MESSAGE_WIDGET_WIDTH 410

class MessageWidget : public QWidget
{
 Q_OBJECT
 public:
  MessageWidget(QWidget *parent=0);
  void setText(const QString &str,const QColor &col);
  bool setUrl(const QString &url);
  void clear();

 private slots:
  void webLoadFinishedData(bool state);
  void refreshData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QFont MessageFont(QString str) const;
  QLabel *d_label;
  QWebView *d_view;
  QTimer *d_refresh_timer;
  QFont d_message_fonts[MESSAGE_FONT_QUANTITY];
  QFontMetrics *d_message_metrics[MESSAGE_FONT_QUANTITY];
  QString d_url;
};

#endif  // MESSAGEWIDGET_H
