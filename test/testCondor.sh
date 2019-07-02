#!/bin/bash

pushd $CMSSW_BASE/src/Analysis/VVAnalysis
if [ ! -d lib ]; then
    mkdir lib
fi

pushd lib
for i in $CMSSW_BASE/lib/${SCRAM_ARCH}/*VVAnalysis*; do
    cp $i `basename $i`
done
popd

manager_name="AnalysisDatasetManager"
adm_path=$(./Utilities/scripts/getConfigValue.py dataset_manager_path)/${manager_name}
analysis="ZGen"
mkdir -p ${manager_name}/FileInfo
mkdir -p ${manager_name}/PlotObjects

pushd ${manager_name}
cp -r ${adm_path}/Utilities .
pushd PlotObjects
cp -r ${adm_path}/PlotObjects/${analysis}* .
popd

pushd FileInfo
cp -r ${adm_path}/FileInfo/${analysis} ${analysis} 
cp -r ${adm_path}/FileInfo/data .
cp -r ${adm_path}/FileInfo/montecarlo .
popd
popd

filelist="test/filelists/DYNano_LOandNLO.txt"
filelist_name=$(basename $filelist) 
cp $filelist $filelist_name

tar chzf ${analysis}_AnalysisCode.tgz Utilities data lib $manager_name $filelist_name $wrapper_name

rm -r lib $manager_name $filelist_name 
