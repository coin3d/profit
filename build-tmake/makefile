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
profit_mt: static_mt

static: makefile.release .release
static_mt: makefile.release_mt .release_mt

shared: makefile.profit .profit
shared_mt: makefile.profit_mt .profit_mt

objects:
	mkdir $@

releaseobjects:
	mkdir $@

.profit: objects
	@$(MAKE) -f makefile.profit

makefile.profit: profit.pro common.pro
	tmake -o makefile.profit profit.pro common.pro 

.profit_mt: objects
	@$(MAKE) -f makefile.profit_mt

makefile.profit_mt: mt.pro profit.pro common.pro
	tmake -o makefile.profit_mt profit.pro common.pro 

.release: releaseobjects
	@$(MAKE) -f makefile.release

makefile.release: release.pro common.pro
	tmake -o makefile.release release.pro common.pro

.release_mt: releaseobjects
	@$(MAKE) -f makefile.release_mt

makefile.release_mt: mt.pro release.pro common.pro
	tmake -o makefile.release_mt release.pro mt.pro common.pro

clean: makefile.profit makefile.release makefile.profit_mt makefile.release_mt
	@$(MAKE) -f makefile.profit clean
	@$(MAKE) -f makefile.release clean
	@$(MAKE) -f makefile.profit_mt clean
	@$(MAKE) -f makefile.release_mt clean

veryclean: clean
	rm makefile.profit makefile.release

