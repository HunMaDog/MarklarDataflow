#!/bin/bash

# Stop when error
set -e

# ###########################
# Setup
# ###########################

# Name of the 3rd party project
BUILDSCRIPT_PROJECT_NAME=WigWag
BUILDSCRIPT_PROJECT_SUBDIR=wigwag

# Absolute path to this script
BUILDSCRIPT_SCRIPT=$(readlink -f "$0")
# Absolute path this script is in
BUILDSCRIPT_SCRIPT_PATH=$(dirname "${BUILDSCRIPT_SCRIPT}")

# build type is default RelWithDebInfo
BUILDSCRIPT_BUILD_TYPE="RelWithDebInfo"
if [ "$1" != "" ]; then
    BUILDSCRIPT_BUILD_TYPE="$1"
fi

# Main build directory
BUILDSCRIPT_BUILD_PATH=${BUILDSCRIPT_SCRIPT_PATH}/build/${BUILDSCRIPT_PROJECT_SUBDIR}/${BUILDSCRIPT_BUILD_TYPE}/
BUILDSCRIPT_PROJECT_PATH=${BUILDSCRIPT_SCRIPT_PATH}/${BUILDSCRIPT_PROJECT_SUBDIR}

# Number of core
BUILDSCRIPT_NUM_OF_CORE="$(nproc --all)"
echo "Number of cores: ${BUILDSCRIPT_NUM_OF_CORE}"

# ###########################
# Preparing
# ###########################

# Remove old files
echo "Removing old files"
rm -rf  ${BUILDSCRIPT_BUILD_PATH}

# create build dir
echo "Create build directory"
mkdir -p ${BUILDSCRIPT_BUILD_PATH}

# git things
echo "Init and update submodules"
git submodule init
git submodule update

# ###########################
# Build
# ###########################

echo "${BUILDSCRIPT_PROJECT_NAME} ${BUILDSCRIPT_BUILD_TYPE} building..."

# building
cd ${BUILDSCRIPT_BUILD_PATH}

cmake ${BUILDSCRIPT_PROJECT_PATH} -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=${BUILDSCRIPT_BUILD_TYPE}

make -j${BUILDSCRIPT_NUM_OF_CORE} all
