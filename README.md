# Custom LED driver

* Connect LED anode pin to GPIO21 (pin 40 on RPI), and LED cathode pin to one of the GND pins (e.g pin 39 on RPI)
* clone the project and run Makefile with `make` command
* Load module into kernel with `sudo insmod led_device.ko`
* Unoad module from kernel with `sudo rmmod led_device`, LED turns OFF before module is unloaded

## Turning LED ON and OFF, while module is loaded
* Turn the LED *ON* with `echo 1 > /dev/customled`  
* Trun the LED *OFF* with `echo 0 > /dev/customled`
* Any other command prints error message to kernel console
* **IF YOU DON'T HAVE PERMISSION TO WRITE TO MODULE, USE SUDO SU COMMAND BEFORE TURNING THE LED ON AND OFF**
