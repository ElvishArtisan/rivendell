// opendb.cpp
//
// Open a Rivendell Database
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: opendb.cpp,v 1.38.4.1 2013/11/13 00:12:54 cvs Exp $
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

#include <stdlib.h>
#include <unistd.h>
#include <qsqldriver.h>
#include <qmessagebox.h>
#include <opendb.h>
#include <createdb.h>
#include <rd.h>
#include <rddb.h>
#include <dbversion.h>
#include <rdcheck_version.h>
#include <rdcheck_daemons.h>
#include <mysql_login.h>
#include <globals.h>

// Includes used for netmask and remote server detection.
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <qobject.h>

/**
 * Get the netmask of an interface and return it via an in_addr struct pointer.
 *
 * Note: uses linux IOCTL call SIOCGIFNETMASK to retrieve the netmask from a
 * temporary socket.
 *
 * @param interface String with the interface to query.
 * @param netmask Pointer to struct in_addr that will be populated with the netmask.
 * @return true on success
 **/
bool get_netmask(const char * interface, struct in_addr * netmask)
{
  int fd;
  struct ifreq ifr;
  struct sockaddr_in *nmask;

  fd=socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family=AF_INET;
  strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
  ioctl(fd, SIOCGIFNETMASK, &ifr);
  close(fd);
  nmask=(struct sockaddr_in *)&ifr.ifr_netmask;
  netmask->s_addr=nmask->sin_addr.s_addr;
  return true;
}


/**
 * Check if creating the database on the local machine or on a remote server in
 * the same subnet.
 *
 * @param host QString with the hostname of machine to create the database.
 * @return true if database is to be created on a remote server.
 **/
bool check_remote_server(QString host) {
  char local_hostname[256];
  int rc;
  struct hostent *temp_hostent;
  struct hostent local_hostent;
  struct hostent host_hostent;
  struct in_addr local_ip;
  struct in_addr host_ip;
  struct in_addr local_netmask;

  // check if host is 'localhost'
  if (0==strncasecmp("localhost", (const char *)host, 255))
    return false;

  rc=gethostname(local_hostname, 255);
  // compare hostnames
  if ((0==rc) && (0!=strncasecmp(local_hostname, (const char *)host, 255))) {
    if ((temp_hostent=gethostbyname(local_hostname)))
      local_hostent=*temp_hostent;
    else
      return false;
    local_ip=*(struct in_addr *)temp_hostent->h_addr;

    if ((temp_hostent=gethostbyname((const char *)host)))
      host_hostent=*temp_hostent;
    else
      return false;
    host_ip=*(struct in_addr *)temp_hostent->h_addr;

    // compare IPs
    if ((local_hostent.h_addrtype == AF_INET) &&
        (host_hostent.h_addrtype == AF_INET) &&
        (local_ip.s_addr != host_ip.s_addr)) {
      // FIXME: ideally do something smarter than just testing eth0 (note use
      // below in format_remote_host() also)
      //   get list of interfaces   if_nameindex()
      //   loop through list of interfaces, get IP, see if it matches local IP
      //   once have good interface, get netmask
      rc=get_netmask("eth0", &local_netmask);

      // compare if IPs are on same subnet.  note: use of bitwise sum
      if ( (local_ip.s_addr & local_netmask.s_addr) == 
                (host_ip.s_addr & local_netmask.s_addr) )
        return true;
    } // endif compare IPs
  } // endif compare hostnames
  return false;
}


/**
 * Format a QString with the MySQL host portion for a remote host.  The
 * resulting string will use the local subnet and mask to generate something
 * like "192.168.1.0/255.255.255.0" .
 *
 * @param host string with the hostname to use for the subnet and mask.
 * @return QString with formated result.
 **/
QString format_remote_host(const char *hostname) {
  struct in_addr local_netmask;
  struct hostent *temp_hostent;
  struct in_addr local_ip;
  struct in_addr local_subnet;

  temp_hostent=gethostbyname(hostname);
  local_ip=*(struct in_addr *)temp_hostent->h_addr;
  // FIXME: ideally do something smarter than just testing eth0 (see above in check_remote_server() also)
  get_netmask("eth0", &local_netmask);
  local_subnet.s_addr=(local_ip.s_addr & local_netmask.s_addr);
  return QString().sprintf("%s/%s", strdupa(inet_ntoa(local_subnet)), strdupa(inet_ntoa(local_netmask)) );
}


