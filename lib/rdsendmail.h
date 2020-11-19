// rdsendmail.h
//
// Send an e-mail message using sendmail(1)
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSENDMAIL_H
#define RDSENDMAIL_H

#include <QString>
#include <QStringList>

bool RDSendMail(QString *err_msg,const QString &subject,const QString &body,
		const QString &from_addr,const QStringList &to_addrs,
		const QStringList &cc_addrs=QStringList(),
		const QStringList &bcc_addrs=QStringList(),bool dry_run=false);
bool RDSendMail(QString *err_msg,const QString &subject,const QString &body,
		const QString &from_addr,const QString &to_addrs,
		const QString &cc_addrs=QString(),
		const QString &bcc_addrs=QString(),bool dry_run=false);


#endif  // RDSENDMAIL
