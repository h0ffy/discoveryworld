import sys,os,socket
import urllib, urllib2
import threading
import conf
#import daemon
import ip2domain
#import rpc
#import cmsdiscovery
#import subdomains






newIP2Domain = ip2domain.IP2Domain("200.68.11.19")
newIP2Domain.domains = list(set(newIP2Domain.domains))

print(newIP2Domain.domains)
