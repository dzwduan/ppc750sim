all:
	cd ppcemul; make all
	cd powerpc; make all

clean:
	cd powerpc; make clean
	cd ppcemul; make clean

