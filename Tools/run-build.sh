#!/bin/bash

SCRIPTNAME="run-build.sh"
RECONFIG=FALSE

function HelpMsg()
{
  echo "Usage: $SCRIPTNAME [Options]"
  echo
  echo "Configure EDK2 build environment, then kicks off build for Nintendo Switch."
  echo
  echo
  echo "Options: "
  echo "  --help, -h, -?        Print this help screen and exit."
  echo
}

function SetWorkspace()
{
  #
  # If WORKSPACE is already set, then we can return right now
  #
  if [ -n "$WORKSPACE" ]
  then
    return 0
  fi

  #
  # Check for BaseTools/BuildEnv before dirtying the user's environment.
  #
  if [ ! -f BaseTools/BuildEnv ] && [ -z "$EDK_TOOLS_PATH" ]
  then
    echo BaseTools not found in your tree, and EDK_TOOLS_PATH is not set.
    echo Please point EDK_TOOLS_PATH at the directory that contains
    echo the EDK2 BuildEnv script.
    return 1
  fi

  #
  # Set $WORKSPACE
  #
  export WORKSPACE=`pwd`

  return 0
}

function SetupEnv()
{
  if [ -n "$EDK_TOOLS_PATH" ]
  then
    . $EDK_TOOLS_PATH/BuildEnv
  elif [ -f "$WORKSPACE/BaseTools/BuildEnv" ]
  then
    . $WORKSPACE/BaseTools/BuildEnv
  elif [ -n "$PACKAGES_PATH" ]
  then 
    PATH_LIST=$PACKAGES_PATH
    PATH_LIST=${PATH_LIST//:/ }
    for DIR in $PATH_LIST
    do
      if [ -f "$DIR/BaseTools/BuildEnv" ]
      then
        export EDK_TOOLS_PATH=$DIR/BaseTools
        . $DIR/BaseTools/BuildEnv
        break
      fi
    done
  else
    echo BaseTools not found in your tree, and EDK_TOOLS_PATH is not set.
    echo Please check that WORKSPACE or PACKAGES_PATH is not set incorrectly
    echo in your shell, or point EDK_TOOLS_PATH at the directory that contains
    echo the EDK2 BuildEnv script.
    return 1
  fi
}

function FixPermission()
{
  chmod +x NintendoSwitchPkg/Tools/*.ps1
  chmod +x NintendoSwitchPkg/Tools/PsModules/*.psm1
  chmod +x NintendoSwitchPkg/Tools/*.sh
}

function SourceEnv()
{
  SetWorkspace &&
  SetupEnv &&
  FixPermission
}

function DevelopmentBuild()
{
  ./NintendoSwitchPkg/Tools/edk2-build.ps1

  if [ ! $? -eq 0 ]; then
      echo "[Builder] Build failed."
      return $?
  fi
}

I=$#
while [ $I -gt 0 ]
do
  case "$1" in
    -?|-h|--help|*)
      HelpMsg
      break
    ;;
  esac
  I=$(($I - 1))
done

if [ $I -gt 0 ]
then
  return 1
else
  echo "[Builder] Configure environment."
  SourceEnv
  if [ ! $? -eq 0 ]; then
      echo "Unable to configure EDK2 environment."
      return $?
  fi

  # Run build
  DevelopmentBuild

  if [ ! $? -eq 0 ]; then
      echo "[Builder] Build failed."
      return $?
  fi
fi
