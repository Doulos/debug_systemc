#!/bin/bash
#
# shellcheck disable=SC2312

# This code is intended for use inside build scripts (e.g., see build-systemc) and provides aids.

(return 0 2>/dev/null) && SOURCED=1 || SOURCED=0
if [[ ${SOURCED} == 0 ]]; then
  echo "Don't run $0, source it" >&2
  exit 1
fi

export UTILS_VERSION
UTILS_VERSION=1.8

cat >/dev/null <<'EOF' ;# Documentation begin_markdown {
SYNOPSIS
========

`utils.bash` - A collection of bash functions useful for scripting.
In particular, these are intended for use in scripts to build (fetch,
configure, compile and install) various apps and libraries (e.g., SystemC).
Note that some have been moved into the `scripts/` directory
parallel to this directory to facilitate easier testing and maintenance.

Note: Capitalizing function names reduces collisions with scripts/executables.

| FUNCTION SYNTAX            | DESCRIPTION
| :------------------------- | :----------
| GetBuildOpts "$0" "$@"     | Parses standard _build_ command-line inputs
| ShowBuildOpts              | Display options variables
| ConfirmBuildOpts || exit   | Asks user to confirm build locations
| SetupLogdir _BASENAME_     | Sets up the logfile directory
| Create_and_Cd DIR          | Creates directory and enters it
| GetSource_and_Cd DIR URL   | Downloads souce and enters directory
| Select_version VERSION     | Checks out specified or latest tagged version
| Configure_tool [TYPE]      | Invokes cmake or autotools
| Compile_tool               |

USAGE
=====

Source this as follows inside your bash script:

```sh
function Realpath()
{
  if [[ $# == 0 ]]; then set - .; fi
  /usr/bin/perl '-MCwd(abs_path)' -le '$p=abs_path(join(q( ),@ARGV));print $p if -e $p' "$*"
}
SCRIPT="$(Realpath "$0")"
SCRIPT="$(dirname "${SCRIPT}")/utils.bash"
if [[ -x "${SCRIPT}" ]]; then
  # shellcheck source=utils.bash
  source "${SCRIPT}" "$0"
else
  echo "Error: Missing ${SCRIPT}" 1>&2; exit 1
fi
SetupLogdir "$0"
```
} end_markdown
EOF

declare -a ARGV
export APPS
export ARGV
export BUILD_SOURCE_DOCUMENTATION
export CC
export CLEAN
export CLEANUP
export CMAKE_BUILD_TYPE
export CMAKE_CXX_STANDARD
export CMAKE_INSTALL_PREFIX
export CXX
export DEBUG
export ERRORS
export GENERATOR
export BUILDER
export WORKTREE_DIR
export LOGDIR
export LOGFILE
export NOPATCH
export NOTREALLY
export SRC
export SYSTEMC_HOME
export STEP_CURRENT
export STEP_MAX
export SUFFIX
export TOOL_NAME
# shellcheck disable=SC2090
export TOOL_INFO
export TOOL_CHECKOUT
export TOOL_SRC
export TOOL_VERS
export TOOL_URL
export TOOL_PATCHES
export BUILD_DIR
export NOFETCH
export NOCOMPILE
export NOINSTALL
export UNINSTALL
export VERBOSITY
export WARNINGS

# Defaults if empty
if [[ -z "${VERBOSITY}" ]]; then VERBOSITY=0;  fi
if [[ -z "${NOFETCH}"   ]]; then NOFETCH=no;   fi
if [[ -z "${NOCOMPILE}" ]]; then NOCOMPILE=no; fi
if [[ -z "${NOINSTALL}" ]]; then NOINSTALL=no; fi

