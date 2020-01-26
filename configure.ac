# **************************************************************************
# configure.ac for SIM's profit library.

m4_define([PROFIT_MAJOR], [0])
m4_define([PROFIT_MINOR], [9])
m4_define([PROFIT_MICRO], [1])
m4_define([PROFIT_RELEASE], PROFIT_MAJOR.PROFIT_MINOR.PROFIT_MICRO)

m4_define([INTERFACE_VERSION],  [0])
m4_define([INTERFACE_REVISION], [0])
m4_define([INTERFACE_AGE],      [0])

# **************************************************************************

AC_INIT([profit], [PROFIT_RELEASE], [<coin-support@coin3d.org>])
AC_PREREQ([2.50])

AC_CONFIG_AUX_DIR([cfg])
AC_CONFIG_SRCDIR([src/profit.c])

AC_CANONICAL_SYSTEM

CYGWIN=
export CYGWIN


sim_ac_build_library=true
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it and to generate an MSWindows .dll file.

SIM_AC_SETUP_MSVCPP_IFELSE([
  SIM_AC_SETUP_MSVCRT([
    CPPFLAGS="$CPPFLAGS $sim_ac_msvcrt_CPPFLAGS"
    CFLAGS="$CFLAGS $sim_ac_msvcrt_CFLAGS"
    CXXFLAGS="$CXXFLAGS $sim_ac_msvcrt_CXXFLAGS"
    LDFLAGS="$LDFLAGS $sim_ac_msvcrt_LDFLAGS"
    LIBS="$LIBS $sim_ac_msvcrt_LIBS"
    # sim_ac_msvcrt_LIBLIBS is set up at the end
    # sim_ac_msvcrt_LIBLDFLAGS is set up at the end
  ])
])

AC_PROG_CC
AM_PROG_CC_STDC

if test "x$am_cv_prog_cc_stdc" = "xno"; then
  AC_MSG_WARN([doesn't look like you are using an ANSI compatible C compiler])
fi

# bin/ltconfig has a problem with detecting C++ preprocessor and failing on
# /lib/cpp if this macro isn't included...
# AC_PROG_CXXCPP
# However, the problem can be avoided if you specify the CXX compiler, even
# though it is not used.

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

AM_INIT_AUTOMAKE([profit], [$VERSION])

AM_DISABLE_STATIC
AM_MAINTAINER_MODE
AC_PROG_LIBTOOL

# **************************************************************************
#  These are used for constructing the profit pkg-config file.
AC_SUBST([PROFIT_EXTRA_CPPFLAGS], [$CPPFLAGS])
AC_SUBST([PROFIT_EXTRA_CFLAGS], [$CFLAGS])
AC_SUBST([PROFIT_EXTRA_CXXFLAGS], [$CXXFLAGS])
AC_SUBST([PROFIT_EXTRA_LDFLAGS], [$LDFLAGS])
AC_SUBST([PROFIT_EXTRA_LIBS], [$LIBS])

AM_CONDITIONAL([BUILD_WITH_MSVC], [$BUILD_WITH_MSVC])

PROFIT_CFLAGS=
AC_SUBST(PROFIT_CFLAGS)

# Make it possible for the user to turn off optimization flags
SIM_AC_COMPILER_OPTIMIZATION

SIM_AC_COMPILE_DEBUG([
  PROFIT_CFLAGS="$PROFIT_CFLAGS -DPROFIT_DEBUG=1"
], [
  PROFIT_CFLAGS="$PROFIT_CFLAGS -DPROFIT_DEBUG=0"
])

SIM_AC_DEBUGSYMBOLS
SIM_AC_COMPILER_WARNINGS

AC_CHECK_HEADERS([string.h])

# *******************************************************************
# * Math library
# *******************************************************************

SIM_AC_CHECK_MATHLIB(
  [PROFIT_EXTRA_LIBS="$PROFIT_EXTRA_LIBS $sim_ac_libm"],
  [SIM_AC_ERROR([no-math-library])])

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

AC_ARG_WITH([suffix],
  [--with-suffix=string  specify library name suffix],
  [SUFFIX=$withval],
  [SUFFIX=])

AC_SUBST([SUFFIX])

AC_SUBST(LDFLAGS)

case $enable_static in
true | yes)  PROFIT_STATIC=true ;;
*)           PROFIT_STATIC=false ;;
esac

case $enable_shared in
true | yes)  PROFIT_SHARED=true ;;
*)           PROFIT_SHARED=false ;;
esac

LIBFLAGS=
AC_SUBST(LIBFLAGS)

if $sim_ac_build_library; then
  if $BUILD_WITH_MSVC; then
    rm -f vc60.pdb;
    LIBFLAGS="$LIBFLAGS $sim_ac_msvcrt_LIBLDFLAGS"
    LIBS="$LIBS $sim_ac_msvcrt_LIBLIBS"
    if $PROFIT_STATIC; then
      # static overrides shared
      :
    else
      PROFIT_CFLAGS="$PROFIT_CFLAGS -DPROFIT_MAKE_DLL"
      PROFIT_EXTRA_CPPFLAGS="$PROFIT_EXTRA_CPPFLAGS -DPROFIT_DLL"
    fi
    debugfile=`pwd`/src/profit${PROFIT_MAJOR_VERSION}${SUFFIX}.pdb
    debugfile=`cygpath -w "$debugfile" | sed 's,\\\\,\\\\\\\\,g'`
    if $PROFIT_STATIC; then
      LIBFLAGS="-LIB $LIBFLAGS"
      LIBFLAGS="$LIBFLAGS /OUT:profit$PROFIT_MAJOR_VERSION$SUFFIX.lib"
      PROFIT_EXTRA_CPPFLAGS="-I$includedir"
      PROFIT_EXTRA_LDFLAGS="-L$libdir"
      PROFIT_EXTRA_LIBS=
    else
      LIBFLAGS="-DLL /INCREMENTAL:NO $LIBFLAGS"
      case $enable_symbols in
      no | false)
	LIBFLAGS="$LIBFLAGS /RELEASE"
	;;
      *)
	PROFIT_CFLAGS="/Fd$debugfile $PROFIT_CFLAGS"
	LIBFLAGS="$LIBFLAGS /DEBUG /PDB:profit$PROFIT_MAJOR_VERSION$SUFFIX.pdb"
	;;
      esac
      LIBFLAGS="$LIBFLAGS /OUT:profit$PROFIT_MAJOR_VERSION$SUFFIX.dll"
    fi
  fi
fi

# **************************************************************************

AM_CONFIG_HEADER([include/config.h])

AC_CONFIG_FILES([
	profit.pc
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

AC_OUTPUT

# **************************************************************************
