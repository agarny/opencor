#!/bin/sh

# Get the script's directory, which should be OpenCOR's base directory

appDir="$(cd "$(dirname "$0")"; pwd)"

# Make sure the path to Python is correct before starting Jupyter

${appDir}/setpythonpath.sh

# Make sure we can find the OpenCOR application to run as a Jupyter kernel

export PATH=${appDir}:$PATH

# Start Jupyter

${appDir}/python/bin/jupyter $*
