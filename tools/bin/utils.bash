#!/bin/bash
#
# shellcheck disable=SC2312

# This code is intended for use inside build scripts (e.g., see build-systemc) and provides aids.

(return 0 2>/dev/null) && SOURCED=1 || SOURCED=0

# Allow display of version
export UTILS_VERSION
UTILS_VERSION=1.10.1
if [[ "$*" =~ "--version" ]]; then
  echo "utils.bash version ${UTILS_VERSION}"
fi

if [[ ${SOURCED} == 0 ]]; then
  echo "Don't run $0, source it" >&2
  exit 1
fi

cat >/dev/null <<'EOF' ;# Documentation begin_markdown {
SYNOPSIS
========

`utils.bash` - A collection of bash functions useful for scripting.
In particular, these are intended for use in scripts to build (fetch,
configure, compile and install) various apps and libraries (e.g., SystemC).
Note that some have been moved into the `scripts/` directory
parallel to this script's directory facilitating easier testing and maintenance.

Note: Capitalizing function names reduces collisions with scripts/executables.

| FUNCTION SYNTAX            | DESCRIPTION
| :------------------------- | :----------
| GetBuildOpts -b BRIEF "$@" | Parses standard _build_ command-line inputs
| ShowBuildOpts              | Display options variables
| ConfirmBuildOpts || exit   | Asks the user to confirm build locations
| SetupLogdir _BASENAME_     | Sets up the logfile directory
| Create_and_Cd DIR          | Creates directory and enters it
| GetSource_and_Cd DIR URL   | Downloads source and enters directory
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

declare -a ARGV;
export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
  CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
  CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
  CXX DEBUG ERRORS GENERATOR BUILDER             \
  WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
  NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
  STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
  TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
  TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
  NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
  VERBOSITY WARNINGS

#NOW="$(date '+%m%d%H%M%Y.%S')"
#TIMESTAMP="$(date +%s)"
TMP="$(mktemp /tmp/Save-XXXX)"
# shellcheck disable=SC2329 # invoked indirectly by the EXIT trap
Cleanup() {
    rm -f "${TMP}"
}
trap Cleanup EXIT

# Defaults if empty
if [[ -z "${VERBOSITY}" ]]; then VERBOSITY=0;  fi
if [[ -z "${MAKECHECK}"   ]]; then MAKECHECK=0;fi
if [[ -z "${NOFETCH}"   ]]; then NOFETCH=no;   fi
if [[ -z "${NOCOMPILE}" ]]; then NOCOMPILE=no; fi
if [[ -z "${NOINSTALL}" ]]; then NOINSTALL=no; fi

