# ESP32-OTA-Webserver

I wanted to have a way to upload new firmware OTA by having the ESP32 act as a AP and serve up a web page to which I could upload a new .bin file. This is what this code does. This was built with Cmake using ESP-IDF 3.3.1

In main.c you can choose to connect to a router or act as a AP. 
For testing it's faster to just connect to your router and use the browser on your PC to point to its IP and upload your .bin file. 
Run this program in conjunction with the COM port so you can see what IP it was given. 
 
Once you know its IP you can open a browser and point to it to upload your new firmware .bin file. After the file has been uploaded it will apply it and reboot. Check the terminal to watch the info to see whats going on.  

Also, make sure if connecting to router or AP, you set those settings in menuconfig under "OTA WiFi Configuration"





  
![OTAWebserver](https://user-images.githubusercontent.com/6456652/62404696-fe067c00-b55b-11e9-98df-09f0a107ba3f.png)
