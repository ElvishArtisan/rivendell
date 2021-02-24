//   rdsocketstrings.cpp
//
//   Human-readable strings for QAbstractSocket::SocketError
//
//   (C) Copyright 2019-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include "rdsocketstrings.h"

//
// This REALLY ought to be provided by QAbstractSocket!
//
QString RDSocketStrings(QAbstractSocket::SocketError err)
{
  QString ret="unknown socket error";

  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    ret="connection refused";
    break;

  case QAbstractSocket::HostNotFoundError:
    ret="host not found";
    break;

  case QAbstractSocket::RemoteHostClosedError:
    ret="remote host closed connection";
    break;

  case QAbstractSocket::SocketAccessError:
    ret="socket access error";
    break;

  case QAbstractSocket::SocketResourceError:
    ret="socket resource error";
    break;

  case QAbstractSocket::SocketTimeoutError:
    ret="connection timed out";
    break;

  case QAbstractSocket::DatagramTooLargeError:
    ret="datagram too large";
    break;

  case QAbstractSocket::NetworkError:
    ret="general network error";
    break;

  case QAbstractSocket::AddressInUseError:
    ret="socket address in use";
    break;

  case QAbstractSocket::SocketAddressNotAvailableError:
    ret="socket address not available";
    break;

  case QAbstractSocket::UnsupportedSocketOperationError:
    ret="unsupported socket operation";
    break;

  case QAbstractSocket::UnfinishedSocketOperationError:
    ret="unfinished socket operation";
    break;

  case QAbstractSocket::ProxyAuthenticationRequiredError:
    ret="proxy authentication required";
    break;

  case QAbstractSocket::SslHandshakeFailedError:
    ret="ssl handshake failed";
    break;

  case QAbstractSocket::ProxyConnectionRefusedError:
    ret="proxy connection refused";
    break;

  case QAbstractSocket::ProxyConnectionClosedError:
    ret="proxy closed connection";
    break;

  case QAbstractSocket::ProxyConnectionTimeoutError:
    ret="proxy connection timed out";
    break;

  case QAbstractSocket::ProxyNotFoundError:
    ret="proxy not found";
    break;

  case QAbstractSocket::ProxyProtocolError:
    ret="proxy protocol error";
    break;

  case QAbstractSocket::OperationError:
    ret="operation error";
    break;

  case QAbstractSocket::SslInternalError:
    ret="SSL internal error";
    break;

  case QAbstractSocket::SslInvalidUserDataError:
    ret="SSL invalid user data error";
    break;

  case QAbstractSocket::TemporaryError:
    ret="temporary error";
    break;

  case QAbstractSocket::UnknownSocketError:
    break;
  }

  return ret;
}
