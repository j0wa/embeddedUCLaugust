/*
  Author :

  Jolan WATHELET

*/
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void listDevices(){

}

void sendOrder(){
    uint8_t clientId = 0;
}

void setAutomation(){

}

int main(){
    /* when used outside the contiki network
    printf("----------Initialisation----------\n");
    printf("Please provide the borderRouter IP : \n");
    printf("\n > ");
    */

    uip_ipaddr_t dest_ipaddr;
    bool borderRouterConnected = false;

    while(!borderRouterConnected){
        if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
            borderRouterConnected = true;
        }else{
            printf("Looking for the borderRouter, please wait.\n");
        }
        sleep(5);
    }

    uint8_t choice = 0;

    while(true){
        choice = 0;
        printf("----------Menu----------\n");
        printf(" 1 - List Connected Devices.\n");
        printf(" 2 - Send an order to a device.\n");
        printf(" 3 - Set an automation.\n");
        printf(" 4 - Quit.\n");
        printf("\n > ");
        scanf("%u", choice);
        if(choice == 1){
            system("clear");
            listDevices();
        }else if(choice == 2){
            system("clear");
            listDevices();
            sendOrder();
        }else if(choice == 3){
            system("clear");
            listDevices();
            setAutomation();
        }else if(choice == 4){
            exit(0);
        }else {
            printf("Invalid option")
        }

    }

    
}