// opendb.cpp
//
// Open a Rivendell Database
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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


void PrintDbError(const QString &err,bool interactive)
{
  if(interactive) {
    QMessageBox::warning(NULL,"RDAdmin - "+QObject::tr("DB Error"),err);
  }
  else {
  }
  exit(256);
}

bool OpenDb(QString dbname,QString login,QString pwd,
	    QString host,QString stationname,bool interactive)
{
  // 
  // Yeesh, this whole method really needs a rewrite!
  // They shoot horses, don't they??
  //

  QString admin_name;
  QString admin_pwd;
  QString msg;
  QString str;
  QString sql;
  QSqlQuery *q;

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
    RDKillDaemons();
    PrintDbError(QObject::tr("Unable to open MySQL database connection."),
		 interactive);
  }

  //
  // Identify DB
  //
  sql=QString("show tables");
  q=new QSqlQuery(sql);
  if(q->first()) {
    delete q;
    sql=QString("select DB from VERSION");
    q=new QSqlQuery(sql);
    if(q->first()) {
      if(q->value(0).toInt()!=RD_VERSION_DATABASE) {
	PrintDbError("Unsupported database version",interactive);
      }
    }
    else {
      PrintDbError("Database is corrupt",interactive);
    }
  }
  else {
    CreateDb();
    InitDb(stationname);
    if(interactive) {
      QMessageBox::information(NULL,"RDAdmin - "+QObject::tr("DB Message"),
			       QObject::tr("Created new Rivendell database."));
    }
  }
  delete q;

  return true;
}
