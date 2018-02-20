#!/bin/bash

# Stop when error
set -e

# ###########################
# Setup
# ###########################

# build type is default RelWithDebInfo
RUNNERSCRIPT_BUILDLIST_FILE=""
if [ "$1" != "" ]; then
   RUNNERSCRIPT_BUILDLIST_FILE="$1"
else
   echo "Need to set not empty build list file!"

   exit;
fi

# build type is default RelWithDebInfo
RUNNERSCRIPT_BUILD_TYPE="RelWithDebInfo"
if [ "$2" != "" ]; then
   RUNNERSCRIPT_BUILD_TYPE="$2"
fi

RUNNERSCRIPT_FILENAME_PREFIX="build.linux."
RUNNERSCRIPT_NAMING_CONVENTION=${RUNNERSCRIPT_FILENAME_PREFIX}"*.sh"

# ###########################
# Build list
# ###########################

IFS=$'\r\n' RUNNERSCRIPT_GLOBBUILD='*' command eval 'RUNNERSCRIPT_BUILDLIST=($(cat ./${RUNNERSCRIPT_BUILDLIST_FILE}))'

if [ ${RUNNERSCRIPT_BUILDLIST} = "" ]; then
   echo "Build list file is empty!"

   exit;
fi

# ###########################
# Build 3Rd Party stuff
# ###########################

for AN_3RDPARTY_ENTRY in ./${RUNNERSCRIPT_NAMING_CONVENTION}
do
  if [ -f ${AN_3RDPARTY_ENTRY} ]; then
    if [[ ${AN_3RDPARTY_ENTRY} =~ "./"([^.]+).([^.]+).([^.]+)".sh" ]]; then 
      if [ ${BASH_REMATCH[3]} != "3rdparty" ]; then
        if [[ "${RUNNERSCRIPT_BUILDLIST[@]}" =~ "${BASH_REMATCH[3]}" ]]; then
          echo "Run ${AN_3RDPARTY_ENTRY}"

          ${AN_3RDPARTY_ENTRY} ${RUNNERSCRIPT_BUILD_TYPE}
        else
          echo "Skip ${AN_3RDPARTY_ENTRY}"
        fi
      fi
    fi
  fi
done
