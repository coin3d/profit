#***************************************************************************
#
# Makefile for Profit
#
# Usage:
#       make 			Makes Dime release version
#	make shared		Makes Dime development version
#
#***************************************************************************

COPY = cp
SYMLINK = ln -sf

#***************************************************************************

all:: profit

screen:
	@echo -e "\033[34m\n" \
	"\t****************************************************************\n" \
	"\t*                       You are making Profit                         *\n" \
	"\t****************************************************************\n" \
	"\033[0m";

profit: static

static: makefile.release .release

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

makefile.release: release.pro
	tmake -o makefile.release release.pro common.pro

clean: makefile.profit makefile.release
	@$(MAKE) -f makefile.profit clean
	@$(MAKE) -f makefile.release clean

veryclean: clean
	rm makefile.profit makefile.release
	rm -rf objects/* releaseobjects/*

