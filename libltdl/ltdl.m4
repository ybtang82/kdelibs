## ltdl.m4 - Configure ltdl for the target system. -*-Shell-script-*-
## Copyright (C) 1999-2000 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
## As a special exception to the GNU General Public License, if you
## distribute this file as part of a program that contains a
## configuration script generated by Autoconf, you may include it under
## the same distribution terms that you use for the rest of that program.

# serial 1 AC_LIB_LTDL

AC_DEFUN(AC_LIB_LTDL,
[AC_PREREQ(2.13)dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_C_CONST])dnl
AC_REQUIRE([AC_C_INLINE])dnl

dnl AC_LIB_LTDL must perform all the checks necessary for compilation
dnl of the ltdl objects -- including compiler checks (above) and header
dnl checks (below).
AC_REQUIRE([AC_HEADER_STDC])dnl

AC_CHECK_HEADERS(malloc.h memory.h stdlib.h stdio.h ctype.h dlfcn.h dl.h dld.h)
AC_CHECK_HEADERS(string.h strings.h, break)
AC_CHECK_FUNCS(strchr index, break)
AC_CHECK_FUNCS(strrchr rindex, break)

AC_REQUIRE([AC_LTDL_ENABLE_INSTALL])dnl
AC_REQUIRE([AC_LTDL_SHLIBEXT])dnl
AC_REQUIRE([AC_LTDL_SHLIBPATH])dnl
AC_REQUIRE([AC_LTDL_SYSSEARCHPATH])dnl
AC_REQUIRE([AC_LTDL_OBJDIR])dnl
AC_REQUIRE([AC_LTDL_DLPREOPEN])dnl
AC_REQUIRE([AC_LTDL_DLLIB])dnl
AC_REQUIRE([AC_LTDL_SYMBOL_USCORE])dnl
])

AC_DEFUN(AC_LTDL_ENABLE_INSTALL,
[AC_ARG_ENABLE(ltdl-install, 
[  --enable-ltdl-install   install libltdl])

AM_CONDITIONAL(INSTALL_LTDL, test x"${enable_ltdl_install-no}" != xno)
AM_CONDITIONAL(CONVENIENCE_LTDL, test x"${enable_ltdl_convenience-no}" != xno)
])])


AC_DEFUN(AC_LTDL_SNARF_CONFIG,
[# Read the libtool configuration
rm -f conftest
./libtool --config > conftest
. ./conftest
rm -f conftest
])

AC_DEFUN(AC_LTDL_SHLIBEXT,
[AC_REQUIRE([AC_LTDL_SNARF_CONFIG])dnl
AC_CACHE_CHECK([which extension is used for shared libraries],
  libltdl_cv_shlibext, [dnl
(
  last=
  for spec in $library_names_spec; do
    last="$spec"
  done
changequote(, )
  echo "$last" | sed 's/\[.*\]//;s/^[^.]*//;s/\$.*$//;s/\.$//' > conftest
changequote([, ])
)
libltdl_cv_shlibext=`cat conftest`
rm -f conftest
])
if test -n "$libltdl_cv_shlibext"; then
  AC_DEFINE_UNQUOTED(LTDL_SHLIB_EXT, "$libltdl_cv_shlibext",
    [Define to the extension used for shared libraries, say, ".so". ])
fi
])

AC_DEFUN(AC_LTDL_SHLIBPATH,
[AC_REQUIRE([AC_LTDL_SNARF_CONFIG])dnl
AC_CACHE_CHECK([which variable specifies run-time library path],
  libltdl_cv_shlibpath_var, [libltdl_cv_shlibpath_var="$shlibpath_var"])
if test -n "$libltdl_cv_shlibpath_var"; then
  AC_DEFINE_UNQUOTED(LTDL_SHLIBPATH_VAR, "$libltdl_cv_shlibpath_var",
    [Define to the name of the environment variable that determines the dynamic library search path. ])
fi
])

AC_DEFUN(AC_LTDL_SYSSEARCHPATH,
[AC_REQUIRE([AC_LTDL_SNARF_CONFIG])dnl
AC_CACHE_CHECK([for the default library search path],
  libltdl_cv_sys_search_path, [libltdl_cv_sys_search_path="$sys_lib_dlsearch_path_spec"])
if test -n "$libltdl_cv_sys_search_path"; then
  case "$lt_target" in
  *-*-mingw*) pathsep=";" ;;
  *) pathsep=":" ;;
  esac
  sys_search_path=
  for dir in $libltdl_cv_sys_search_path; do
    if test -z "$sys_search_path"; then
      sys_search_path="$dir"
    else
      sys_search_path="$sys_search_path$pathsep$dir"
    fi
  done
  AC_DEFINE_UNQUOTED(LTDL_SYSSEARCHPATH, "$sys_search_path",
    [Define to the system default library search path. ])
