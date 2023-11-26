

from bs4 import BeautifulSoup
import cfscrape
from collections import deque
import json
import os
from os import system
import re
import requests
import requests.exceptions
import requests as res
from requests import get
import sys
import time
from time import gmtime, strftime
from urllib.error import URLError
from urllib.parse import urlsplit
import urllib3
import urllib.request
from urllib.request import urlopen
import urllib.parse
import webtech

def ip_location(target):
    try:
        target = input("\033[1;91m[+] Enter Domain or IP Address: \033[1;m").lower()
        url = ("http://ip-api.com/json/")
        response = urllib.request.urlopen(url + target)
        data = response.read()
        jso = json.loads(data)
        os.system("reset")
        print("\033[34m[~] Searching IP Location Finder: \033[0m".format(url) + target)
        time.sleep(1.5)

        print("\n [+] \033[34mUrl: " + target + "\033[0m")
        print(" [+] " + "\033[34m" + "IP: " + jso["query"] + "\033[0m")
        print(" [+] " + "\033[34m" + "Status: " + jso["status"] + "\033[0m")
        print(" [+] " + "\033[34m" + "Region: " + jso["regionName"] + "\033[0m")
        print(" [+] " + "\033[34m" + "Country: " + jso["country"] + "\033[0m")
        print(" [+] " + "\033[34m" + "City: " + jso["city"] + "\033[0m")
        print(" [+] " + "\033[34m" + "ISP: " + jso["isp"] + "\033[0m")
        print(" [+] " + "\033[34m" + "Lat & Lon: " + str(jso['lat']) + " & " + str(jso['lon']) + "\033[0m")
        print(" [+] " + "\033[34m" + "Zipcode: " + jso["zip"] + "\033[0m")
        print(" [+] " + "\033[34m" + "TimeZone: " + jso["timezone"] + "\033[0m")
        print(" [+] " + "\033[34m" + "AS: " + jso["as"] + "\033[0m" + "\n")

    except URLError:
        print("\033[1;31m[-] Please provide a valid IP address!\033[1;m")