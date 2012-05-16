#!/bin/bash
# @author   walh@zhaw.ch
# @date     16-05-2012
# @brief    release build script for sw_stack PRP-1
### ----------------------------------------------------------------------------
set -e

### log info -------------------------------------------------------------------
echo "#########################################################################"
echo "### Build SW_Stack_PRP-1                                              ###"
echo "#########################################################################"

### remove old builds, create new build ----------------------------------------
rm -rf build
mkdir build

### get release number ---------------------------------------------------------
echo "### Build new release"
if head -n 1 changelog.txt | grep '^Release ' ; then
    VERSION=$(head -n 1 changelog.txt | sed -e 's/^Release //' | tr -d '[:space:]')
fi
mkdir build/sw_stack_prp1_v_${VERSION}
echo

### copy documentation and changelog--------------------------------------------
mkdir build/sw_stack_prp1_v_${VERSION}/doc
echo "### Copy documentation"
if [ -f ../doc/PRP-1_stack.pdf ]; then
    cp ../doc/PRP-1_stack.pdf build/sw_stack_prp1_v_${VERSION}/doc/
    echo "PRP-1_stack.pdf"
else
    echo "Docu PRP-1_stack.pdf does not exist"
    exit 1
fi
if [ -f changelog.txt ] ; then
    cp changelog.txt build/sw_stack_prp1_v_${VERSION}/
    echo "changelog.txt"
fi
echo

### SVN version ----------------------------------------------------------------
if [ -d .svn ] ; then
    echo "### Get SVN version"
    SVN_REVISION=$(svnversion ..)
    SVN_URL=$(svn info ..|grep ^URL $hdl|sed -e 's/^URL: //')
    echo "SVN version: $SVN_REVISION"
    echo "Releases in changelog.txt:"
    grep -i ^release changelog.txt | head -n 3
fi
echo

### SVN exports ----------------------------------------------------------------
echo "### Export SVN repository"
echo "-> prp"
svn export --force ../prp/ build/sw_stack_prp1_v_${VERSION}/prp
echo "-> prp_pcap_tap_userspace"
svn export --force ../prp_pcap_tap_userspace build/sw_stack_prp1_v_${VERSION}/prp_pcap_tap_userspace
echo

### SVN logfile ----------------------------------------------------------------
echo "Release version : $VERSION" > build/sw_stack_prp1_v_${VERSION}/release_version.txt
echo "SVN revision    : $SVN_REVISION ($SVN_URL)" >> build/sw_stack_prp1_v_${VERSION}/release_version.txt
echo "Packaging date  : $(LANG=C date) " >> build/sw_stack_prp1_v_${VERSION}/release_version.txt
echo >> build/sw_stack_prp1_v_${VERSION}/release_version.txt

### Create zip file ------------------------------------------------------------
echo "### Build zip file"
cd build
zip --quiet -r ../sw_stack_prp1_v_${VERSION}.zip  *
echo "done..."
echo


exit 0
