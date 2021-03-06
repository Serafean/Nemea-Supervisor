#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([nemea-supervisor], [1.5.1], [svepemar@fit.cvut.cz])
AC_CONFIG_SRCDIR([supervisor.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign silent-rules])
AM_SILENT_RULES([yes])
RELEASE=1
AC_SUBST(RELEASE)
USERNAME=`git config --get user.name`
USERMAIL=`git config --get user.email`
AC_SUBST(USERNAME)
AC_SUBST(USERMAIL)
# Check for rpmbuild
AC_CHECK_PROG(RPMBUILD, rpmbuild, rpmbuild, [""])
AM_CONDITIONAL(MAKE_RPMS, test x$RPMBUILD != x)
PKG_PROG_PKG_CONFIG

pkgdatadir=${datadir}/nemea-supervisor
AC_SUBST(pkgdatadir)
rundir=${runstatedir}/nemea-supervisor
defaultsocketdir=${rundir}
defaultconfdir=${sysconfdir}/nemea
localstatedir=${localstatedir}/nemea
libexecdir=${libexecdir}/nemea
AC_SUBST(defaultsocketdir)

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_ENABLE([debug],
        AC_HELP_STRING([--enable-debug],
        [Enable build with debug symbols and without optimizations.]),
        [if test "$enableval" = "yes"; then
                CFLAGS="-Wall -g -O0 $CFLAGS"
                CXXFLAGS="-Wall -g -O0 $CXXFLAGS"
        fi], [CFLAGS="-Wall -O3 $CFLAGS"
              CXXFLAGS="-Wall -O3 $CXXFLAGS"])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_DEFINE_DIR(initdir, [sysconfdir]/../init.d, [Path to init.d (init script)])
AC_DEFINE_DIR(bindir, [bindir], [Path to binaries])
AC_DEFINE_DIR(defaultsocketdir, [defaultsocketdir], [Default path to socket directory])
AC_DEFINE_DIR(defaultconfdir, [defaultconfdir], [Default path to configuration files directory])
AC_DEFINE_DIR(rundir, [rundir], [Default path to socket directory])

DEFAULT_PATH_TO_SOCKET=${defaultsocketdir}/nemea-supervisor.sock
AC_DEFINE_UNQUOTED([DEFAULT_PATH_TO_SOCKET], "${DEFAULT_PATH_TO_SOCKET}", [Default path to daemon socket])
AC_SUBST(DEFAULT_PATH_TO_SOCKET)
DEFAULT_PATH_TO_CONFIGS=${defaultconfdir}
AC_DEFINE_UNQUOTED([DEFAULT_PATH_TO_CONFIGS], "${DEFAULT_PATH_TO_CONFIGS}", [Default path to configuration files])
AC_SUBST(DEFAULT_PATH_TO_CONFIGS)

# Checks for programs.
AC_PROG_CC
AC_PROG_MKDIR_P
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_PROG(GIT, git, git, [""])

AC_ARG_ENABLE(repobuild, AS_HELP_STRING([--disable-repobuild],
                [disable local compilation without system installed Nemea libraries, default: yes]),
[case "${enableval}" in
        yes) repobuild=true ;;
        no)  repobuild=false ;;
        *)   AC_MSG_ERROR([bad value ${enableval} for --disable-repobuild]) ;;
esac], [repobuild=true])

if test -n "$GIT"; then
    GIT_VERSION=$($GIT log --format="%h" HEAD^..HEAD | head -1)
else
   GIT_VERSION=""
fi
AC_DEFINE_UNQUOTED(GIT_VERSION, ["$GIT_VERSION"], [Git revision])

# Checks for libraries.

AX_LIBTRAP_CHECK
AX_LIBXML2_CHECK

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h string.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 memset mkdir realpath select socket strchr strdup strstr getopt_long getopt])

