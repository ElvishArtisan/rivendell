// rdbutton_panel.h
//
// Component class for sound panel widgets
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDBUTTON_PANEL_H
#define RDBUTTON_PANEL_H

#include <QDateTime>
#include <QJsonValue>
#include <QLabel>
#include <QSignalMapper>

#include <rdairplay_conf.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdpanel_button.h>
#include <rdbutton_dialog.h>
#include <rdwidget.h>

//
// Widget Settings
//
#define PANEL_MAX_BUTTON_COLUMNS 40
#define PANEL_MAX_BUTTON_ROWS 23

class RDButtonPanel : public RDWidget
{
  Q_OBJECT;
 public:
  RDButtonPanel(RDAirPlayConf::PanelType type,int number,const QString &title,
		QWidget *parent);
  ~RDButtonPanel();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDAirPlayConf::PanelType panelType() const;
  int number() const;
  QString title() const;
  void setTitle(const QString &str);
  RDPanelButton *panelButton(int row,int col) const;
  void setActionMode(RDAirPlayConf::ActionMode mode);
  void setAllowDrags(bool state);
  void setAcceptDrops(bool state);
  void clear();
  QJsonValue json() const;

 public slots:
  void setVisible(bool state);

 signals:
  void buttonClicked(int pnum,int col,int row);

 private slots:
  void buttonClickedData(int id);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateViewport();
  int panel_number;
  QString panel_title;
  QSignalMapper *panel_button_mapper;
  RDPanelButton *panel_button[PANEL_MAX_BUTTON_ROWS][PANEL_MAX_BUTTON_COLUMNS];
  RDAirPlayConf::PanelType panel_type;
};

#endif  // RDBUTTON_PANEL_H
