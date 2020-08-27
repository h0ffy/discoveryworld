#!/usr/bin/env python
#	DiscoveryWorld Agent
#	Written by A.... / JennySec

import sys,os,socket
import re
import urllib, urllib2
import conf
import ip2domain
import commands
import whatweb

class CMSDiscovery:
	def __init__(self,url):
		self.url = url
		self.domain = url.split('/')[2]
		self.cms = ["",""]
		self.whversion = ""
		self.run()
	def __enter__(self):
		return(self)
	def __del__(self):
		self.__exit__()
	def __exit__(self):
		self.url = ""
		self.domain = ""
		self.whversion = ""
		self.cms = ["",""]
	def run(self):
		self.cmsversion()
		self.whweb()
							
	def cmsversion(self):
		try:
			command1_format1 = "ruby2.1 dist/cmversion.rb %s" % (self.url)
			ret = commands.getoutput(command1_format1)
			ret = ret.strip("\n").strip("\r")
			self.cms[0] = ret.split(",")[0]
			self.cms[1] = ret.split(",")[1]
		except:
			pass


#disable possible not include in final version
	def sublister(self):
		ret = []
		output_file = "/tmp/%s.txt" % (self.domain)
		command1_format1 = 'python dist/sublist3r.py -t %d -o /tmp/%s.txt -d 1 >/dev/null' % (self.domain,self.domain)
		
		try:
		        commands.getoutput(command1_format1)
		        f = open(output_file,'r')
		        subdomains = f.readlines()		
		        f.close()
		        os.remove(output_file)
			
			for subdomain in subdomains:
				ret.append(subdomain)

		        return(ret)
		except:
		        pass
		        ret=[]
		
		
		        return(ret)
		
#Implementation WhatWeb Class GPL
	def whweb(self):
	  try:
	    wh = WhatWeb(self.url)
	    wh.run()
	    self.whversion = wh.result
	    return(wh.result)
	  
	  except:
	     pass
	   
	  return(0)

