#!%PYTHON_BANGPATH%

# xcmd_server.py
#
# pypad regression test script for Rivendell
#
#  Exercise every PAD accessor method of 'pypad.Update' for each update.
#
#   (C) Copyright 2019 Patrick Linstruth <patrick@deltecent.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import socketserver
import xml.etree.ElementTree as ET

class XcmdTCPHandler(socketserver.BaseRequestHandler):

    count=0;

    def handle(self):
        while True:
            try:
                self.data = self.request.recv(1024).strip()

            except:
                print("Recv exception\n");
                break

            if not self.data:
                print("No data\n");
                break

            if self.data[0] == 0xff:
                print("0xff\n");
                break

            XcmdTCPHandler.count=XcmdTCPHandler.count+1

            print("{0}:{1} wrote:".format(*self.client_address))
            print(self.data)

            try:
                command=self.data.decode('utf-8').split(u'=')

            except:
                break;

            print(command)

            if command[0].upper() == 'XCMD':

                print("Received XCMD\n")

                try:
                    xml=ET.fromstring(command[1])

                    # Don't respond to every 4th request
                    if XcmdTCPHandler.count%4:
                        self.request.sendall(b"+\r\n\r\n")

                except:
                    self.request.sendall(b"-\r\n\r\n")

            else:
                self.request.sendall(b"!\r\n\r\n")


    def finish(self):
            print("{0}:{1} disconnected".format(*self.client_address))

if __name__ == "__main__":
    HOST, PORT = "localhost", 1099

    # Create the server, binding to localhost on port 1099
    server = socketserver.TCPServer((HOST, PORT), XcmdTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    try:
        server.serve_forever()

    except:
        print("Shutdown\n")
        server.shutdown()

    finally:
        print("Close\n")
        server.server_close()

    print("Stopped\n")
