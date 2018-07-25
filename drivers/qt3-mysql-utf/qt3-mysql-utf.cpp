// qt3-mysql-utf.cpp
//
// Qt3 SQL plug-in for MySQL with Unicode UTF-8 support
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
//
// Based on the stock Qt3 MySQL driver
//   Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
//
//   This file may be used under the terms of the Q Public License as
//   defined by Trolltech ASA and appearing in the file LICENSE.QPL
//   included in the packaging of this file.  Licensees holding valid Qt
//   Commercial licenses may use this file in accordance with the Qt
//   Commercial License Agreement provided with the Software.
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

#include "qt3-mysql-utf.h"
#include "qsqlextension.h"

#include <qdatetime.h>
#include <qvaluevector.h>
#include <qsqlrecord.h>

#define QMYSQLUTF_DRIVER_NAME "QMYSQLUTF8"

QPtrDict<QSqlOpenExtension> *qSqlOpenExtDict();

static int qMySqlConnectionCount = 0;
static bool qMySqlInitHandledByUser = FALSE;

class QMYSQLUTFOpenExtension : public QSqlOpenExtension
{
public:
    QMYSQLUTFOpenExtension( QMYSQLUTFDriver *dri )
	: QSqlOpenExtension(), driver(dri) {}
    ~QMYSQLUTFOpenExtension() {}

    bool open( const QString& db,
	       const QString& user,
	       const QString& password,
	       const QString& host,
	       int port,
	       const QString& connOpts );
    
private:
    QMYSQLUTFDriver *driver;
};

bool QMYSQLUTFOpenExtension::open( const QString& db,
				const QString& user,
				const QString& password,
				const QString& host,
				int port,
				const QString& connOpts )
{
    return driver->open( db, user, password, host, port, connOpts );
}

class QMYSQLUTFDriverPrivate
{
public:
    QMYSQLUTFDriverPrivate() : mysql(0) {}
    MYSQL*     mysql;
};

class QMYSQLUTFResultPrivate : public QMYSQLUTFDriverPrivate
{
public:
    QMYSQLUTFResultPrivate() : QMYSQLUTFDriverPrivate(), result(0) {}
    MYSQL_RES* result;
    MYSQL_ROW  row;
    QValueVector<QVariant::Type> fieldTypes;
};

QSqlError qMakeError( const QString& err, int type, const QMYSQLUTFDriverPrivate* p )
{
    return QSqlError(QMYSQLUTF_DRIVER_NAME ": " + err, QString(mysql_error( p->mysql )), type, mysql_errno( p->mysql ));
}

QVariant::Type qDecodeMYSQLType( int mysqltype, uint flags )
{
    QVariant::Type type;
    switch ( mysqltype ) {
    case FIELD_TYPE_TINY :
    case FIELD_TYPE_SHORT :
    case FIELD_TYPE_LONG :
    case FIELD_TYPE_INT24 :
	type = (flags & UNSIGNED_FLAG) ? QVariant::UInt : QVariant::Int;
	break;
    case FIELD_TYPE_YEAR :
	type = QVariant::Int;
	break;
    case FIELD_TYPE_LONGLONG :
	type = (flags & UNSIGNED_FLAG) ? QVariant::ULongLong : QVariant::LongLong;
	break;
    case FIELD_TYPE_DECIMAL :
    case FIELD_TYPE_FLOAT :
    case FIELD_TYPE_DOUBLE :
	type = QVariant::Double;
	break;
    case FIELD_TYPE_DATE :
	type = QVariant::Date;
	break;
    case FIELD_TYPE_TIME :
	type = QVariant::Time;
	break;
    case FIELD_TYPE_DATETIME :
    case FIELD_TYPE_TIMESTAMP :
	type = QVariant::DateTime;
	break;
    case FIELD_TYPE_BLOB :
    case FIELD_TYPE_TINY_BLOB :
    case FIELD_TYPE_MEDIUM_BLOB :
    case FIELD_TYPE_LONG_BLOB :
	type = (flags & BINARY_FLAG) ? QVariant::ByteArray : QVariant::CString;
	break;
    default:
    case FIELD_TYPE_ENUM :
    case FIELD_TYPE_SET :
    case FIELD_TYPE_STRING :
    case FIELD_TYPE_VAR_STRING :
	type = QVariant::String;
	break;
    }
    return type;
}

