-----------------
Instructions
-----------------
The machine will only accept an integer number of steps. For this reason, it is better to work in steps than in cm --> step conversions, which would result in more rounding errors. The range of the motors is ~1.75 in = 4.45 cm = 7000 steps. If you enter a float or string, it will go into an infinite loop. In the terminal, press "ctrl + c" to kill the program.

The natural speed of the motors upon turning on the device may be changed manually in the main() function. 2500 steps/s = 1.58 cm/s is the maximum speed that should be used. The default speed is 2000 steps/s = 1.27 cm/s

Calibration must be done manually. When the machine is turned on and off, it believes it is at (0,0). When the program is quit via -1, it automatically goes to the actual (0,0), so calibration will not have to be done if the -1 command is used. If the program is terminated manually (i.e. motor turned off during a program), you must manually calibrate it by moving it to the actual (0,0) (fastest way to do this is use command 8) and then turning the machine on and off.


-----------------------------
Specifications and maintenance
------------------------------
Motor screw type: P10. advance per step: 0.0002500 in. speed at 1000: .25 in/sec

To lubricate, do not use "silicone oils, heavy greases, or any type of spray lubricant that may contain strong solvents or fluorocarbons". This is the only information given by Velmex.

The standard RS-232 communication settings on the VXM are 9600 baud, 8 data, no
parity, and 1 stop bit. These will probably not need to be changed to run the program.


--------------------
Programming stuff
--------------------
There is a makefile in the folder. type "make" to compile the program and "./r" to run.

Before using on Linux, make sure to enable access to serial port for non-root users:
$ sudo chmod o+rw /dev/ttyS0
where ttyS0 is the serial port being used. You can see which one you have via getports(). If you are getting "system error" on checkerror(sp_open), this is probably it. To make the computer automatically do this whenever turned on, type in:
$ sudo usermod -a -G dialout <username>


common problem: "cannot open shared object file: no such file or directory"
solution:
$ echo $LD_LIBRARY_PATH
If nothing is displayed:
$ LD_LIBRARY_PATH=/usr/local/lib
$ export LD_LIBRARY_PATH

To make the computer automatically do this, go to the home directory and open .bashrc, then type in "export LD_LIBRARY_PATH=/usr/local/lib"


------------------
Bug testing:
------------------

printf("%s %d %s", "output waiting: ", sp_output_waiting(ports[0]), "\n"); (shows number of bytes waiting in buffer, drain() makes sure they go through)

printf("drain error: ");  checkerror(sp_drain(ports[0]));  (shows whether drain was succesful)

printf("%s %d %s", sp_blocking_write(ports[0], vxm_command, len, 500), "\n"); (shows number of bytes written)

checkerror() is self-explanatory and pretty useful.

