# Custom LED driver

* Connect LED anode pin to GPIO21 (pin 40 on RPI), and LED cathode pin to one of the GND pins (e.g pin 39 on RPI)
* clone the project and run Makefile with `make` command
* Load module into kernel with `sudo insmod led_device.ko`
* Turn the LED *ON* with `echo 1 > /dev/customled` and *OFF* with `echo 0 > /dev/customled`. Any other command prints error message to kernel console
* Unoad module from kernel with `sudo rmmod led_device`, LED turns OFF before module is unloaded
