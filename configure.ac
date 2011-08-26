AC_INIT(libevebrowser,0.0.1)
AM_INIT_AUTOMAKE(libevebrowser,0.0.1)

dnl AM_MAINTAINER_MODE provides the option to enable maintainer mode
AM_MAINTAINER_MODE

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

dnl make aclocal work in maintainer mode
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")

AM_CONFIG_HEADER(config.h)

dnl check for tools
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL


PKG_CHECK_MODULES(PNG, libpng >= 0.0.1)
AC_SUBST(PNG_CFLAGS)
AC_SUBST(PNG_LIBS)

PKG_CHECK_MODULES(FUSION, fusion >= 0.0.1)
AC_SUBST(FUSION_CFLAGS)
AC_SUBST(FUSION_LIBS)

PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 0.0.1)
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)

PKG_CHECK_MODULES(DFBINT, directfb-internal >= 0.0.1)
AC_SUBST(DFBINT_CFLAGS)
AC_SUBST(DFBINT_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

AC_OUTPUT(Makefile)
