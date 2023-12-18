# WaterR

WaterR is an open-source device developed in the Dodson lab at the University of Bristol to automate fluid restriction in experimental rodents. It allows the user to: set water-availability time windows, set a maximum daily volume allowance, or deliver ad libitum water to assess baseline water consumption. Automating water access-control offers the opportunity to significantly refine current practice. The ability to monitor water consumption on a cage-by-cage basis and the flexibility offered in programming different water-access regimens allows the implementation of tailored restriction approaches, while at the same time offering a standardised platform to aid reproducibility across different laboratories. 
<p align="center">
    <img src="img/final/WaterR_cages.jpeg" width="550">
</p>
&nbsp;

## List of components
In the table below there is a list of all components necessary to assemble WaterR. A source for each component is added for ease but every component is readily available from other sources as well. Links or part numbers are added for ease; part numbers are provided as manufacturer numbers instead of supplier as it makes it easier to source the items from a different supplier should you need to do so. Multi-buy options are provided when possible as they are cheaper, the quantity required for a single WaterR unit is listed in the table.  
Note: it is important that the power supply is a grounded, desktop power supply (i.e. a "brick") with a low ripple current (<100 mV peak-to-peak), otherwise it could interfere with the signal from the piezoelectric vibration sensor. These can be quite expensive (£40-50 each) but they provide up to 6-8 Amps, and as 1 WaterR unit uses ~500 mA you can safely power multiple units from one supply. Refrain from using a cheap switching power supply!

| Component                         | Source            | Catalog num / reference | Q.ty for 1 unit    |
|-----------------------------------|-------------------|-------------------------|--------------------|
| **Main items**                    |                   |                         |                    |
| ELEGOO Mega 2560                  | Amazon            | https://amzn.to/380VEm9 | 1                  |
| LCD keypad shield                 | Amazon            | https://amzn.to/3uISKfw | 1                  |
| Data Logging shield with RTC      | Amazon            | https://amzn.to/3ra0zZz | 1                  |
| Stepper Motor 28byj-48, 5V, 1:64  | Amazon            | https://amzn.to/2NCnqOH | 1                  |
| A4 acrylic sheet, 2mm             | Amazon            | https://amzn.to/2Ps0bYr | 2                  |
| Terumo 20ml syringe               | Medline Scientific| BS-20ES                 | 1                  |
| **Spout block**                   |                   |                         |                    |
| Piezoelectric sensor              | RS components     | 285-784                 | 1                  |
| Hook up wire, red                 | RS components     | 872-4460                | 30 cm              |
| Hook up wire, black               | RS components     | 872-4458                | 30 cm              |
| Resistor, 10 MΩ                   | Farnell           | RE03841                 | 1                  |
| Heath-shrink tubing               | Amazon            | https://amzn.to/3uOrmwP | various sizes      |
| 2.54 mm pin header , 2x1          | RS components     | 251-8086                | 1                  |
| Spiral Wrap                       | RS components     | 227-980                 | 15 cm              |
| Fitting, female luer to 1/16 barb | Cole-Parmer       | 45508-00                | 1                  |
| Fitting, male luer to 1/16 barb   | Cole-Parmer       | 45505-22                | 1                  |
| Tygon tubing, 1.6 ID x 3.2mm OD   | Cole-Parmer       | ACF00002                | 20 cm              |
| Surflo cathether 16Gx51mm         | VWR International | SR+OX1651C1             | 1                  |
| **Power**                         |                   |                         |                    |
| DC power splitter, 5 way          | Farnell           | C-PS-5W                 | 1 shared among 5   |
| DC power extensions, 1M           | Farnell           | PSG02089                | 1                  |
| Power supply, 9V, 4.5A            | RS components     | 175-9086                | 1 shared among 6-8 |
| **Fasteners**                     |                   |                         |                    |
| Screw, M3 x 20mm                  | Technobots        | 4304-320                | 4                  |
| Screw, M3 x 8mm                   | Technobots        | 4304-308                | 2                  |
| Screw, M3 x 6mm                   | Technobots        | 4304-306                | 14                 |
| Screw, M3 x 8mm, countersunk      | Technobots        | 4304-408                | 2                  |
| Nut, M3                           | Technobots        | 4310-025                | 6                  |
| Zip ties, 250mm	            | Amazon            | https://amz.run/5GKi    | 1                  |
| Dupont wires, 10cm                | Amazon            | https://amzn.to/3834WxT | 6 (f-m), 1 (m-m)   |
| CR1225 lithium battery            | Farnell           | CR1225                  | 1 (or 0)           |


