Analysis code for WZ/ZZ analyses. Some scripts using selections to skim Ntuples and C++ code to make histogram files for WZ.

+ [Setup](#setup)
+ [Overview](#overview)
+ [Specifics](#overview)
    + [NanoAOD](#nanoaod)
        + [Skimming NanoAOD](#skimming-nanoaod)
     + [UWVV nutples](#producing-uwvv-ntuples)
        + [Skimming UWVV ntuples](#skimming-uwvv-ntuples)
    + [Producing processed histogram file](#running-analysis-code)
        + [Z selector example](#the-z-selector-example)
        + [Writing your own selector](#implementing-your-own-selector)
        + [WZ selector](#the-wz-selector)
        + [WZ Background estimation](#nonprompt-background-estimate-for-wz)
    + [Statistical analysis](#running-statistical-analysis)
    + [Producing plots](#plotting)
    

# Setup
-----------
CMSSW version: CMSSW_X_Y_Z

```bash
X_Y_Z="10_6_0" # or 9_2_14
username="stremreich" # or your username
```

To checkout and compile:
git clone https://github.com/stremreich/VVAnalysis.git 
scram b -j 8
```

You will also want to install a separate package that contains information on the datasets used and histograms and binning. It is not required that you use this file, but it will be convenient for managing larger datasets (e.g., for getting the correct cross sections). It's recommended that you fork this into your github because some files/settings will be user specific.

install_path = ~username/work/ (or whatever you prefer)

```console
cd install_path
git clone git@github.com:<username>/AnalysisDatasetManager.git
```

You should create a new configuration file following the example [here](../master/Templates/config.kdlong) to have settings specific to you. Use the name config.<username>.

# Overview
-----------
This repository includes scripts to run all steps (starting with [UWVV ntuples](https://github.com/uwcms/UWVV])) to produce the results of [SMP-18-001](http://cms.cern.ch/iCMS/analysisadmin/cadilines?line=SMP-18-001). Many steps should be trivially modifiable for other analyses.

Current developement is in progress to also allow the use of NanoAOD in place of the UWVV ntuples. NanoAOD is a centrally-maintained ntuple which can signficantly reduce manpower needed to maintain private ntuples. More detail is given in the [NanoAOD](#nanoaod) section.

The analysis processeeds in several steps.

1. Produce ntuples using [UWVV](https://github.com/uwcms/UWVV]). See this package for further documentation. NanoAOD is also supported for some processes. In principle other packages could be used without much trouble, but variable names etc would need to be modified. Output is generally stored on a distributed storage system, e.g. /hdfs at UW or /eos at CERN.
2. Skim ntuples or NanoAOD to create smaller files that can be copied to a "local" storage disk, such as /data at uwlogin or /nfs_scratch at Wisconsin. 
3. Run analysis code to estimate backgrounds and produce histograms with final selections.
4. Plotting code combines histograms by scaling with cross section and luminosity information. Colors and CMS style are implemented.

# Specifics
-----------
Each step deserves some degree of explanation. They are also all driven by independent scripts, and can be run separately. The ntuple skimming is generally run independently. It is not required, but is advantageous to reduce files sizes for convenience in storage and processing. It is absolutely necessary that the tightest condition you use in your skim be looser the selection you implement in a later selector, however. For a fully MC driven analysis, one can often implement the full selection at the skim step and produce plots from here. Note, however, that the statistical tools are designed to run over the output of the histogram files produced by the selector.

## NanoAOD

https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookNanoAOD

### Skimming NanoAOD

https://github.com/cms-nanoAOD/nanoAOD-tools

Dilepton and 3 lepton specific skimming tools:

https://github.com/kdlong/NanoVVSkims

## Producing UWVV Ntuples

See the documentation in [UWVV](https://github.com/uwcms/UWVV]). It is possible to use other ntuples, but you will need to make extensive changes to variable names in the skimming step and branch names in the analysis code.

## Skimming UWVV Ntuples

This code is based on ["cut strings"](https://root.cern.ch/doc/v608/classTCut.html) applied to ROOT TTrees. It is driven by the script [skimNtuples.py](skimNtuples.py). This script takes as argument the analysis name and a list of selections to be applied. These selections are defined in the [Cuts folder](Cuts) of this repository. To implement a new analysis, one should create a new folder in this repository. To add a selection, a new selection.json file should be added. Follow the example of e.g. [3LooseLeptonsNoVeto.json](Cuts/WZxsec2016/3LooseLeptonsNoVeto.json). Conditions can be object specific (e.g. pt for each muon or elector) or state specific.

Run ```./skimNtuples.py --help``` for a full list of arguments to this script.

Generally you will want to farm this step out to condor, as it's real utility is to take files on hdfs, skim them, and produce output which can be copied locally. The script [farmoutNtupleSkim.py](farmoutNtupleSkim.py) is intended for this. It reads information about the input datasets from [AnalysisDatasetManager](https://github.com/kdlong/AnalysisDatasetManager) and configures jobs to be submitted to condor using ```farmoutAnalysisJobs.sh```, a script which is UW condor specific. Modifications would be required for use with other resources.

Run ```./farmoutNtupleSkim.py --help``` for a full list of arguments to this script.

An example to produce the output for the WZ inclusive analysis, with loosely IDed leptons (necessary for the fake rate step) would be

```./farmoutNtupleSkim.py -f data* -s 3MediumLeptonsNoVeto
```

This will submit jobs for each file with a name matching the pattern "data*", defined in AnalysisDatasetManager, creating a skim of events passing the [3LooseLeptonsNoVeto.json](Cuts/WZxsec2016/3LooseLeptonsNoVeto.json) selection. The script creates submit folders for each dataset, by default in the location ```/<storage>/<username>/YYYY-MM-DD_VVAnalysisJobs```, where <storage> is either /nfs_scratch or /data. It will produce output files copied to ```/store/user/<username>/VVAnalysisJobs_YYYY-MM-DD```. If you want to copy these locally, you can use the script [copyFromHdfs.py](https://github.com/kdlong/AnalysisDatasetManager/blob/master/copyFromHdfs.py).
  
``` ./copyFromHdfs.py /hdfs/store/user/<username>/...
```


## Running analysis code

### The Z selector example

See [this commit](https://github.com/kdlong/VVAnalysis/commit/18a1d903e149653fff3985b43f1acc834632a7ac) for an example of how to add histograms to a selector using the configuration setup. [These corresponding changes](https://github.com/kdlong/AnalysisDatasetManager/commit/39909f6e76046b6ab39b293fa3ea209d1cd202a8) to the [AnalysisDatasetManager repository](https://github.com/kdlong/AnalysisDatasetManager) are necessary.

The main program that runs the analysis is the file ```./Utilities/scripts/makeHistFile.py``` . This file is a wrapper for the running the selector defined in your src directory over the files specified in the AnalysisDatasetManager. This code works with a conviention used in most of this code suite, that is a selection, analysis, and input tier are needed for most all functions to run. for completeness, each option for the code will be explained and the user can know how to use the software:

* **-s**: Selection. User defined selection to be implimented in Analyzer. Need to put in flags in cc files for make any difference (ie PlotObjects area)
* **-v**: Version. Pretty self explanitory
* **-a**: Analysis. Name used to determine with cc/TSelector will fun over your files. Map defined in ```./Utilities/python/SelectorTools.py``` (Name from AnalysisDatasetManager)
* **-f**: Filenames. The name of the files to be run over, in quotes seperated by commas. The filenames are those specified in the AnalysisDatasetManager, specifically with the FileInfo folder.
* **-j**: Number of Cores. Same as with Make, number of cores used to run the jobs
* **-c**: Channels: If you want to only run over a certain number of channels, put those channels in quotes seperated by commas. Default to Inclusive
* **-o**: Output File: Name of the outfile. One one is made, all the samples are put into folders after the filename
* **-b**: Hist Names: If you only want specific histograms, put names in quotes seperated by commas
* **--input_tier**: Name corresponding to the skim used on the files (ie FileInfo area)
* **--lumi**: Self Explanitory
* **--test**: Confusing naming (without context at least), but basically doesn't do Data driven backgrounds
* **--uwvv**: Legacy Ntuple format
* **--noHistConfig**: Ignore Histogram information from ADM. Generally unsafe, would ignore

So, if you want to run a basic analysis, you could write:

``` sh
./Utilities/scripts/makeHistFile.py -a Zstudy_2016 
	                                --input_tier NanoDileptonSkims 
									-s TightWithLooseVeto
									-f "dy, data_2016"
									--lumi 35.9
									--test
									-o output_zstudy.root
```
So this corresponds to running the ZSelector over the events in the dy and data_2016 files that were skimmed with NanoDileptonSkims and analyzed to TightWithLooseVeto. The Luminosity is 35.9 and we are ignoring data driven backgrounds (--test)

### Implementing your own selector

### The WZ selector

### Nonprompt background estimate for WZ

## Running Statistical Analysis 

## Plotting

## Addendum: AnalysisDatasetManager

The [AnalysisDatasetManager](https://github.com/kdlong/AnalysisDatasetManager) is a repo used for handing generic histogram and file information across softwares. Because of it's importance, it's worth explaining (may move this to the actual repo, but here for now). It is worth it to fork your own version of this repo.

This section will divided up into explaining each of the important folders in this repo and how they relate to the code

### FileInfo

The FileInfo folder contains two main parts. The first is a folder named after the Anaysis in question. As per the Z selector example above, there is a Zstudy_2016 folder for that specific study. Inside each analysis folder is a python file (all the data is stored in json-esque data format, but in python file for convience) with names corresponding to the input-tier/skim name. If our input files were skimmed by the "NanoDileptonSkim" skim for our Z study, we'd expect to see a ```./AnalysisDatasetManager/FileInfo/Zstudy_2016/NanoDileptonSkim.py ``` file that contains the information of the files. For an example of what the file should look like, [look here](https://github.com/kdlong/AnalysisDatasetManager/blob/master/FileInfo/Zstudy_2016/NanoDileptonSkim.py).

The other use is to cold monte carlo information that is used for plotting. It also allows you to put extra information about the MC's name, generator, etc so that has easy reference for later. It also allows you to put in k-factors so that doesn't have to be done by hand. All of this is locate in ```./AnalysisDatasetManager/FileInfo/montecarlo/```.

### PlotGroups

The PlotGroups gives a place to show how different MC samples will be added together in the plot. So for Zstudy_2016, there is a ```PlotGroups/Zstudy_2016.py``` that lists which files go with which overall group and assigns a name and color to it. These colors are what are specified in the ```./AnalysisDatasetManager/Styles``` folder.

### PlotObjects

Like the FileInfo directory, the PlotObjects contains folders for each analysis and a file in each analysis folder for the selection made. These files each contain information about the different plots, the initialize subcategory allowing for setting up the histogram based on the input quantites for a TH1 object and the Attributes being the ROOT functions to be run on the TH1 with it's corresponding input value. From this, the axis can be labelled and simple formatting can be done with relative ease. 

-------------------------------------------------------------------------------

With all of these understood, one can create different selection and apply different coloring convientions, have different skims for different studies, etc in one place so that one's skimming, analysis, and plotting software all have a common area to look for the configuration input.


