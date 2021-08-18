/*
  Author :

  Jolan WATHELET

  based on code from contiki-ng examples folder and Z1 official code examples
*/

#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h> 
#include <ctype.h>
#include <stdlib.h>
#include "sys/log.h"
#include "utils.h"
#include "protocol.h"

#include "dev/leds.h"


static struct simple_udp_connection udp_conn;
int clientId = 0; 

/*---------------------------------------------------------------------------*/
PROCESS(light_process, "light process");
AUTOSTART_PROCESSES(&light_process);

/*---------------------------------------------------------------------------*/
static void
udp_default_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  message *msg_recieved = message_from_udp(data);

  if(msg_recieved->messageType == ACK_INIT){
    clientId = msg_recieved->clientId;
  }else if(msg_recieved->messageType == ACTION){
    if(msg_recieved->action == TOGGLE){
      leds_toggle(LEDS_ALL);
    }else if(msg_recieved->action == ON){
      leds_on(LEDS_ALL);
    }else if(msg_recieved->action == OFF){
      leds_off(LEDS_ALL);
    }
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light_process, ev, data)
{
    static struct etimer init_timer;
    uip_ipaddr_t dest_ipaddr;

    PROCESS_BEGIN();
    
    printf("Starting the connected light\n");

    /* Initialize protocol connection */
    simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_default_callback);
    etimer_set(&init_timer, SEND_INIT_INTERVAL);

    while(clientId == 0) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&init_timer));
        if(clientId == 0){
          if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
          message *msg = message_new();
          message_set_type(msg,INIT);
          message_set_action(msg,LIGHT);
          simple_udp_sendto(&udp_conn, msg, 3, &dest_ipaddr);
          free(msg);
          LOG_INFO("Init request sent\n");
        } else {
          LOG_INFO("Not reachable yet\n");
        }
        etimer_restart(&init_timer);
        }
    }
    LOG_INFO("id is %d",clientId);

    PROCESS_END();
}