QMYSQLUTFResult::QMYSQLUTFResult( const QMYSQLUTFDriver* db )
: QSqlResult( db )
{
    d =   new QMYSQLUTFResultPrivate();
    d->mysql = db->d->mysql;
}

QMYSQLUTFResult::~QMYSQLUTFResult()
{
    cleanup();
    delete d;
}

MYSQL_RES* QMYSQLUTFResult::result()
{
    return d->result;
}

void QMYSQLUTFResult::cleanup()
{
    if ( d->result ) {
	mysql_free_result( d->result );
    }
    d->result = NULL;
    d->row = NULL;
    setAt( -1 );
    setActive( FALSE );
}

bool QMYSQLUTFResult::fetch( int i )
{
    if ( isForwardOnly() ) { // fake a forward seek
	if ( at() < i ) {
	    int x = i - at();
	    while ( --x && fetchNext() );
	    return fetchNext();
	} else {
	    return FALSE;
	}
    }    
    if ( at() == i )
	return TRUE;
    mysql_data_seek( d->result, i );
    d->row = mysql_fetch_row( d->result );
    if ( !d->row )
	return FALSE;
    setAt( i );
    return TRUE;
}

bool QMYSQLUTFResult::fetchNext()
{
    d->row = mysql_fetch_row( d->result );
    if ( !d->row )
	return FALSE;
    setAt( at() + 1 );
    return TRUE;
}

bool QMYSQLUTFResult::fetchLast()
{
    if ( isForwardOnly() ) { // fake this since MySQL can't seek on forward only queries
	bool success = fetchNext(); // did we move at all?
	while ( fetchNext() );
	return success;
    }
    my_ulonglong numRows = mysql_num_rows( d->result );
    if ( !numRows )
	return FALSE;
    return fetch( numRows - 1 );
}

bool QMYSQLUTFResult::fetchFirst()
{
    if ( isForwardOnly() ) // again, fake it
	return fetchNext();
    return fetch( 0 );
}

QVariant QMYSQLUTFResult::data( int field )
{
    if ( !isSelect() || field >= (int) d->fieldTypes.count() ) {
	qWarning( "QMYSQLUTFResult::data: column %d out of range", field );
	return QVariant();
    }
    
    QString val( d->row[field] );
    switch ( d->fieldTypes.at( field ) ) {
    case QVariant::LongLong:
	return QVariant( val.toLongLong() );
    case QVariant::ULongLong:
	return QVariant( val.toULongLong() );
    case QVariant::Int: 
	return QVariant( val.toInt() );
    case QVariant::UInt:
	return QVariant( val.toUInt() );
    case QVariant::Double:
	return QVariant( val.toDouble() );
    case QVariant::Date:
	if ( val.isEmpty() ) {
	    return QVariant( QDate() );
	} else {
	    return QVariant( QDate::fromString( val, Qt::ISODate )  );
	}
    case QVariant::Time:
	if ( val.isEmpty() ) {
	    return QVariant( QTime() );
	} else {
	    return QVariant( QTime::fromString( val, Qt::ISODate ) );
	}
    case QVariant::DateTime:
	if ( val.isEmpty() )
	    return QVariant( QDateTime() );
	if ( val.length() == 14u )
	    // TIMESTAMPS have the format yyyyMMddhhmmss
	    val.insert(4, "-").insert(7, "-").insert(10, 'T').insert(13, ':').insert(16, ':');
	return QVariant( QDateTime::fromString( val, Qt::ISODate ) );
    case QVariant::ByteArray: {
	unsigned long* fl = mysql_fetch_lengths( d->result );
	QByteArray ba;
	ba.duplicate( d->row[field], fl[field] );
	return QVariant( ba );
    }
    default:
    case QVariant::String:
    case QVariant::CString:
      return QVariant(QString::fromUtf8(d->row[field] ));      
    }
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLUTFResult::data: unknown data type");
#endif
    return QVariant();
}

