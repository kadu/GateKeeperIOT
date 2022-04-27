#pragma once

#include <ESP8266Webhook.h>

void envia_evento(String mensagem="", String key = "", String event = "") {
  if(key == "" || event == "") {
    Serial.println("Enviando evento para IFTTT Cancelado - Chaves não configuradas");
    return;
  }
  Serial.print("Enviando evento para IFTTT ");

  Webhook webhook(key, event);        //Create an object

  int request = webhook.trigger(mensagem);              //Trigger with 0 values
  if(request == 200){
    Serial.println("OK!");
  }
  else{
    Serial.println("Failed");
  }
}