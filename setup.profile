#!/usr/bin/env bash

(return 0 2>/dev/null) && SOURCED=1 || SOURCED=0
if [[ ${SOURCED} == 0 ]]; then
  echo "Don't run $0, source it" >&2
  exit 1
fi

#$Info: Sets up environment. $
export ENTRY_DIR GIT_WORK_DIR PROJECT_NAME PROJECT_DIR SETUP_PATH PATH
export SYSTEMC_HOME LD_LIBRARY_PATH DYLD_LIBRARY_PATH

# NOTES
# - APPS is occasionally used to reference tools installed outside the project.
# - PROJECT_NAME and SETUP_PATH are for debug purposes.
# - PROJECT_DIR is used to locate the top directory
# - SYSTEMC_HOME refers to the installation location of SystemC - *NOT* the source
# - LD_LIBRARY_PATH and DLYD_LIBRARY_PATH are used for dynamically linked executables
# - PROJECT_DIR is used to locate the extern and include directories.
#
# - Key directories and files include:
# $PROJECT_DIR/
#   ├── CMakeLists.txt -- top-level used for regression testing
#   ├── GNUmakefile -- used by instructors for maintenance
#   ├── INSTALLATION.md
#   ├── LICENSE
#   ├── Makefile.defs
#   ├── VERSION.txt
#   ├── cmake/ -- CMake scripts
#   ├── extern/ -- empty, used for certain external 3rd party installs
#   │   ├── ABOUT.md
#   │   └── bin/ -- useful for building and running
#   ├── include/ -- headers shared amoung exercises
#   └── setup.profile

function Realpath()
{
  if [[ $# == 0 ]]; then set - .; fi
  # shellcheck disable=SC2016
  local PERLSCRIPT='$p=abs_path(join(q( ),@ARGV));print $p if -e $p'
  /usr/bin/env perl '-MCwd(abs_path)' -le "${PERLSCRIPT}" "$*"
}

function Header()
{
  if builtin command -v header >/dev/null 2>&1; then
    header "$@"
  else
    while [[ "$1" =~ ^- ]]; do shift; done
    local text
    text="$(tr '[:lower:]' '[:upper:]' <<<"$*")" #>>>
    printf "[1;96m%s[0m\n" "${text}"
  fi
}

function Project_setup()
{
  # @brief does the real work of setup
  export ACTION='add' ENTRY_DIR SETUP_PATH DEBUG=0
  for arg in "$@"; do
    case "${arg}" in
      --debug|-d) DEBUG=1 ;;
      add|rm|update) ACTION="${arg}" ;;
      *) ;;
    esac
  done
  ENTRY_DIR="$(pwd)"

  # Color support
  #
  # shellcheck disable=SC2034
  local RED="[91m" BLU="[92m" YLW="[93m" GRN="[94m" MAG="[95m" CYN="[96m" WHT="[97m" BLD="[m" OFF="[0m" 

  echo "${BLD}${MAG}ACTION is ${ACTION}${OFF}"
  case "${ACTION}" in
    add|repeat)

      if git rev-parse --show-toplevel 1>/dev/null 2>&1; then
        # shellcheck disable=SC2034
        GIT_WORK_DIR="$(git rev-parse --show-toplevel)"
      else
        GIT_WORK_DIR=""
      fi
      PROJECT_DIR="$(dirname "${SETUP_PATH}")"
      if command -v find 1>/dev/null 2>&1; then
        PROJECT_BIN="$(find "${PROJECT_DIR}" -maxdepth 2 -type "d" -name "bin")"
      else
        PROJECT_BIN="$(Realpath bin)"
      fi
      PROJECT_NAME="$(basename "${PROJECT_DIR}")"
      export ENTRY_DIR GIT_WORK_DIR PROJECT_DIR PROJECT_NAME
      PROJECT_BIN="${PROJECT_DIR}/tools/bin"
      # shellcheck disable=SC1091
      source "${PROJECT_DIR}/tools/scripts/Essential-IO"

      APPS="${HOME}/.local/apps"
      SYSTEMC_HOME="${APPS}/systemc"
      LD_LIBRARY_PATH="${HOME}/.local/apps/systemc/lib"
      DYLD_LIBRARY_PATH="${HOME}/.local/apps/systemc/lib"
      Prepend_path PATH "${PROJECT_BIN}"
      Unique_path PATH
      if [[ -n "${GIT_WORK_DIR}" ]]; then
        Header -uc -Color -hbar=- "${GIT_WORK_DIR/*\/}"
      else
        Header -uc -Color -hbar=- "${ENTRY_DIR/*\/}"
      fi
      Report_info -ylw "${ENTRY_DIR}"
      echo "${BLD}${CYN}PROJECT_BIN=${OFF}'${YLW}${PROJECT_BIN}${OFF}'"
      echo "${BLD}${WHT}$1:${OFF} ${PROJECT_NAME} environment set up"
      ;;
    rm|-rm|--rm)
      unset GIT_WORK_DIR
      PROJECT_NAME="$(basename "${PROJECT_DIR}")"
      Remove_path PATH "${PROJECT_BIN}"
      echo "${BLD}${CYN}$1: ${PROJECT_NAME} environment removed${OFF}"
      ;;
    *)
      ;;
  esac
}

function Check_version()
{
  # @brief return the version of tool
  local version
  if command -v "$1" 1>/dev/null 2>&1; then
    echo -n "$1 "
    version="$1 $(command "$1" --version)"
    # shellcheck disable=SC2312
    perl -e 'printf qq{%s\n},$& if "@ARGV" =~ m{\b[1-9]+([.][0-9]+)+}' "${version}"
  fi
}

function Check_environment()
{
  # @brief test for a few critical bits
  # - Only invoked if -v is passed when sourcing
  export PROJECT_DIR
  cd "${PROJECT_DIR}" 2>/dev/null || return 1
  Reset-errors
  local dir
  for dir in cmake extern; do
    if [[ ! -d "${PROJECT_DIR}/${dir}" ]]; then
      Report_warning "Missing ${dir}/ directory -- suspicious"
    fi
  done
  # What tools are available
  local tool version
  for tool in make ninja cmake ctest; do
    Check_version "${tool}"
  done
}

if [[ ! -r "${HOME}/.inputrc" ]]; then
  Report_warning "Missing ${HOME}/.inputrc"
  cat <<'EOM'
You might want at least:
echo >>$HOME/.inputrc "set editing-mode vi"
EOM
fi
if [[ ! -r "${HOME}/.gdbinit" ]]; then
  Report_warning "Missing ${HOME}/.gdbinit needed for debugging with GDB"
  cat <<'EOM'
You might want:
echo >>$HOME/.gdbinit "add-auto-load-safe-path $(pwd)"
EOM
fi

if [[ "$0" =~ sh$ ]]; then
  alias NOP='printf ""'
fi

# Works in ZSH and BASH
# shellcheck disable=SC2154
if [[ -n "${ZSH_VERSION}" ]]; then
  SETUP_PATH="$(Realpath "$0")"
else
  SETUP_PATH="$(Realpath "${BASH_SOURCE[0]}")"
fi
Project_setup "${SETUP_PATH}" "$@"
if [[ "$2" == "-v" ]]; then
  ( Check_environment )
  Summary setup.profile
fi

# vim:nospell
