Analysis code for WZ/ZZ analyses. Some scripts using selections to skim Ntuples and C++ code to make histogram files for WZ.

+ [Setup](#setup)
+ [Overview](#overview)
+ [Specifics](#overview)
    + [Producing ntuples](#producing-ntuples)
    + [Skimming ntuples](#producing-ntuple-skims)
    + [Producing processed histogram file](#running-analysis-code)
        + [WZ selector](#the-wz-selector)
        + [WZ Background estimation](#nonprompt-background-estimate-for-wz)
        + [Writing your own selector](#implementing-your-own-selector)
    + [Statistical analysis](#running-statistical-analysis)
    + [Producing plots](#plotting)
    

# Setup
-----------
<<<<<<< HEAD
CMSSW version: CMSSW_9_4_8
X_Y_Z = 9_4_8
username=stremreich (or your username)
=======
CMSSW version: CMSSW_8_0_30 # or CMSSW_9_4_8
```bash
X_Y_Z="8_0_30" # or 9_4_8, match with the CMSSW of UWVV ideally
username="kdlong" # or your username
```

To checkout and compile:
>>>>>>> 21a252ce9b6bf5113282f33bd409605e872ebb65

```console
cmsrel CMSSW_version
cd CMSSW_X_Y_Z/src
mkdir Analysis
cd Analysis
<<<<<<< HEAD
git clone https://github.com/stremreich/VVAnalysis.git 
=======
git clone git@github.com:<username>/VVAnalysis.git
>>>>>>> 21a252ce9b6bf5113282f33bd409605e872ebb65
scram b -j 8
```

You will also need to install a separate package that contains information on the datasets used and histograms and binning. It's recommended that you fork this into your github because some files/settings will be user specific.

install_path = ~username/work/ (or whatever you prefer)

```console
cd install_path
git clone git@github.com:<username>/AnalysisDatasetManager.git
```

You should edit the configuration file found [here](../master/Templates/config) to have settings specific to you. 

# Overview
-----------
This repository includes scripts to run all steps (starting with [UWVV ntuples](https://github.com/uwcms/UWVV])) to produce the results of [SMP-18-001](http://cms.cern.ch/iCMS/analysisadmin/cadilines?line=SMP-18-001). Many steps should be trivially modifiable for other analyses. 

The analysis processeeds in several steps.

1. Produce ntuples using [UWVV](https://github.com/uwcms/UWVV]). See this package for further documentation. In principle another package could be used without much trouble, but variable names etc would need to be modified. Output is generally stored on a distributed storage system, e.g. /hdfs at UW or /eos at CERN.
2. Skim ntuples to create smaller files that can be copied to a "local" storage disk, such as /data at uwlogin or /nfs_scratch at Wisconsin.
3. Run analysis code to estimate backgrounds and produce histograms with final selections.
4. Plotting code combines histograms by scaling with cross section and luminosity information. Colors and CMS style are implemented.

# Specifics
-----------
Each step deserves some degree of explanation. They are also all driven by independent scripts, and can be run separately. The ntuple skimming is generally run independently. It is not required, but is advantageous to reduce files sizes for convenience in storage and processing. It is absolutely necessary that the tightest condition you use in your skim be looser the selection you implement in a later selector, however. For a fully MC driven analysis, one can often implement the full selection at the skim step and produce plots from here. Note, however, that the statistical tools are designed to run over the output of the histogram files produced by the selector.

## Producing Ntuples

See the documentation in [UWVV](https://github.com/uwcms/UWVV]). It is possible to use other ntuples, but you will need to make extensive changes to variable names in the skimming step and branch names in the analysis code.

## Producing Ntuple Skims

This code is based on ["cut strings"](https://root.cern.ch/doc/v608/classTCut.html) applied to ROOT TTrees. It is driven by the script [skimNtuples.py](skimNtuples.py). This script takes as argument the analysis name and a list of selections to be applied. These selections are defined in the [Cuts folder](Cuts) of this repository. To implement a new analysis, one should create a new folder in this repository. To add a selection, a new selection.json file should be added. Follow the example of e.g. [3LooseLeptonsNoVeto.json](Cuts/WZxsec2016/3LooseLeptonsNoVeto.json). Conditions can be object specific (e.g. pt for each muon or elector) or state specific.

Run ```./skimNtuples.py --help``` for a full list of arguments to this script.

Generally you will want to farm this step out to condor, as it's real utility is to take files on hdfs, skim them, and produce output which can be copied locally. The script [farmoutNtupleSkim.py](farmoutNtupleSkim.py) is intended for this. It reads information about the input datasets from [AnalysisDatasetManager](https://github.com/kdlong/AnalysisDatasetManager) and configures jobs to be submitted to condor using ```farmoutAnalysisJobs.sh```, a script which is UW condor specific. Modifications would be required for use with other resources.

Run ```./farmoutNtupleSkim.py --help``` for a full list of arguments to this script.

An example to produce the output for the WZ inclusive analysis, with loosely IDed leptons (necessary for the fake rate step) would be

```./farmoutNtupleSkim.py -f data* -s 3MediumLeptonsNoVeto```

This will submit jobs for each file with a name matching the pattern "data*", defined in AnalysisDatasetManager, creating a skim of events passing the [3LooseLeptonsNoVeto.json](Cuts/WZxsec2016/3LooseLeptonsNoVeto.json) selection. The script creates submit folders for each dataset, by default in the location ```/<storage>/<username>/YYYY-MM-DD_VVAnalysisJobs```, where <storage> is either /nfs_scratch or /data. It will produce output files copied to ```/store/user/<username>/VVAnalysisJobs_YYYY-MM-DD```. If you want to copy these locally, you can use the script [copyFromHdfs.py](https://github.com/kdlong/AnalysisDatasetManager/blob/master/copyFromHdfs.py).
  
```./copyFromHdfs.py /hdfs/store/user/<username>/...```

### Running analysis code

### The WZ selector

### Nonprompt background estimate for WZ

### Implementing your own selector

## Running Statistical Analysis 

## Plotting

