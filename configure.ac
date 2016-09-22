#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([com.kizos.libcore], [1.6.1])
AC_CANONICAL_HOST
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign -Wall subdir-objects])

# Checks for programs.
AM_PROG_AR
AC_PROG_CXX
AM_PROG_AS
LT_INIT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SSIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_MKTIME
#AC_FUNC_MMAP
#AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime getpagesize localtime_r memset munmap strerror])

# Add trailling spaces
test -n "${CPPFLAGS}" && CPPFLAGS+=" "
test -n "${CXXFLAGS}" && CXXFLAGS+=" "
test -n "${LIBS}"     && LIBS+=" "
test -n "${LDFLAGS}"  && LDFLAGS+=" "



#
# Kizbox framework
#
# The following variables set up some preprocessor macros you may use into you c/c++ source code or
# into the .in files.
#

FRAMEWORK_NAME='Core'
FRAMEWORK_LT_VERSION_INFO='4:1:0'
APPLICATION_NAME="Test/${FRAMEWORK_NAME}"
APPLICATION_NAME_LC="$(echo ${APPLICATION_NAME} | sed 's/.*/\L&/')"

KIZBOX_FRAMEWORK_HOST_PREFIX='com.kizbox.Framework'
KIZBOX_FRAMEWORK_PATH_PREFIX='/com/kizbox/Framework'
FRAMEWORK_HOST="${KIZBOX_FRAMEWORK_HOST_PREFIX}.${FRAMEWORK_NAME}"
FRAMEWORK_PATH="${KIZBOX_FRAMEWORK_PATH_PREFIX}/${FRAMEWORK_NAME}"

OVERKIZ_APPLICATION_HOST_PREFIX="com.overkiz.Application"
OVERKIZ_APPLICATION_PATH_PREFIX="/com/overkiz/Application"
APPLICATION_HOST="${OVERKIZ_APPLICATION_HOST_PREFIX}.${APPLICATION_NAME}"
APPLICATION_PATH="${OVERKIZ_APPLICATION_PATH_PREFIX}/${APPLICATION_NAME}"

# Variables you may use as @MY_VAR@ into .in files:
AC_SUBST([FRAMEWORK_NAME])
AC_SUBST([FRAMEWORK_LT_VERSION_INFO])

AC_SUBST([KIZBOX_FRAMEWORK_HOST_PREFIX])
AC_SUBST([KIZBOX_FRAMEWORK_PATH_PREFIX])
AC_SUBST([FRAMEWORK_HOST])
AC_SUBST([FRAMEWORK_PATH])

AC_SUBST([APPLICATION_NAME])
AC_SUBST([APPLICATION_NAME_LC])
AC_SUBST([OVERKIZ_APPLICATION_HOST_PREFIX])
AC_SUBST([OVERKIZ_APPLICATION_PATH_PREFIX])
AC_SUBST([APPLICATION_HOST])
AC_SUBST([APPLICATION_PATH])

# C/C++ macros you may use into your source code:
AC_DEFINE_UNQUOTED([FRAMEWORK_NAME],                  ["${FRAMEWORK_NAME}"],                  [Defines framework name])
AC_DEFINE_UNQUOTED([KIZBOX_FRAMEWORK_HOST_PREFIX],    ["${KIZBOX_FRAMEWORK_HOST_PREFIX}"],    [Defines the host kizbox framework prefix])
AC_DEFINE_UNQUOTED([KIZBOX_FRAMEWORK_PATH_PREFIX],    ["${KIZBOX_FRAMEWORK_PATH_PREFIX}"],    [Defines the path kizbox framework prefix])
AC_DEFINE_UNQUOTED([FRAMEWORK_HOST],                  ["${FRAMEWORK_HOST}"],                  [Defines the host framework])
AC_DEFINE_UNQUOTED([FRAMEWORK_PATH],                  ["${FRAMEWORK_PATH}"],                  [Defines the path framework])

