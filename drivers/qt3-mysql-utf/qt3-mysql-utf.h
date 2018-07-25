// qt3-mysql-utf.h
//
// Qt3 SQL plug-in for MySQL with Unicode UTF-8 support
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
//
// Based on the stock Qt3 MySQL driver
//   Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#ifndef QT3_MYSQL_UTF_H
#define QT3_MYSQL_UTF_H

#include <qsqldriver.h>
#include <qsqldriverplugin.h>
#include <qsqlresult.h>
#include <qsqlfield.h>
#include <qsqlindex.h>

#include <mysql/mysql.h>

#define Q_EXPORT_SQLDRIVER_MYSQL

class QMYSQLUTFDriverPlugin : public QSqlDriverPlugin
{
public:
    QMYSQLUTFDriverPlugin();

    QSqlDriver* create( const QString & );
    QStringList keys() const;
};

class QMYSQLUTFDriverPrivate;
class QMYSQLUTFResultPrivate;
class QMYSQLUTFDriver;
class QSqlRecordInfo;

class QMYSQLUTFResult : public QSqlResult
{
    friend class QMYSQLUTFDriver;
public:
    QMYSQLUTFResult( const QMYSQLUTFDriver* db );
    ~QMYSQLUTFResult();

    MYSQL_RES* result();
protected:
    void		cleanup();
    bool		fetch( int i );
    bool		fetchNext();
    bool		fetchLast();
    bool		fetchFirst();
    QVariant		data( int field );
    bool		isNull( int field );
    bool		reset ( const QString& query );
    int			size();
    int			numRowsAffected();
private:
    QMYSQLUTFResultPrivate* d;
};

class Q_EXPORT_SQLDRIVER_MYSQL QMYSQLUTFDriver : public QSqlDriver
{
    friend class QMYSQLUTFResult;
public:
    QMYSQLUTFDriver( QObject * parent=0, const char * name=0 );
    QMYSQLUTFDriver( MYSQL * con, QObject * parent=0, const char * name=0 );
    ~QMYSQLUTFDriver();
    bool		hasFeature( DriverFeature f ) const;
    bool		open( const QString & db,
			      const QString & user = QString::null,
			      const QString & password = QString::null,
			      const QString & host = QString::null,
			      int port = -1 );
    void		close();
    QSqlQuery		createQuery() const;
    QStringList		tables( const QString& user ) const;
    QSqlIndex		primaryIndex( const QString& tablename ) const;
    QSqlRecord		record( const QString& tablename ) const;
    QSqlRecord		record( const QSqlQuery& query ) const;
    QSqlRecordInfo	recordInfo( const QString& tablename ) const;
    QSqlRecordInfo	recordInfo( const QSqlQuery& query ) const;
    QString		formatValue( const QSqlField* field,
				     bool trimStrings ) const;
    MYSQL*		mysql();
    // ### remove me for 4.0
    bool open( const QString& db,
	       const QString& user,
	       const QString& password,
	       const QString& host,
	       int port,
	       const QString& connOpts );
    
protected:
    bool		beginTransaction();
    bool		commitTransaction();
    bool		rollbackTransaction();
private:
    void		init();
    QMYSQLUTFDriverPrivate* d;
};


#endif  // QT3_MYSQL_UTF_H
