#!/bin/sh
############################################################################
#
#  gen-changelog is a wrapper-script for generating ChangeLog files
#  using the cvs2cl script.
#

moduledir=${0%/[^/]*}
module=${moduledir##*/}
cvs2cl=$HOME/store/cvs/cvs2cl/cvs2cl.pl
headerfile=/tmp/$project.header
cd $moduledir

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

