#! /bin/sh
# **************************************************************************
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#   Morten Eriksen <mortene@sim.no>
#

PROJECT=Profit
MACRODIR=cfg/m4

AUTOCONF_VER=2.49b
AUTOMAKE_VER=1.4a
LIBTOOL_VER=1.3.5

echo "Verifying installed configuration tool versions..."

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



# abnormal exit?
${DIE=false} && echo "" && exit 1

echo "Running aclocal..."
aclocal -I $MACRODIR --output=cfg/aclocal.m4

echo "Running autoheader..."
autoheader -l cfg

echo "Running automake..."
cp cfg/aclocal.m4 .
automake
rm aclocal.m4

echo "Running autoconf..."
autoconf -l cfg

echo "Done."