AC_DEFINE_UNQUOTED([APPLICATION_NAME],                ["${APPLICATION_NAME}"],                [Defines application name])
AC_DEFINE_UNQUOTED([APPLICATION_NAME_LC],             ["${APPLICATION_NAME_LC}"],             [Defines application name in lower case for log])
AC_DEFINE_UNQUOTED([OVERKIZ_APPLICATION_HOST_PREFIX], ["${OVERKIZ_APPLICATION_HOST_PREFIX}"], [Defines the host overkiz application prefix])
AC_DEFINE_UNQUOTED([OVERKIZ_APPLICATION_PATH_PREFIX], ["${OVERKIZ_APPLICATION_PATH_PREFIX}"], [Defines the path overkiz application prefix])
AC_DEFINE_UNQUOTED([APPLICATION_HOST],                ["${APPLICATION_HOST}"],                [Defines the host application])
AC_DEFINE_UNQUOTED([APPLICATION_PATH],                ["${APPLICATION_PATH}"],                [Defines the path application])

AC_DEFINE_UNQUOTED([RTC_SANITY_CHECK_YEAR], [$(date "+%Y")], [Define the year used in rtc sanity checks])
# Here you may add some application constants
# ...



#
# Pkg-config
#
# The following variables set up the pkg-config file.
#

PKGCONFIG_DESCRIPTION="Framework ${FRAMEWORK_NAME}"
PKGCONFIG_URL="https://github.com/Overkiz/libcore"
PKGCONFIG_REQUIRES=
PKGCONFIG_PRIVATE_REQUIRES=
PKGCONFIG_CONFLICTS=
PKGCONFIG_LIBDIRS="-L\${libdir}"
PKGCONFIG_LIBS="-l${FRAMEWORK_NAME}"
PKGCONFIG_PRIVATE_LIBS=
PKGCONFIG_INCDIRS="-I\${includedir}"
PKGCONFIG_DEFS=

AC_SUBST([PKGCONFIG_DESCRIPTION])
AC_SUBST([PKGCONFIG_URL])
AC_SUBST([PKGCONFIG_REQUIRES])
AC_SUBST([PKGCONFIG_PRIVATE_REQUIRES])
AC_SUBST([PKGCONFIG_CONFLICTS])
AC_SUBST([PKGCONFIG_LIBDIRS])
AC_SUBST([PKGCONFIG_LIBS])
AC_SUBST([PKGCONFIG_PRIVATE_LIBS])
AC_SUBST([PKGCONFIG_INCDIRS])
AC_SUBST([PKGCONFIG_DEFS])



# Checks for packages.

AC_ARG_ENABLE(
  [pkg-config],
  [AS_HELP_STRING([--disable-pkg-config],					[Disable pkg-config [default=no]])],
  [
    case "${enableval}" in
    yes) pkgconfig='true';;
    no)  pkgconfig='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --disable-pkg-config]);;
    esac
  ],
  [
    pkgconfig='true'
  ]
  )
AM_CONDITIONAL([PKGCONFIG], [test x${pkgconfig} = xtrue])



AC_MSG_CHECKING([for supported architecture])
case "${host_cpu}" in
arm*)
  arch='arm'
  ARCH_CPPFLAGS=""
  ARCH_CFLAGS=""
  ARCH_CXXFLAGS="-funwind-tables"
  ARCH_LDFLAGS=""
  ;;
i?86*)
  arch='i386'
  ARCH_CPPFLAGS=""
  ARCH_CFLAGS=""
  ARCH_CXXFLAGS="-funwind-tables"
  ARCH_LDFLAGS=""
  ;;
x86_64*)
  arch='x86_64'
  ARCH_CPPFLAGS=""
  ARCH_CFLAGS=""
  ARCH_CXXFLAGS="-funwind-tables"
  ARCH_LDFLAGS=""
  ;;
*)
  AC_MSG_RESULT([unsupported])
  AC_MSG_ERROR([architecture ${host_cpu} is not supported! Only arm x86 and x86_64 architectures are supported!])
  ;;
esac
AC_MSG_RESULT([$arch])
AC_SUBST([arch])
AC_SUBST([ARCH_CPPFLAGS])
AC_SUBST([ARCH_CFLAGS])
AC_SUBST([ARCH_CXXFLAGS])
AM_CONDITIONAL([ARCH_ARM], [test "${arch}" = 'arm'])
AM_CONDITIONAL([ARCH_i386], [test "${arch}" = 'i386'])
AM_CONDITIONAL([ARCH_x86_64], [test "${arch}" = 'x86_64'])



