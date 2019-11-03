#!/usr/bin/env python
import ROOT
import subprocess
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
from python import HistTools
import os
import logging
import sys

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--lumi", "-l", type=float,
        default=35.87, help="luminosity value (in fb-1)")
    parser.add_argument("--maxEntries", "-m", type=int,
        default=-1, help="Max entries to process")
    parser.add_argument("--output_file", "-o", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--test", action='store_true',
        help="Run test job (no background estimate)")
    parser.add_argument("--debug", action='store_true',
        help="Print verbose info")
    ntuple_group = parser.add_mutually_exclusive_group(required=False)
    ntuple_group.add_argument("--uwvv", action='store_true',
        help="Use UWVV format ntuples in stead of NanoAOD")
    ntuple_group.add_argument("--bacon", action='store_true',
        help="Use Bacon format ntuples in stead of NanoAOD")
    parser.add_argument("--noHistConfig", action='store_true',
        help="Don't rely on config file to specify hist info")
    parser.add_argument("-j", "--numCores", type=int, default=1,
        help="Number of cores to use (parallelize by dataset)")
    parser.add_argument("--input_tier", type=str,
        default="", help="Selection stage of input files")
    parser.add_argument("--with_background", action='store_true',
                    help="Don't run background selector")
    parser.add_argument("--background_input", type=str, default="",
                        required=False, help="Selection to use for background selector")
    parser.add_argument("--regions", type=str,
        default="", help="Define subregions for dataset, format 'dataset=region1,region2,...;'")
    parser.add_argument("--year", type=str,
        default="default", help="Year of Analysis")
    parser.add_argument("-c", "--channels", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["eee","eem","emm","mmm"], help="List of channels"
                        "separated by commas. NOTE: set to Inclusive for NanoAOD")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
    return vars(parser.parse_args())

def makeHistFile(args):
    ROOT.gROOT.SetBatch(True)

    manager_path = ConfigureJobs.getManagerPath()
    if manager_path not in sys.path:
        sys.path.insert(0, "/".join([manager_path, 
            "AnalysisDatasetManager", "Utilities/python"]))

    tmpFileName = args['output_file']
    toCombine = args['with_background'] 
    fOut = ROOT.TFile(tmpFileName if not toCombine else tmpFileName.replace(".root", "sel.root"), "recreate")
    combinedNames = [fOut.GetName()]

    addScaleFacs = False
    if args['analysis'] == "WZxsec2016" or args['analysis'] == 'Zstudy_2016':
        addScaleFacs = True
    addScaleFacs=False
    fr_inputs = []
    sf_inputs = [ROOT.TParameter(bool)("applyScaleFacs", False)]

    if addScaleFacs:
        fScales = ROOT.TFile('data/scaleFactors.root')
        mCBTightFakeRate = fScales.Get("mCBTightFakeRate")
        eCBTightFakeRate = fScales.Get("eCBTightFakeRate")

        muonIsoSF = fScales.Get('muonIsoSF')
        muonIdSF = fScales.Get('muonTightIdSF')
        electronTightIdSF = fScales.Get('electronTightIdSF')
        electronGsfSF = fScales.Get('electronGsfSF')
        pileupSF = fScales.Get('pileupSF')

        #fPrefireEfficiency = ROOT.TFile('data/Map_Jet_L1FinOReff_bxm1_looseJet_JetHT_Run2016B-H.root')
        fPrefireEfficiency = ROOT.TFile('data/Map_Jet_L1FinOReff_bxm1_looseJet_SingleMuon_Run2016B-H.root')
        prefireEff = fPrefireEfficiency.Get('prefireEfficiencyMap')

        fr_inputs = [eCBTightFakeRate, mCBTightFakeRate,]
        sf_inputs = [electronTightIdSF, electronGsfSF, muonIsoSF, muonIdSF, pileupSF, prefireEff]
        sf_inputs.append(ROOT.TParameter(bool)("applyScaleFacs", True))

    if args['input_tier'] == '':
        args['input_tier'] = args['selection']
    selection = args['selection'].split("_")[0]
    analysis = "/".join([args['analysis'], selection])
    hists, hist_inputs = UserInput.getHistInfo(analysis, args['hist_names'], args['noHistConfig'])

    selector = SelectorTools.SelectorDriver(args['analysis'], args['selection'], args['input_tier'], args['year'])
    selector.setNumCores(args['numCores'])
    selector.setOutputfile(fOut.GetName())
    selector.setInputs(sf_inputs+hist_inputs)
    selector.setMaxEntries(args['maxEntries'])

    if args['uwvv']:
        selector.setNtupeType("UWVV")
        selector.setChannels(args['channels'])
        logging.debug("Processing channels " % args['channels'])
    elif args['bacon']:
        selector.setNtupeType("Bacon")
        selector.setAddSumWeights(False)
    else:
        selector.setNtupeType("NanoAOD")

    if args['filenames']:
        selector.setDatasets(args['filenames'])
    else:
        selector.setFileList(*args['inputs_from_file'])

    if args['regions']:
        selector.setDatasetRegions(args['regions'])
    elif args['analysis'] == 'LowPileupW':
        regions = ["GenPtW_%i_%i" % (i, i+10) for i in range(0, 100, 10)]
        regions += ["GenPtW_100"]
        selector.setDatasetRegions("wmv_0j_nlo=" + ','.join(regions))
        selector.setDatasetRegions("wmv_1j_nlo=" + ','.join(regions))
        selector.setDatasetRegions("wmv_2j_nlo=" + ','.join(regions))

    mc = selector.applySelector()

    if args['with_background']:
        # TODO: Should also have an option to specify input file list for background
        if args['background_input'] and args['filenames']:
            selector.setInputTier(args['background_input'])
            selector.setDatasets(args['filenames'])
        selector.isBackground()
        selector.setAddSumWeights(False)
        selector.unsetDatasetRegions()
        selector.setInputs(sf_inputs+hist_inputs+fr_inputs)
        output_name = tmpFileName.replace(".root", "bkgd.root")
        selector.setOutputfile(output_name)
        bkgd = selector.applySelector()
        combinedNames.append(output_name)
    selector.outputFile().Close()

    if len(combinedNames) > 1:
        rval = subprocess.call(["hadd", "-f", tmpFileName] + combinedNames)
        if rval == 0:
            map(os.remove, combinedNames)
    fOut = ROOT.TFile(tmpFileName, "update")
    OutputTools.addMetaInfo(fOut)

def main():
    args = getComLineArgs()
    logging.basicConfig(level=(logging.DEBUG if args['debug'] else logging.WARNING))

    makeHistFile(args)
    exit(0)

if __name__ == "__main__":
    main()
