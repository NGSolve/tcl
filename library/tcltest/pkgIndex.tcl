# Tcl package index file, version 1.1
# This file is generated by the "pkg_mkIndex" command
# and sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

package ifneeded tcltest 1.0 [list tclPkgSetup $dir tcltest 1.0 \
	{{tcltest.tcl source {::tcltest::bytestring ::tcltest::cleanupTests \
	::tcltest::dotests ::tcltest::makeDirectory ::tcltest::makeFile \
	::tcltest::normalizeMsg ::tcltest::removeDirectory \
	::tcltest::removeFile ::tcltest::restoreState ::tcltest::saveState \
	::tcltest::test ::tcltest::threadReap ::tcltest::viewFile memory \
	::tcltest:grep ::tcltest::getMatchingTestFiles }}}]

