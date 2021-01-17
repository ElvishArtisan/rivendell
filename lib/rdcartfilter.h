// rdcartfilter.h
//
// Filter widget for picking Rivendell carts.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCARTFILTER_H
#define RDCARTFILTER_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <rdlibrarymodel.h>
#include <rdprofile.h>
#include <rdwidget.h>

class RDCartFilter : public RDWidget
{
  Q_OBJECT;
 public:
  RDCartFilter(bool show_drag_box,QWidget *parent=0);
  ~RDCartFilter();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void loadConfig(RDProfile *p);
  void saveConfig(FILE *f) const;
  QString filterSql(const QStringList &and_fields=QStringList()) const;
  QString filterText() const;
  QString selectedGroup() const;
  QString selectedSchedCode() const;
  bool dragEnabled() const;
  RDCart::Type showCartType() const;
  void setShowCartType(RDCart::Type type);
  bool limitSearch() const;
  void setLimitSearch(bool state);
  bool userIsAdmin() const;
  void setUserIsAdmin(bool state);
  RDLibraryModel *model() const;
  static QString phraseFilter(const QString &phrase,bool incl_cuts);
  static QString groupFilter(const QString &group,const QStringList &groups);
  static QString typeFilter(bool incl_audio,bool incl_macro,RDCart::Type mask);

 public slots:
  void setModel(RDLibraryModel *model);
  void setFilterText(const QString &str);
  void setSelectedGroup(const QString &grpname);
  void changeUser();

 signals:
  void selectedGroupChanged(const QString &grpname);
  void filterChanged(const QString &where_sql);
  void dragEnabledChanged(bool state);

 private slots:
  void setMatchCount(int matches);
  void filterChangedData(const QString &str);
  void searchClickedData();
  void clearClickedData();
  void groupChangedData(const QString &str);
  void checkChangedData(int n);
  void dragsChangedData(int n);
  void searchLimitChangedData(int state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDLibraryModel *d_model;
  QLineEdit *d_filter_edit;
  QLabel *d_filter_label;
  QComboBox *d_group_box;
  QLabel *d_group_label;
  QComboBox *d_codes_box;
  QLabel *d_codes_label;
  QComboBox *d_codes2_box;
  QLabel *d_codes2_label;
  QLineEdit *d_matches_edit;
  QLabel *d_matches_label;
  QPushButton *d_search_button;
  QPushButton *d_clear_button;
  QCheckBox *d_allowdrag_box;
  QLabel *d_allowdrag_label;
  bool d_show_drag_box;
  QCheckBox *d_showaudio_check;
  QLabel *d_showaudio_label;
  QCheckBox *d_showmacro_check;
  QLabel *d_shownotes_label;
  QCheckBox *d_shownotes_box;
  QLabel *d_showmatches_label;
  QCheckBox *d_showmatches_box;
  QLabel *d_showmacro_label;
  RDCart::Type d_show_cart_type;
  QString d_default_group;
  bool d_user_is_admin;
};


#endif  // RDCARTFILTER_H