bool OpenDb(QString dbname,QString login,QString pwd,
	    QString host,QString stationname,bool interactive)
{
  // 
  // Yeesh, this whole method really needs a rewrite!
  // They shoot horses, don't they??
  //

  int db_ver;
  QString admin_name;
  QString admin_pwd;
  QString sql;
  QSqlQuery *q;
  QString msg;
  MySqlLogin *mysql_login;
  QString str;
  int err=0;
  QString err_str="";

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(admin_config->mysqlDriver());
  if(!db) {
    return false;
  }
  db->setDatabaseName(dbname);
  db->setUserName(login);
  db->setPassword(pwd);
  db->setHostName(host);
  if(!db->open()) {
    /*
    if(!interactive) {
      return false;
    }
    */
    RDKillDaemons();
    if(interactive) {
      msg=QObject::tr("Unable to access the Rivendell Database!\n\
Please enter a login for an account with\n\
administrative rights on the mySQL server,\n\
and we will try to get this straightened out.");
      mysql_login=new MySqlLogin(msg,&admin_name,&admin_pwd);
      if(mysql_login->exec()!=0) {
	delete mysql_login;
	db->removeDatabase(dbname);
	return false;
      }
      delete mysql_login;
    }
    else {
      admin_name=admin_admin_username;
      admin_pwd=admin_admin_password;
      if(!admin_admin_hostname.isEmpty()) {
	db->setHostName(admin_admin_hostname);
      }
    }
    db->setUserName(admin_name);
    db->setPassword(admin_pwd);
    if(db->open()) {      // Fixup DB Access Permsissions
      PrintError(QObject::tr("Wrong access permissions for accessing mySQL!"),
		 interactive);
      db->removeDatabase("mysql");
      return false;
    }
    else {
      db->setDatabaseName("mysql");
      if(!db->open()) {   // mySQL is hosed -- scream and die.
	PrintError(QObject::tr("Unable to connect to mySQL!"),interactive);
	db->removeDatabase("mysql");
	return false;
      }
      else {              // Create a new Rivendell Database
	sql=QString().sprintf("create database %s",(const char *)dbname);
	q=new QSqlQuery(sql);
	if(!q->isActive()) {   // Can't create DB.
	  delete q;
	  PrintError(QObject::tr("Unable to create a Rivendell Database!"),
		     interactive);
	  db->removeDatabase("mysql");
	  return false;
	}
	delete q;

        // Check if creating the database on the local machine or on a remote
        // server in the same subnet.  If creating on a remote server, set the
        // host portion of the MySQL user to the subnet that is common between
        // the local workstation and the server.
        if (check_remote_server(host)) {
          host=format_remote_host(host);
        }
	sql=QString().sprintf("insert into user set Host=\"%s\",\
            User=\"%s\",Password=PASSWORD(\"%s\")",
			      (const char *)host, (const char *)login,(const char *)pwd);
	q=new QSqlQuery(sql);
	delete q;
	sql=QString().
	  sprintf("insert into db set Host=\"%s\",Db=\"%s\",\
            User=\"%s\",Select_priv=\"Y\",Insert_priv=\"Y\",Update_priv=\"Y\",\
            Delete_priv=\"Y\",Create_priv=\"Y\",Drop_priv=\"Y\",\
            Index_priv=\"Y\",Alter_priv=\"Y\",Lock_tables_priv=\"Y\"",
		 (const char *)host, (const char *)dbname,(const char *)login);
	q=new QSqlQuery(sql);
	delete q;
	q=new QSqlQuery("flush privileges");
	delete q;
	db->close();   // Relinquish admin perms
	if(!admin_admin_dbname.isEmpty()) {
	  dbname=admin_admin_dbname;
	}
	db->setDatabaseName(dbname);
	db->setUserName(login);
	db->setPassword(pwd);
	db->setHostName(host);
	if(!db->open()) {   // Can't open new database
	  PrintError(QObject::tr("Unable to connect to new Rivendell Database!"),
		     interactive);
	  db->removeDatabase(dbname);
	  return false;
	}
	if(!CreateDb(login,pwd)) {   // Can't create tables.
	  PrintError(QObject::tr("Unable to create Rivendell Database!"),
		     interactive);
	  db->removeDatabase(dbname);
	  return false;
	}
	db->close();
	db->setDatabaseName(dbname);
	db->setUserName(login);
	db->setPassword(pwd);
	if(!db->open()) {
	  PrintError(QObject::tr("Unable to connect to Rivendell Database!"),
		     interactive);
	  db->removeDatabase(dbname);
	  return false;
	}	  
	if(!InitDb(login,pwd,stationname)) {  // Can't initialize tables.
	  PrintError(QObject::tr("Unable to initialize Rivendell Database!"),
		     interactive);
	  db->removeDatabase(dbname);
	  return false;
	}
	if(interactive) {
	  QMessageBox::information(NULL,QObject::tr("RDAdmin"),
			      QObject::tr("New Rivendell Database Created!"));
	}
	return true;
      }
    }
    return false;
  }
  if((db_ver=RDCheckVersion())<RD_VERSION_DATABASE) {
    if(db_ver==0) {    // Pre-historic database version!
      if(!interactive) {
	PrintError(QObject::tr("Unable to upgrade database"),false);
	return false;
      }
      msg=QObject::tr("The Rivendell Database is too old to be upgraded,\n\
and so must be replaced.  This will DESTROY any\n\
existing audio and data!  If you want to do this,\n\
enter a username and password for a mySQL account\n\
with administrative privledges, otherwise hit cancel.");
      if(interactive) {
	mysql_login=new MySqlLogin(msg,&admin_name,&admin_pwd);
	if(mysql_login->exec()!=0) {
	  delete mysql_login;
	  db->removeDatabase(dbname);
	  return false;
	}
	delete mysql_login;
      }
      else {
	admin_name=admin_admin_username;
	admin_pwd=admin_admin_password;
      }
      RDKillDaemons();
      db->close();
      db->setDatabaseName("mysql");
      db->setUserName(admin_name);
      db->setPassword(admin_pwd);
      if(!db->open()) {
	PrintError(QObject::tr("Unable to log into Administrator account!"),
		   interactive);
	db->removeDatabase(dbname);
	return false;
      }	  
      q=new QSqlQuery(QString().sprintf ("drop database %s",(const char *)dbname));
      delete q;
      q=new QSqlQuery(QString().sprintf("create database %s",(const char *)dbname));
      if(!q->isActive()) {   // Can't create DB.
	delete q;
	PrintError(QObject::tr("Unable to create a Rivendell Database!"),
		   interactive);
	db->removeDatabase("mysql");
	return false;
      }
      delete q;
      sql=QString().
	sprintf("grant all on %s to %s identified by \"%s\"",
		(const char *)dbname,(const char *)login,(const char *)pwd);
      q=new QSqlQuery(sql);
      if(!q->isActive()) {  // Can't authorize DB.
	PrintError(QObject::tr("Unable to authorize a Rivendell Database!"),
		   interactive);
	db->removeDatabase("mysql");
	return false;
      }
      db->close();   // Relinquish admin perms
      db->setDatabaseName(dbname);
      db->setUserName(login);
      db->setPassword(pwd);
      if(!db->open()) {   // Can't open new database
	PrintError(QObject::tr("Unable to connect to new Rivendell Database!"),
		   interactive);
	db->removeDatabase(dbname);
	return false;
      }
      if(!CreateDb(login,pwd)) {   // Can't create tables.
	PrintError(QObject::tr("Unable to create Rivendell Database!"),
		   interactive);
	db->removeDatabase(dbname);
	return false;
      }
      db->close();
      db->setDatabaseName(dbname);
      db->setUserName(login);
      db->setPassword(pwd);
      if(!db->open()) {
	PrintError(QObject::tr("Unable to connect to Rivendell Database!"),
		   interactive);
	db->removeDatabase(dbname);
	return false;
      }	  
      if(!InitDb(login,pwd,stationname)) {   // Can't initialize tables.
	PrintError(QObject::tr("Unable to initialize Rivendell Database!"),
		   interactive);
	db->removeDatabase(dbname);
	return false;
      }
    }
    else {             // Out-of-date version
      if(interactive) {
	if(QMessageBox::warning(NULL,QObject::tr("Update Needed"),
		  QObject::tr("The Rivendell Database needs to be updated.\n\
All audio and settings will be preserved, but\n\
this will STOP any audio playout or recording\n\
on this machine for a few seconds.  Continue?"),
				QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  db->removeDatabase(dbname);
	  return false;
	}      
      }
      RDKillDaemons();
      if((err=UpdateDb(db_ver))!=UPDATEDB_SUCCESS) {
	err_str=QObject::tr("Unable to update Rivendell Database:");
	switch(err) {
	case UPDATEDB_BACKUP_FAILED:
	  err_str+=QObject::tr("\nDatabase backup failed!");
	  break;

	case UPDATEDB_QUERY_FAILED:
	  err_str+=QObject::tr("\nSchema modification failed!");
	  break;

	default:
	  err_str+=QObject::tr("\nUnknown/unspecified error!");
	  break;
	}
	PrintError(err_str,interactive);
	db->removeDatabase(dbname);
	return false;
      }
      str=QString(
        QObject::tr("The Rivendell Database has been updated to version"));
      msg=QString().
	sprintf("%s %d",(const char *)str,RD_VERSION_DATABASE);
      if(!admin_skip_backup) {
	msg+=QObject::tr("\nand a backup of the original database saved in ");
	msg+=admin_backup_filename;
      }
      msg+=".";
      if(interactive) {
	QMessageBox::information(NULL,QObject::tr("Database Updated"),msg);
      }
    }
  }

  return true;
}
