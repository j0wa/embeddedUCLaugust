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
#include "dev/button-sensor.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "sys/log.h"
#include "utils.h"
#include "protocol.h"

//For the temperature sensor
#include "dev/i2cmaster.h" 
#include "dev/tmp102.h"

//Take the temperature every 10 seconds
#define TMP102_READ_INTERVAL (10 * CLOCK_SECOND) 

static struct simple_udp_connection udp_conn;
int clientId = 0; 
bool subbed = false; 

/*---------------------------------------------------------------------------*/
PROCESS(temp_process, "temp_process");
AUTOSTART_PROCESSES(&temp_process);

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

    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(temp_process, ev, data)
{
    static struct etimer init_timer;
    static struct etimer read_interval;

    uip_ipaddr_t dest_ipaddr;

    PROCESS_BEGIN();
    
    printf("Starting the temperature sensor\n");

    /* Initialize protocol connection */
    simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_default_callback);
    etimer_set(&init_timer, SEND_INIT_INTERVAL);

    while(clientId == 0) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&init_timer));
        if(clientId == 0){
          if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
          message *msg = message_new();
          message_set_type(msg,INIT);
          message_set_action(msg,TEMPERATURE_SENSOR);
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

    //initializing the temperature sensor
    int16_t  tempint;
    int16_t  raw;
    uint16_t absraw;
    int16_t  sign;
    tmp102_init();

    etimer_set(&read_interval, TMP102_READ_INTERVAL); 

    while(1) {  
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&read_interval));  // wait for timer expiration
        if(subbed){
            /*
            Code provided by zolertia to use de temperature sensor
            http://zolertia.sourceforge.net/wiki/index.php/Mainpage:Contiki_drivers#TMP102_sensor
            */
            sign = 1;
            raw = tmp102_read_temp_raw();  // Reading from the sensor  
            absraw = raw;
            if (raw < 0) { // Perform 2C's if sensor returned negative data
            absraw = (raw ^ 0xFFFF) + 1;
            sign = -1;
            }
            tempint  = (absraw >> 8) * sign; //tempint is the only data we need, we can get more precise information if needed in the future.

            if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) { 
                message *msgTemp = message_new();
                message_set_type(msgTemp,ACTION);
                message_set_clientId(msgTemp,clientId);
                tempint = tempint + 50 + 50; //adding 50 degree to take negative value into account, and 50 for the reserved space for action codes in the action field.
                message_set_action(msgTemp,tempint);
                simple_udp_sendto(&udp_conn, msgTemp, 3, &dest_ipaddr);
                free(msgTemp);
            }
            else {
                printf("Lost the borderRouter\n");
            }
            etimer_restart(&read_interval);
        }
    }
    PROCESS_END();
}