#***************************************************************************
#
# Makefile for Profit
#
# Usage:
#       make 			Makes Dime release version
#	make shared		Makes Dime development version
#
#***************************************************************************

release: makefile.release .release

shared: makefile.profit .profit

objects:
	mkdir $@

releaseobjects:
	mkdir $@

.profit: objects
	@$(MAKE) -f makefile.profit

makefile.profit: profit.pro common.pro
	tmake -o makefile.profit profit.pro common.pro 

.release: releaseobjects
	@$(MAKE) -f makefile.release

makefile.release: release.pro common.pro
	tmake -o makefile.release release.pro common.pro

clean: makefile.profit makefile.release
	@$(MAKE) -f makefile.profit clean
	@$(MAKE) -f makefile.release clean
	rm makefile.profit
	rm makefile.release