fi
])

AC_DEFUN(AC_LTDL_OBJDIR,
[AC_CACHE_CHECK([for objdir],
  libltdl_cv_objdir, [libltdl_cv_objdir="$objdir"
if test -n "$objdir"; then
  :
else
  rm -f .libs 2>/dev/null
  mkdir .libs 2>/dev/null
  if test -d .libs; then
    libltdl_cv_objdir=.libs
  else
    # MS-DOS does not allow filenames that begin with a dot.
    libltdl_cv_objdir=_libs
  fi
rmdir .libs 2>/dev/null
fi])
AC_DEFINE_UNQUOTED(LTDL_OBJDIR, "$libltdl_cv_objdir/",
  [Define to the sub-directory in which libtool stores uninstalled libraries. ])
])

AC_DEFUN(AC_LTDL_DLPREOPEN,
[AC_REQUIRE([AC_LTDL_GLOBAL_SYMBOL_PIPE])dnl
AC_CACHE_CHECK([whether libtool supports -dlopen/-dlpreopen],
       libltdl_cv_preloaded_symbols, [dnl
  if test -n "$global_symbol_pipe"; then
    libltdl_cv_preloaded_symbols=yes
  else
    libltdl_cv_preloaded_symbols=no
  fi
])
if test x"$libltdl_cv_preloaded_symbols" = x"yes"; then
  AC_DEFINE(HAVE_PRELOADED_SYMBOLS, 1,
    [Define if libtool can extract symbol lists from object files. ])
fi
])

AC_DEFUN(AC_LTDL_DLLIB,
[LIBADD_DL=
AC_CHECK_LIB(dl, dlopen, [AC_DEFINE(HAVE_LIBDL, 1,
   [Define if you have the libdl library or equivalent. ]) LIBADD_DL="-ldl"],
[AC_CHECK_FUNC(dlopen, [AC_DEFINE(HAVE_LIBDL, 1,
   [Define if you have the libdl library or equivalent.])])])
AC_CHECK_FUNC(shl_load, [AC_DEFINE(HAVE_SHL_LOAD, 1,
   [Define if you have the shl_load function.])],
[AC_CHECK_LIB(dld, shl_load,
  [AC_DEFINE(HAVE_SHL_LOAD, 1,
     [Define if you have the shl_load function.])
   LIBADD_DL="$LIBADD_DL -ldld"])
])
AC_CHECK_LIB(dld, dld_link, [AC_DEFINE(HAVE_DLD, 1,
  [Define if you have the GNU dld library.])dnl
test "x$ac_cv_lib_dld_shl_load" = yes || LIBADD_DL="$LIBADD_DL -ldld"])
AC_SUBST(LIBADD_DL)

if test "x$ac_cv_func_dlopen" = xyes || test "x$ac_cv_lib_dl_dlopen" = xyes; then
 LIBS_SAVE="$LIBS"
 LIBS="$LIBS $LIBADD_DL"
 AC_CHECK_FUNCS(dlerror)
 LIBS="$LIBS_SAVE"
fi
])

AC_DEFUN(AC_LTDL_GLOBAL_SYMBOL_PIPE,
[dnl Check for command to grab the raw symbol name followed
dnl by C symbol name from nm.
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_PROG_NM])dnl
# Check for command to grab the raw symbol name followed by C symbol from nm.
AC_MSG_CHECKING([command to parse $NM output])
AC_CACHE_VAL(ac_cv_sys_global_symbol_pipe,
[# These are sane defaults that work on at least a few old systems.
# {They come from Ultrix.  What could be older than Ultrix?!! ;)}

changequote(,)dnl
# Character class describing NM global symbol codes.
ac_symcode='[BCDEGRST]'

# Regexp to match symbols that can be accessed directly from C.
ac_sympat='\([_A-Za-z][_A-Za-z0-9]*\)'

# Transform the above into a raw symbol and a C symbol.
ac_symxfrm='\1 \2\3 \3'

# Transform an extracted symbol line into a proper C declaration
ac_global_symbol_to_cdecl="sed -n -e 's/^. .* \(.*\)$/extern char \1;/p'"

# Define system-specific variables.
case "$host_os" in
aix*)
  ac_symcode='[BCDT]'
  ;;
cygwin* | mingw*)
  ac_symcode='[ABCDGISTW]'
  ;;
