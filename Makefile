export LOAD_LIBRARY_PATH="/usr/local/lib"

.PHONY: serial
serial:
	gcc -Wall -o r -L/usr/local/lib -L. -lserialport vxm.c
