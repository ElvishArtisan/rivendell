#!/usr/bin/python

import rivendell.PyPAD

def ProcessPad(update,priv):
    print
    if update.hasNowPad():
        print update.padFields("NOW: %a - %t")
    else:
        print "NOW: [none]"
    if update.hasNextPad():
        print update.padFields("NEXT: %A - %T")
    else:
        print "NEXT: [none]"

rcvr=rivendell.PyPAD.PyPADReceiver()
rcvr.setCallback(ProcessPad)
rcvr.start("localhost")
