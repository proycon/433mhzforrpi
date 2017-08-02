433mhzforrpi
==============

Software for raspberry pi to control 433 mhz modules including old and new kaku (klik aan klik uit, Click-on-Click-off) modules.

Dependencies
-------------

Ensure you have a sane build environent and the wiringPi library:

Raspbian:

```
$ sudo apt-get install wiringpi
```

To compile wiringpi from scratch:

```
$ git clone git://git.drogon.net/wiringPi
$ cd wiringPi
$ git pull origin
$ cd wiringPi
$ ./build
$ cd ..
```

Compilation
-------------

Just run make:

```
$ make
$ sudo make install
```


Usage
-----------


Run the tool as follows:

```
$ 433send GPIO_PIN $protocol $group $unit of/off
```

Pins are numbered using the wiringPi pin numbering scheme, see https://projects.drogon.net/raspberry-pi/wiringpi/pins/:
Note you'll need sudo or add your user to the ``gpio`` group to be able to access the GPIO pins:

Protocol is one of:
* oldkaku -- Klik-aan-Klik-Uit (old style)
* newkaku -- Klik-aan-Klik-Uit (new style)
* elro -- Elro Flamingo and others, address configured with DIP switches
* action

Klik-aan-klik-Uit old style:

```
$ 433send 8 oldkaku M 10 on
```

Klik-aan-klik-Uit new style:

```
$ 433send 8 newkaku 123 1 on
```

Elro Flamingo:

```
$ 433send 8 elro 2 E on
```

To pair a new Kaku switch, send the on signal right after plugging it in.

Original Notes
===============

Bereik optimaliseren
Als je moeite hebt met het schakelen, is waarschijnlijk het bereik te klein.
Dit heeft meestal drie oorzaken:

1 De ontvanger zit in de buurt van veel electronica: verplaats deze naar een wat 'rustigere' plek
2 De antenne heeft niet de juiste lengte, dit moet een draad met een harde kern van exact 17,3 cm of een veelvoud hiervan zijn
3 Het vermogen is te laag, boost het vermogen van de transmitter naar maximaal 12V door die twee pinnen op een externe adapter aan te sluiten

Ontvanger
Ook kun je met de broncode aan de slag om de functionaliteiten uit te
bouwen. Voor de originele Arduino library klik hier. Een receiver
aankoppelen zou niet al te moeilijk moeten zijn!

Webserver
Koppel dit tooltje aan bijvoorbeeld een PHP of Node.js server om via je
browser of smartphone je lampen te schakelen met een mobiele website, NFC
tags bij de deur, Wi-Fi detectie.. you name it!

Bron: http://weejewel.tweakblogs.net/blog/8665/lampen-schakelen-met-een-raspberry-pi.html