function Realpath()
{
  if [[ $# == 0 ]]; then set - .; fi
  /usr/bin/perl '-MCwd(abs_path)' -le '$p=abs_path(join(q( ),@ARGV));print $p if -e $p' "$*"
}

# Using Essential-IO
SCRIPTDIR="$(Realpath "$(dirname "$0")"/../scripts)"
if [[ ! -r "${SCRIPTDIR}/Essential-IO" ]]; then
  SCRIPTDIR="$(Realpath "${HOME}"/.local/scripts)"
fi
if [[ ! -r "${SCRIPTDIR}/Essential-IO" ]]; then
  SCRIPTDIR="$(Realpath "$(dirname "$0")")"
fi
if [[ ! -r "${SCRIPTDIR}/Essential-IO" ]]; then
  printf "FATAL: Missing required source file '%s'\n" "${SCRIPTDIR}/Essential-IO"
  crash
fi
# shellcheck disable=SC2250,SC1091,SC1090
source "$SCRIPTDIR/Essential-IO"

PATCHDIR="$(Realpath "$(dirname "$0")"/../patches)"

#-------------------------------------------------------------------------------
function Require()
{ # FILE(S) to source
  local BINDIR SCPT REQD
  if [[ $# != 1 ]]; then
    echo "Fatal: Require only allows one argument" 1>&2; exit 1
  fi
  BINDIR="$(Realpath "$(dirname "$0")"/../bin)"
  SCPT="$1"; shift
  REQD="${BINDIR}/${SCPT}"
  if [[ -f "${REQD}" ]]; then
    # shellcheck disable=SC1090
    source "${REQD}"
  else
    echo "Fatal: Missing ${REQD}" 1>&2; exit 1
  fi
}

#-------------------------------------------------------------------------------
# Verify existance of tools in search path

function Needs()
{
  local I J X D P
  I=0 J=0
  for X in "$@"; do
    (( ++I ));
    for D in $(/usr/bin/perl -le 'print join(q{\n},split(/:/,$ENV{PATH}))'); do
      Report_debug "Testing  ${D}/${X}"
      P="$(Realpath "${D}/${X}")"
      if [[ -n "${P}" && -x "${P}" && ! -d "${P}" ]]; then
        (( ++J ));
        echo "${P}"
        break
      else
        P=""
      fi
    done
    if [[ -z "${P}" ]]; then Report_error "Missing ${X}"; fi
  done
  if [[ "${I}" == "${J}" ]]; then
    return 0
  else
    return 1
  fi
}

function Step_Next()
{
  export STEP_CURRENT STEP_MAX
  if [[ -z "${STEP_CURRENT}" ]]; then STEP_CURRENT=0; fi
  (( ++STEP_CURRENT ));
  if [[ -n "${STEP_MAX}" && "${STEP_CURRENT}" -ge "${STEP_MAX}" ]]; then return 1; fi
}

function Step_Show()
{
  if [[ ${VERBOSITY} == 0 ]]; then return; fi
  export STEP_CURRENT
  if [[ -z "${STEP_CURRENT}" ]]; then STEP_CURRENT=0; fi
  local STEP=0
  (( STEP = STEP_CURRENT + 1 ))
  Report_info "Step ${STEP} $*"
}

#-------------------------------------------------------------------------------

Setup-Color on

function ShowBuildOpts()
{
  Ruler -blu "Build Options"
  for (( i=0; i<${#ARGV[@]}; ++i )); do
    Report_debug "ARGV[${i}]='${ARGV[${i}]}'"
  done
  ShowVars -x\
    APPS \
    BUILD_SOURCE_DOCUMENTATION \
    CLEAN \
    CLEANUP \
    CMAKE_CXX_STANDARD \
    CMAKE_INSTALL_PREFIX \
    CC \
    CXX \
    DEBUG \
    WORKTREE_DIR \
    LOGDIR \
    LOGFILE \
    NOTREALLY \
    SRC \
    SYSTEMC_HOME \
    SYSTEMC_SRC  \
    STEP_CURRENT \
    STEP_MAX \
    SUFFIX \
    TOOL_NAME \
    TOOL_INFO \
    TOOL_SRC \
    TOOL_VERS \
    TOOL_URL \
    TOOL_CHECKOUT \
    TOOL_PATCHES \
    BUILD_DIR \
    CMAKE_BUILD_TYPE \
    BUILDER \
    GENERATOR \
    NOFETCH \
    NOCOMPILE \
    NOINSTALL \
    UNINSTALL \
    VERBOSITY \
    ;
  Ruler -blu
}

function ConfirmBuildOpts()
{
  ShowBuildOpts
  while true; do
    printf "Confirm above options (Y/n)? "
    read -r REPLY
    case "${REPLY}" in
      y|Y|yes) return 0 ;;
      n|N|no) return 1 ;;
      *) REPLY=""; echo "Must reply with 'y' or 'n'" ;;
    esac
  done
}

function GetBuildOpts()
{
  Step_Show "Get build options"

# Establishes options for building
#
#-------------------------------------------------------------------------------
# The following is formatted for use with -help
#|
#|NAME
#|----
#|
#|  $0 - helper script for build scripts
#|
#|SYNOPSIS
#|--------
#|
#|  GetBuildOpts() "${0}" "$@"
#|
#|DESCRIPTION
#|-----------
#|
#|  Read command-line options and sets various environment variables.
#|
#|PARSED COMMAND-LINE OPTIONS
#|---------------------------
#|
#|  Option             |  Alternative      | Description
#|  ------             |  ------------     | -----------
#|  --build-dir=DIR    |  -bd DIR          | source subdirectory to build in
#|  --builder=TYPE     |  -bld TYPE        | cmake, autotools, or boost
#|  --build-type TYPE  |  -bt TYPE         | Debug, Release, or RelWithDebInfo
#|  --cc=C_COMPILER    |  CC=C_COMPILER    | chooses C compiler executable (e.g., gcc or clang)
#|  --clang            |                   | quick --cc=clang --cxx=clang++
#|  --clean            |  -clean           | reinstall source
#|  --cleanup          |  -cleanup         | remove source after installation
#|  --cxx=CPP_COMPILER |  CXX=CPP_COMPILER | chooses C++ compiler executable (e.g., g++ or clang++)
#|  --debug            |  -d               | developer use
#|  --default          |                   | quick -i=$HOME/.local -src=$HOME/.local/src
#|  --gcc              |                   | quick --cc=gcc --cxx=g++
#|  --generator=GEN    |                   | generator (for cmake)
#|  --home             |                   | quick -i $HOME -s $HOME/src
#|  --info=TEXT        |  -info TEXT       | choose installation directory
#|  --install=DIR      |  -i DIR           | choose installation directory
#|  --nogetbuildopts   |  -na              | don't automatically GetBuildOpts
#|  --notreally        |  -n               | don't execute, just show possibilities
#|  --no-install       |  -no-install      | do not install
#|  --no-patch         |  -no-patch        | do not patch
#|  --patch[=name]     |  -patch [name]    | apply patches
#|  --src=DIR          |  -s DIR           | choose source directory
#|  --std=N            |  -std=N           | set make C++ compiler version where N={98,11,14,17,20,...}
#|  --steps=N          |  -steps=N         | Only execute N steps
#|  --systemc=DIR      |  -sc              | reference SystemC installation
#|  --suffix=TEXT      |  -suf TEXT        | set suffix for installation name
#|  --tool=NAME        |  -tool NAME       | set the desired tool name for tool source
#|  --uninstall        |  -rm              | remove if possible -- not always supported
#|  --url=URL          |  -url URL         | set the URL for the source code
#|  --use-https        |                   | change URL to use https protocol
#|  --use-ssh          |                   | change URL to use git ssh protocol
#|  --use-lwg          |                   | change URL to use Accellera private repo
#|  --verbose          |  -v               | echo more information (may be repeated)
#|  --version=X.Y.Z    |  -vers X.Y.Z      | set the desired tool version
#|  --quiet            |  -q               | echo less information (may be repeated)
#|
#|OUTPUTS
#|-------
#|
#| Below are some of the environment variables are used by scripts:
#|
#| - $APPS
#| - $CC
#| - $CLEAN
#| - $CLEANUP
#| - $BUILD_DIR
#| - $CMAKE_BUILD_TYPE
#| - $CMAKE_CXX_STANDARD {98|03|11|14|17|20}
#| - $CMAKE_INSTALL_PREFIX
#| - $CXX
#| - $DEBUG
#| - $ERRORS integer
#| - $LOGDIR
#| - $LOGFILE
#| - $NOCOMPILE
#| - $NOFETCH
#| - $NOINSTALL
#| - $NOTREALLY
#| - $SRC directory
#| - $SUFFIX
#| - $SYSTEMC_HOME
#| - $SYSTEMC_SRC
#| - $TOOL_CHECKOUT
#| - $TOOL_INFO
#| - $TOOL_NAME
#| - $TOOL_PATCHES
#| - $TOOL_SRC
#| - $TOOL_URL
#| - $TOOL_VERS
#| - $WORKTREE_DIR
#| - $BUILDER {cmake|autotools}
#| - $GENERATOR {|Ninja|Unix Makefiles}
#| - $TOOL_URL
#| - $UNINSTALL
#| - $VERBOSITY integer
#| - $WARNINGS integer
#|
#| Use -devhelp for internal documentation.
#|
#|LICENSE
#|-------
#|
#| Copyright 2023 by Doulos Inc.
#| Licensed using Apache 2.0

  if [[ "$2" =~ ^-{1,2}(na|nogetbuildopts)$ ]]; then return; fi

  # Grab program name
  local SCRIPT
  # shellcheck disable=SC2034
  SCRIPT="$1"
  shift

  # Defaults
  #-------------------------------------------------------------------------------
  CMAKE_CXX_STANDARD=17 # <---------- VERSION OF C++ YOUR COMPILER SUPPORTS
  if [[ "${CXX}" == "" ]]; then
    if [[ "$(command -v clang++||true)" != "" ]]; then
      CXX=clang++
      CC=clang
    elif [[ "$(command -v g++||true)" != "" ]]; then
      CXX=g++
      CC=gcc
    else
      Report_fatal "Unable to determine C++ compiler"
    fi
  fi
  if git rev-parse --show-toplevel 2>/dev/null 1>/dev/null; then
    WORKTREE_DIR="$(git rev-parse --show-toplevel)"
  else
    # Report_warning "Not inside a git controlled area"
    WORKTREE_DIR="Unknown"
  fi

  #-------------------------------------------------------------------------------
  # Scan command-line for options
  #-------------------------------------------------------------------------------
  while [[ $# != 0 ]]; do
    case "$1" in
    -devhelp|--devhelp)
      HelpText -md "${0}";
      exit 0
      ;;
    -h|-help|--help)
      HelpText "${0}";
      exit 0
      ;;
    -n|--not-really|--notreally)
      NOTREALLY="-n"
      shift
      ;;
    -no-fetch|--no-fetch|--nofetch)
      NOFETCH="yes"
      shift
      ;;
    -no-compile|--no-compile|--nocompile)
      NOCOMPILE="yes"
      shift
      ;;
    -no-install|--no-install|--noinstall)
      NOINSTALL="yes"
      shift
      ;;
    -no-patch|-nopatch|--no-patch|--nopatch)
      NOPATCH=1
      shift
      ;;
    --build-dir=*|-bd)
      if [[ "$1" != '-bd' ]]; then
        BUILD_DIR="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        BUILD_DIR="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --build-type=*|-bt)
      if [[ "$1" != '-bd' ]]; then
        CMAKE_BUILD_TYPE="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        CMAKE_BUILD_TYPE="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --builder=*|-bld)
      if [[ "$1" != '-gen' ]]; then
        BUILDER="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        BUILDER="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --cc=*|CC=*)
      CC="${1//*=}"
      if [[ "${CC}" =~ .*gcc ]];then
        CXX=g++
      elif [[ "${CC}" =~ .*clang++ ]]; then
        CXX=clang++
      fi
      shift
      ;;
    --checkout=*)
      TOOL_CHECKOUT="${1//*=}"
      shift
      ;;
    --clang)
      CC=clang
      CXX=clang++
      shift
      ;;
    --clean|-clean)
      CLEAN=1;
      shift
      ;;
    --cleanup|-cleanup)
      CLEANUP=1;
      shift
      ;;
    --cxx=*|CXX=*)
      CXX="${1//*=}"
      if [[ "${CXX}" == g++ ]]; then
        CC=gcc
      elif [[ "${CXX}" == clang++ ]]; then
        CC=clang
      fi
      shift
      ;;
    -d|-debug|--debug)
      DEBUG=1;
      shift
      ;;
    --default)
      APPS="${HOME}.local/apps"
      SRC="${HOME}/.local/src"
      shift
      ;;
    --doxy)
      BUILD_SOURCE_DOCUMENTATION=on
      shift
      ;;
    --gcc)
      CC=gcc
      CXX=g++
      shift
      ;;
    --generator=*|-gen)
      if [[ "$1" != '-gen' ]]; then
        GENERATOR="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        GENERATOR="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --info=*|-info)
      if [[ "$1" != '-info' ]]; then
        TOOL_INFO="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_INFO="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    -i|--install=*|--apps=)
      if [[ "$1" != '-i' ]]; then
        APPS="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        APPS="$2"
        shift
      else
        Report_fatal "Need directory argument for $1"
      fi
      shift
      ;;
    --extern)
      APPS="${WORKTREE_DIR}"
      SRC="${WORKTREE_DIR}"
      ;;
    --home)
      APPS="${HOME}"
      SRC="${HOME}/src"
      shift
      ;;
    -pref|--prefix=*)
      if [[ "$1" != '-pref' ]]; then
        CMAKE_INSTALL_PREFIX="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        CMAKE_INSTALL_PREFIX="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --root-dir=*|-rd)
      if [[ "$1" != '-rd' ]]; then
        WORKTREE_DIR="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        WORKTREE_DIR="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    -patch|--patch=*)
      NOPATCH=0
      PATCH=
      if [[ "$1" != '-patch' ]]; then
        PATCH="${1//*=}"
      elif [[ $# -gt 1 && ! "$2" =~ ^- ]]; then
        PATCH="$2"
        shift
      else
        PATCH="${TOOL_SRC}"
      fi
      if [[ -n "${PATCH}" ]]; then
        PATCH="${PATCHDIR}/${PATCH}"
      fi
      if [[ ! "${PATCH}" =~ [.]patch(es)?$ ]]; then
        PATCH="${PATCH}.patches"
      fi
      if [[ -r "${PATCH}" ]]; then
        TOOL_PATCHES="${PATCH}"
      else
        Report_fatal "Patch '${PATCH}' does not exist"
      fi
      shift
      ;;
    -s|--src=*)
      if [[ "$1" != '-s' ]]; then
        SRC="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SRC="$2"
        shift
      else
        Report_fatal "Need directory argument for $1"
      fi
      shift
      ;;
    --std=*|-std=*)
      CMAKE_CXX_STANDARD="${1//*=}"
      shift
      ;;
    --steps=*|-steps=*)
      STEP_MAX="${1//*=}"
      shift
      ;;
    --systemc=*|-sc)
      if [[ "$1" != '-sc' ]]; then
        SYSTEMC_HOME="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SYSTEMC_HOME="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --suffix=*|-suf)
      if [[ "$1" != '-suf' ]]; then
        SUFFIX="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SUFFIX="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --tool=*|-tool)
      if [[ "$1" != '-tool' ]]; then
        TOOL_NAME="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_NAME="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    --uninstall|-rm)
      CLEANUP=1;
      shift
      ;;
    --use-lwg)
      TOOL_URL="git@github.com:OSCI-WG/systemc.git"
      shift
      ;;
    --use-https)
      TOOL_URL="$(perl -le '$_=shift@ARGV;s{git.github.com:}{https://github.com/};print $_' "${TOOL_URL}" )"
      shift
      ;;
    --use-ssh)
      TOOL_URL="$(perl -le '$_=shift@ARGV;s{https://github.com/}{git\@github.com:};print $_' "${TOOL_URL}" )"
      shift
      ;;
    --url=*|-url)
      TOOL_URL="${1//*=}"
      shift
      ;;
    --quiet|-q)
      VERBOSITY=0
      shift
      ;;
    --verbose|-v)
      VERBOSITY=1
      shift
      ;;
    --version=*|-vers)
      if [[ "$1" != '-vers' ]]; then
        TOOL_VERS="${1//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_VERS="$2"
        shift
      else
        Report_fatal "Need argument for $1"
      fi
      shift
      ;;
    *)
      ARGV[${#ARGV[@]}]="$1"
      shift
      ;;
    esac
  done

  #-------------------------------------------------------------------------------
  # Defaults if not set
  if [[ -z "${APPS}" || ! -d "${APPS}" ]]; then
    APPS="${HOME}/.local/apps"
  fi
  if [[ -z "${SYSTEMC_HOME}" ]]; then
    SYSTEMC_HOME="${APPS}/systemc"
  fi
  if [[ -z "${CMAKE_INSTALL_PREFIX}" ]]; then
    CMAKE_INSTALL_PREFIX="${APPS}"
  fi
  if [[ -z "${BUILDER}" ]]; then
    BUILDER=cmake
  fi
  if [[ -z "${BUILD_DIR}" ]]; then
    BUILD_DIR="build-${BUILDER}-$(basename "${CC}")"
  fi
  if [[ -z "${CMAKE_BUILD_TYPE}" ]]; then
    CMAKE_BUILD_TYPE="RelWithDebInfo"
  fi
  if [[ -z "${BUILD_SOURCE_DOCUMENTATION}" ]]; then
    BUILD_SOURCE_DOCUMENTATION="off"
  fi

  #-------------------------------------------------------------------------------
  # Test some assumptions
  if [[ "${BUILDER}" =~ (cmake|autotools) ]]; then
    Comment all is ok
  else
    Report_error "BUILDER must be one of 'cmake' or 'autotools'"
  fi

  #-------------------------------------------------------------------------------
  # Setup apps directory
  mkdir -p "${APPS}" || Report_fatal "Failed to find/create ${APPS} directory"

  #-------------------------------------------------------------------------------
  Report_info "Setup source directory"
  if [[ -z "${SRC}" || ! -d "${SRC}" ]]; then
    if [[ "${APPS}" == '/apps' ]]; then
      SRC="${APPS}/src"
    elif [[ "${APPS}" != '' ]]; then
      SRC="$(dirname "${APPS}")/src"
    elif [[ "${HOME}" == "$(pwd||true)" ]]; then
      SRC="${HOME}/.local/src"
    else
      SRC="$(pwd)/src"
    fi
  fi
  mkdir -p "${SRC}" || Report_fatal "Failed to find/create ${SRC} directory"

  cd "${SRC}" || Report_fatal "Unable to change into source directory"

  if [[ -n "${DEBUG}" && "${DEBUG}" != 0 ]]; then
    ShowBuildOpts
  fi

  Step_Next || return 1
}

function SetupLogdir()
{
  Step_Show "Set up log directory $1"
  local NONE
  NONE="$(_C none)"
  # Creates log directory and sets initial LOGFILE
  export LOGFILE
  LOGDIR="${HOME}/logs"
  mkdir -p "${LOGDIR}"
  case $# in
    1)
      Logfile "$(basename "$1")"
      ;;
    0)
      if [[ -z "${LOGFILE}" ]]; then
        echo "$(_C red)Warning:${NONE} Did not specify LOGFILE${NONE}"
      else
        Logfile
      fi
      ;;
    *)
      echo "$(_C red)Error: Too many parameters specified${NONE}"
      ;;
  esac
  Step_Next || return 1
}

