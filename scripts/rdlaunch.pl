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
# Get the virtual hostname
#
$_=$ENV{"HOSTNAME"};
my @parts=split "-";
if(scalar @parts lt 2) {
  print "rdlaunch.pl: unable to determine virtual hostname\n";
  exit 256;
}
my $hostname=$parts[0];

#
# Launch the module
#
my $cmd="ssh -X -c ".$ssh_cipher." -i ".$ssh_identity." ".$virt_user.'@'.$hostname.' '.$module;
system($cmd);
