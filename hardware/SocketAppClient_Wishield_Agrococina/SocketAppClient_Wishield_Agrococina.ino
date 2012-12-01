        /*
        * Socket App
        *
        * A simple socket application example using the WiShield 1.0
        */

#include <WiShield.h>
#include <string.h>
extern "C" {   
    #include <uip.h>
}

    //    extern "C" int connectToServer(void);
           
#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,242};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"agrococina"};		// max 32 bytes

unsigned char security_type = 2;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"rhokbilbao"};	// max 64 characters


// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 1
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 2
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00	// Key 3
								};
// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

// End of wireless configuration parameters ----------------------------------------

        boolean connectToServer(void);
       
        char            insert[] = "INSERT@43.29564@-2.99729@supercocina@2100000000@4@arroz";
        char            update[] = "UPDATE@micrococina@114@24.0@60.3@5100500000@stop";
        char            *toSend;
        unsigned char   server_ip[] = {192,168,1,73};
        unsigned int    ip_port   = 9999;
        unsigned int    xmitCount = 0;        
        boolean         connectedToServer = false;
        boolean         inited    = false;       
        boolean         firstTime = true;
        unsigned long   nextXMitTime;
        unsigned long   period_to_update = 60000;
       
        char            buffer[105];

        void setup()
        {
           Serial.begin(9600);       
           Serial.println("starting");             
           connectedToServer = false;             
           WiFi.init();       
        }

        void loop()
        {  
        
           WiFi.run();
           delay(200);
           
           if (millis() > nextXMitTime && inited && !connectedToServer) {
              Serial.print("Connecting to server... ");
              xmitCount = 0;
              if (connectToServer()) {
                  connectedToServer = true;
                  Serial.println("connection OK");
                  delay(200);
              }
              else {
                  Serial.println("connection NOT OK");
              }
           }                 
        }
       
        // Make the initial connection to the remote server at 192.168.0.2
        boolean connectToServer(void)
        {                       
           struct uip_conn *conn;
           uip_ipaddr_t ipaddr;
           
           uip_ipaddr(&ipaddr, server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
           conn = uip_connect(&ipaddr, HTONS(ip_port));
           
          // uip_connect seems to never return NULL even in the case of a non-connect
          // Fortuantely you'll get a timeout if the connection is not good.
          // But it would be nice if this happened right away.
          return (conn != NULL);
        }
       
        // The 'extern "C"' allows these functions to be called directly by the framework.  You see the framework is
        // written in 'C' and Arduino code is compiled to C++.  The two are similar but not the same.
       
        // It's easer to have these functions here in the Arduino code because:
        // (1) You can use Serial.println commands
        // (2) Integration with other Arduino code is easier.
        // (3) No need to pass strings back and forth in large buffers.  String processing can be done directly on RX values

        extern "C" {
        

            // Called by the framework when framework is ready.
            // This function was previously in a "C" file.
            // Lets me know the app has been inited and is ready to connect.               
            void socket_app_init() {
                Serial.println("inited!!!!!");
                inited = true;
                nextXMitTime = millis();
            }
            
           
            // Called by the framework when there's an event which needs attention
            // Here's where the work gets done.
            // Actually is the uip.c where this function is called. MANDATORY
            void socket_app_appcall() {
                
                struct socket_app_state *s = &(uip_conn->appstate);      
                
                // connection timed out.  Not good but we can try again.  Retry immediately
                if (uip_timedout()) {
                    Serial.println("{timedout}");
                    connectedToServer = false;
                }   
              
                // A connection was just made and it's OK to send data.
                if (uip_connected()) {
                    Serial.println("{connected}");
                    //int iLen = strlen("PistoManchego"); //size of TXT-Record field
                    //toSend = (char *)malloc(strlen((const char*)"Pisto")+strlen((const char*)"Manchego")); 
                    //memset(toSend,'\0',iLen);
                    if(firstTime){
                      //strcpy(toSend, (const char*)"Pisto");
                      //strcat(toSend, (const char *)"Manchego");          
                      Serial.print("To send insert: ");
                      Serial.println(insert);
                      uip_send(insert, strlen(insert));
                      firstTime = false;
                    }
                    else{
                      Serial.print("To send update: ");
                      Serial.println(update);
                      uip_send(update, strlen(update));
                      //strcpy(toSend, (const char*)"PisNoManchego");
                    }
                    //Serial.print("To send: ");
                    //Serial.println(toSend);
                    //uip_send(toSend, strlen(toSend));
                    //nextXMitTime += 10000;        // XMit again in 60 seconds
                    //free(toSend);
                    
                }           
                // connection was broken.  Perhaps server went down or cable unplugged?  Retry immediately
                if (uip_aborted()) {
                    Serial.println("{aborted}");
                    connectedToServer = false;
                }
                
               
                // connection was closed.  This is usually expected after the transaction is complete
                // It seems we'll get this once with the UIP_NEWDATA flag set, and then again with the
                // UIP_NEWDATA cleared.  We only want to advance 'nextXMitTime' once, so we ignore the
                // UIP_CLOSED unless it has no new data
                if(uip_closed() && !uip_newdata()) {
                    Serial.println("{closed}");
                    connectedToServer = false;
                    //nextXMitTime += 10000;        // XMit again in 60 seconds
                    nextXMitTime += period_to_update;
               }   
                         
               // An error occurred and we need to retransmit.
               if (uip_rexmit()) {
                   Serial.println("{retransmit}");
                   uip_send(buffer, strlen(buffer));                       
               }       
               
               // The current packet is done being transmitted and now we're OK to transmit another packet.
               if (uip_acked()) {                                 
                  if (xmitCount < 10000) {
                      Serial.print("xmit: ");
                      Serial.println(xmitCount, DEC);
                      // Build a string that is 100 lines long
                      // We can actually transmit a packet up to the size of 'uip_mss()', which is tyipcally 327 bytes.
                     
                      sprintf(buffer, "<%04d>012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789ab\r\n", xmitCount);
                      xmitCount += strlen(buffer);
                      uip_send(buffer, strlen(buffer));       
                  }
                  else {        // send another linefeed to indicate the end of this POST data.
                      if (uip_connected()) {
                          uip_send("\r\n", 2);
                      }
                  }
                  PSOCK_CLOSE(&s->p);
               }

               // here's where new data arrives.  Do any string processing here.
               if(uip_newdata()) {         
                  Serial.print("something received..what to do with: ");    
                  //unsigned int len = uip_datalen();
                  //Serial.write((const uint8_t*)uip_appdata, len);
                  //Serial.print(len);
                  PSOCK_CLOSE(&s->p);    
               }             
               
            }
        }           