bool QMYSQLUTFResult::isNull( int field )
{
    if ( d->row[field] == NULL )
	return TRUE;
    return FALSE;
}

bool QMYSQLUTFResult::reset ( const QString& query )
{
    if ( !driver() )
	return FALSE;
    if ( !driver()-> isOpen() || driver()->isOpenError() )
	return FALSE;
    cleanup();

    QCString encQuery = query.utf8();
    if ( mysql_real_query( d->mysql, encQuery, encQuery.length() ) ) {
	setLastError( qMakeError("Unable to execute query", QSqlError::Statement, d ) );
	return FALSE;
    }
    if ( isForwardOnly() ) {
	if ( isActive() || isValid() ) // have to empty the results from previous query
	    fetchLast();
	d->result = mysql_use_result( d->mysql );
    } else {
	d->result = mysql_store_result( d->mysql );
    }
    if ( !d->result && mysql_field_count( d->mysql ) > 0 ) {
	setLastError( qMakeError( "Unable to store result", QSqlError::Statement, d ) );
	return FALSE;
    }
    int numFields = mysql_field_count( d->mysql );
    setSelect( !( numFields == 0) );
    d->fieldTypes.resize( numFields );
    if ( isSelect() ) {
	for( int i = 0; i < numFields; i++) {
	    MYSQL_FIELD* field = mysql_fetch_field_direct( d->result, i );
	    if ( field->type == FIELD_TYPE_DECIMAL )
		d->fieldTypes[i] = QVariant::String;
	    else
		d->fieldTypes[i] = qDecodeMYSQLType( field->type, field->flags );
	}
    }
    setActive( TRUE );
    return TRUE;
}

int QMYSQLUTFResult::size()
{
    return isSelect() ? (int)mysql_num_rows( d->result ) : -1;
}

int QMYSQLUTFResult::numRowsAffected()
{
    return (int)mysql_affected_rows( d->mysql );
}

/////////////////////////////////////////////////////////
static void qServerEnd()
{
#ifndef Q_NO_MYSQL_EMBEDDED
# if MYSQL_VERSION_ID >= 40000
    mysql_server_end();
# endif // MYSQL_VERSION_ID
#endif // Q_NO_MYSQL_EMBEDDED
}

static void qServerInit()
{
#ifndef Q_NO_MYSQL_EMBEDDED
# if MYSQL_VERSION_ID >= 40000
    if ( qMySqlInitHandledByUser || qMySqlConnectionCount > 1 )
        return;

    // this should only be called once
    // has no effect on client/server library
    // but is vital for the embedded lib
    if ( mysql_server_init( 0, 0, 0 ) ) {
#  ifdef QT_CHECK_RANGE
	qWarning( "QMYSQLUTFDriver::qServerInit: unable to start server." );
#  endif
    }
    
# endif // MYSQL_VERSION_ID
#endif // Q_NO_MYSQL_EMBEDDED
}

QMYSQLUTFDriver::QMYSQLUTFDriver( QObject * parent, const char * name )
    : QSqlDriver( parent, name ? name : QMYSQLUTF_DRIVER_NAME )
{
    init();
    qServerInit();
}

/*!
    Create a driver instance with an already open connection handle.
*/

QMYSQLUTFDriver::QMYSQLUTFDriver( MYSQL * con, QObject * parent, const char * name )
    : QSqlDriver( parent, name ? name : QMYSQLUTF_DRIVER_NAME )
{
    init();
    if ( con ) {
	d->mysql = (MYSQL *) con;
	setOpen( TRUE );
	setOpenError( FALSE );
        if (qMySqlConnectionCount == 1)
            qMySqlInitHandledByUser = TRUE;
    } else {
        qServerInit();
    }
}

void QMYSQLUTFDriver::init()
{
  //    qSqlOpenExtDict()->insert( this, new QMYSQLUTFOpenExtension(this) );
    d = new QMYSQLUTFDriverPrivate();
    d->mysql = 0;
    qMySqlConnectionCount++;
}

