#!/bin/sh

# Author: Morten Eriksen, <mortene@sim.no>. Loosely based on Ralph
# Levien's script for Gnome.

DIE=false

PROJECT=Profit

AUTOCONF_VER=2.49a
AUTOMAKE_VER=1.4a
LIBTOOL_VER=1.3.5

# FIXME: check for minimum version number? 19990822 mortene.
if test -z "`autoconf --version | grep \" $AUTOCONF_VER\" 2> /dev/null`"; then
  cat <<EOF

  Invalid Version of Autoconf
  ---------------------------
  You must use the CVS development version of autoconf ($AUTOCONF_VER)
  to generate configure information and Makefiles for $PROJECT.
  You can find the pre-release snapshot at:

  ftp://alpha.gnu.org/gnu/autoconf/autoconf-2.49a.tar.gz

EOF
  DIE=true
fi

if test -z "`automake --version | grep \" $AUTOMAKE_VER\" 2> /dev/null`"; then
  cat <<EOF

  Invalid Version of Automake
  ---------------------------
  You must use the CVS development version of automake to ($AUTOMAKE_VER)
  to generate configure information and Makefiles for $PROJECT.

  The CVS automake repository can be fetched by running the following
  set of commands:

  $ cvs -d :pserver:anoncvs@anoncvs.cygnus.com:/cvs/automake login
  $ cvs -d :pserver:anoncvs@anoncvs.cygnus.com:/cvs/automake co automake

EOF
  DIE=true
fi

if test -z "`libtool --version | grep \" $LIBTOOL_VER \" 2> /dev/null`"; then
  cat <<EOF

  Invalid Version of Libtool
  --------------------------
  You must have libtool version $LIBTOOL_VER installed to generate
  configure information and Makefiles for $PROJECT.

  Get ftp://ftp.gnu.org/pub/gnu/libtool/libtool-1.3.5.tar.gz

EOF
  DIE=true
fi

# FIXME: check for more tools? 19990822 mortene.

$DIE && exit 1

echo "Running aclocal..."
aclocal

echo "Running autoheader..."
autoheader

echo "Running automake..."
automake

echo "Running autoconf..."
autoconf

echo
echo "Now type './configure' and 'make' to compile $PROJECT."

