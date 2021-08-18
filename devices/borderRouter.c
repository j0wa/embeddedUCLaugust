/*
  Author :

  Jolan WATHELET

  based on code from contiki-ng examples folder and Z1 official code examples
*/

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "protocol.h"

#include "sys/log.h"

static struct simple_udp_connection udp_conn;
int clientIdCounter = 1; 
activeDevices devices[256]; 

/*---------------------------------------------------------------------------*/
PROCESS(udp_server_process, "BorderUDPserver");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  message* msg_recieved = message_from_udp(data);
  if(msg_recieved->messageType == INIT){
    message *msgAck = message_new();
    message_set_type(msgAck,ACK_INIT);
    message_set_clientId(msgAck,clientIdCounter);
    //devices[clientIdCounter-1].clientId = clientIdCounter;
    //devices[clientIdCounter-1].deviceType = msg_recieved->action;
    //devices[clientIdCounter-1].deviceAddr = sender_addr;
    clientIdCounter++;
    simple_udp_sendto(&udp_conn, msgAck, 3, sender_addr);
    free(msgAck);
    LOG_INFO("AckInit sent\n");

  }
  messageType msgT = msg_recieved->messageType;
  LOG_INFO("msg = %d\n",msgT);
  LOG_INFO("Received request from ");
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");

}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
