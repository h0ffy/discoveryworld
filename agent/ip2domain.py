#!/usr/bin/env python
#	DiscoveryWorld Agent
#	Written by A.... / JennySec

import sys,os,socket
import re
import urllib, urllib2
import conf
import ip2domain

class IP2Domain:
	def __init__(self,ip):
		self.ip = ip
		self.domains = []
		self.run()
	def __enter__(self):
		return(self)
	def __del__(self):
		self.__exit__()
	def __exit__(self):
		i=0
		for domain in self.domains:
			self.domains[i] = ""
			i+=1
	def run(self):
		#self.ip2host()
		self.bing2ip()

	def bing2ip(self):
			page = 0
			uri_format = 'https://www.bing.com/search?q=IP%%3A%s&qs=n&FORM=PQRE&pq=IP%%3A%s&first=%d'	
			i = 0	

			for i in range(0,4):
				uri = uri_format % (self.ip,self.ip,page)
				response = urllib2.urlopen(uri)
				raw_html = response.read()
				response.close()
				page = int((page/10+1) + (page+10))
				extracted = re.findall('<li\sclass="b_algo".*?href="(.*?)".*?</a>',raw_html)
				
				for extracted_url in extracted:
					domain = re.findall(r'(?:[a-zA-Z0-9](?:[a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?\.)+[a-zA-Z]{2,6}',extracted_url)
					final = domain[0].strip("\n").strip("\r")
					self.domains.append(final)
			
				i+=1	

							

			

	def ip2host(self):
			uri = 'http://www.ip2hosts.com/csv.php?ip=%s' % (self.ip)
			response = urllib2.urlopen(uri)
			raw_csv = response.read()
			response.close()
			domains_list = raw_csv.split(",")
			for domain in domains_list:
				if(domain!=""):
					domain = domain.strip("\n").strip("\r").strip("http:\/\/").strip("https:\/\/")
					self.domains.append(domain)		
			

