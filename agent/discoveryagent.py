#!/usr/bin/env python
#	DiscoveryWorld Agent
#	Written by A.... / JennySec

import sys,os,socket
import urllib, urllib2
import threading
import conf
import daemon
import ip2domain
from agent import ClientAgent


def banner():
	print "\t*****************************************************************"
	print "\t*\t\t\t\t\t\t\t\t*"
	print "\t*\t\t\tDiscoveryWorld Agent\t\t\t*"
	print "\t*\t\t\t\t\t\t\t\t*"
	print "\t*****************************************************************"

def main():
	sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind((conf.LISTEN_IP,conf.LISTEN_PORT))
	
	while True:
		sock.listen(50)
		(clientsock, (ip,port)) = sock.accept()
		newThread = ClientAgent(ip,port,clientsock)
		newThread.start()

	sys.exit(0)
	return(0)



if __name__ == "__main__":
	banner()
	#with daemon.DaemonContext():
	main()
	
