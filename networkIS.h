#ifndef networkIS_H
#define networkIS_H

// #include <Ethernet_Generic.hpp>
// #include <EthernetClient.h>
// #include <EthernetServer.h>

#include <SPI.h>
#include <Ethernet.h>
#include "basic_funcs.h"
#include <ArduinoJson.h>
#include <stdlib.h>


#include "IO.h"

static byte Default_mac[] = {   0x00, 0x22, 0x8F, 0x10, 0xA0, 0x01  };

static IPAddress server_ip(192,168,224,4);//10,118,16,153);192.168.224.12
static IPAddress own_ip(192, 168, 224, 3);

//EthernetServer server(80);
#define DATE_RESPONSE_LINE 1

class request {
   private:

   public: 

   EthernetClient client,user_cmd;
   EthernetServer server = EthernetServer(80);
   
   // byte saved_mac[6];
   // byte* Get_MAC( );
   
   int networkCheck();
   void networkSetup();
   void try_connect_to_server();

   void sendTime();
   void read_data_back();
   void rdisconnect();

   void get_request();
   void handleRoot();

   void process_command(DynamicJsonDocument jsonDoc);
   void data_from_fifo_to_server();

   void get_info_from_other_core():

   void read_from

   
   
   
};


#endif