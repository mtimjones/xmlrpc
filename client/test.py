#!/usr/bin/python

import xmlrpclib

server_url = 'http://192.168.2.151/device'
server = xmlrpclib.Server(server_url);

result = server.getDeviceStats( "username", "password", 0 );

print "Status : ", result['status']
print "Last Command was : ", result['lastCommand'];
print "Current State is : ", result['currentState'];