hpux*)
  ac_global_symbol_to_cdecl="sed -n -e 's/^T .* \(.*\)$/extern char \1();/p' -e 's/^. .* \(.*\)$/extern char \1;/p'"
  ;;
irix*)
  ac_symcode='[BCDEGRST]'
  ;;
solaris*)
  ac_symcode='[BDT]'
  ;;
esac

# If we're using GNU nm, then use its standard symbol codes.
if $NM -V 2>&1 | egrep '(GNU|with BFD)' > /dev/null; then
  ac_symcode='[ABCDGISTW]'
fi
changequote([,])dnl

# Try without a prefix undercore, then with it.
for ac_symprfx in "" "_"; do

  ac_cv_sys_global_symbol_pipe="sed -n -e 's/^.*[ 	]\($ac_symcode\)[ 	][ 	]*\($ac_symprfx\)$ac_sympat$/$ac_symxfrm/p'"

  # Check to see that the pipe works correctly.
  ac_pipe_works=no
  rm -f conftest.$ac_ext
  cat > conftest.$ac_ext <<EOF
#ifdef __cplusplus
extern "C" {
#endif
char nm_test_var;
void nm_test_func(){}
#ifdef __cplusplus
}
#endif
int main(){nm_test_var='a';nm_test_func;return 0;}
EOF

  if AC_TRY_EVAL(ac_compile); then
    # Now try to grab the symbols.
    ac_nlist=conftest.nm
  
    if AC_TRY_EVAL(NM conftest.$ac_objext \| $ac_cv_sys_global_symbol_pipe \> $ac_nlist) && test -s "$ac_nlist"; then

      # Try sorting and uniquifying the output.
      if sort "$ac_nlist" | uniq > "$ac_nlist"T; then
	mv -f "$ac_nlist"T "$ac_nlist"
      else
	rm -f "$ac_nlist"T
      fi

      # Make sure that we snagged all the symbols we need.
      if egrep ' nm_test_var$' "$ac_nlist" >/dev/null; then
	if egrep ' nm_test_func$' "$ac_nlist" >/dev/null; then
	  cat <<EOF > conftest.c
#ifdef __cplusplus
extern "C" {
#endif

EOF
	  # Now generate the symbol file.
	  eval "$ac_global_symbol_to_cdecl"' < "$ac_nlist" >> conftest.c'

	  cat <<EOF >> conftest.c
#if defined (__STDC__) && __STDC__
# define lt_ptr_t void *
#else
# define lt_ptr_t char *
# define const
#endif

/* The mapping between symbol names and symbols. */
const struct {
  const char *name;
  lt_ptr_t address;
}
changequote(,)dnl
lt_preloaded_symbols[] =
changequote([,])dnl
{
EOF
	sed 's/^. \(.*\) \(.*\)$/  {"\2", (lt_ptr_t) \&\2},/' < "$ac_nlist" >> conftest.c
	cat <<\EOF >> conftest.c
  {0, (lt_ptr_t) 0}
};

#ifdef __cplusplus
}
#endif
EOF
	  # Now try linking the two files.
	  mv conftest.$ac_objext conftstm.$ac_objext
	  ac_save_LIBS="$LIBS"
	  ac_save_CFLAGS="$CFLAGS"
	  LIBS="conftstm.$ac_objext"
	  CFLAGS="$CFLAGS$no_builtin_flag"
	  if AC_TRY_EVAL(ac_link) && test -s conftest; then
	    ac_pipe_works=yes
	  else
	    echo "configure: failed program was:" >&AC_FD_CC
	    cat conftest.c >&AC_FD_CC
	  fi
	  LIBS="$ac_save_LIBS"
	  CFLAGS="$ac_save_CFLAGS"
	else
	  echo "cannot find nm_test_func in $ac_nlist" >&AC_FD_CC
	fi
      else
	echo "cannot find nm_test_var in $ac_nlist" >&AC_FD_CC
      fi
    else
      echo "cannot run $ac_cv_sys_global_symbol_pipe" >&AC_FD_CC
    fi
  else
    echo "$progname: failed program was:" >&AC_FD_CC
    cat conftest.c >&AC_FD_CC
  fi
  rm -rf conftest* conftst*

  # Do not use the global_symbol_pipe unless it works.
  if test "$ac_pipe_works" = yes; then
    if test x"$ac_symprfx" = x"_"; then
      ac_cv_sys_symbol_underscore=yes
    else
      ac_cv_sys_symbol_underscore=no
    fi
    break
  else
    ac_cv_sys_global_symbol_pipe=
  fi
