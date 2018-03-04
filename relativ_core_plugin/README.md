# Arduino Tracker
Plugin for OSVR that uses an Arduino + MPU6050 to do rotational headtracking. 

* Arduino code is based off of [Jeffs MPU6050 DMP6 example script](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)
* Plugin uses [Wills C++ serial library](https://github.com/wjwwood/serial)

# How to use
1. Open Arduino_Tracker_Sketch in the Arduino IDE, and upload it to your board.
    * Open the serial monitor and set the baudrate to 115200 and if you see something like this, it worked: ![Q:WW:XX:YY:ZZ](https://i.imgur.com/SwF4zel.png)
2. Place the dll into your osvr-plugins-0 folder.
3. Add the following to your osvr_server_config file, substituting "com4" with the com port your arduino is connected to:
```
"drivers": [{
		"plugin": "inf_osvr_arduino",
		"driver": "ArduinoTracker",
		"params": {
			"port":"com4"
		}
	}],
	...
	"aliases": {
		"/me/head": "/inf_osvr_arduino/ArduinoTracker/semantic/arduino"
	}
```
4. Run the calibration procedure the first time you use the plugin with the key combo CTRL + SHIFT + I. See notes below for more info...
5. Place your headset in the "Forward" direction and press the key combo CTRL + SHIFT + O to reset this position to 0 (looking directly forward on the Z axis)

##### notes
* The X axis of your MPU should be pointing toward the left, and y axis pointing toward you and be mounted on the front of your headset for the axes to align.
* Use the key combo CTRL + SHIFT + I from anywhere to begin the calibration procedure.
    * Calibration will sometimes hang. Simply reconnect the arduino to reset and try again (plugin will auto-reconnect to arduino).
    * You will need to run calibration the first time you use the plugin. The calibration procedure saves the offsets to eeprom so you should only need to run it once.
* Use the key combo CTRL + SHIFT + O from anywhere to reset your current rotation to 0 (looking directly forward on the Z axis).
	* Because the MPU6050 doesn't have a magnetometer, you may need to reset the rotation occasionally as it has a tendency to drift over time.