# Define SUP_GETOPT()
if test "x$ac_cv_func_getopt_long" = xyes; then
  AC_DEFINE_UNQUOTED([SUP_GETOPT(argc, argv, optstr, longopts)],
    [getopt_long(argc, argv, optstr, longopts, NULL)],
    [Supervisor getopt macro. Argc and argv are number and values of arguments, optstr is a string containing legitimate option characters, longopts is the array of option structures (unused on system without getopt_long())])
elif test "x$ac_cv_func_getopt" = xyes; then
  AC_DEFINE_UNQUOTED([SUP_GETOPT(argc, argv, optstr, longopts)],
  [getopt(argc, argv, optstr)],
  [Supervisor getopt macro. Argc and argv are number and values of arguments, optstr is a string containing legitimate option characters, longopts is the array of option structures (unused for on system without getopt_long())])
else
  AC_MSG_ERROR([getopt() was not found, module depend on it...])
fi

AC_ARG_ENABLE([munin], AC_HELP_STRING([--disable-munin],[Disable Munin integration]),
            [AM_CONDITIONAL([ENABLE_MUNIN], [test "x$enableval" = xyes])],
            [AM_CONDITIONAL([ENABLE_MUNIN], [true])])

AC_ARG_WITH([systemdsystemunitdir],
	[AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files])],
	[],
	[with_systemdsystemunitdir=auto])

AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"], [
def_systemdsystemunitdir=$(pkg-config --variable=systemdsystemunitdir systemd)])

AS_IF([test "x$def_systemdsystemunitdir" = "x"], [AS_IF([test "x$with_systemdsystemunitdir" = "xyes"], [with_systemdsystemunitdir=no])],
[with_systemdsystemunitdir="$def_systemdsystemunitdir"])
#AC_MSG_ERROR([systemd support requested but pkg-config unable to query systemd package])],

AS_IF([test "x$with_systemdsystemunitdir" = "xno" -o "x$with_systemdsystemunitdir" = "xauto"], [with_systemdsystemunitdir=/usr/lib/systemd/system])

AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])


AC_CONFIG_FILES([Makefile nemea-supervisor.spec nemea-supervisor systemd/Makefile systemd/nemea-supervisor.service munin/Makefile munin/nemea_supervisor configs/Makefile configs/prepare_default_config.sh configs/warning.sup configs/data-sources/ipfixcol.sup configs/supervisor_config_template.xml configs/detectors/amplification_detection.sup configs/detectors/voipfrauddetection.sup configs/loggers/amplification_logger.sup configs/loggers/brute_force_hist_logger.sup configs/loggers/brute_force_logger.sup configs/loggers/dnstunnel_logger.sup configs/loggers/flowmeter_logger.sup configs/loggers/haddrscan_aggr_logger.sup configs/loggers/hoststats_logger.sup configs/loggers/ipblacklistfilter_logger.sup configs/loggers/miner_detector_logger.sup configs/loggers/sip_bf_logger.sup configs/loggers/voip_fraud_logger.sup configs/loggers/vportscan_aggr_logger.sup configs/amplification_detection.mkdir configs/brute_force_detector.mkdir configs/brute_force_hist_detector.mkdir configs/dnstunnel_detection.mkdir configs/flow_meter.mkdir configs/haddrscandetector.mkdir configs/hoststats.mkdir configs/ipblacklistfilter.mkdir configs/miner_detector.mkdir configs/multopt.mkdir configs/sip_bf_detector.mkdir configs/voipfrauddetection.mkdir configs/vportscandetector.mkdir])
AC_OUTPUT
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  ASM.(32 bit only)..: $ASM"
echo "  Static binary......: $static"
echo
echo "Documentation..........: ${build_doc}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  CXXFLAGS.............: $CXXFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo "  LIBS.................: $LIBS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo "  munin plugin.........: `if [[ -z  "$ENABLE_MUNIN_TRUE" ]]; then echo yes; else echo no; fi`"
echo "  systemd..............: `if [[ -z  "$ENABLE_SYSTEMD_TRUE" ]]; then echo yes; else echo no; fi`"
echo
