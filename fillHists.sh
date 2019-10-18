#!/bin/bash

## to run: ./Fillhists.sh -o test.root -f "4top2016"

runTime=$(stat -c "%Y" .compile)
modTime=$(ls -tr src/* interface/* | tail -n 1 | xargs stat -c "%Y")

if [ "$modTime" -gt "$runTime" ]; then
    echo Recompiling
    scramv1 b -j 10 1>/dev/null
    if [ $? != 0 ]; then
	echo "Error"
	exit 1
    fi
    touch .compile
    echo
fi

EXE=./Utilities/scripts/makeHistFile.py
ANA=Eff
YEAR=2016
INP=4TopComparison
#SEL=FourTopCutBasedEl
#SEL=TightWithLooseVeto
SEL=test
#SEL=BEfficiency

$EXE -a $ANA --input_tier $INP -s $SEL \
--year $YEAR --test ${@}
