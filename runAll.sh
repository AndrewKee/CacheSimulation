#! /bin/bash

echo "Running Production Traces"

mkdir -p TraceResults

for entry in ../Traces/traces-1M/*
do
  echo "Production Entry: $entry"

        for config in Config/*
        do
                echo "  -----> Running Configuration: $config"

                cat $entry | ./main.o $config

                echo "  -----> Finished Running: $config"

                echo "  -----> Moving Results into Directory"

                cp results.dat TraceResults/${entry##*/}_${config##*/}
        done

done