# Make directory and enter
function Create_and_Cd() # DIR
{
  if [[ $# != 1 ]]; then return 1; fi # Assert
  Step_Show "Create source ${1}"
  _do mkdir -p "${1}" || Report_fatal "Unable to create ${1}" || return 1
  if ! _do cd "${1}" ; then Report_fatal "Unable to enter ${1}"; exit 1; fi
  Step_Next || return 1
}

# Download and enter directory and patch (TODO)
function GetSource_and_Cd() # DIR URL
{
  if [[ $# != 2 ]]; then return 1; fi # Assert
  Step_Show "Get source code from $2 and enter $1"
  local DIR URL
  DIR="$1"
  URL="$2"
  if [[ -n "${CLEAN}" && "${CLEAN}" == 1 && -d "${DIR}" ]]; then
    _do rm -fr "${DIR}"
  fi
  if [[ "${NOFETCH}" == "-n" ]]; then
    Report_info "Skipping fetch of ${TOOL_NAME} as requested"
    if ! _do cd "${DIR}" ; then Report_fatal "Unable to enter ${DIR}"; exit 1; fi
    Step_Next && return 0 || return 1
  fi
  if [[ "${URL}" =~ [.]git$ ]]; then
    if [[ -d "${DIR}/.git" ]]; then
      _do git -C "${DIR}" pull --no-edit origin master
    else
      if [[ -d "${DIR}/." ]]; then
        _do rm -fr "${DIR}" "${DIR}-save"
        _do mv "${DIR}" "${DIR}-save"
        _do rm -fr "${DIR}"
      fi
      _do git clone "${URL}" "${DIR}" || Report_fatal "Unable to clone into ${DIR}" || exit 1
    fi
    if ! _do cd "${DIR}" ; then Report_fatal "Unable to enter ${DIR}"; exit 1; fi
    if [[ -n "${TOOL_CHECKOUT}" ]]; then
      _do git checkout "${TOOL_CHECKOUT}"
    fi
    if [[ ${NOPATCH} == 0 && -n "${TOOL_PATCHES}" ]]; then
      _do git  am --empty=drop "${TOOL_PATCHES}"
    fi
  elif [[ "${URL}" =~ ^https://.+tgz$ ]]; then
    local ARCHIVE WDIR
    ARCHIVE="$(basename "${URL}")"
    _do wget "${URL}" || Report_fatal "Unable to download from ${URL}" || exit 1
    _do tar xf "${ARCHIVE}" || "Unable to expand ${ARCHIVE}" || exit 1
    WDIR="$(tar tf "${URL}" | head -1)"
    if ! _do cd "${WDIR}" ; then Report_fatal "Unable to enter ${WDIR}"; exit 1; fi
  else
    Report_fatal "Unknown URL type - currently only handle *.git or *.tgz" || exit 1
  fi
  Step_Next || return 1
}

#Checks out specified or latest tagged version
function Select_version()
{
  if [[ "${TOOL_CHECKOUT}" != "" ]]; then return 0; fi
  if [[ $# != 1 ]]; then return 1; fi # Assert
  Step_Show "Selecting version $1"
  SELECTED="$1"
  case "${SELECTED}" in
    latest)
      SELECTED="$(git tag | tail -1)"
      ;;
    *) ;; # use specified version
  esac
  _do git checkout "${SELECTED}"
}

# Arguments are optional
# shellcheck disable=SC2120
function Configure_tool() # [TYPE]
{
  export NOLOG=1
  Step_Show "Configure $1"
  Report_info -grn "Configuring ${TOOL_NAME}"
  if [[ $# == 1 ]]; then # option generator
    BUILDER="$1"
    shift
  else
    if [[ -z "${BUILDER}" ]]; then return 1; fi # Assert
  fi
  if [[ $# != 0 ]]; then return 1; fi # Assert
  case "${BUILDER}" in
    cmake)
      _do rm -fr "${BUILD_DIR}"
      local HOST_OS TARGET_ARCH APPLE T
      HOST_OS="$(uname -s)"
      TARGET_ARCH="$(uname -m)"
      T=
      # Handle Apple Silicon
      APPLE=
      if [[ "${HOST_OS}" == Darwin ]]; then
        APPLE="-DCMAKE_APPLE_SILICON_PROCESSOR=${TARGET_ARCH}"
        T+=A
      fi
      if [[ -n "${GENERATOR}" ]];then
        T+=G
      fi
      # Avoid empty arguments
      case "${T}" in
        A) _do cmake -B "${BUILD_DIR}"\
            -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"\
            -DCMAKE_CXX_STANDARD="${CMAKE_CXX_STANDARD}"\
            -DBUILD_SOURCE_DOCUMENTATION="${BUILD_SOURCE_DOCUMENTATION}"\
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
            "${APPLE}"
            ;;
        G) _do cmake -G "${GENERATOR}" -B "${BUILD_DIR}"\
            -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"\
            -DCMAKE_CXX_STANDARD="${CMAKE_CXX_STANDARD}"\
            -DBUILD_SOURCE_DOCUMENTATION="${BUILD_SOURCE_DOCUMENTATION}"\
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
            ;;
        AG) _do cmake -G "${GENERATOR}" -B "${BUILD_DIR}"\
            -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"\
            -DCMAKE_CXX_STANDARD="${CMAKE_CXX_STANDARD}"\
            -DBUILD_SOURCE_DOCUMENTATION="${BUILD_SOURCE_DOCUMENTATION}"\
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
            "${APPLE}"
            ;;
        *) _do cmake -B "${BUILD_DIR}"\
            -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"\
            -DCMAKE_CXX_STANDARD="${CMAKE_CXX_STANDARD}"\
            -DBUILD_SOURCE_DOCUMENTATION="${BUILD_SOURCE_DOCUMENTATION}"\
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"\
            ;;
      esac
      ;;
    autotools)
      reconfigure
      _do mkdir -p "${BUILD_DIR}"
      _do cd "${BUILD_DIR}" || Report_fatal "Unable to enter ${BUILD_DIR}"
      if ! _do cd "${BUILD_DIR}" ; then Report_fatal "Unable to enter ${BUILD_DIR}"; exit 1; fi
      _do env CXXFLAGS="-std=c++${CMAKE_CXX_STANDARD} -I/opt/local/include -I${SYSTEMC_HOME}/include"\
          ../configure --prefix="${SYSTEMC_HOME}"
      ;;
    boost)
      _do ./bootstrap.sh --prefix="${WORKTREE_DIR}/extern"
      ;;
    *)
      Report_error "Unknown builder '${BUILDER}'"
      ;;
  esac
  Step_Next || return 1
  NOLOG=0
}
alias Generate=Configure_tool

# Arguments are optional
# shellcheck disable=SC2120
function Compile_tool()
{
  export NOLOG=1
  Step_Show "Compile"
  if [[ "${NOCOMPILE}" == "-n" ]]; then
    Report_info "Skipping compilation of ${TOOL_NAME} as requested"
    Step_Next && return 0 || return 1
  fi
  Report_info -grn "Compiling ${TOOL_NAME}"
  case "${BUILDER}" in
    cmake)
      _do cmake --build "${BUILD_DIR}"
      ;;
    autotools)
      if ! _do cd "${BUILD_DIR}" ; then Report_fatal "Unable to enter ${BUILD_DIR}"; exit 1; fi
      _do make
      ;;
    boost)
      _do ./b2
      ;;
    *)
      Report_error "Unknown builder '${BUILDER}'"
      ;;
  esac
  Step_Next || return 1
  NOLOG=0
}

