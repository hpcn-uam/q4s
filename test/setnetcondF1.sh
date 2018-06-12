#!/bin/bash
#Autor: Jesús Molina Merchán 	

echo hpcn.es | sudo -S tput clear
tput bold 


ms_delay=$1
jitter_max=$2
lossprcent=$3
bandwidth_max=$4
cd ../Q4S_client-server/q4sServer
screen -d -m ./Q4SServer