AC_ARG_ENABLE(
  [valgrind],
  [AS_HELP_STRING([--enable-valgrind],			[Enable valgring debug support [default=no]])],
  [
    case "${withval}" in
    yes) valgrind='true';;
    no)  valgrind='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-valgrind]);;
    esac
  ],
  [
    valgrind='false'
  ]
  )
if test "${valgrind}" = 'true' ; then
  PKGCONFIG_DEFS+="-DVALGRIND=1 "
  CPPFLAGS+="-DVALGRIND=1 "
  AC_DEFINE([VALGRIND],							[1],									[Enable valgrind])
fi



AC_ARG_WITH(
  [mprotectsize],
  [AS_HELP_STRING([--with-mprotect-size],			[Specify number of memory pages to protect (before and after stack) [default=1]])],
  [
    case "${withval}" in
    yes|no)   AC_MSG_ERROR([bad value ${enableval} for --with-mprotect-size]);;
    *)        mprotectsize=${withval};;
    esac
  ],
  [
    mprotectsize='1'
  ]
  )
AC_DEFINE_UNQUOTED([MPROTECT_SIZE],					[${mprotectsize}],						[mprotect size])



AC_ARG_ENABLE(
  [doxygen],
  [AS_HELP_STRING([--enable-doxygen],						[Enable doxygen doc generation [default=no]])],
  [
    case "${enableval}" in
    yes) doxygen='true';;
    no)  doxygen='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen]);;
    esac
  ],
  [
    doxygen='false'
  ]
  )
AM_CONDITIONAL([DOXYGEN], [test "${doxygen}" = 'true'])



AC_ARG_ENABLE(
  [tests],
  [AS_HELP_STRING([--enable-tests],							[Enable build tests [default=false]])],
  [
    case "${enableval}" in
    yes) tests='true';;
    no)  tests='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-tests]);;
    esac
  ],
  [
    tests='false'
  ]
  )
AS_IF(
  [test "${tests}" = "true"],
  [
    PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.10)
    AC_SUBST(CPPUNIT_LIBS)
    AC_SUBST(CPPUNIT_FLAGS)
  ]
  )
AM_CONDITIONAL([TESTS], [test "${tests}" = "true"])



AC_ARG_ENABLE(
  [gcov],
  [AS_HELP_STRING([--enable-gcov],							[Enable gcov [default=no]])],
  [
    case "${enableval}" in
    yes) gcov='true';;
    no)  gcov='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-gcov]);;
    esac
  ],
  [
    gcov='no'
  ]
  )
AS_IF(
  [test "${gcov}" = "true"],
  [
    AC_DEFINE([GCOV],								[1],									[Enable gcov])
    CFLAGS="$CFLAGS --coverage"
    CXXFLAGS="$CXXFLAGS --coverage"
    LDFLAGS="$LDFLAGS --coverage -lgcov"
  ]
  )
AM_CONDITIONAL(GCOV, [test "${gcov}" = "true"])



AC_MSG_CHECKING([whether the RELEASE macro is enabled])
AC_ARG_ENABLE(
  [release],
  [AS_HELP_STRING([--enable-release],                                           [Enable the RELEASE macro [default=no]])],
  [
    case "${enableval}" in
    yes) release='true';;
    no)  release='false';;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-release]);;
    esac
  ],
  [
    release='false'
  ]
  )
if test "${release}" = 'true' ; then
  AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_RELEASE],                            [1],                                  [Enable TRACE macro])
        PKGCONFIG_DEFS+=" -DHAVE_RELEASE=1"
else
  AC_MSG_RESULT([no])
fi
AM_CONDITIONAL([RELEASE], [test "${release}" = 'true'])



AC_CONFIG_FILES(
  [
    Makefile
    include/Makefile
    src/Makefile
    tests/Makefile
    pkg-config.pc
  ]
  )

AC_OUTPUT
