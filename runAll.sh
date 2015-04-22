#! /bin/bash

echo "Running Production Traces"

#let us just input the trace folder suffix when we call the 
#script, makes it more universal
traceType=$1

traceOutDir="TraceResults-$traceType"
traceFile="traces-$traceType"

mkdir -p $traceOutDir

for entry in ./Traces/$traceFile/*
do
	echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	echo "---------------------- $filename --------------------------"
	echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
    echo " "
        for config in Config/*
        do
        		cleanConfig=${config##*/}
        		filename=${entry##*/}
                echo "  --> Running $filename with Configuration: ${cleanConfig%.*}"

                # echo "----------------------------------------------------------------" >> results.dat
                # echo "${entry##*/}_${config##*/}				Simulation Results" >> results.dat
                # echo "----------------------------------------------------------------" >> results.dat
                #if we're in one of these traces, we need to use the zcat command
                if [ "$traceType" = "1M" ] || [ "$traceType" = "5M" ] || [ "$traceType" = "long" ] ;
                then
                	#make sure we only run for the .gz files, no need to run twice
                	if [[ $filename == *.gz ]]
                		then
                			zcat < $entry | ./main.o $config ${filename%.*}_${config##*/}
                	fi
                else
                	cat $entry | ./main.o $config ${filename%.*}_${config##*/}
                fi

                echo "  -----> Finished Running: ${cleanConfig%.*}"

                echo "  -----> Moving Results into Directory $traceOutDir"
                echo " "
                mv ${filename%.*}_${config##*/} $traceOutDir/${filename%.*}_${config##*/}
        done

done