## Tools
M3 Hex screwdriver (2mm)  
M3 Hex nut driver (5.5mm)  
Electric drill (4mm drill bit)  
Dremel  
Soldering iron  
Wire cutter


## 3D printed parts
You should print all the files in the /stl_parts folder. FDM printers are better suited for this job as it doesn’t require laser precision and it’s faster. Everything except for the Pump_motor_pinion.stl and the Spout_cover.stl can be printed at 0.15mm layer size, the former should be printed at least at 0.1mm or 0.06mm. Pump_rack should be printed sideways to avoid having weak line spots at the top of the teeth.


## Controller block
Take the keypad shield and cut the pin that would connect to pin 10 of the Arduino as shown in figure (this is to avoid conflicts with the clock/sd shield). 
<p align="center">
    <img src="img/arduino_block/slot_pin10removed_circle.jpg" width="400">
</p>
&nbsp; 

Take a male-male dupont cable and cut it in half. Solder the cut extremities onto the clock/sd shield, in the holes labelled SDA and SCL on the board (as shown in image). Sometimes the shield comes with 2.54 mm header soldered in this position, in this case just solder the wires to the corresponding header pins.
Add the CR1225 button battery in the clock/sd shield if it came without one. If it came with one you can still swap in a fresh one as these shields can sit on a shelf for years, affecting the amount of charge left in the battery.
<p align="center">
    <img src="img/arduino_block/rtcShield_wires.jpg" width="200">
    <img src="img/arduino_block/rtcShield_soldered.jpg" width="354">
</p>
&nbsp;

Slot the clock/sd shield onto the Arduino and connect the SDA wire on the SDA pin (pin 20) on the Arduino and do the same with clock/sd shield SCL and Arduino SCL (pin 21). Slot the LCD keypad shield on top of the clock/sd shield.
<p align="center">
    <img src="img/arduino_block/rtcShield_slotted.jpg" width="400">
</p>
&nbsp;

Now we can upload the software required to sync the clock and the WaterR firmware.


