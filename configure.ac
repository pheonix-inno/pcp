# -*-sh-*-
#
#    This file is part of Pretty Curved Privacy (pcp1).
#
#    Copyright (C) 2013-2015 T.Linden.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    You can contact me by mail: <tlinden AT cpan DOT org>.
#

AC_PREREQ(2.61)

define([pcpversion], esyscmd([sh -c "cat VERSION"]))dnl
AC_INIT([pcp], [pcpversion], [pcp@daemon.de])
#AC_INIT(pcp, `cat VERSION`)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)

AC_CONFIG_HEADER(include/pcp/config.h)
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

ORIG_CFLAGS="${CFLAGS:-none}"


# Checks for programs
AC_PROG_CXX
AC_PROG_CXXCPP

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_SED
AC_PROG_AWK
AC_PROG_INSTALL

# remove flags set by AC_PROG_CC (duplicates and/or invalid for clang)
# FIXME: why did I do this?!
#CFLAGS=""
#CXXFLAGS=""


# Host speciffic checks
AC_CANONICAL_HOST

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h err.h stdlib.h string.h unistd.h stdio.h getopt.h\
                 limits.h stddef.h stdint.h sys/types.h sys/stat.h \
                 termios.h arpa/inet.h netinet/in.h wctype.h)

AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS( \
    arc4random_buf \
    arc4random \
    fread \
    fopen \
    free \
    fwrite \
    fseek \
    ftruncate \
    fprintf \
    isatty \
    malloc \
    memset \
    memcpy \
    mmap \
    perror \
    posix_memalign \
    setrlimit \
    strnlen \
    strnstr \
    strlen \
    strtol \
    sizeof \
    tcgetattr \
    umask \
    towlower \
    getopt_long \
    vasprintf
)

cross_compile="no"
AC_MSG_CHECKING([compiler and flags for sanity])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[ #include <stdio.h> ]], [[ exit(0); ]])],
        [   AC_MSG_RESULT([yes]) ],
        [
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([*** compiler cannot create working executables, check config.log ***])
        ],
        [
	    AC_MSG_WARN([cross compiling: not checking compiler sanity])
	    [cross_compile="yes"]
	]
)

_havenacl=no
_ldlib=""
_have_json=no

AC_ARG_WITH([libsodium],
            [AS_HELP_STRING([--with-libsodium],
                            [Specify libsodium prefix])],
            [search_libsodium="yes"],
            [])

if test "x$search_libsodium" = "xyes"; then
    if test -r "${with_libsodium}/include/sodium.h"; then
        CFLAGS="-I${with_libsodium}/include ${CFLAGS}"
        LDFLAGS="-L${with_libsodium}/lib ${LDFLAGS}"
        _havenacl=yes
	_ldlib="${with_libsodium}/lib"
    fi
fi

AC_ARG_WITH([libsodium-include-dir],
            [AS_HELP_STRING([--with-libsodium-include-dir],
                            [Specify libsodium include prefix])],
            [search_libsodium_include="yes"],
            [])

if test "x$search_libsodium_include" = "xyes"; then
    if test -r "${with_libsodium_include_dir}/sodium.h"; then
        CFLAGS="-I${with_libsodium_include_dir} ${CFLAGS}"
	_havenacl=yes
    fi
fi

AC_ARG_WITH([libsodium_lib_dir],
            [AS_HELP_STRING([--with-libsodium-lib-dir],
                            [Specify libsodium library prefix])],
            [search_libsodium_lib="yes"],
            [])

if test "x$search_libsodium_lib" = "xyes"; then
    if test -r "${with_libsodium_lib_dir}/libsodium.dylib" -o -r "${with_libsodium_lib_dir}/libsodium.so" -o -r "${with_libsodium_lib_dir}/libsodium.a"; then
        LDFLAGS="-L${with_libsodium_lib_dir} ${LDFLAGS}"
	_havenacl=yes
	_ldlib="${with_libsodium_lib_dir}"
    fi
fi

if test "x${_havenacl}" = "xno"; then
	AC_MSG_CHECKING([pkg-config for libsodium])
	if pkg-config --exists libsodium; then
		# found it
		LDFLAGS="`pkg-config --libs libsodium` ${LDFLAGS}"
		CFLAGS="`pkg-config --cflags libsodium` ${CFLAGS}"
		_ldlib=`pkg-config --libs libsodium | cut -d ' ' -f 1 | cut -d L -f 2`
		_havenacl=yes
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
fi

if test "x${_havenacl}" != "xno" -a "x$cross_compile" = "xno"; then
    LIBS="-lsodium" # gcc
    export LDFLAGS="$LDFLAGS"
    export CFLAGS="$CFLAGS"
    export LIBS="$LIBS"
    AC_MSG_CHECKING([libsodium version compatible])
    AC_RUN_IFELSE([
	    AC_LANG_PROGRAM([[
#include <sodium.h>
			]],[[
			if (sodium_library_version_major() >= 7) { exit(0); }
			else { exit(1); }    
			    ]])],
	[
	    AC_MSG_RESULT([yes])
	    ],
	[
	    AC_MSG_ERROR([no, libsodium too old. please update your libsodium installation. or maybe the path in "$LDFLAGS" is not in LD_LIBRARY_PATH?])
	]
    )
fi


AC_ARG_WITH([json],
            [AS_HELP_STRING([--with-json],
                            [enable JSON support])],
            [search_json="yes"],
            [])
		  
if test "x$search_json" = "xyes"; then
    # use pkg only
    # FIXME: search
    _have_json="yes"
    LDFLAGS="$LDFLAGS -ljansson"
    CFLAGS="$CFLAGS -DHAVE_JSON=1"
fi

