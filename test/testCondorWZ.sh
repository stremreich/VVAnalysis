#!/bin/bash

make_2016_mclist() {
    make_file_list "WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8" "wz3lnu_powheg" 1
    make_file_list "WLLJJ_WToLNu_EWK_TuneCUETP8M1_13TeV_madgraph-madspin-pythia8" "wzjj_ewk" 1
    make_file_list "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" "DYm50__LO" 1
    make_file_list "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8" "zg" 1
    make_file_list "TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8" "tt_lep" 1
    make_file_list "TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8" "ttz" 1
    make_file_list "ZZTo4L_13TeV_powheg_pythia8" "zz4l_powheg" 1
}


make_2016_datalist() {
    # No reason data and MC can't go together but it's easier for now to do one or the other
    make_file_list "SingleMuon/Run2016B_ver2-Nano1June2019_ver2-v1" "data_SingleMuon_Run2016B_ver2-Nano1June2019_ver2-v1" 0
    make_file_list "SingleMuon/Run2016C-Nano1June2019-v1" "data_SingleMuon_Run2016C-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2016D-Nano1June2019-v1" "data_SingleMuon_Run2016D-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2016E-Nano1June2019-v1" "data_SingleMuon_Run2016E-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2016F-Nano1June2019-v1" "data_SingleMuon_Run2016F-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2016G-Nano1June2019-v1" "data_SingleMuon_Run2016G-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2016H-Nano1June2019-v1" "data_SingleMuon_Run2016H-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016B_ver2-Nano1June2019_ver2-v1" "data_SingleElectron_Run2016B_ver2-Nano1June2019_ver2-v1" 0
    make_file_list "SingleElectron/Run2016C-Nano1June2019-v1" "data_SingleElectron_Run2016C-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016D-Nano1June2019-v1" "data_SingleElectron_Run2016D-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016E-Nano1June2019-v1" "data_SingleElectron_Run2016E-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016F-Nano1June2019-v1" "data_SingleElectron_Run2016F-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016G-Nano1June2019-v1" "data_SingleElectron_Run2016G-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2016H-Nano1June2019-v1" "data_SingleElectron_Run2016H-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016B_ver2-Nano1June2019_ver2-v1" "data_DoubleMuon_Run2016B_ver2-Nano1June2019_ver2-v1" 0
    #make_file_list "DoubleMuon/Run2016C-Nano1June2019-v1" "data_DoubleMuon_Run2016C-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016D-Nano1June2019-v1" "data_DoubleMuon_Run2016D-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016E-Nano1June2019-v1" "data_DoubleMuon_Run2016E-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016F-Nano1June2019-v1" "data_DoubleMuon_Run2016F-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016G-Nano1June2019-v1" "data_DoubleMuon_Run2016G-Nano1June2019-v1" 0
    #make_file_list "DoubleMuon/Run2016H-Nano1June2019-v1" "data_DoubleMuon_Run2016H-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016B_ver2-Nano1June2019_ver2-v1" "data_DoubleEG_Run2016B_ver2-Nano1June2019_ver2-v1" 0
    #make_file_list "DoubleEG/Run2016C-Nano1June2019-v1" "data_DoubleEG_Run2016C-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016D-Nano1June2019-v1" "data_DoubleEG_Run2016D-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016E-Nano1June2019-v1" "data_DoubleEG_Run2016E-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016F-Nano1June2019-v1" "data_DoubleEG_Run2016F-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016G-Nano1June2019-v1" "data_DoubleEG_Run2016G-Nano1June2019-v1" 0
    #make_file_list "DoubleEG/Run2016H-Nano1June2019-v1" "data_DoubleEG_Run2016H-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016B_ver2-Nano1June2019_ver2-v1" "data_MuonEG_Run2016B_ver2-Nano1June2019_ver2-v1" 0
    #make_file_list "MuonEG/Run2016C-Nano1June2019-v1" "data_MuonEG_Run2016C-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016D-Nano1June2019-v1" "data_MuonEG_Run2016D-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016E-Nano1June2019-v1" "data_MuonEG_Run2016E-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016F-Nano1June2019-v1" "data_MuonEG_Run2016F-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016G-Nano1June2019-v1" "data_MuonEG_Run2016G-Nano1June2019-v1" 0
    #make_file_list "MuonEG/Run2016H-Nano1June2019-v1" "data_MuonEG_Run2016H-Nano1June2019-v1" 0
}
 
