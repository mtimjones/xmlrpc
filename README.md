# xmlrpc

Source code to an article I wrote in 2003 on a lightweight embeddable XMLRPC
server in C.

--

Lightweight Embedded XML-RPC Server                                             
                                                                                
This C based implementation of a lightweight XML-RPC server is written in       
C and has been tested with C and Python remote clients.                         
                                                                                
It can be built on a Linux host by simply typing 'make'.  It can be easily      
ported to other RTOS/Kernels -- the only requirement is a BSD-like sockets      
API (with support for select).                                                  
                                                                                
The XML-RPC server can be easily extended with user-defined functions.  New     
user functions can be placed in userfunc.c and then initialized via the         
'addMethod' call in init.c                                                      
                                                                                
Building client applications requires a client RPC library.  The two RPC        
libraries tested with this server were:                                         
                                                                                
  Eric Kidd's XML-RPC Library for C/C++                                         
  http://xmlrpc-c.sourceforge.net                                               
                                                                                
  XML-RPC for Python Website                                                    
  http://www.pythonware.com/products/xmlrpc                                     
                                                                                