function Install_tool()
{
  export NOLOG=1
  Step_Show "Install"
  if [[ "${NOINSTALL}" == "yes" ]]; then
    Report_info "Skipping installation of ${TOOL_NAME} as requested"
    Step_Next && return 0 || return 1
  fi
  Report_info -grn "Installing ${TOOL_NAME} to final location"
  case "${BUILDER}" in
    cmake)
      _do cmake --install "${BUILD_DIR}"
      ;;
    autotools)
      _do make install
      ;;
    boost)
      _do ./b2 install --prefix=
      ;;
    *)
      Report_error "Unknown builder '${BUILDER}'"
      ;;
  esac
  Step_Next || return 1
  NOLOG=0
}

function Cleanup()
{
  export NOLOG=1
  if [[ $# = 1 ]]; then return 1; fi # Assert
  Step_Show "Clean up"
  if [[ -n "${CLEANUP}" && "${CLEANUP}" == 1 ]]; then
    if ! _do cd "${SRC}" ; then Report_fatal "Unable to enter ${SRC}"; exit 1; fi
    rm -fr "${1}"
  fi
  Step_Next || return 1
  NOLOG=0
}

function Main()
{
  if [[ "${BASH_VERSINFO[0]}" -ge 5 ]]; then return 1; fi # Assert
  if [[ $# != 0 ]]; then
    GetBuildOpts "$0" "$@"
    if [[ ${#ARGV[@]} -gt 0 ]]; then
      if [[ -n "${DEBUG}" ]]; then echo "${ARGV[@]}"; fi
    fi
  fi
}

Main "$@"

# vim:nospell
