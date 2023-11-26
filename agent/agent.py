#!/usr/bin/env python
#	DiscoveryWorld Agent
#	Written by A.... / JennySec

import sys,os,socket
import urllib, urllib2
import threading
import conf
import daemon
import ip2domain
import rpc
import cmsdiscovery
import subdomains

class ClientAgent(threading.Thread):
	def __init__(self,ip,port,clientsock):
		threading.Thread.__init__(self)
		self.ip = ip
		self.port = port
		self.clientsock = clientsock
		self.sockstat = 1

	def run(self):
		try:
			domains = []
			url_list = []
			cms_list = []
			self.clientsock.send("###DiscoveryAgent\n\r")

			msg = self.clientsock.recv(1024)
			token = msg.split(";")[0]
			rhost = msg.split(";")[1]
			port = int(msg.split(";")[2])
			service = msg.split(";")[3]
			
			token = token.strip("\r").strip("\n")
			rhost = rhost.strip("\r").strip("\n")
			service = service.strip("\r").strip("\n")

			if conf.TOKEN==token:
				self.clientsock.send("###AUTHOK\n\r")
			else:
				self.clientsock.close()	
				return(0)
					
			self.clientsock.send("###RHOST" + rhost + "\n\r")
			if service=="HTTP" or service=="HTTPS":
				domains_list = rpc.get_domains(rhost)
				urls = []
				urls.append("%s://%s:%d" % (service.lower(), rhost,port))
				for domain in domains_list:
					urls.append("%s://%s:%d" % (service.lower(), domain, port))
				for url in urls:
					newCMSDiscovery = cmsdiscovery.CMSDiscovery(url)
					if newCMSDiscovery.cms[0]!="" and newCMSDiscovery.cms[1]!="" or newCMSDiscovery.whversion!="":
						rpc.put("WEBCMS####%s,%s,%s,%s" % (url,newCMSDiscovery.cms[0],newCMSDiscovery.cms[1],newCMSDiscovery.whversion))
				# WHATWEB (I dont like write f** whatweb parser)
				# while url in url_list:
				#	outfile = whatweb(url) 		
				#	whatweb2mysql(outfile)
				#	os.remove(outfile)
			elif service=="RESOLVE":
				rpc.put("IP####" + rhost)
				print("IP####" + rhost)
				newIP2Domain = ip2domain.IP2Domain(rhost)
				newIP2Domain.domains = list(set(newIP2Domain.domains))
				for domain in newIP2Domain.domains:
					rpc.put("DOMAIN####" + domain + "," + newIP2Domain.ip)  # add domain to DB by RPC
					print("DOMAIN####" + domain + "," + newIP2Domain.ip)  # add domain to DB by RPC

				#sudbdomains = []
				#ips = []	
				#for domain in newIP2Domain.domains:
				#	try:
				#		newSubDomain = subdomains.SubDomains(domain)		# subdomains listing
				#		newSubDomain.domains = list(set(newSubDomain.domains))
				#		for subdomain in newSubDomain.domains:
				#			subdomain = subdomain.strip("\n").strip("\r")
				#			try:
				#				subip = str(socket.gethostbyname(subdomain))
				#			except:
				#				subip = "0.0.0.0" # after fix error from db by framework
				#	
				#			subdomains.append(subdomain)
				#			ips.append(subip)
				#			print("SUBDOMAIN####" + subdomain + "," + ips[i])	# add subdomain to DB by RPC
				#	except:
				#			pass
				#i = 0
				#for subdomain in subdomains:		
				#	rpc.put("DOMAIN####" + subdomain + "," + ips[i])	# add subdomain to DB by RPC
				#	print("SUBDOMAIN2####" + subdomain + "," + ips[i])	# add subdomain to DB by RPC
				#	i+=1	
				
			self.clientsock.send("###END\n\r")
			self.clientsock.close()
			self.sockstat=0

		except:
			if self.sockstat==1:
				self.clientsock.close()
			print "Error 1"			
			pass

