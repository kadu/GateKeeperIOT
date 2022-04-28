LED COLORS
----------

- (0) INICIANDO        - BLUE
- (1) CONECTADO        - GREEN
- (2) PORTAO FECHADO   - FAST BLINK GREEN
- (3) PORTAO ABERTO          - RED
- (4) NOTIFICACAO SILENCIADA - F BLINK RED
- (9) ERRO                   - YELLOW




# Welcome to Gate Keeper IOT!

Hi! I'm the **Gate Keeper**, an IOT sensor to avoid the forgotten open garage door. I was made with ESP8266 (Wemos D1 "version") with some integrations: **Alexa** and **IFTTT**,  making it possible to do a lot of integrations.

If you want to make your own GateKeeper, feel free, and here I will share some information to help you guy! :P

Materials:
-
-	Wemos D1 (Esp8266) [link](https://s.click.aliexpress.com/e/_AW9CTH)
-	Push Button
-	WS2812b Led
-	EndStop [link](https://s.click.aliexpress.com/e/_A1LsVD)
- Personalized PCB (Wemos shield) [link] (https://oshwlab.com/nicodemosbr/gatekeeperiot)

# Integrations
## Alexa
To have access to a whole world of integrations was added inside my body the ability to talk with my old sister Alexa, It's so simple, just search for a device in your Alexa ecosystem and you will find me. Don't worry if you see a light bulb with my name, it is me! If garage open, the light will be on.

## IFTTT
If you are able to configure a trigger event in IFTTT site, I have a configuration that made possible to send webhooks to an IFTTT endpoint, and this thing open the possibilities for integration.

## Gatekeeper IOT - (It's ME, and I'm not Mario)
There is something important to you know about me, first one: Be Kind to me, I'm hardware with feelings ;), second, I need internet to work, I don't know other protocols, just TCP/IP ;). The easiest way to configure me is double click the giant button in my case, with this, all my light will be turned off  and I will create an Access Point to you access by your phone or some other device with WiFi, the access point will be called -> **GateKeeper_XXXXXX**, a perfect name, what do you think? :) Just connect on IT, automatically will open a configuration Portal on your device with a screen like this (image below), just click in **Configure** Wifi, just follow the screen and fill the information, your Wifi Credentials and IFTTT Keys (if you want), Remember I told you to write it down right?
You be able to configure after how many minutes I will send notifications message, and I will use this value to resend notifications, It means that if you forgot Garage Door opened after the value that you was configured, I will send a notification, and if anyone didn't close the Garage Door, I will notify you again after this interval. Here I need to whisper something to you, You know that big button in my case if you just press it, the notification will be snoozed in 30 minutes, it was made to you that can't let your visits go, and saying goodbye for a long time, a long handshake and things like that ;). And you need to know that I'm a good GateKeeper, if after this snooze time, you forgot your Garage Door Open, I will let you known, don't worry ;)
And now, just hit the **SAVE** button and *VOALÀ!, if all the keys are correct, I will connect to the web and keep waiting for someone that forgot to close the Garage Door and I will **scream loud!**
**Another important thing** I was made to work all day and all night, without food and water, but sometimes my brain can try to fly out of this little case, in this time you only need to press twice the big button and I will do a harakiri and restart myself. It hurts, don't do this all the time, please, I beg you!
Thank you! \o/
And all robots can count sheep now!

![Wifi Configuration Screen](https://github.com/kadu/GateKeeperIOT/blob/master/assets/gatekeeperiot_wificonfig.jpg?raw=true)

![Wifi Configuration Screen II](https://github.com/kadu/GateKeeperIOT/blob/master/assets/gatekeeper_full_config.png?raw=true)

**Did you want a PCB to have my soul in IT ?**
You can get Gerber or buy my GatekeeperIOT Shield for Wemos D1 on [JLCPCB](https://jlcpcb.com/), check the [link](https://oshwlab.com/nicodemosbr/gatekeeperiot)