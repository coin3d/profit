#!/bin/sh
############################################################################
#
#  gen-changelog is a wrapper-script for generating ChangeLog files
#  using the cvs2cl script.
#

project=profit
cvs2cl=$HOME/store/cvs/cvs2cl/cvs2cl.pl
projectdir=$HOME/code/coin/src/$project
headerfile=/tmp/$project.header

############################################################################
cat > $headerfile <<ENDOFHEADER
ChangeLog
=========

This is the ChangeLog file for the $project library.  This file is automatically
generated every night.  Entries are in reversed chronological order.

See http://www.red-bean.com/~kfogel/cvs2cl.shtml for information about the
cvs2cl script used to generate this file.

============================================================================

ENDOFHEADER
############################################################################

cd $projectdir

# generate ChangeLog, but strip off uninteresting entries
cvs log | $cvs2cl --stdin --header $headerfile --separate-header --prune \
  --ignore 'ChangeLog$' \
  --ignore '(Makefile\.in|configure|aclocal\.m4|config\.sub|config\.guess)$' \
  --ignore '(ltconfig|ltmain\.sh|missing|mkinstalldirs|stamp-h.*|install-sh)$' \
  --ignore 'config\.h\.in$' \
  --ignore 'conf-macros/' \
  --ignore "src/Inventor/$GUI/common/"

rm ChangeLog.bak $headerfile

cvs commit -m "Automatic ChangeLog generation" ChangeLog

