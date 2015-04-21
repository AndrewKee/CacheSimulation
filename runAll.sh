#! /bin/bash

echo "Running Production Traces"

traceType=$1

traceOutFile="TraceResults-$traceType"
traceFile="traces-$traceType"

mkdir -p $traceOutFile

for entry in ./Traces/$traceFile/*
do
  echo "Production Entry: $entry"

        for config in Config/*
        do

        		filename=${entry##*/}

                echo "  -----> Running Configuration: $config"

                echo "----------------------------------------------------------------" >> results.dat
                echo "${entry##*/}_${config##*/}				Simulation Results" >> results.dat
                echo "----------------------------------------------------------------" >> results.dat
                if [ "$traceType" = "1M" ] || [ "$traceType" = "5M" ] || [ "$traceType" = "long"] ;
                then
                	if [[ $filename == *.gz ]];
                	then
                		zcat < $entry | ./main.o $config
                	else
                		echo "$filename"
                	fi
                else
                	cat $entry | ./main.o $config
                fi

                echo "  -----> Finished Running: $config"

                echo "  -----> Moving Results into Directory"

                

                mv results.dat $traceOutFile/${filename%.*}_${config##*/}
        done

done