QMYSQLUTFDriver::~QMYSQLUTFDriver()
{
    qMySqlConnectionCount--;
    if (qMySqlConnectionCount == 0 && !qMySqlInitHandledByUser)
        qServerEnd();

    delete d;

    if ( !qSqlOpenExtDict()->isEmpty() ) {
	QSqlOpenExtension *ext = qSqlOpenExtDict()->take( this );
	delete ext;
    }
}

bool QMYSQLUTFDriver::hasFeature( DriverFeature f ) const
{
    switch ( f ) {
    case Transactions:
// CLIENT_TRANSACTION should be defined in all recent mysql client libs > 3.23.34
#ifdef CLIENT_TRANSACTIONS
	if ( d->mysql ) {
	    if ( ( d->mysql->server_capabilities & CLIENT_TRANSACTIONS ) == CLIENT_TRANSACTIONS )
		return TRUE;
	}
#endif
	return FALSE;
    case QuerySize:
	return TRUE;
    case BLOB:
	return TRUE;
    case Unicode:
	return TRUE;
    default:
	return FALSE;
    }
}

bool QMYSQLUTFDriver::open( const QString& db,
			 const QString& user,
			 const QString& password,
			 const QString& host,
			 int port)
{
  return open(db,user,password,host,port,"");
}

bool QMYSQLUTFDriver::open( const QString& db,
			 const QString& user,
			 const QString& password,
			 const QString& host,
			 int port,
			 const QString& connOpts )
{
    if ( isOpen() )
	close();
 
    unsigned int optionFlags = 0;
    
    QStringList raw = QStringList::split( ';', connOpts );
    QStringList opts;
    QStringList::ConstIterator it;
    
    // extract the real options from the string
    for ( it = raw.begin(); it != raw.end(); ++it ) {
	QString tmp( *it );
	int idx;
	if ( (idx = tmp.find( '=' )) != -1 ) {
	    QString val( tmp.mid( idx + 1 ) );
	    val.simplifyWhiteSpace();
	    if ( val == "TRUE" || val == "1" )
		opts << tmp.left( idx );
	    else
		qWarning( "QMYSQLUTFDriver::open: Illegal connect option value '%s'", tmp.latin1() );
	} else {
	    opts << tmp;
	}
    }
    
    for ( it = opts.begin(); it != opts.end(); ++it ) {
	QString opt( (*it).upper() );
	if ( opt == "CLIENT_COMPRESS" )
	    optionFlags |= CLIENT_COMPRESS;
	else if ( opt == "CLIENT_FOUND_ROWS" )
	    optionFlags |= CLIENT_FOUND_ROWS;
	else if ( opt == "CLIENT_IGNORE_SPACE" )
	    optionFlags |= CLIENT_IGNORE_SPACE;
	else if ( opt == "CLIENT_INTERACTIVE" )
	    optionFlags |= CLIENT_INTERACTIVE;
	else if ( opt == "CLIENT_NO_SCHEMA" )
	    optionFlags |= CLIENT_NO_SCHEMA;
	else if ( opt == "CLIENT_ODBC" )
	    optionFlags |= CLIENT_ODBC;
	else if ( opt == "CLIENT_SSL" )
	    optionFlags |= CLIENT_SSL;
	else 
	    qWarning( "QMYSQLUTFDriver::open: Unknown connect option '%s'", (*it).latin1() );
    }

    if ( (d->mysql = mysql_init((MYSQL*) 0)) &&
	    mysql_real_connect( d->mysql,
				host,
				user,
				password,
				db.isNull() ? QString("") : db,
				(port > -1) ? port : 0,
				NULL,
				optionFlags ) )
    {
	if ( !db.isEmpty() && mysql_select_db( d->mysql, db )) {
	    setLastError( qMakeError("Unable open database '" + db + "'", QSqlError::Connection, d ) );
	    mysql_close( d->mysql );
	    setOpenError( TRUE );
	    return FALSE;
	}
    } else {
	    setLastError( qMakeError( "Unable to connect", QSqlError::Connection, d ) );
	    mysql_close( d->mysql );
	    setOpenError( TRUE );
	    return FALSE;
    }
    setOpen( TRUE );
    setOpenError( FALSE );
    return TRUE;
}

