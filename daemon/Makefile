# vie mar 28 23:23:30 CET 2014
#
# Arch linux:
# 	extra/geoip
# 	community/liboping
# 	extra/libbsd
#

CFLAGS = `mysql_config --cflags` --prefix=/opt --std=c++11
LDFLAGS = -g -loping -lpthread -lpcap -ltinyxml2 -lpolarssl -lnet -lconfig -lGeoIP -lcrypto -lc++ -lbsd -lssh `mysql_config --libs` --enable-add-ons=libidn,nptl --enable-stackguard-randomization --enable-obsolete-rpc --prefix=/opt --enable-sanity-checks --enable-recursive-check
OBJ_MAIN = obj/main.o obj/misc.o obj/output.o obj/input.o obj/discovery.o obj/config.o obj/threads.o obj/db.o obj/portscan.o obj/net_utils.o obj/services.o obj/netstatus.o obj/watchdog.o
OBJ_TEST = obj/test.o obj/misc.o obj/output.o obj/input.o obj/discovery.o obj/config.o obj/threads.o obj/db.o obj/portscan.o obj/net_utils.o obj/services.o obj/netstatus.o obj/watchdog.o
OBJ_MODULES = obj/modules/module_agent.o obj/modules/module_https.o obj/modules/module_ssh.o obj/modules/protocol_http.o obj/modules/module_ssl.o obj/modules/protocol_ftp.o obj/modules/protocol_whois.o obj/modules/protocol_smtp.o obj/modules/module_detector.o 
OBJ_CONTRIB = obj/contrib/slre.o
OBJ = $(OBJ_MAIN) $(OBJ_MODULES) $(OBJ_CONTRIB)
OBJT = $(OBJ_TEST) $(OBJ_MODULES) $(OBJ_CONTRIB)
INCLUDE = src/
EXE = bin/discoveryd
TEST = bin/test
CONF_FILE = etc/discoveryworld.conf
CC = gcc
INSTALLDIR = /opt/discoveryworld/bin
CONFIGDIR = /opt/discoveryworld/etc
LOGDIR = /var/log/discoveryworld

all: $(EXE) $(TEST)

obj/contrib/slre.o: src/contrib/slre.c
	$(CC) -c --std=c11 -o $@ -I. -Isrc/contrib/ $<

obj/main.o: 	src/main.cpp 
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/input.o: 	src/input.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/net_utils.o: src/net_utils.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/output.o: 	src/output.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/discovery.o: src/discovery.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/config.o: 	src/config.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/threads.o: 	src/threads.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/db.o:	src/db.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/misc.o:	src/misc.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/portscan.o:	src/portscan.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
obj/services.o:	src/services.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/watchdog.o:	src/watchdog.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
obj/test.o: src/test.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
obj/modules/protocol_http.o: src/modules/protocol_http.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/protocol_whois.o: src/modules/protocol_whois.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/protocol_smtp.o: src/modules/protocol_smtp.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/protocol_ftp.o: src/modules/protocol_ftp.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/module_ssh.o: src/modules/module_ssh.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

	
obj/modules/module_ssl.o: src/modules/module_ssl.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
obj/modules/module_https.o: src/modules/module_https.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/module_agent.o: src/modules/module_agent.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<

obj/modules/module_detector.o: src/modules/module_detector.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
obj/netstatus.o: src/netstatus.cpp
	$(CC) -c $(CFLAGS) -o $@ -I$(INCLUDE) $<
	
	
	

$(EXE): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^

#TEST
$(TEST): $(OBJT)
	$(CC) -o $@ $(LDFLAGS) $^


clean:
	rm $(OBJ)

clean-all:
	rm $(OBJ) $(EXE)
	

install:
	install -d $(INSTALLDIR)
	install -m 755 $(EXE) $(INSTALLDIR)
	install -d $(CONFIGDIR)
	install -m 660 $(CONF_FILE) $(CONFIGDIR) 
	install -d $(LOGDIR)
	install -m 660 $(LOGDIR)	

uninstall:
	rm -rf $(INSTALLDIR)
	rm -rf $(LOGDIR)
	rm -rf $(CONFIGDIR)


