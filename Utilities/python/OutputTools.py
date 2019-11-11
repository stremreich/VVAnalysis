import ROOT
import datetime
import subprocess
import sys

def addMetaInfo(fOut):
    metaInfo = fOut.mkdir("MetaInfo")
    metaInfo.cd()
    time = ROOT.TNamed("datetime", str(datetime.datetime.now()))
    command = ROOT.TNamed("command", ' '.join(sys.argv))
    githash = ROOT.TNamed("githash", subprocess.check_output(['git', 'log', '-1', '--format="%H"']))
    gitdiff = ROOT.TNamed("gitdiff", subprocess.check_output(['git', 'diff',]))
    time.Write()
    command.Write()
    githash.Write()
    gitdiff.Write()

def writeOutputListItem(item, directory):
    if item.ClassName() == "TList":
        d = directory.Get(item.GetName())
        if not d:
            d = directory.mkdir(item.GetName())
            ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
    elif hasattr(item, 'Write'):
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)
    directory.cd()