void QMYSQLUTFDriver::close()
{
    if ( isOpen() ) {
	mysql_close( d->mysql );
	setOpen( FALSE );
	setOpenError( FALSE );
    }
}

QSqlQuery QMYSQLUTFDriver::createQuery() const
{
    return QSqlQuery( new QMYSQLUTFResult( this ) );
}

QStringList QMYSQLUTFDriver::tables( const QString& typeName ) const
{
    QStringList tl;
    if ( !isOpen() )
	return tl;
    if ( !typeName.isEmpty() && !(typeName.toInt() & (int)QSql::Tables) )
	return tl;

    MYSQL_RES* tableRes = mysql_list_tables( d->mysql, NULL );
    MYSQL_ROW	row;
    int i = 0;
    while ( tableRes && TRUE ) {
	mysql_data_seek( tableRes, i );
	row = mysql_fetch_row( tableRes );
	if ( !row )
	    break;
	tl.append( QString(row[0]) );
	i++;
    }
    mysql_free_result( tableRes );
    return tl;
}

QSqlIndex QMYSQLUTFDriver::primaryIndex( const QString& tablename ) const
{
    QSqlIndex idx;
    if ( !isOpen() )
	return idx;
    QSqlQuery i = createQuery();
    QString stmt( "show index from %1;" );
    QSqlRecord fil = record( tablename );
    i.exec( stmt.arg( tablename ) );
    while ( i.isActive() && i.next() ) {
	if ( i.value(2).toString() == "PRIMARY" ) {
	    idx.append( *fil.field( i.value(4).toString() ) );
	    idx.setCursorName( i.value(0).toString() );
	    idx.setName( i.value(2).toString() );
	}
    }
    return idx;
}

QSqlRecord QMYSQLUTFDriver::record( const QString& tablename ) const
{
    QSqlRecord fil;
    if ( !isOpen() )
	return fil;
    MYSQL_RES* r = mysql_list_fields( d->mysql, tablename.local8Bit().data(), 0);
    if ( !r ) {
	return fil;
    }
    MYSQL_FIELD* field;
    while ( (field = mysql_fetch_field( r ))) {
	QSqlField f ( QString( field->name ) , qDecodeMYSQLType( (int)field->type, field->flags ) );
	fil.append ( f );
    }
    mysql_free_result( r );
    return fil;
}

QSqlRecord QMYSQLUTFDriver::record( const QSqlQuery& query ) const
{
    QSqlRecord fil;
    if ( !isOpen() )
	return fil;
    if ( query.isActive() && query.isSelect() && query.driver() == this ) {
	QMYSQLUTFResult* result =  (QMYSQLUTFResult*)query.result();
	QMYSQLUTFResultPrivate* p = result->d;
	if ( !mysql_errno( p->mysql ) ) {
	    for ( ;; ) {
		MYSQL_FIELD* f = mysql_fetch_field( p->result );
		if ( f ) {
		    QSqlField fi( QString((const char*)f->name), qDecodeMYSQLType( f->type, f->flags ) );
		    fil.append( fi  );
		} else
		    break;
	    }
	}
	mysql_field_seek( p->result, 0 );
    }
    return fil;
}

QSqlRecordInfo QMYSQLUTFDriver::recordInfo( const QString& tablename ) const
{
    QSqlRecordInfo info;
    if ( !isOpen() )
	return info;
    MYSQL_RES* r = mysql_list_fields( d->mysql, tablename.local8Bit().data(), 0);
    if ( !r ) {
	return info;
    }
    MYSQL_FIELD* field;
    while ( (field = mysql_fetch_field( r ))) {
	info.append ( QSqlFieldInfo( QString( field->name ),
				qDecodeMYSQLType( (int)field->type, field->flags ),
				IS_NOT_NULL( field->flags ),
				(int)field->length,
				(int)field->decimals,
				QString( field->def ),
				(int)field->type ) );
    }
    mysql_free_result( r );
    return info;
}

