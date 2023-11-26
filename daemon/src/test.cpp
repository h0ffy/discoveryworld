#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <curl/curl.h>
#include <tinyxml2.h>
#include <assert.h>
#include <openssl/ssl.h> 
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include "input.h"
#include "config.h"
#include "threads.h"
#include "output.h"
#include "discovery.h"
#include "net_utils.h"
#include "modules/protocol_http.h"
#include "modules/protocol_smtp.h"
#include "modules/module_ssl.h"
#include "modules/module_https.h"
#include "modules/module_ssh.h"
#include "modules/module_agent.h"
#include "modules/protocol_ftp.h"
#include "modules/protocol_whois.h"
#include "misc.h"

int main(int argc, char *argv[]) {
 config *conf = new config("/home/pumu/Repository/jennycorp/jennysec/jennysec/tools/discoveryworld/discoveryworld/daemon/etc/discoveryworld.conf");
 ssh *sh = new ssh();
 http *lhttp = new http();
 https *lhttps = new https();
 smtp *lsmtp = new smtp();
 whois *lwhois = new whois();
 
 printf("FASFS");
 ftp *lftp = new ftp();
 
 tinyxml2::XMLDocument xml_doc;
 tinyxml2::XMLError xml_error;
 tinyxml2::XMLNode *xml_root,*xml_body;
 /*
 assert(argc!=2);
 
 if((xml_error = xml_doc.LoadFile(argv[1]))!=tinyxml2::XML_SUCCESS) {
     printf("Error");
     return 0;
 }
   */
  
 char *host = "216.58.210.196";
 if(ssl::testport(host,443,0)==true)
   printf("SSL OK\n");
 else
   printf("SSL NO\n");
 

 lwhois->query("216.58.210.196");
 
 
 sh->finger("192.168.15.120",22);
 //lhttp->finger("192.168.15.21",80);
 /*
 lhttp->request_init(0,"/index.php",1,"www.google.com", 80);
 lhttp->request_make();
 lhttp->request_run();
 lhttp->request_headextract();
*/
 
 lhttps->request_init(0,"/",1,"216.58.210.196", 443);
 lhttps->request_make();
 lhttps->request_run();
 lhttps->request_headextract();
 
 
 lsmtp->finger("mail.o2.co.uk",25);
 
 
 
 //lftp->request_init();
 /*
 lftp->open("192.168.15.120", 21);
 lftp->request_add("SYST",NULL);
 lftp->build();
 
 printf("###### FTP #######\n");
 printf("%s\n\n", lftp->reply);
 printf("##################\n");
 
 fflush(stdout);exit(0);
 
 */
 printf("##### SSH #######\n");
 printf("Reply: %s\nFinger: %s\n", sh->reply, sh->cert);
 printf("#################\n\n");
 printf("##### HTTPS #######\n");
 printf("Reply: %s\n", lhttps->reply);
 printf("#################\n\n");
 printf("##### SMTP #######\n");
 printf("Reply: %s\n", lsmtp->reply);
 printf("#################\n\n");
 printf("##### WHOIS #######\n");
 printf("Reply: %s\n", lwhois->reply);
 printf("#################\n\n");
 
 
 /*
 printf("###### HTTPS ######\n");
 printf("%s\n", https_finger("216.58.210.196", 443));
 printf("###################\n");
 */
 
 return(0); 
}
