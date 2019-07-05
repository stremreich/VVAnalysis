#!/bin/bash

# TODO: Make a general script (almost certainly in python)

make_file_list() {
    das_path=$1
    name=$2
    filename=temp_${name}.out
    dasgoclient --query="file dataset=/${das_path}/RunIISummer16NanoAODv4-PUMoriond17_Nano14Dec2018_102X_mcRun2_asymptotic_v6*/NANOAODSIM" > $filename
    sed -i "s/^/${name}@/" $filename
}


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
analysis="WZxsec2016"
mkdir -p ${manager_name}/FileInfo
mkdir -p ${manager_name}/PlotObjects

pushd ${manager_name}
cp -r ${adm_path}/Utilities .
pushd PlotObjects
cp -r ${adm_path}/PlotObjects/${analysis}* .
popd

#Need to send the relavent files to the condor machine (could also count on afs)
pushd FileInfo
cp -r ${adm_path}/FileInfo/${analysis} ${analysis} 
cp -r ${adm_path}/FileInfo/data .
cp -r ${adm_path}/FileInfo/montecarlo .
popd
popd

# TODO: The DAS paths should be stored in ADM, then this can be easily automated
make_file_list "WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8" "wz3lnu_powheg"
make_file_list "WLLJJ_WToLNu_EWK_TuneCUETP8M1_13TeV_madgraph-madspin-pythia8" "wzjj_ewk"
make_file_list "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" "DYm50__LO"
make_file_list "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" "zg"
make_file_list "TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8" "tt_lep"
make_file_list "TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8" "ttz"
make_file_list "ZZTo4L_13TeV_powheg_pythia8" "zz"

cat temp*.out >> temp_all.out

filelist_name="WZ2016MC.txt"
# Randomize so jobs tend to take a similar amount of time
sort -R temp_all.out > $filelist_name
rm temp*.out

# TODO: Add check to see if proxy is valid
cp /tmp/x509up_u$UID userproxy

# TODO: These should actually be template files that get filled
condor_dir=/nfs_scratch/kdlong/WZAnalysis_CondorTest
tar chzf ${condor_dir}/${analysis}_AnalysisCode.tgz Utilities data lib userproxy $manager_name $filelist_name $wrapper_name
cp Templates/CondorSubmit/submit_testWZ.jdl $condor_dir
cp Templates/CondorSubmit/wrapRunSelector.sh $condor_dir

rm -r lib $manager_name $filelist_name userproxy
