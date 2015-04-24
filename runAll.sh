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
    filename=${entry##*/}
	echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	echo "---------------------- $filename --------------------------"
	echo " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
    echo " "
        for config in Config/*
        do
        		cleanConfig=${config##*/}
        	
                totalfilename="${filename%.*}_${config##*/}"
                echo "  --> Running $filename with Configuration: ${cleanConfig%.*}"

                # echo "----------------------------------------------------------------" >> results.dat
                # echo "${entry##*/}_${config##*/}				Simulation Results" >> results.dat
                # echo "----------------------------------------------------------------" >> results.dat
                #if we're in one of these traces, we need to use the zcat command
                if [ "$traceType" = "1M" ] || [ "$traceType" = "5M" ] || [ "$traceType" = "long" ] ;
                then
                	#make sure we only run for the .gz files, no need to run twice
                	# if [[ $filename == *.gz ]]
                	# 	then
                			zcat < $entry | ./main.o $config $totalfilename
                	# fi
                else
                	cat $entry | ./main.o $config $totalfilename
                fi

                echo "  -----> Finished Running: ${cleanConfig%.*}"

                echo "  -----> Moving Results into Directory $traceOutDir"
                
                mv $totalfilename $traceOutDir/$totalfilename

                echo " "
        done

done
