import ROOT
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("input_file", type=str)
args = parser.parse_args()

rtfile = ROOT.TFile(args.input_file)
metaInfo = rtfile.Get("MetaInfo")
if not metaInfo:
    print "ERROR! Can't find metaInfo in file"
    exit(1)
print metaInfo.Get("command").GetTitle()
