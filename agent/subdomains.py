#!/usr/bin/env python
#	DiscoveryWorld Agent
#	Written by A.... / JennySec

import sys,os
import conf
import commands
import tldextract 

class SubDomains:
	def __init__(self,domain):
		tld = tldextract.extract(domain)
		self.domain = tld.domain + "." + tld.suffix
		self.domains = []
		self.run()
	def __enter__(self):
		return(self)
	def __del__(self):
		self.__exit__()
	def __exit__(self):
		self.domain = ""
	def run(self):
		self.sublister()		
							
	def sublister(self):
		output_file = "/tmp/%s.txt" % (self.domain)
		command1_format1 = 'python dist/sublist3r/sublist3r.py -t 1 -o /tmp/%s.txt -d %s' % (self.domain,self.domain)
		try:
		        commands.getoutput(command1_format1)
		        f = open(output_file,'r')
		        subdomains = f.readlines()		
		        f.close()
		        os.remove(output_file)
			
			for subdomain in subdomains:
				self.domains.append(subdomain.strip("\n").strip("\r"))
		except:
		        pass
		        self.domains=[]
		