QSqlRecordInfo QMYSQLUTFDriver::recordInfo( const QSqlQuery& query ) const
{
    QSqlRecordInfo info;
    if ( !isOpen() )
	return info;
    if ( query.isActive() && query.isSelect() && query.driver() == this ) {
	QMYSQLUTFResult* result =  (QMYSQLUTFResult*)query.result();
	QMYSQLUTFResultPrivate* p = result->d;
	if ( !mysql_errno( p->mysql ) ) {
	    for ( ;; ) {
		MYSQL_FIELD* field = mysql_fetch_field( p->result );
		if ( field ) {
		    info.append ( QSqlFieldInfo( QString( field->name ),
				qDecodeMYSQLType( (int)field->type, field->flags ),
				IS_NOT_NULL( field->flags ),
				(int)field->length,
				(int)field->decimals,
				QVariant(),
				(int)field->type ) );
		
		} else
		    break;
	    }
	}
	mysql_field_seek( p->result, 0 );
    }
    return info;
}

MYSQL* QMYSQLUTFDriver::mysql()
{
     return d->mysql;
}

bool QMYSQLUTFDriver::beginTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return FALSE;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	qWarning( "QMYSQLUTFDriver::beginTransaction: Database not open" );
#endif
	return FALSE;
    }
    if ( mysql_query( d->mysql, "BEGIN WORK" ) ) {
	setLastError( qMakeError("Unable to begin transaction", QSqlError::Statement, d ) );
	return FALSE;
    }
    return TRUE;
}

bool QMYSQLUTFDriver::commitTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return FALSE;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	qWarning( "QMYSQLUTFDriver::commitTransaction: Database not open" );
#endif
	return FALSE;
    }
    if ( mysql_query( d->mysql, "COMMIT" ) ) {
	setLastError( qMakeError("Unable to commit transaction", QSqlError::Statement, d ) );
	return FALSE;
    }
    return TRUE;
}

bool QMYSQLUTFDriver::rollbackTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return FALSE;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	qWarning( "QMYSQLUTFDriver::rollbackTransaction: Database not open" );
#endif
	return FALSE;
    }
    if ( mysql_query( d->mysql, "ROLLBACK" ) ) {
	setLastError( qMakeError("Unable to rollback transaction", QSqlError::Statement, d ) );
	return FALSE;
    }
    return TRUE;
}

QString QMYSQLUTFDriver::formatValue( const QSqlField* field, bool trimStrings ) const
{
    QString r;
    if ( field->isNull() ) {
	r = nullText();
    } else {
	switch( field->type() ) {
	case QVariant::ByteArray: {
	
	    const QByteArray ba = field->value().toByteArray();
	    // buffer has to be at least length*2+1 bytes
	    char* buffer = new char[ ba.size() * 2 + 1 ];
	    /*uint escapedSize =*/ mysql_escape_string( buffer, ba.data(), ba.size() );
	    r.append("'").append(buffer).append("'");
	    delete[] buffer;
	}
	break;
	case QVariant::String:
	case QVariant::CString: {
	    // Escape '\' characters
	    r = QSqlDriver::formatValue( field );
	    r.replace( "\\", "\\\\" );
	    break;
	}
	default:
	    r = QSqlDriver::formatValue( field, trimStrings );
	}
    }
    return r;
}


QMYSQLUTFDriverPlugin::QMYSQLUTFDriverPlugin()
    : QSqlDriverPlugin()
{
}

QSqlDriver* QMYSQLUTFDriverPlugin::create( const QString &name )
{
    if ( name == "QMYSQLUTF8" ) {
	QMYSQLUTFDriver* driver = new QMYSQLUTFDriver();
	return driver;
    }
    return 0;
}

QStringList QMYSQLUTFDriverPlugin::keys() const
{
    QStringList l;
    l  << "QMYSQLUTF8";
    return l;
}

Q_EXPORT_PLUGIN( QMYSQLUTFDriverPlugin )
