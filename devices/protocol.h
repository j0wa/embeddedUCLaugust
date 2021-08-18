/*
  Author :

  Jolan WATHELET

*/

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#ifndef PROTOCOL_H
#define PROTOCOL_H

enum action 
{TOGGLE = 0,
ON = 1,
OFF = 2,
SET_VALUE = 3
};

enum device 
{LIGHT = 0,
TEMPERATURE_SENSOR = 1,
WALL_SWITCH = 2,
THERMOSTAT = 3,
MOVEMENT_SWITCH = 4
};

typedef enum
{INIT = 0,
ACK_INIT = 1,
ERR_ID_UNDEFINED = 2,
START_SUBSCRIBE = 3,
STOP_SUBSCRIBE = 4,
ACTION = 5
} messageType;

typedef struct message{
    uint8_t messageType;
    uint8_t clientId;
    uint8_t action;
}message;

typedef struct activeDevices{
    uint8_t clientId;
    uint8_t deviceType;
    uip_ipaddr_t deviceAddr;
}activeDevices;

message *message_new();
void message_set_type(message *msg,messageType msgT);
void message_set_clientId(message *msg,uint8_t clientId);
void message_set_action(message *msg,uint8_t a);
message *message_from_udp(const uint8_t *data);

#endif