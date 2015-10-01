About
=====

This tool is intended to be a simple mechanism to list and report network
interface information.  In the past, this information could be culled out
of ifconfig.  However, many Linux distributions are no longer including
ifconfig by default.  Also, the output of ifconfig can be somewhat
unpredictable.  One of the major design goals of this tool was to be output
friendly for shell script writers and system admins.  You should be able to
use this tool with sed, awk, cut, wc, etc..

License
=======

This software is copyright 2015 Bryan Christ and can be redistributed
and/or modified under the terms of the GNU General Public License version 2.

Credit
======

Much of the source code for this program was inspired by GETIFADDRS(3),
comments on Stack Overflow, and NETDEVICE(7).
