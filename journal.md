---
title: "Horizon"
author: "Anthony Maxwell"
description: "A distraction-free ereader."
created_at: "2026-05-09"
---

(The first 3 entries are entered after the fact, due to not knowing the requirement of a journal. Beyond those three, however, screenshots are taken while working)

# May 9th: Began project!

Today I began the project by initializing PlatformIO. I haven't used PlatformIO before, however decided it would be a good idea to learn it rather than conventional arduino ide.

I began by adding in my MCU of choice. In this case, an ESP32-S3, and connecting the pins.

![MCU configuration](journal/image.png)

1. The power pins, connected to the power system shown in the next image.
2. GND, connected to the power system.
3. A reset button to reset the MCU.

I also created a power delivery system. I knew I'd use usb, however didn't think data was necessary. And so, I picked a 6 pin power only usb c receptacle. I used the first step down converter (5v -> 3.3v) chatgpt suggested, which in this case was the NCP1529A, with configuration to step down from 5v to 3.3v using resistors. This provides 3.3V, with GND provided by the usb.

![PD Configuration](journal/image2.png)

# May 10th: Making the MCU functional.

Today, I set out to provide everything a dev board normally would to the ESP32-S3. QSPI RAM, etc.
I also fixed some flaws in my PD system.

QSPI was simple. A quick google brang up the recommended chip, and the pins where very self explanatory.

![QSPI Chip](journal/image3.png)
![QSPI Esp-side](journal/image4.png)

I brought a battery into the PD loop, and chose the BQ24074RGT battery charger for it's simplicity.

Just add in the charger, with the battery.

![Battery charger](journal/image5.png)

That's all for today.

# May 11th: Adding the display.

Today brought a lot of progress. I'll show the progress in order of the github commits, which represent the sessions.

## First session

I couldn't dig up the docs, so took chatgpt's word about the pinout of the display driver I was using. Big mistake, as you'll see later.

![Display connectors](journal/image6.png)

No other changes, except for choosing the SPI positions on the ESP32-S3 for the display.

## Second session

Replaced the port with a data port. USB 2 is sufficient, so I used that. Everything else remained the same.

![USB port](journal/image7.png)

I dug up the docs for the display and found that chatgpt was horribly wrong.

![Pinout of display](journal/image8.png)

I also added the crystal for time, using the CDX-1293.

![Time crystal](journal/image9.png)

I also added decoupling capacitors.

![Capacitors example](journal/image10.png)

## Third session

I added the touch interface.

![alt text](journal/image11.png)

![alt text](journal/image12.png)

# May 13th: Antennas and PCB design

Today I added an antenna to the esp32.

[WIP] add img

I then later replaced it with a proper implementation.

![antenna config](journal/image13.png)

WIP add footprint img

I also replaced the PCB with a proper two layer one, the bottom layer being GND.

I later chose to make the PCB 4 layer, distributing 3.3v, GND and having two routing layers.

![PCB](journal/image14.png)

![PCB in 3D](journal/image15.png)

I also added haptic and a button for sleep. AFAIK, The PCB is complete!

![Final PCB](journal/image16.png)

I also did some driver work today, attempting to create an LVGL driver for the driver board.

# May 14th: Redid the PCB

Today I finally recieved advice on my pcb, and generally it was simple: Don't use chatgpt. So, I restarted and remade my antenna system and decapacitance, etc. from docs.

Notable schematic changes:

![Connected power pins](journal/image17.png)
Split power into three. I'm pretty sure there is supposed to be some seperation, but for now this is enough.

![Connected power pins properly](journal/image18.png)
I also properly applied decapacitance, with the proper capacitors for each pin, according to the image at the top of [this](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32s3/schematic-checklist.html) page.

![Redid antenna properly](journal/image19.png)
I redid the antenna setup according to the above link as well. A lot of the stuff was TBD however, so I'll take care of that later.

I also learnt a bit from the feedback, in relation to the pcb:

- Put the esp32 near the antenna, and keep the traces between them short
- Keep capacitors and similar close.

![PCB](journal/image20.png)
Eugh the web viewer is ugly.

# May 15th: Finished the new PCB!

I learnt about DRC, and used it to locate many unconnected connections I missed. Connecting them all up, I achieve this result. I also expanded keepout zone for the antenna.

![Final PCB](journal/image21.png)

I also went over the feedback I recieved again, and added in resistors to the status LED.

![Final PCB this time (I promise)](journal/image22.png)

I then remembered my SD card, and added it in with minimal effort. I did, however, have to expand the PCB a bit to make space for the footprint.

![SD Card schematic](journal/image23.png)
![SD Card PCB](journal/image24.png)

Now, seemingly done with the hardware for now, I wanted to delay going on to the CAD step, although that is probably going to be the fastest step. I've had enough of my macbook randomly stopping Lapse from recording.

Time for some firmware!!! I decided to forget about getting it to run on the device for now, and just focus on getting it to run on my PC with a good interface.

# May 16th: Began firmware

As I said before, I already had a basic setup that "should" render through the driver. I'd look at testing it later, however my first priority was getting some good DX.

My plan was simple:

- LVGL has native windows support
- Most of my firmware doesn't depend on native features
  Meaning logically, what I do next is very simple:
- The main os works on it's own. Specific functions are provided by "native" files, with a condition on which to use.

So, I began.
