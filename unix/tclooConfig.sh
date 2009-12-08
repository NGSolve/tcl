# tclooConfig.sh --
#
# This shell script (for sh) is generated automatically by TclOO's configure
# script, or would be except it has no values that we substitute. It will
# create shell variables for most of the configuration options discovered by
# the configure script. This script is intended to be included by TEA-based
# configure scripts for TclOO extensions so that they don't have to figure
# this all out for themselves.
#
# The information in this file is specific to a single platform.
#
# RCS: @(#) $Id: tclooConfig.sh,v 1.1.2.3 2009/12/08 18:39:20 dgp Exp $

# These are mostly empty because no special steps are ever needed from Tcl 8.6
# onwards; all libraries and include files are just part of Tcl.
TCLOO_LIB_SPEC=""
TCLOO_STUB_LIB_SPEC=""
TCLOO_INCLUDE_SPEC=""
TCLOO_PRIVATE_INCLUDE_SPEC=""
TCLOO_CFLAGS=-DUSE_TCLOO_STUBS
TCLOO_VERSION=0.6.2
