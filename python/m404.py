#!/usr/bin/python

# M404 - A vacuum fluorescent display monitor for Raspberry Pi.
#
# Copyright (c) 2017 James Thompson
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.

import socket
import time
import subprocess
from netifaces import interfaces, ifaddresses, AF_INET
import serial
import re
import math

from datetime import timedelta
import signal, os

def handler(signum, frame):
    ser404.write("\x0b")
    ser404.write("Stopping on signal " + str(signum))
    os.abort()

looptime = 60
ips=''

ser404 = serial.Serial('/dev/ttyUSB0', 115200)

# Catch any of the signals that might cause the process to exit. Note
# that by catching exceptions (later on) seems to cause these signlas
# to get caught as exceptions, so the signal handler does not actually
# run.
#signal.signal(signal.SIGABRT, handler)
signal.signal(signal.SIGALRM, handler)
signal.signal(signal.SIGBUS, handler)
signal.signal(signal.SIGFPE, handler)
signal.signal(signal.SIGHUP, handler)
signal.signal(signal.SIGILL, handler)
signal.signal(signal.SIGINT, handler)
signal.signal(signal.SIGPIPE, handler)
signal.signal(signal.SIGPOLL, handler)
signal.signal(signal.SIGQUIT, handler)
signal.signal(signal.SIGSEGV, handler)
#signal.signal(signal.SIGSTOP, handler) # Can't catch?
signal.signal(signal.SIGSYS, handler)
signal.signal(signal.SIGTERM, handler)
#signal.signal(signal.SIGTSTP, handler) # Don't want to catch
#signal.signal(signal.SIGTTIN, handler) # Don't want to catch
#signal.signal(signal.SIGTTOU, handler) # Don't want to catch
signal.signal(signal.SIGTRAP, handler)
signal.signal(signal.SIGURG, handler)
signal.signal(signal.SIGUSR1, handler)
signal.signal(signal.SIGUSR2, handler)

ser404.write("\x0b")                    # Clear the display
ser404.write("\x13")                    # Set overwrite mode
ser404.write("\x04\xFF")                # Set brightness to max

ser404.write("\x03\xfc\xfc\x63\x18\xc7\xe0")
ser404.write("\x03\xfd\xaa\xaa\xaa\xaa\xaa")

def padout(s):
    s = s + '                                        ' #
    s = s[0:39]
    s += '|'
    return s

#try:
if True:
    while 1:
        allifaces = []
        for ifaceName in interfaces():
            if ifaceName[0:2] != "lo":
                addresses = [i['addr'] for i in ifaddresses(ifaceName).setdefault(AF_INET, [{'addr':'-'}] )]
                name = ifaceName + ' ' + ', '.join(addresses)
                allifaces.append(name)

        ips = ' '.join(allifaces)

        rawup = "0 0"
        with open('/proc/uptime', 'r') as f:
            rawup = f.readline()
            f.close()

        rawsplit = rawup.split()[0]
        uptime_seconds = float(rawsplit)
        # uptime_seconds += 99 * (24 * 60 * 60)
        # uptime_seconds += 23 * 60 * 60

        with open('/proc/loadavg', 'r') as f:
            rawload = f.readline()
            f.close

        load1m = rawload.split()[0]
#        print "1 min avg:", load1m
        load1m = 0.0 + float(load1m)
#        print "1 min avg:", load1m

        (fload, iload) = math.modf(load1m)
#        print iload, fload

        loadbricks = 40 * fload
#        print "loadbricks = ", loadbricks
        ceilbricks = int(math.ceil(loadbricks))
#        print "ceilbricks = ", ceilbricks
        if ceilbricks > 40:
            ceilbricks = 40

        if ceilbricks == 0:
            g_msg = padout("\xfd")
        else:
            g_msg = padout(ceilbricks * "\x7f")

        cpucount = iload
#        print "cpucount =", cpucount
        cpucounti = int(cpucount)
#        print "cpucounti =", cpucounti
        
        if cpucounti > 40:
            cpucounti = 40

        for i in range(cpucounti):
            g_msg = g_msg[:i] + '\xfc' + g_msg[i:]
            
        tdelt = timedelta(seconds = uptime_seconds)

        if (uptime_seconds > (10 * 24 * 60 * 60)):
            us = str(tdelt).split(':', 2)
            uptime_string = 'up ' + us[0] + ':' + us[1]
        else:
            uptime_string = 'up ' + str(tdelt).split('.', 2)[0]
            
        # If uptime is less than a day, display the time zone and year
        if (uptime_seconds < (24 * 60 * 60)):
            date = subprocess.check_output(['date', '+%a %b %d %H:%M:%S %Z %Y']).strip()
        else:
            date = subprocess.check_output(['date', '+%a %b %d %H:%M:%S']).strip()

            
        # if uptime_seconds < (24 * 60 * 60):
        #     uptime_string = str(tdelt).split('.', 2)[0]
        # else:
        #     us = str(tdelt).split(':', 2)
        #     uptime_string = us[0] + ':' + us[1]
        #     uptime_string = re.sub('\s*day[s]?,\s*', 'd ', uptime_string)
        
        # date = subprocess.check_output(['date', '+%a %b %d %H:%M:%S %Z %Y']).strip()

#    print uptime_string
#    print date

#    ser404.write("\x04\x20")           # Set brightness to minimum (good for photos)

        load = ' '.join(rawload.split())
        load = 'Load ' + load
        # print "'" + load + "'"

        u_msg = padout(date + ' ' + uptime_string)
        i_msg = padout(ips)
        l_msg = padout(load)
        g_msg = padout(g_msg)

        # print "u_msg length =", len(u_msg), '|', u_msg, '|'
        # print "i_msg length =", len(i_msg), '|', i_msg, '|'
        # print "l_msg length =", len(l_msg), '|', l_msg, '|'

        ser404.write("\x10\x00")            # Home the display without clearing
        ser404.write("\x13")                # Ensure we're in overwrite mode

        ser404.write(u_msg)
        time.sleep(0.1)
        ser404.write(i_msg)
        time.sleep(0.1)
        ser404.write(l_msg)
        time.sleep(0.1)
        ser404.write(g_msg)
#        ser404.write(("\x7f" * 20) + ("\xfd" * 20))

        time.sleep(0.2)
    #    break

#except:
#    ser404.write("\x0b")
#    ser404.write("M404 monitor stopping...")