done
])

ac_result=yes
if test -z "$ac_cv_sys_global_symbol_pipe"; then
   ac_result=no
fi
AC_MSG_RESULT($ac_result)
])

AC_DEFUN(AC_LTDL_SYMBOL_USCORE,
[dnl does the compiler prefix global symbols with an underscore?
AC_REQUIRE([AC_LTDL_GLOBAL_SYMBOL_PIPE])dnl
AC_MSG_CHECKING([for _ prefix in compiled symbols])
AC_CACHE_VAL(ac_cv_sys_symbol_underscore,
[ac_cv_sys_symbol_underscore=no
cat > conftest.$ac_ext <<EOF
void nm_test_func(){}
int main(){nm_test_func;return 0;}
EOF
if AC_TRY_EVAL(ac_compile); then
  # Now try to grab the symbols.
  ac_nlist=conftest.nm
  if AC_TRY_EVAL(NM conftest.$ac_objext \| $ac_cv_sys_global_symbol_pipe \> $ac_nlist) && test -s "$ac_nlist"; then
    # See whether the symbols have a leading underscore.
    if egrep '^. _nm_test_func' "$ac_nlist" >/dev/null; then
      ac_cv_sys_symbol_underscore=yes
    else
      if egrep '^. nm_test_func ' "$ac_nlist" >/dev/null; then
	:
      else
	echo "configure: cannot find nm_test_func in $ac_nlist" >&AC_FD_CC
      fi
    fi
  else
    echo "configure: cannot run $ac_cv_sys_global_symbol_pipe" >&AC_FD_CC
  fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.c >&AC_FD_CC
fi
rm -rf conftest*
])
AC_MSG_RESULT($ac_cv_sys_symbol_underscore)
AC_LTDL_DLSYM_USCORE
])

AC_DEFUN(AC_LTDL_DLSYM_USCORE,
[AC_REQUIRE([AC_LTDL_SYMBOL_USCORE])dnl
if test x"$ac_cv_sys_symbol_underscore" = xyes; then
  if test x"$ac_cv_func_dlopen" = xyes ||
     test x"$ac_cv_lib_dl_dlopen" = xyes ; then
	AC_CACHE_CHECK([whether we have to add an underscore for dlsym],
		libltdl_cv_need_uscore, [dnl
		AC_TRY_RUN([
#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <stdio.h>

#ifdef RTLD_GLOBAL
# define LTDL_GLOBAL	RTLD_GLOBAL
#else
# ifdef DL_GLOBAL
#  define LTDL_GLOBAL	DL_GLOBAL
# else
#  define LTDL_GLOBAL	0
# endif
#endif

/* We may have to define LTDL_LAZY_OR_NOW in the command line if we
   find out it does not work in some platform. */
#ifndef LTDL_LAZY_OR_NOW
# ifdef RTLD_LAZY
#  define LTDL_LAZY_OR_NOW	RTLD_LAZY
# else
#  ifdef DL_LAZY
#   define LTDL_LAZY_OR_NOW	DL_LAZY
#  else
#   ifdef RTLD_NOW
#    define LTDL_LAZY_OR_NOW	RTLD_NOW
#   else
#    ifdef DL_NOW
#     define LTDL_LAZY_OR_NOW	DL_NOW
#    else
#     define LTDL_LAZY_OR_NOW	0
#    endif
#   endif
#  endif
# endif
#endif

fnord() { int i=42;}
main() { void *self, *ptr1, *ptr2; self=dlopen(0,LTDL_GLOBAL|LTDL_LAZY_OR_NOW);
    if(self) { ptr1=dlsym(self,"fnord"); ptr2=dlsym(self,"_fnord");
	       if(ptr1 && !ptr2) { dlclose(self); exit(0); } } exit(1); } 
],	libltdl_cv_need_uscore=no, libltdl_cv_need_uscore=yes,
	libltdl_cv_need_uscore=cross
)])
  fi
fi

if test x"$libltdl_cv_need_uscore" = xyes; then
  AC_DEFINE(NEED_USCORE, 1,
    [Define if dlsym() requires a leading underscode in symbol names. ])
fi
])
