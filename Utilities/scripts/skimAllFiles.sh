#!/bin/bash

selection=$1
if [[ $selection == "" ]];
then
    echo "Provide a selection name as the first argument."
fi


match1=".root"
word1="_$selection"

matchdata="WZ2017/data/"
matchmc="/MC/"
word2="${selection}/"

echo "Running on data:"
if [ -d "/data/strembat/WZ2017/data/$selection" ];
then
    for filename in /data/strembat/WZ2017/data/*.root; do
    [ -e "$filename" ] || continue
    if [[ $filename != *_histfile.root ]];
    then
	echo "$filename" > tempskimfilelist.txt
	echo "skimming $filename..."
	tempname=${filename%%"${match1}"*}${word1}${filename#*"${filename%%"${match1}"*}"}
	#echo "tempname: $tempname"
	outname="${tempname%%${matchdata}*}${matchdata}${word2}${tempname##*${matchdata}}"
	echo "outname: $outname"

	python Utilities/scripts/skimNtuples.py -a WZ2017 -s $selection -f tempskimfilelist.txt -o $outname
    else
	echo "$filename is a histfile, skipping..."
    fi
    done
else
    echo "Selection directory doesn't exist."
    echo "mkdir /data/strembat/WZ2017/data/$selection and re-run."
fi

echo "Running on MC:"
if [ -d "/data/strembat/WZ2017/MC/$selection" ];
then
    for filename in /data/strembat/WZ2017/MC/*.root; do
    [ -e "$filename" ] || continue
    if [[ $filename != *_histfile.root ]];
    then
	echo "$filename" > tempskimfilelist.txt
	echo "skimming $filename..."
	tempname=${filename%%"${match1}"*}${word1}${filename#*"${filename%%"${match1}"*}"}
	#echo "tempname: $tempname"
	outname="${tempname%%${matchmc}*}${matchmc}${word2}${tempname##*${matchmc}}"
	echo "outname: $outname"

	python Utilities/scripts/skimNtuples.py -a WZ2017 -s $selection -f tempskimfilelist.txt -o $outname
    else
	echo "$filename is a histfile, skipping..."
    fi
    done
else
    echo "Selection directory doesn't exist."
    echo "mkdir /data/strembat/WZ2017/MC/$selection and re-run."
fi

#echo "Running on MC:"
#if [ -e /data/strembat/WZ2017/MC/$selection];
#then
#    for filename in /data/strembat/WZ2017/MC/*.root; do
#    [ -e "$filename" ] || continue
#    if [[ $filename != *_histfile.root ]];
#    then
#	outname=${filename%%"${match}"*}${word}${filename#*"${filename%%"${match}"*}"}
#	echo "running histfilemaker.py on $filename..."
#	python histfilemaker.py "$filename" "${outname}"
#    else
#	echo "$filename is a histfile, skipping..."
#    fi
#    done
#else
#    echo "Selection directory doesn't exist."
#    echo "mkdir /data/strembat/WZ2017/MC/$selection and re-run."
#fi
