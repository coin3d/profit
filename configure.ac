# **************************************************************************
# configure.ac for SIM's profit library.

m4_define([PROFIT_MAJOR], [0])
m4_define([PROFIT_MINOR], [9])
m4_define([PROFIT_MICRO], [1])
m4_define([PROFIT_RELEASE], PROFIT_MAJOR.PROFIT_MINOR.PROFIT_MICRO)

m4_define([INTERFACE_VERSION],  [0])
m4_define([INTERFACE_REVISION], [0])
m4_define([INTERFACE_AGE],      [0])

AC_INIT([profit], [PROFIT_RELEASE], [<profit-patches@coin3d.org>])

AC_PREREQ([2.49])

AC_CONFIG_AUX_DIR([cfg])
AC_CONFIG_SRCDIR([src/profit.c])

AC_PROG_CC
AM_PROG_CC_STDC

if test "x$am_cv_prog_cc_stdc" = "xno"; then
  AC_MSG_WARN(
    [you probably need an ANSI compatible C compiler to build profit!])
fi

AC_SUBST([VERSION], [$PACKAGE_VERSION]) # for make dist

AC_SUBST([PROFIT_MAJOR_VERSION], [PROFIT_MAJOR])
AC_SUBST([PROFIT_MINOR_VERSION], [PROFIT_MINOR])
AC_SUBST([PROFIT_MICRO_VERSION], [PROFIT_MICRO])

AC_DEFINE([PROFIT_MAJOR_VERSION],       [PROFIT_MAJOR],
  [the major part of the profit release version number])
AC_DEFINE([PROFIT_MINOR_VERSION],       [PROFIT_MINOR],
  [the minor part of the profit release version number])
AC_DEFINE([PROFIT_MICRO_VERSION],       [PROFIT_MICRO],
  [the micro part of the profit release version number])

AC_DEFINE([PROFIT_INTERFACE_VERSION],   [INTERFACE_VERSION],
  [the current profit interface version])
AC_DEFINE([PROFIT_INTERFACE_REVISION],  [INTERFACE_REVISION],
  [the current profit interface revision])
AC_DEFINE([PROFIT_INTERFACE_AGE],       [INTERFACE_AGE],
  [the current profit interface age])

AC_SUBST([PROFIT_LIBTOOL_VERSION_INFO],
  [INTERFACE_VERSION:INTERFACE_REVISION:INTERFACE_AGE])

AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([profit], [$VERSION])
AM_CONFIG_HEADER([config.h])

AM_DISABLE_STATIC
AM_MAINTAINER_MODE
AC_PROG_LIBTOOL

SIM_AC_COMPILE_DEBUG([
  CXXFLAGS="$CXXFLAGS -DPROFIT_DEBUG=1"
], [
  CXXFLAGS="$CXXFLAGS -DPROFIT_DEBUG=0"
])
SIM_AC_DEBUGSYMBOLS
SIM_COMPILER_WARNINGS

AC_CHECK_HEADERS([string.h])

# **************************************************************************
# Check if host CPU is bigendian or littleendian

AC_ARG_WITH(
  [bigendian],
  AC_HELP_STRING([--with-bigendian], [set endianness for architecture (default=auto)]), [
  case "${withval}" in
    yes)  want_bigendian=yes ;;
    no)   want_bigendian=no ;;
    auto) want_bigendian=auto ;;
    *) AC_MSG_ERROR(bad value "${withval}" for --with-bigendian) ;;
  esac
], [want_bigendian=auto])

if test "$want_bigendian" = "yes"; then
  AC_DEFINE(WORDS_BIGENDIAN, 1)
elif test "$want_bigendian" = "auto"; then
  AC_C_BIGENDIAN
  if test "$ac_cv_c_bigendian" = "unknown"; then
    AC_MSG_ERROR(couldn't decide on the architecture endianness - specify endianness with the --with-bigendian option)
  fi
fi

# **************************************************************************

AC_OUTPUT([
	Makefile
	src/Makefile
	src/util/Makefile
	src/nodes/Makefile
	include/Makefile
	include/profit/Makefile
	include/profit/util/Makefile
	include/profit/nodes/Makefile
	examples/Makefile
	examples/fltdump/Makefile
	examples/fltstrip/Makefile
        examples/fltnode/Makefile
	examples/flt2vrml/Makefile
])

# **************************************************************************
