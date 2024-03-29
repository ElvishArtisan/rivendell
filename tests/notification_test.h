// upload_test.h
//
// Test Rivendell RDNotification class
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef NOTIFICATION_TEST_H
#define NOTIFICATION_TEST_H

#include <QObject>

#include <rdripc.h>

#define NOTIFICATION_TEST_USAGE ""

class MainObject : public QObject
{
  Q_OBJECT
  public:
    MainObject(QObject *parent=0);

  private slots:
    void notificationReceivedData(RDNotification *notify);

  private:
};


#endif  // NOTIFICATION_TEST_H
