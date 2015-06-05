#!/usr/bin/perl -W

# rdlaunch.pl
#
# Launch a Rivendell module on a remote system.
#
#  (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

use Net::DNS;

my $usage="rdlaunch.sh <module>";

#
# Read command-line
#
if(scalar @ARGV ne 1) {
  print $usage."\n";
  exit 256;
}
my $module=$ARGV[0];

#
# Configuration and Paths
#
my $ssh_cipher="blowfish";
my $ssh_identity=$ENV{"HOME"}."/.ssh/id_dsa_rdvirt";
my $virt_user="rd";

#
# Fetch the SRV Record
#
#  FIXME: We should examine all returned records and sort by priority.
#
my $res   = Net::DNS::Resolver->new;
my $reply = $res->query("_rdvirt._tcp.".$ENV{"HOSTNAME"}, "SRV");

if (!$reply) {
    print "rdlaunch.pl: no SRV record found [", $res->errorstring, "]\n";
    exit 256;
}
my @fields=split /\s+/,($reply->answer)[0]->string;
if(scalar @fields lt 8) {
    print "rdlaunch.pl: SRV record is malformatted\n";
    exit 256;
}
my $hostname=$fields[7];
my $port=$fields[6];

#
# Launch the module
#
my $cmd="ssh -X -c ".$ssh_cipher." -i ".$ssh_identity." -p".$port." ".$virt_user.'@'.$hostname.' '.$module;
system($cmd);