function Realpath()
{
  if [[ $# == 0 ]]; then set - .; fi
  /usr/bin/perl '-MCwd(abs_path)' -le '$p=abs_path(join(q( ),@ARGV));print $p if -e $p' "$*"
}

# Using Essential-IO
export PORTABLE_SCRIPTS_HOME
PORTABLE_SCRIPTS_HOME="${PORTABLE_SCRIPTS_HOME:-${HOME}/.portable-scripts}"
SCRIPT_DIR=
SCRIPT_DIR="$(Realpath "$(dirname "$0")"/../scripts)"
if [[ ! -r "${SCRIPT_DIR}/Essential-IO" ]]; then
  SCRIPT_DIR="$(Realpath "${PORTABLE_SCRIPTS_HOME}/scripts")"
fi
if [[ ! -r "${SCRIPT_DIR}/Essential-IO" ]]; then
  SCRIPT_DIR="$(Realpath "$(dirname "$0")")"
fi
if [[ ! -r "${SCRIPT_DIR}/Essential-IO" ]]; then
  printf "FATAL: Missing required source file '%s'\n" "${SCRIPT_DIR}/Essential-IO"
  crash
fi
# shellcheck disable=SC2250,SC1091,SC1090
source "$SCRIPT_DIR/Essential-IO"

UTILS_DIR="$(Realpath "$(dirname "$0")"/../bin)"
UTILS_SCRIPT="${UTILS_DIR}/utils.bash"

PATCH_DIR="$(Realpath "$(dirname "$0")"/../patches)"

#-------------------------------------------------------------------------------
function Require()
{ # FILE(S) to source
  local BIN_DIR SCRIPT_NAME REQUIRED
  if [[ $# != 1 ]]; then
    echo "Fatal: Require only allows one argument" 1>&2; exit 1
  fi
  BIN_DIR="$(Realpath "$(dirname "$0")"/../bin)"
  SCRIPT_NAME="$1"; shift
  REQUIRED="${BIN_DIR}/${SCRIPT_NAME}"
  if [[ -f "${REQUIRED}" ]]; then
    # shellcheck disable=SC1090
    source "${REQUIRED}"
  else
    echo "Fatal: Missing ${REQUIRED}" 1>&2; exit 1
  fi
}

#-------------------------------------------------------------------------------
# Verify existence of tools in search path

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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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
    MAKECHECK \
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
    TOOL_BASE \
    TOOL_VERS \
    TOOL_URL \
    TOOL_PATCHES \
    BUILD_DIR \
    CMAKE_BUILD_TYPE \
    BUILDER \
    GENERATOR \
    MAKECHECK \
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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  ShowBuildOpts
  while true; do
    printf "Confirm above options (y/n)? "
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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  Step_Show "Get build options"

# Establishes options for building
#
#-------------------------------------------------------------------------------
# The following is formatted for use with -help
#|
#|NAME
#|----
#|
#|  BRIEF
#|
#|COMMAND-LINE OPTIONS
#|---------------------------
#|
#|  Option             |  Alternative      | Description
#|  ------             |  ------------     | -----------
#|  --build-dir=DIR    |  -bd DIR          | source subdirectory to build in
#|  --builder=TYPE     |  -bld TYPE        | cmake, autotools, or boost
#|  --build-type TYPE  |  -bt TYPE         | Debug, Release, or RelWithDebInfo
#|  --cc=C_COMPILER    |  CC=C_COMPILER    | chooses C compiler executable (e.g., gcc or clang)
#|  --check            |  -ck              | run 'make check' after build
#|  --clang            |                   | quick --cc=clang --cxx=clang++
#|  --clean            |  -clean           | reinstall source
#|  --cleanup          |  -cleanup         | remove source after installation
#|  --cxx=CPP_COMPILER |  CXX=CPP_COMPILER | chooses C++ compiler executable (e.g., g++ or clang++)
#|  --debug            |  -d               | developer use
#|  --default          |                   | quick -i=$HOME/.portable-scripts -src=$HOME/.portable-scripts/src
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
#|  --versions         |                   | list available versions       
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
#| - $MAKECHECK
#| - $NOCOMPILE
#| - $NOFETCH
#| - $NOINSTALL
#| - $NOTREALLY
#| - $SRC directory
#| - $SUFFIX
#| - $SYSTEMC_HOME
#| - $SYSTEMC_SRC
#| - $TOOL_INFO
#| - $TOOL_BASE base directory name for tool source
#| - $TOOL_NAME fancy name for display
#| - $TOOL_PATCHES
#| - $TOOL_SRC usually ${HOME}/.portable-scripts/src
#| - $TOOL_TAG 
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
    local ARG="$1"
    case "${ARG}" in
    -devhelp|--devhelp)
      HelpText -md -b 'utils.bash' "${UTILS_SCRIPT}";
      exit 0
      ;;
    -h|-help|--help)
      HelpText -b "${TOOL_BRIEF}" "${UTILS_SCRIPT}";
      exit 0
      ;;
    -ck|-check|--check)
      MAKECHECK=1
      ;;
    -n|--not-really|--notreally)
      NOTREALLY="-n"
      ;;
    -no-fetch|--no-fetch|--nofetch)
      NOFETCH="yes"
      ;;
    -no-compile|--no-compile|--nocompile)
      NOCOMPILE="yes"
      ;;
    -no-install|--no-install|--noinstall)
      export NOINSTALL
      NOINSTALL="yes"
      ;;
    -no-patch|-nopatch|--no-patch|--nopatch)
      NOPATCH=1
      ;;
    --build-dir=*|-bd)
      if [[ "${ARG}" != '-bd' ]]; then
        BUILD_DIR="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        BUILD_DIR="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --build-type=*|-bt)
      if [[ "${ARG}" != '-bd' ]]; then
        CMAKE_BUILD_TYPE="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        CMAKE_BUILD_TYPE="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --builder=*|-bld)
      if [[ "${ARG}" != '-gen' ]]; then
        BUILDER="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        BUILDER="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --cc=*|CC=*)
      CC="${ARG//*=}"
      if [[ "${CC}" =~ .*gcc ]];then
        CXX=g++
      elif [[ "${CC}" =~ .*clang++ ]]; then
        CXX=clang++
      fi
      ;;
    --clang)
      CC=clang
      CXX=clang++
      ;;
    --clean|-clean)
      CLEAN=1;
      ;;
    --cleanup|-cleanup)
      CLEANUP=1;
      ;;
    --cxx=*|CXX=*)
      CXX="${ARG//*=}"
      if [[ "${CXX}" == g++ ]]; then
        CC=gcc
      elif [[ "${CXX}" == clang++ ]]; then
        CC=clang
      fi
      ;;
    -d|-debug|--debug)
      DEBUG=1;
      ;;
    --default)
      APPS="${PORTABLE_SCRIPTS_HOME}/apps"
      SRC="${PORTABLE_SCRIPTS_HOME}/src"
      ;;
    --doxy)
      BUILD_SOURCE_DOCUMENTATION=on
      ;;
    --gcc)
      CC=gcc
      CXX=g++
      ;;
    --generator=*|-gen)
      if [[ "${ARG}" != '-gen' ]]; then
        GENERATOR="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        GENERATOR="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --info=*|-info)
      if [[ "${ARG}" != '-info' ]]; then
        TOOL_INFO="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_INFO="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    -i|--install=*|--apps=)
      if [[ "${ARG}" != '-i' ]]; then
        APPS="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        APPS="$2"
        shift
      else
        Report_fatal "Need directory argument for ${ARG}"
      fi
      ;;
    --extern)
      APPS="${WORKTREE_DIR}"
      SRC="${WORKTREE_DIR}"
      ;;
    --home)
      APPS=~
      SRC=~/src
      ;;
    -pref|--prefix=*)
      if [[ "${ARG}" != '-pref' ]]; then
        CMAKE_INSTALL_PREFIX="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        CMAKE_INSTALL_PREFIX="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --root-dir=*|-rd)
      if [[ "${ARG}" != '-rd' ]]; then
        WORKTREE_DIR="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        WORKTREE_DIR="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    -patch|--patch=*)
      NOPATCH=0
      PATCH=
      if [[ "${ARG}" != '-patch' ]]; then
        PATCH="${ARG//*=}"
      elif [[ $# -gt 1 && ! "$2" =~ ^- ]]; then
        PATCH="$2"
        shift
      else
        PATCH="${TOOL_SRC}"
      fi
      if [[ -n "${PATCH}" ]]; then
        PATCH="${PATCH_DIR}/${PATCH}"
      fi
      if [[ ! "${PATCH}" =~ [.]patch(es)?$ ]]; then
        PATCH="${PATCH}.patches"
      fi
      if [[ -r "${PATCH}" ]]; then
        TOOL_PATCHES="${PATCH}"
      else
        Report_fatal "Patch '${PATCH}' does not exist"
      fi
      ;;
    -s|--src=*)
      if [[ "${ARG}" != '-s' ]]; then
        SRC="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SRC="$2"
        shift
      else
        Report_fatal "Need directory argument for ${ARG}"
      fi
      ;;
    --std=*|-std=*)
      CMAKE_CXX_STANDARD="${ARG}//*=}"
      ;;
    --steps=*|-steps=*)
      STEP_MAX="${ARG//*=}"
      ;;
    --systemc=*|-sc)
      if [[ "${ARG}" != '-sc' ]]; then
        SYSTEMC_HOME="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SYSTEMC_HOME="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --suffix=*|-suf)
      if [[ "${ARG}" != '-suf' ]]; then
        SUFFIX="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        SUFFIX="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --tool=*|-tool)
      if [[ "${ARG}" != '-tool' ]]; then
        TOOL_NAME="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_NAME="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    --uninstall|-rm)
      CLEANUP=1;
      ;;
    --use-lwg)
      TOOL_URL="git@github.com:OSCI-WG/systemc.git"
      ;;
    --use-https)
      TOOL_URL="$(perl -le '$_=shift@ARGV;s{git.github.com:}{https://github.com/};print $_' "${TOOL_URL}" )"
      ;;
    --use-ssh)
      TOOL_URL="$(perl -le '$_=shift@ARGV;s{https://github.com/}{git\@github.com:};print $_' "${TOOL_URL}" )"
      ;;
    --url=*|-url)
      TOOL_URL="${ARG//*=}"
      ;;
    --loud|-L)
      VERBOSITY=2
      ;;
    --quiet|-q)
      VERBOSITY=0
      ;;
    --verbose|-v)
      VERBOSITY=1
      ;;
    --versions)
      _do git -C "${TOOL_SRC}/${TOOL_BASE}" tag
      exit 0
      ;;
    --version=*|-vers)
      if [[ "${ARG}" != '-vers' ]]; then
        TOOL_VERS="${ARG//*=}"
      elif [[ $# -gt 1 && -d "$2" ]]; then
        TOOL_VERS="$2"
        shift
      else
        Report_fatal "Need argument for ${ARG}"
      fi
      ;;
    *)
      ARGV[${#ARGV[@]}]="${ARG}"
      ;;
    esac
    shift
  done

  #-------------------------------------------------------------------------------
  # Defaults if not set
  if [[ -z "${APPS}" || ! -d "${APPS}" ]]; then
    APPS="${PORTABLE_SCRIPTS_HOME}/apps"
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
    BUILD_DIR="target"
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
    elif [[ ~ == "$(pwd||true)" ]]; then
      SRC="${PORTABLE_SCRIPTS_HOME}/src"
    else
      SRC="$(pwd)/src"
    fi
  fi
  _do mkdir -p "${SRC}" || Report_fatal "Failed to find/create ${SRC} directory"

  _do builtin cd "${SRC}" || Report_fatal "Unable to change into source directory"

  if [[ -n "${DEBUG}" && "${DEBUG}" != 0 ]]; then
    ShowBuildOpts
  fi

  Step_Next || return 1
}

function SetupLogdir()
{
  Step_Show "Set up log directory $1"
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  local NONE
  NONE="$(_C none)"
  # Creates log directory and sets initial LOGFILE
  export LOGFILE
  LOGDIR=~/logs
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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  if [[ $# != 1 ]]; then return 1; fi # Assert
  Step_Show "Create source for ${1}"
  _do mkdir -p "${1}" || Report_fatal "Unable to create ${1}" || return 1
  if ! _do builtin cd "${1}" ; then Report_fatal "Unable to enter ${1}"; exit 1; fi
  Step_Next || return 1
}

# Download and enter directory and patch (TODO)
function GetSource_and_Cd() # DIR URL
{
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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
    if ! _do builtin cd "${DIR}" ; then Report_fatal "Unable to enter ${DIR}"; exit 1; fi
    Step_Next && return 0 || return 1
  fi
  # Git
  if [[ "${URL}" =~ [.]git$ ]]; then
    if [[ -d "${DIR}/.git" ]]; then
      _do git -C "${DIR}" pull --no-edit origin master
    else
      # If directory exists, move it out of the way
      if [[ -d "${DIR}/." ]]; then
        Report_warning "Directory already existed without git repo. Renamed ${DIR}-${TMP}"
        _do mv "${DIR}" "${DIR}-${TMP}"
      fi
      _do git clone "${URL}" "${DIR}" || Report_fatal "Unable to clone into ${DIR}" || exit 1
    fi
    if ! _do builtin cd "${DIR}" ; then Report_fatal "Unable to enter ${DIR}"; exit 1; fi
    if [[ -n "${TOOL_VERS}" ]]; then
      _do git checkout "${TOOL_VERS}" .
    fi
    if [[ ${NOPATCH} == 0 && -n "${TOOL_PATCHES}" ]]; then
      _do git  am --empty=drop "${TOOL_PATCHES}"
    fi
  elif [[ "${URL}" =~ ^https://.+tgz$ ]]; then
    local ARCHIVE WORK_DIR
    ARCHIVE="$(basename "${URL}")"
    _do wget "${URL}" || Report_fatal "Unable to download from ${URL}" || exit 1
    _do tar xf "${ARCHIVE}" || "Unable to expand ${ARCHIVE}" || exit 1
    WORK_DIR="$(tar tf "${URL}" | head -1)"
    if ! _do builtin cd "${WORK_DIR}" ; then Report_fatal "Unable to enter ${WORK_DIR}"; exit 1; fi
  else
    Report_fatal "Unknown URL type - currently only handle *.git or *.tgz" || exit 1
  fi
  Step_Next || return 1
}

#Checks out specified or latest tagged version
function Select_version()
{
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  if [[ $# != 1 ]]; then return 1; fi # Assert
  Step_Show "Selecting version $1"
  local SELECTED="$1"
  case "${SELECTED}" in
    last)
      SELECTED="$(git tag | tail -1)"
      ;;
    *) ;; # use specified version
  esac
  _do git checkout "${SELECTED}" .
}

# Arguments are optional
# shellcheck disable=SC2120
function Configure_tool() # [TYPE]
{
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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
      _do builtin cd "${BUILD_DIR}" || Report_fatal "Unable to enter ${BUILD_DIR}"
      if ! _do builtin cd "${BUILD_DIR}" ; then Report_fatal "Unable to enter ${BUILD_DIR}"; exit 1; fi
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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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
      if [[ ${MAKECHECK} == 1 ]]; then
        _do cmake --build "${BUILD_DIR}" -- check
      fi
      ;;
    autotools)
      if ! _do builtin cd "${BUILD_DIR}" ; then Report_fatal "Unable to enter ${BUILD_DIR}"; exit 1; fi
      _do make
      if [[ ${MAKECHECK} == 1 ]]; then
        _do make -C "${BUILD_DIR}" check
      fi
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
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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

# shellcheck disable=SC2329 # invoked indirectly by the EXIT trap
function Cleanup()
{
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
  export NOLOG=1
  if [[ $# = 1 ]]; then return 1; fi # Assert
  Step_Show "Clean up"
  if [[ -n "${CLEANUP}" && "${CLEANUP}" == 1 ]]; then
    if ! _do builtin cd "${SRC}" ; then Report_fatal "Unable to enter ${SRC}"; exit 1; fi
    rm -fr "${1}"
  fi
  Step_Next || return 1
  NOLOG=0
}

function Main()
{
  declare -a ARGV
  export APPS ARGV BUILD_SOURCE_DOCUMENTATION CC   \
    CLEAN CLEANUP CMAKE_BUILD_TYPE                 \
    CMAKE_CXX_STANDARD CMAKE_INSTALL_PREFIX        \
    CXX DEBUG ERRORS GENERATOR BUILDER             \
    WORKTREE_DIR LOGDIR LOGFILE NOPATCH            \
    NOTREALLY TMP SRC SYSTEMC_HOME STEP_CURRENT    \
    STEP_MAX SUFFIX TOOL_BRIEF TOOL_NAME TOOL_INFO \
    TOOL_SRC TOOL_BASE TOOL_TAG TOOL_VERS TOOL_URL \
    TOOL_PATCHES BUILD_DIR MAKECHECK NOFETCH       \
    NOCOMPILE NOINSTALL UNINSTALL UTILS_SCRIPT     \
    VERBOSITY WARNINGS
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
