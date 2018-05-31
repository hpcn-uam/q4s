#!/bin/bash
#Autor: Jesús Molina Merchán 	

echo hpcn.es | sudo -S tput clear
tput bold 


ms_delay=$1
jitter_max=$2
lossprcent=$3
bandwidth_max=$4
sudo tc qdisc add dev lo root netem delay $ms_delay"ms" $jitter_max"ms" loss random $lossprcent"%"  rate $bandwidth_max"Mbit"
cd ../Q4S_client-server/q4sServer
./Q4SServer
cd ../../test
tc qdisc del dev lo root
mv measured/measure_server.txt  server_lt_$ms_delay_jt_$jitter_max_pl_$lossprcent_bw_$bandwidth_max.txt
sudo tc qdisc del dev eth0 root