# Welcome to Gate Keeper IOT!

Hi! I'm the **Gate Keeper**, an IOT sensor to avoid the forgotten open garage door. I was made with ESP8266 (NodeMCU "version") with some integrations: - PushBullet to send notifications to cellphone and SinricPro to notify Alexa ecosystem, making possible to do a lot of integrations.

If you want to make your own Gate Keeper, feel free, and here I will share some informations to help you guy! :P

Materials:
-
-	NodeMCU (Esp8266) [link](https://pt.aliexpress.com/item/33053690164.html?spm=a2g0o.productlist.0.0.dd5110637Bu5xG&algo_pvid=d8aa9f51-9589-45ff-b7a1-ed7fcb66bd9c&algo_expid=d8aa9f51-9589-45ff-b7a1-ed7fcb66bd9c-4&btsid=0bb0623f16026316680272069e1797&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
-	Push Button
-	3 LEDs (I've bought Green, Red and White)
-	PCB Board (for prototyping) [link](https://pt.aliexpress.com/item/4000815013977.html?spm=a2g0o.productlist.0.0.35532e061eZ4KH&algo_pvid=639263ae-efe6-4378-a303-12988bbb231a&algo_expid=639263ae-efe6-4378-a303-12988bbb231a-10&btsid=0bb0622d16026317784321628eefc6&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
-	EndStop [link](https://pt.aliexpress.com/item/32816845922.html?spm=a2g0o.productlist.0.0.61b849d5r6wWzX&algo_pvid=e8e1b72d-ba9f-4185-9389-bfa3f54b15b7&algo_expid=e8e1b72d-ba9f-4185-9389-bfa3f54b15b7-4&btsid=0bb0624116026318333293077e45fa&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
-	Power Supply Module [link](https://pt.aliexpress.com/item/33012749903.html?spm=a2g0o.productlist.0.0.4308a2b6PNvVe8&algo_pvid=381adef4-145e-4ed6-a1a2-5e4c8b0656ae&algo_expid=381adef4-145e-4ed6-a1a2-5e4c8b0656ae-9&btsid=0bb0623916026318886035922e4f7e&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
-	3d Printed Case [link](https://www.tinkercad.com/things/jcsMkj5Jorg)
![3D Case](https://github.com/kadu/GateKeeperIOT/blob/master/assets/case.png?raw=true)

# CIRCUIT
This is a base circuit, feel free to change or add some new capabilities ;)
![Schematic of project](https://github.com/kadu/GateKeeperIOT/blob/master/assets/gateKeeper_bb.png?raw=true)

# Integrations
## PushBullet
I was made to send notifications to your phone, to be able to do this, it was necessary to have a help from a notification gateway, called push bullet, You will need to create a account in it, and when you was setting me up, I will request your API key, and a Fingerprint. The API key you will found on Pushbullet Painel, under Settings Menu, just click on "Create Access Token", save that code ;)
To get the Fingerprint, is just go to  https://www.grc.com/fingerprints.htm and on Custom Site Fingerprinting, just put URL **api.pushbullet.com** and get the strange Hex Code called (Security Certificate's Authentic Fingerprint), save that code too :P.
**Important**: The integration will use the email that you used to create your accound, if someone else need to be notified, just repeat the process with all people emails!
## Alexa
To have access to a whole world of integrations, was added inside my body the ability to talk with my old sister Alexa, to do this is necessary to communicate with a gateway called Sinric.Pro, this guy helps me to talk with my sister, we had a virtual fight in the past, because of that, just create an accound here https://sinric.pro/ and create a device under "Device" menu, click in "Add Device", in **Device Type** choose **Contact Sensor** and next, next ;). After that you will need to copy some characters strings, first one is you *DEVICE ID*, and on **Credenticals** you will find *APP KEY* and *APP SECRET*, and as I said before, save all this things, you will need to configure me!
## Gatekeeper IOT - (It's ME, and I'm not Mario)
There are something important to you known about me, first one: Be Kind with me, I'm a hardware with feelings ;), secound, I need internet to work, I don't know another protocols, just TCP/IP ;).
The easiest way to configure me is double click the giant button in my case, with this, my Green Light will blink Fast and I will create an Access Point to you access by your phone or some other device with WiFi, the access point will be called -> **GateKeeperIOT**, a perfect name, what do you think ? :)
Just connect on IT, automaticly will open a configuration Portal on your device with a Screen like this (image below), just click in **Configure Wifi**, just follow the screen and fill the information, your Wifi Credentials, and the Pushbullet and Alexa keys, Remember I told you to write it down right?

You be able to configure after how many minutes I will send notifications message, and I will use this value to resend notifications, It means that if you forgot Garage Door opened after the value that you was configured, I  will send a notification, and if anyone didn't close the Garage Door, I will notificate you again after this interval.
Here I need to whisper something to you, You know that big button in my case, if you just press it, the notification will be snoozed in 30 minutes, it was made to you that can't let your visits go, and saying goodbye for a long time, a long hand shake and things like that ;). And you need to know that I'm a good Gate Keeper, if after this snooze time, you forgot your Garage Door Open, I will let you known, don't worry ;)

And now, just hit the **SAVE** button and *VOALÀ!, if all the keys are correct, I will connect to the *web* and keep waiting for someone that forgot to close the Garage Door and I will **scream loud!**

**Another Important thing**
I was made to work all day and all night, without food and water, but sometime my brain can try to fly out of this little case, in this time you only need to press the big button for a long time (4 secounds) and when you release the button, I will do a *haraquiri* and restart myself. It hurts, don't do this all the time, please, I beg you!

Thank you! \o/

And all robots can count sheep now!

![Wifi Configuration Screen](https://github.com/kadu/GateKeeperIOT/blob/master/assets/gatekeeperiot_wificonfig.jpg?raw=true)
