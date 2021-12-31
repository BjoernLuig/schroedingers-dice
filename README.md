# Schrödingers Dice

A device, which generates truly random numbers using the timestamps of radioactive decay. It uses an <a href="https://github.com/2969773606/GeigerCounter-V1.1">open-source Geiger counter</a> to detect radioactive decays of a small amount of americanuim-241 which is contained in an <a href="https://en.wikipedia.org/wiki/Smoke_detector#Ionization">ionization smoke detector</a>. Each press of the big red button gives a new random number.

This project is highly inspired by <a href="https://www.youtube.com/watch?v=gwIGnATzBTg&list=FLIN6YKhyzmC_FIHTg9GV0EA&index=13&t=27s">The muon-powered, universe-bifurcating, random number machine</a> by <a href="https://www.youtube.com/channel/UCCWeRTgd79JL0ilH0ZywSJA">AlphaPhoenix</a>.

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/random.jpg" width="500"/>



## Functionality

### Setting the range for random numbers

The range of the random numbers can be set between 1 and 999 using the green and blue button.

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/random-selection.jpg" width="500"/>



### Activity and equivalent dose

It also shows the activity A and equivalent dose E. This mode can be accessed using the yellow button.

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/activity.jpg" width="500"/>



### Battery voltage

The battery voltage U can be displayed by pressing the yellow button twice.

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/voltage.jpg" width="500"/>



## How it works

### Physics

Radioactive decays are in contrast to a throw of a dice or the random generated numbers of a computer truly random. This might not be of any practical use but is philosophically very interesting. (Further information: https://www.youtube.com/watch?v=SxP30euw3-0, https://www.youtube.com/watch?v=ZuvK-od647c)

To detect the radiation of decays (or other radiations) we use a <a href="https://en.wikipedia.org/wiki/Geiger counter">Geiger counter</a> and as a source of radiation an <a href="https://en.wikipedia.org/wiki/Smoke_detector#Ionization">ionization smoke detector</a> containing americanium-241 (see picture below). (Booth can easily be obtained via the internet.)

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/americanium.jpg" width="500"/>



### Circuit

The components of this project are shown in the fritzing diagram below. An Arduino nano reads the signals of an Geiger counter, runs the code and displays everything on an LCD-display. It also reads the battery voltage using a voltage divider and an analog pin. The detection of a decay can be displayed with a lamp/LED. (In the Photos above, I even added a second lamp to indicate that the device is on.)

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/circuit.png" width="700"/>

The picture below shows all the electronics.

<img src="https://github.com/BjoernLuig/schroedingers-dice/blob/main/imgs/overview.jpg" width="700"/>
