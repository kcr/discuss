AC_INIT
AC_CONFIG_SRCDIR([client/discuss.c])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P

lispdir='${datarootdir}/emacs/site-lisp'
AC_SUBST(lispdir)

dnl Checks for libraries.
ATHENA_KRB4
ATHENA_KRB5
ATHENA_REGEXP
ATHENA_UTIL_COM_ERR
ATHENA_UTIL_SS
ATHENA_ZEPHYR

AC_ARG_WITH(
  [pager],
  [AS_HELP_STRING([--with-pager=PAGER], [specify the default pager (default is more)])],
  [],
  [AC_PATH_PROG([with_pager], [more])])
AC_DEFINE_UNQUOTED([PAGER], ["$with_pager"])

AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket])

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h unistd.h termios.h])

dnl Checks for library functions.
AC_CHECK_FUNCS([srand48 sigaction])

AC_CONFIG_FILES([Makefile client/Makefile dsgrep/Makefile edsc/Makefile ets/Makefile
                 include/Makefile libds/Makefile mclient/Makefile server/Makefile
                 usp/Makefile])
AC_OUTPUT
