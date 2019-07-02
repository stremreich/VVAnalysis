#!/usr/bin/env bash

# Following implementation by N. Smith, Fermilabl
# https://gitlab.cern.ch/ncsmith/monoZ/tree/master/selector 

tar xvzf ZGen_AnalysisCode.tgz

# TODO: these could be filled in from a template
CMSSW_RELEASE_BASE="/cvmfs/cms.cern.ch/slc7_amd64_gcc700/cms/cmssw-patch/CMSSW_10_4_0_patch1/"

source /cvmfs/cms.cern.ch/cmsset_default.sh
pushd $CMSSW_RELEASE_BASE
eval `scramv1 runtime -sh`
popd
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
export X509_USER_PROXY=userproxy
./Utilities/scripts/makeHistFile.py $@ || exit $?

