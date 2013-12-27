fast_tv_grab_nl
===============

Fast TV channel information grabber

fast_tv_grab_nl is a C++ TV channel information grabber.  
The functionality is similar to tv_grab_nl_PY (https://code.google.com/p/tvgrabnlpy).

fast_tv_grab_nl grabs TV channel and programme information from two possible providers:
* http://www.tvgids.nl
* http://www.horizon.tv

Information is converted into the XMLTV format.  
Programme categories are converted to the categories known by TVHeadend.

How to build:
------------

The current Makefile is created for compilation on a bootstrapped Synology DS1813+ (or any other x86_64 Intel Atom diskstation)  

Dependencies:
* boost-dev
* boost-date-time
* boost-filesystem
* boost-program-options
* boost-regex
* boost-serialization
* boost-system
* boost-thread
* libcurl-dev

Usage:
------

Create config file: (edit config file, and remove unwanted channels):
~~~~
./fast_tv_grab_nl -p <provider> --createconfig
~~~~
The config file can be found in ~/.xmltv/fast_tv_grab_nl.conf  
Grab channel info and feed into TVHeadend:
~~~~
/fast_tv_grab_nl -p <provider> --quiet | socat - UNIX:/usr/local/tvheadend/var/epggrab/xmltv.sock
~~~~  
  
J. Dierkse  
j.dierkse@madeo.nl
