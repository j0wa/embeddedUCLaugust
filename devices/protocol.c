/*
  Author :

  Jolan WATHELET

*/

#include "protocol.h"

message *message_new(){
    message *msg = malloc(sizeof(message));
    msg->clientId = 0;
    return msg;
}

void message_set_type(message *msg,messageType msgT){
    msg->messageType = msgT;
}

void message_set_clientId(message *msg,uint8_t clientId){
    msg->clientId = clientId;
}

void message_set_action(message *msg,uint8_t a){
    msg->action = a;
}

message *message_from_udp(const uint8_t *data){
    message *msg_recieved = (message*) data;
    return msg_recieved;
}