make_2017_mclist() {
    make_file_list "WZTo3LNu_13TeV-powheg-pythia8/RunIIFall17NanoAODv4-PU2017_12Apr2018_Nano14Dec2018_102X_mc2017_realistic_v6-v1" "wz3lnu_powheg" 1
    make_file_list "WZTo3LNu_0Jets_MLL-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "wz3lnu_mgmlm_0j" 1 
    make_file_list "WZTo3LNu_1Jets_MLL-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "wz3lnu_mgmlm_1j" 1 
    make_file_list "WZTo3LNu_2Jets_MLL-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "wz3lnu_mgmlm_2j" 1 
    make_file_list "WZTo3LNu_3Jets_MLL-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "wz3lnu_mgmlm_3j" 1 
    make_file_list "WLLJJ_WToLNu_EWK_TuneCP5_13TeV_madgraph-madspin-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "wzjj_ewk" 1
    make_file_list "DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17NanoAODv4-PU2017RECOSIMstep_12Apr2018_Nano14Dec2018_102X_mc2017_realistic_v6_ext1-v1" "DYm50__LO" 1
    make_file_list "ZGToLLG_01J_5f_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_102X_mc2017_realistic_v7-v1" "zg" 1
    make_file_list "TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_new_pmx_102X_mc2017_realistic_v7-v1" "tt_lep" 1
    make_file_list "TTZToLLNuNu_M-10_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17NanoAODv4-PU2017_12Apr2018_Nano14Dec2018_102X_mc2017_realistic_v6-v1" "ttz" 1
    make_file_list "ZZTo4L_13TeV_powheg_pythia8/RunIIFall17NanoAODv5-PU2017_12Apr2018_Nano1June2019_new_pmx_102X_mc2017_realistic_v7-v1" "zz4l_powheg" 1
}

make_2017_datalist() {
    # No reason data and MC can't go together but it's easier for now to do one or the other
    make_file_list "SingleMuon/Run2017B-Nano1June2019-v1" "data_SingleMuon_Run2017B-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2017C-Nano1June2019-v1" "data_SingleMuon_Run2017C-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2017D-Nano1June2019-v1" "data_SingleMuon_Run2017D-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2017E-Nano1June2019-v1" "data_SingleMuon_Run2017E-Nano1June2019-v1" 0
    make_file_list "SingleMuon/Run2017F-Nano1June2019-v1" "data_SingleMuon_Run2017F-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2017B-Nano1June2019-v1" "data_SingleElectron_Run2017B-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2017C-Nano1June2019-v1" "data_SingleElectron_Run2017C-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2017D-Nano1June2019-v1" "data_SingleElectron_Run2017D-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2017E-Nano1June2019-v1" "data_SingleElectron_Run2017E-Nano1June2019-v1" 0
    make_file_list "SingleElectron/Run2017F-Nano1June2019-v1" "data_SingleElectron_Run2017F-Nano1June2019-v1" 0
}

# TODO: Make a general script (almost certainly in python)
copy_lib_files() {
    if [ ! -d lib ]; then
        mkdir lib
    fi

    pushd lib
    for i in $CMSSW_BASE/lib/${SCRAM_ARCH}/*VVAnalysis*; do
        cp $i `basename $i`
    done
    popd
}

copy_manager_files() {
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
}

make_file_list() {
    das_path=$1
    name=$2
    isMC=$3
    filename=temp_${name}.out
    if [ $isMC -eq 1 ]; then
        dasgoclient --query="file dataset=/${das_path}/NANOAODSIM" > $filename
    else
        dasgoclient --query="file dataset=/${das_path}/NANOAOD" > $filename
    fi
    sed -i "s/^/${name}@/" $filename
}

pushd $CMSSW_BASE/src/Analysis/VVAnalysis

base_dir="/data/${USER}"
if [[ "$HOSTNAME" == *"hep.wisc.edu"* ]]; then
    base_dir="/nfs_scratch/${USER}"
fi

if [[ "$1" == "2016MC" ]]; then
    make_2016_mclist
    filelist_name="WZ2016MC.txt"
    condor_dir=${base_dir}/WZAnalysis_CondorTest2016MC
elif [[ "$1" == "2016Data" ]]; then
    make_2016_datalist
    filelist_name="WZ2016Data.txt"
    condor_dir=${base_dir}/WZAnalysis_CondorTest2016Data
elif [[ "$1" == "2017MC" ]]; then
    make_2017_mclist
    filelist_name="WZ2017MC.txt"
    condor_dir=${base_dir}/WZAnalysis_CondorTest2017MC
elif [[ "$1" == "2017Data" ]]; then
    make_2017_datalist
    filelist_name="WZ2017Data.txt"
    condor_dir=${base_dir}/WZAnalysis_CondorTest2017Data
else
    echo "Invalid input parameter" $1
    echo "Must be 2016/2017/2018 data or MC"
    exit 1
fi

cat temp*.out >> temp_all.out
# Hardcode 3 per file
numfiles=$(wc -l < temp_all.out)
# Ceiling function
let numjobs=($numfiles+3-1)/3
echo "Number of jobs is" $numjobs

# Randomize so jobs tend to take a similar amount of time
sort -R temp_all.out > $condor_dir/$filelist_name
rm temp*.out

copy_lib_files
copy_manager_files

if [ ! -d $condor_dir/logs ]; then
    mkdir -p $condor_dir/logs
fi

# TODO: Add check to see if proxy is valid
cp /tmp/x509up_u$UID userproxy

# TODO: These should actually be template files that get filled
tar chzf ${condor_dir}/${analysis}_AnalysisCode.tgz Utilities data lib userproxy $manager_name $wrapper_name
sed "s/FILELIST_REPLACE/${filelist_name%.*}/" Templates/CondorSubmit/submit_testWZ.jdl > $condor_dir/submit_testWZ.jdl 
sed -i "s/NUMJOBS_REPLACE/${numjobs}/" $condor_dir/submit_testWZ.jdl 
cp Templates/CondorSubmit/wrapRunSelector.sh $condor_dir

rm -r lib $manager_name userproxy