## Software
1. Install the [Arduino IDE](https://www.arduino.cc/en/software) if you have not done so already.

2. Download the WaterR repository to your computer.

3. Download the contents of the /libraries folder and paste them in your user/documents/Arduino/libraries folder. 

4. Connect the Arduino to your PC through USB. Run the IDE and make sure you select Tools -> Board -> Arduino Mega or Mega 2560. Also, in Tools -> Port select the COM port assigned to your Arduino.

5. In the /Arduino_sketches folder there are 3 sketches that should be uploaded to the Arduino. Open and Upload one after the other in the following order:

> Before uploading the sync_clock sketch, open the Serial Monitor. Set the baud rate to 9600.
- Upload **Sync_clock**.  
This sketch synchronizes the DS1307 clock chip on the Arduino RTC shield with the clock on your PC. If everything is wired correctly and the sketch successfully uploads, you should see the Arduino outputting date and time to the serial monitor. Now close the serial monitor. Make sure not to restart the Arduino (do not disconnect, reset, nor open the Serial Monitor) as this could introduce clock discrepancies.
- Upload **Load_defaults**.  
This sketch loads some default values in the Arduino EEPROM.
- Upload **WaterR_main**.  
This sketch is the main WaterR firmware. If you cannot read any character on the lcd screen, you should adjust the contrast by turning the screw on the blue potentiometer clockwise until you can comfortably see white characters. 
6. You can safely disconnect the Arduino from the PC. WaterR is thought as a stand-alone device so after this sketch is uploaded it should not be necessary to reconnect it to a PC unless you want to modify the main program, in which case you will need to re-upload the modified version.
<p align="center">
    <img src="img/arduino_block/contrast_clockwise.jpg" width="350">
    <img src="img/arduino_block/contrast_ok.jpg" width="350">
</p>


## Spout block
Remove the intravenous catheter from the packaging. Remove the plastic sleeve and the luer cap/filter. Cut it so that the metal cannula is 4 cm long. Best way of cutting it is making an incision with a dremel and then use pliers to snap it. Blunt the edge.
<p align="center">
    <img src="img/spout_block/spout_01.jpg" width="350">
    <img src="img/spout_block/spout_02.jpg" width="355">
</p>
&nbsp;

Slide a piece of 2.4mm heat-shrink tubing (enough to completely cover the piezoelectric sensor) on the metal end up to the proximal end.
Insert the piezo element between the heat shrink tubing and the metal cannula, with the 2 leads toward the female luer end. Apply heat to shrink the tubing, securing the piezo element.
<p align="center">
    <img src="img/spout_block/spout_03.jpg" width="350">
    <img src="img/spout_block/spout_04.jpg" width="350">
</p>
&nbsp;

Solder the 2 leads to 2 lengths of wire (30 cm).
<p align="center">
    <img src="img/spout_block/spout_08.jpg" width="400">
</p>
&nbsp;

Slide 1 pieces of 3.2mm heat shrink tubing (blue in the image) and a 6.4mm one (yellow) on the plastic part of the spout. Shrink it to secure the wires in place. Make sure that the leads of the piezo element are a bit loose and not too much in traction.
<p align="center">
    <img src="img/spout_block/spout_09.jpg" width="350">
    <img src="img/spout_block/spout_10.jpg" width="350">
</p>
&nbsp;

Strip the distal end of the wires as shown in the picture. Solder a 10 MΩ resistor at the wire junctions (so that it is wired in parallel to the piezo element) and a 2x1 header pin at the terminals. Cover the solder joints with heat shrink tubing. You can use small pieces of heat shrink tubing to keep the wires together and tidy.
<p align="center">
    <img src="img/spout_block/spout_05.jpg" width="300">
    <img src="img/spout_block/spout_05bis.jpg" width="300">
    <img src="img/spout_block/spout_07.jpg" width="300">
</p>
&nbsp;

Take 20 cm of tubing and connect it to the barbed end of both the male and female luer fitting.
Carefully thread the spout into the 3D printed hook support. Fix it in place using the male luer fitting, then screw the 3D printed cylinder into the thread to protect the piezo element (carefully not to damage the piezo element leads). Make sure the piezo is parallel to the ground (i.e. fully on top or fully below the spout) to maximize sensitivity.
Wrap both the tubing and the wires in the wire protector spiral (15 cm).
<p align="center">
    <img src="img/spout_block/spout_11.jpg" width="300">
    <img src="img/spout_block/spout_12.jpg" width="300">
    <img src="img/spout_block/spout_13.jpg" width="300">
</p>


## Pump block
Put the rack into its slot and make sure it slides freely with no resistance.
Slot the pinion on the stepper motor shaft. It should be tightly press-fit so print it again if it is even slightly loose.
Use 2 8mm screws + nuts to secure the stepper motor in position. The easiest way of doing this is to initially not tighten the screws all the way but have them slightly loose. Push the pinion onto the rack with one finger, then align the motor so that the 2 eyelets are horizontal. Tighten the screws, making sure the pinion is making full contact with the rack.
<p align="center">
    <img src="img/pump_block/pinion_fit.jpg" width="354">
    <img src="img/pump_block/motor_mounted.jpg" width="200">
</p>


## Main Assembly
Take the cage that you will be using the device with. Put the plastic feet on the top grid in the final position you want them to go. Put the first acrylic sheet on top of them and mark the position of the threaded holes of the feet.
<p align="center">
    <img src="img/acryl_drill/drill_feet.jpg" width="400">
</p>
&nbsp;

Position the pump block as shown, flush with the acrylic sheet on the side of the water bottle slot in the cage. Mark the screw holes position. Make sure the pump is not obstructing the hole for the plastic foot in the same corner. Also the flat standoff should not be in the rack’s way.
<p align="center">
    <img src="img/acryl_drill/drill_pumpBlock.jpg" width="400">
</p>
&nbsp;

Position the front panel, the corner and flat standoffs on the base acrylic sheet. Turn everything upside down (you will have to mark where to drill underneath the sheet). Mark the holes. Additionally, mark a small cutout as shown in the picture, this will make some space for when the keypad will be in place making it easier to access the buttons.
<p align="center">
    <img src="img/acryl_drill/drill_standoffs.jpg" width="350">
    <img src="img/acryl_drill/drill_standoffs_upsideDown.jpg" width="350">
</p>
&nbsp;

Position the stepper motor driver: put the front panel and the controller block where they will end up in the final product to make sure the driver is not going to be in the way but close enough to be wired with 10cm dupont wires. Mark the holes (2 opposing corners out of 4 will be enough).
With a 4mm drill bit cut all the holes you have marked so far. Use a Dremel (a small battery one should do the job) to remove the front buttons cutout.
<p align="center">
    <img src="img/acryl_drill/drill_positionDriver.jpg" width="400">
</p>
&nbsp;

Mount the keypad shield on the front panel with the 20mm screws + nuts as shown. Make sure not to overtighten it, having the nuts flush with the end of the screws is enough.
<p align="center">
    <img src="img/main_assembly/frontPanel_screws.jpg" width="350">
    <img src="img/main_assembly/frontPanel_keypad.jpg" width="350">
</p>
&nbsp;

Screw the front panel (with the keypad shield), the standoffs and the feet on the acrylic base sheet with 6mm screws.
<p align="center">
    <img src="img/main_assembly/front_stand_feet.jpg" width="400">
</p>
&nbsp;

Secure the motor driver with 2 6mm screws + nuts. Screw the pump block on with 2 countersunk 8mm screws + nuts. Connect the motor’s white jst connector to the driver white receptacle.
<p align="center">
    <img src="img/main_assembly/pumpBlock.jpg" width="281">
    <img src="img/main_assembly/pumpBlock_driver.jpg" width="500">
</p>
&nbsp;

Use male-to-female dupont wires to connect the Arduino with the motor driver as follows:  
	Pin31 -> in1  
	Pin33 -> in2  
	Pin35 -> in3  
	Pin37 -> in4  
	5V -> 5-12V (+)  
	Gnd -> 5-12V (-)  
<p align="center">
    <img src="img/wiring/wiring_motorDriver.jpg" width="400">
</p>
&nbsp;

Connect the 2 pins from the spout to A8 and A9 as shown in picture (it doesn’t matter which way around).
<p align="center">
    <img src="img/wiring/wiring_piezoSensor.jpg" width="400">
</p>
&nbsp;

Slot the Arduino + rtc/sd shield onto the keypad shield that is already secured to the front panel. Make sure wires are not under too much tension and make reliable contact.
<p align="center">
    <img src="img/wiring/slot_frontPanel.jpg" width="400">
</p>
&nbsp;

To secure the front panel assembly, slide a zip tie between the pcb and the lcd screen of the keypad shield as shown in the picture and tie it. Make sure the "head" of the zip tie ends on the back of the panel and not at the bottom or top (preventing the acrylic sheets from fitting). This step is easier if you temporarily unscrew the front panel from the base acrylic sheet.
<p align="center">
    <img src="img/front_panel/front_panel_ziptie_fit.jpg" width="350">
    <img src="img/front_panel/front_panel_ziptie.jpg" width="350">
</p>
&nbsp;

Take the cover acrylic sheet and put it on top of the standoffs and front panel. Mark the holes. Draw a 20x4cm cutout on top of the syringe module (the resulting square should share a side with the front edge and one with the right edge of the sheet as shown in figure. Drill the holes with a 4mm drill bit and use a Dremel to remove the cutout. Secure the cover sheet with 6mm screws.
<p align="center">
    <img src="img/top_panel/mark.jpg" width="350">
    <img src="img/top_panel/cut.jpg" width="350">
</p>


## Syringe Mod
Although the pump block can fit most 20ml syringes, Terumo 20ml syringes have been giving the best results in terms of volume accuracy, as the rubber plunger seal has a very thin ridge that minimizes resistance when the motor pushes it. You can reduce resistance further by removing the back ridge by just cutting it out with a pair of scissors as shown in the pictures. This is recommended for fixed volume and monitoring use cases.
<p align="center">
    <img src="img/syringe_mod/syringe.jpg" width="300">
    <img src="img/syringe_mod/syringe_plunger.jpg" width="300">
    <img src="img/syringe_mod/syringe_rubber_cap.jpg" width="169">
</p>

## Final product
<p align="center">
    <img src="img/final/top.jpg" width="300">
    <img src="img/final/side.jpg" width="300">
    <img src="img/final/front.jpg" width="300">
</p>


## WaterR operation
WaterR default screen shows the timers that trigger availability on and off again. On the right side of this screen, the volume of the allowance is displayed (if set).
You can navigate the menus with the up/down buttons, select a sub-menu with either the right or select buttons, and go back with the left button. If no buttons are pressed for 5 seconds, the screen goes back to default. 

### Sub-menus
* **Set Timer:** this allows you to set on and off timers. The little arrow indicates the hour/minute you are currently changing. Use up/down buttons to modify, select/right to ok and step to the next one. When the off timer minutes are set and select/right is pressed, timers are saved to memory.
* **Set Volume:** this allows to set the maximum volume that is available daily, in milliliters. Use up/down button to change, select/right to confirm, left to back.
* **Adjust syringe:** this allows you to manually move the motor, it is used when reloading a syringe.
* **Set piezo Thresh:** this allows you to set the sensitivity threshold for the piezoelectric element to trigger water delivery. It is set to 200 by default, it can be modified in increments of 5. After the initial setup, it should be calibrated in the following way: make sure both on and off timer as set at 00:00, and volume is set at 0 (this is ad libitum mode). Decrease the Piezo Thresh value in steps of 5, one step at a time, until continuous water delivery is triggered. Then, raise the Piezo Thresh by three/four 5-step above the value that trigger continuous delivery. When setting up a new unit, make sure to observe the mice long enough to see if they can easily trigger water delivery, without any false positive trigger.
* **Adjust Clock:** this option shows the current time. It can be manually adjusted, to compensate for clock drift or to implement daylight savings as appropriate.  

### Modes of operation
* **Ad libitum delivery:** both timers are set at 00:00, volume is set at 0. Water will be delivered every time a lick is detected.
* **Fixed time window availability:** volume is set at 0, timer on is set earlier than timer off. If current time is within timer on and timer off, water will be delivered when a lick is detected. Currently this window cannot go across days (i.e. you can set 17:30-21:45, but not 23:20-4:30).
* **Fixed volume:** timer on is set at the same time as timer off (i.e. 00:00-00:00, 4:00-4:00, 17:21-17:21). Volume is set at anything above 0. Water will be delivered when a lick is detected until the allowance is all used up. Current remaining allowance is shown in the default screen. Daily allowance is reset at time indicated by timer on and timer off (or when the device is reset/power cycled).
* **Hybrid:** timer on is set earlier than timer off, volume is set anything above 0. If current time is within timer on and timer off, water will be delivered unless the volume allowance has been used up. Daily allowance resets at the time indicated by timer on.
