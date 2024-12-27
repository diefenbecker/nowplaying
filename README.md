# nowplaying vinyl holder

Where to put the vinyl record sleeve while it is spinning on the record player? Until now, the sleeve has been placed next to the record player. In itself a good place and no problem, but there are also smart holders on the Internet. These usually display the words “Now playing” or “Now spinning” and hold the record on the wall, on the sideboard or wherever by means of a suitably shaped wooden, acrylic or metal structure. I found that boring, so I had to find another solution.

If there's already a holder like this standing around, it can do something while there's no record sleeve in it. What could be more obvious than to use a microcontroller with a display? The result is a first prototype of my “Now Spinning” record holder.

I used a NodeMCU with ESP8266 and eight MAX7219 LED 8×8 matrix modules. The display is set to a permanent text (in my case “NOW SPINNING”) by means of a button that remains pressed by the set record sleeve.

If the button is not pressed (i.e. the plate is stowed somewhere else again), various information is shown on the display, the display of which changes every 5 seconds. This would be the internal temperature, which is recorded by the BME280 sensor, and then various other data that is not measured locally, but is transferred to the NodeMCU “from outside” by calling an HTTP endpoint.

This HTTP post is done by a simple PHP script which is called via cron on a Raspberry Pi. The data to be displayed is read by this script from my FHEM (yes, I still use FHEM and am still very happy with it) and the URL of the NodeMCU is called with the data to be transferred. Five arguments are currently being processed:

- Outdoor temperature
- Battery status of the PV system
- Current power of the PV system
- Temperature of the top water buffer
- Temperature of the water buffer at the bottom

Calling up the URL and transferring the data to the NodeMCU looks like the following example:

`<ip_des_nodemcu>/receivedata?outdoor_temperature=17.3&accu_state=67&pv=1.5&buffer_top=70&buffer_bottom=56`

In the PHP script (senddata_example.php), the reading data from FHEM is queried via CURL, then the URL with the measured values is assembled and then called again via CURL from the NodeMCU. Why PHP: Because it was the quickest way for me. When I have time, I'll change it to Python. So far it has worked without any problems.

This script can then be called via linux cronjob

`*/3 * * * * php /usr/local/bin/senddata_example.php`

The code for the NodeMCU is relatively straightforward so far. The WLAN access data still has to be permanently assigned in the code. I use the Parola Library for the MAX7219. For the BME280 the library from Adafruit. The switch is connected to D0 and GND.