AM_CONDITIONAL([BUILDJSON], [test "x$_have_json" = "xyes"])


# Check for some target-specific stuff
case "$host" in
    *aix*)
	# libm is required as well
	CFLAGS="$CFLAGS -D_AIX_SOURCE=1"
	LDFLAGS="$LDFLAGS -lm"
	;;
    *-*-android*) ;;
    *-*-cygwin*) ;;
    *-*-dgux*) ;;
    *-*-darwin*) ;;
    *-*-dragonfly*) ;;
    *-*-haiku*) ;;
    *-*-hpux*) ;;
    *-*-irix5*) ;;
    *-*-irix6*) ;;
    *-*-k*bsd*-gnu | *-*-kopensolaris*-gnu) ;;
    *-*-linux*) ;;
    *-*-netbsd*) ;;
    *-*-freebsd*)
	# ports install to /usr/local by default, check
	if test -d "/usr/local/lib" -a -d "/usr/local/include"; then
	    CFLAGS="$CFLAGS -I/usr/local/include"
	    LDFLAGS="$LDFLAGS -L/usr/local/lib"
	fi
	;;
    *-*-bsdi*) ;;
    *-next-*) ;;
    *-*-openbsd*) ;;
    *-*-solaris*) ;;
    *-*-sunos4*) ;;
    *-ncr-sysv*) ;;
    *-sni-sysv*) ;;
    *-*-sysv4.2*) ;;
    *-*-sysv5*) ;;
    *-*-sysv*) ;;
    *-*-sco*) ;;
    *-*-unicos*) ;;
    *-dec-osf*) ;;
    *-*-nto-qnx*) ;;
    *-*-ultrix*) ;;
    *-*-lynxos) ;;
esac
AC_CHECK_LIB(sodium, sodium_init, , [AC_MSG_ERROR([cannot link with -lsodium, install libsodium.])])

if test -n "$_ldlib"; then
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${_ldlib}"
fi

if test "$cross_compile" = "no"; then
AC_MSG_CHECKING([is libsodium compiled correctly])
AC_RUN_IFELSE([
AC_LANG_PROGRAM([[
#include <sodium.h>
#include <stdlib.h>
#if crypto_box_PUBLICKEYBYTES != 32 || crypto_box_SECRETKEYBYTES != 32 || crypto_sign_PUBLICKEYBYTES != 32 || crypto_sign_PUBLICKEYBYTES != 32
#   error "libsodium not built correctly"
#endif
]],[[exit(0);]])],
    [
	AC_MSG_RESULT([yes])
    ],
    [
	AC_MSG_ERROR([no. please check your libsodium installation, consider re-installing])
    ]
)
fi

# prepare FLAGS
CFLAGS="$CFLAGS -Werror -Wextra -Wall"

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--disable-debug], [Disable debugging]))

AS_IF([test "x$enable_debug" != "xno"], [
	  CFLAGS="$CFLAGS -g -DDEBUG"
	  enable_debug="yes"
])

AC_ARG_ENABLE([optimize],
    AS_HELP_STRING([--disable-optimize], [Disable optimization]))

AS_IF([test "x$enable_optimize" != "xno"], [
	  case $enable_optimize in
	      -O*)
		  CFLAGS="$CFLAGS $enable_optimize"
		  enable_optimize="$enable_optimize"
		  ;;
	      *)
		  CFLAGS="$CFLAGS -O2"
		  enable_optimize="-O2"
		  ;;
	  esac
])


CXXFLAGS="$CFLAGS"

# FIXME: check for libm
LIBS="$LIBS -lm"

# conditionals for bindings and stuff

# c++
AC_ARG_ENABLE([cpp-binding],
	      [AS_HELP_STRING([--disable-cpp-binding],
                              [Disable C++ binding])],
	     )

AS_IF([test "x$enable_cpp_binding" != "xno"], [
	  enable_cpp_binding=yes
      ])

AM_CONDITIONAL([BUILDCPP], [test "x$enable_cpp_binding" != "xno"])

# py
AC_ARG_ENABLE([python-binding],
              [AS_HELP_STRING([--enable-python-binding],
                              [Enable python binding])
	      ],
              [python="yes"],
              [])

if test "x$python" = "xyes"; then
    if ! python -c "import cffi" > /dev/null 2>&1; then
	python="no"
	AC_MSG_ERROR([python or cffi is not installed])
    fi
else
    python="no"
fi

AM_CONDITIONAL([BUILDPY], [test "x$python" = "xyes"])



AC_SUBST(PACKAGE_VERSION)

# Specify output files
AC_CONFIG_FILES([Makefile include/Makefile libpcp/Makefile src/Makefile man/Makefile \
		 tests/Makefile libpcp/libpcp1.pc bindings/cpp/Makefile bindings/py/Makefile])




AC_OUTPUT

AC_MSG_RESULT([
  Build configured for $PACKAGE $VERSION:
    CC:                   ${CC}       
    CFLAGS:               ${CFLAGS}     
    CXX:                  ${CXX}      
    CXXFLAGS:             ${CXXFLAGS}    
    LDFLAGS:              ${LDFLAGS}    
    LIBS:                 ${LIBS}      
    DEBUG:                ${enable_debug}    
    optimize:             ${enable_optimize}  
                         
    prefix:               ${prefix}     
    libdir:               ${libdir}     
    includedir:           ${includedir}   
                         
    target platform:      ${host}        
    cross compile:        ${cross_compile}   
                         
    build python binding: ${python}       
    build c++ binding:    ${enable_cpp_binding} 
                         
    json support:         ${_have_json}
    
  Type 'make' to build, 'make install' to install.
  To execute unit tests, type 'make test'.
])
