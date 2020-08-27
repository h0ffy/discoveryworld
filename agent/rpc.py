#!/usr/bin/env python
import sys,os
import socket
import MySQLdb
import conf
from geoip import geolite2

def db_exists(db,str_type,str_arg):
	try:
		cursor = db.cursor()
		if str_type == "IP":
			cursor.execute('SELECT (IP) FROM targets WHERE IP="%s"' % (str_arg))
			result = cursor.fetchone()
		if str_type == "WEBCMS":
			cursor.execute('SELECT (URL) FROM cmsweb WHERE URL="%s"' % (str_arg))
			result = cursor.fetchone()
		if result == None:
			return(0)
	
	except:
		pass

	return(1)



def processor(db,raw):
	if parser(db,raw) == 1:
		return(1)
	else:
		# regular expresion tester
		return(0)

	return(0)



def adquisitor(db,raw_array, option):
	i=0
	for raw in raw_array:
		if i != 0 and option==1:
			ret = processor(db,raw)
			sys.exit(0)
		else:
			ret = processor(db,raw)
			sys.exit(0)
		i+=1	
	
	return(ret)

def parser(db,text):
	try:
		cursor = db.cursor()	
		text = text.strip("\n").strip("\r")
		str_type = str(text).split("####")[0]
		str_args = str(text).split("####")[1]
		RECOGNIZE_STATUS=1
### DOMAIN
		if str_type == "DOMAIN":
			list_args = str_args.split(",")
			domain =  list_args[0]	#	domain
			ip =  list_args[1]	#	ip
			query_insert = "INSERT INTO domains (TID,DOMAIN) VALUES ('%d', '%s')" #% (ip,domain)
			#query_update = 'UPDATE domains SET DOMAINS="%s" WHERE IP="%s"'
			query_listing = "SELECT TID,DOMAIN FROM domains WHERE TID='%d'"
			all_domains = []			
			duplicate = 0
			
			#if db_exists(db,"IP",ip)==0:
			#	cursor.close()
			#	return(1)
				#cursor.execute(query_insert)
			#else:
			cursor.execute("SELECT TID FROM targets WHERE IP='%s'" % (ip))
			res  = cursor.fetchone() 
			tid = int(res[0])
		
			cursor.execute(query_listing % tid)
			results = cursor.fetchall()
			if results != ():
				for row in results:
					dbdomain = row[1]
					all_domains.append(dbdomain)
					if domain == dbdomain:
						duplicate=1	
		
			if duplicate == 0:
				cursor.execute(query_insert % (tid, domain))
				db.commit()
			else:
				print "Exists: %s\r" % (domain) 			

			cursor.close()	
			return(1)
				

### IP
		elif str_type == "IP":
			list_args = str_args.split(",")
			if len(list_args)==2:
				ip = list_args[0]	#	IP
				geo = list_args[1] 	#	GEO
			else:
				ip = str_args
				match = geolite2.lookup(ip)
				try:
					geo = str(match.country)
				except:
					geo = "ANY"
					pass

			query_insert = ( "INSERT INTO targets (IP,GEO) "
					 "VALUES ('%s','%s')" % (ip,geo)
			)
			if db_exists(db,str_type,ip):
				cursor.close()
				return(1)
			else:
				data = (ip,geo)
				cursor.execute(query_insert)
				db.commit()
				cursor.close()
				return(1)

			cursor.close()
			return(0)
### WEBCMS		
		elif str_type == "WEBCMS": 
			list_args = str_args.split(",")

			if len(list_args) != 3:
				return(0)

			url = list_args[0]		#	url
			cms_name = list_args[1] 	#	cms name
			cms_version = list_args[2]	#	cms version
			whversion = list_args[3]
				
			if db_exists(db, str_type, url) == 1:
				cursor.close()
				return(1)

				
			domain = url.split("/")[2]
			domain = domain.split(":")[0]
			cursor.execute("SELECT TID,DID FROM domains WHERE DOMAIN='%s'" % (domain))
			#debug
			row = cursor.fetchone()
			tid = int(row[0])
			sid = int(row[1]) # sid is subdomain id not implemented in v1 (vincule to DID domain id) 
			query_insert = "INSERT INTO cmsweb (TID,SID,URL,CMS_NAME,CMS_VERSION,WH_VERSION) VALUES ('%d','%d','%s', '%s', '%s','%s')" % (tid,sid,url, cms_name, cms_version,whversion)
			cursor.execute(query_insert)
			db.commit() 	
			cursor.close()
			return(1)
### MAIL
		elif str_type == "MAIL":
			print list_args[0]	#	mail
	
		else:
			cursor.close()
			return(0)

	
		return(1)


	except:
		print "Error 2"
		pass

	return(0)

def put(text):
 	db = MySQLdb.connect(conf.DBHOST,conf.DBUSER,conf.DBPASS,conf.DBNAME)	
	parser(db,text)	
	db.close()

def get_domains(ip):
	db = MySQLdb.connect(conf.DBHOST,conf.DBUSER,conf.DBPASS,conf.DBNAME)
	cursor = db.cursor()
	query_select = "SELECT domains.DOMAIN FROM domains INNER JOIN targets ON domains.TID = targets.TID  WHERE targets.IP='%s'" % (ip)
	cursor.execute(query_select)
	results = cursor.fetchall()
	domains = []

	for result in results:
		domains.append(result[0])
	

	cursor.close()
	db.close()

	return(domains)

