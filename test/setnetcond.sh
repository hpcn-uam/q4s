#!/bin/bash
#Autor: Jesús Molina Merchán 
function static_network(){
#Gets input lhe frames from original video
#using YUV_PROFILE, WIDTH, HEIGHT, FPS_RATE, FRAME_N, ... 
	tput clear
	tput bold 
    echo -e \\nGenerating  network filters\\n
	tput sgr0

	#echo "Please input Server:   ms_delay_s jitter_max_s lossprcent_s bandwidth_max_s ms ms % Mbit "
	#read ms_delay_s jitter_max_s lossprcent_s bandwidth_max_s
#################################
#################################
	#echo "Please input:   ms_delay jitter_max lossprcent bandwidth_max ms ms % Mbit "
	#read ms_delay jitter_max lossprcent bandwidth_max

	#SERVER

	#CLIENT


	for ms_delay in 10 5 1; 
	do
		for jitter_max in 4 1 0; 
		do
			for lossprcent in 10 5 1; 
			do
				for bandwidth_max in 50 28 20; 
				do
					#xterm -e ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh $ms_delay $jitter_max $lossprcent $bandwidth_max
					xterm -e ./setnetcondF1.sh $ms_delay $jitter_max $lossprcent $bandwidth_max
					echo 1
					sleep 10

					sudo tc qdisc add dev eth0 root netem delay $ms_delay"ms" $jitter_max"ms" loss random $lossprcent"%" rate $bandwidth_max"Mbit"
	    			cd ../Q4S_client-server/q4sCLient
	    			./Q4SClient
	    			cd ../../test
	    			#FileName="client_lt_"$ms_delay"_jt_"$jitter_max"_pl_"$lossprcent"_bw_"$bandwidth_max".txt"
	    			#echo $FileName
					scp measured/measure_client.txt "measured/client_lt_"$ms_delay"_jt_"$jitter_max"_pl_"$lossprcent"_bw_"$bandwidth_max".txt"
					sudo tc qdisc del dev eth0 root
				done
			done
		done
	done
	# tc qdisc change dev eth1 root netem loss 1%
	# tc qdisc change dev eth1 root netem delay 80ms 10ms

	tput bold 
    echo -e \\n[OK!]
	tput sgr0
	echo -e \\n\\n

} 
function dynamic_network(){
#Gets input lhe frames from original video
#using YUV_PROFILE, WIDTH, HEIGHT, FPS_RATE, FRAME_N, ... 
	tput clear
	tput bold 
    echo -e \\nGenerating  network filters\\n
	tput sgr0
	echo "Please input Server:   ms_delay_s jitter_max_s lossprcent_s bandwidth_max_s ms ms % Mbit "
	read ms_delay_s jitter_max_s lossprcent_s bandwidth_max_s
#################################
xterm -e ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh $ms_delay_s $jitter_max_s $lossprcent_s 
#################################

	echo "Please input:   ms_delay jitter_max lossprcent bandwidth_max ms ms % Mbit "
	read ms_delay jitter_max lossprcent bandwidth_max
	# tc qdisc change dev eth1 root netem loss 1%
	# tc qdisc change dev eth1 root netem delay 80ms 10ms


	tput bold 
    echo -e \\n[OK!]
	tput sgr0
	echo -e \\n\\n

} 
echo hpcn.es | sudo -S tput clear

#sudo tput clear #this gets sudo access for later commands (sudo ./run_vma_in_batch -> over 9000 problems)
tput bold 
option=""
	while [ "$option" != Quit ]
	do
		tput bold 
		echo Select runnning mode:
		tput sgr0
		options=("F1" "F2" "Quit")
		PS3='Select mode: '
		select option in "${options[@]}"
		do
			case $option in
				"F1")
	    			static_network
	    			cd ../Q4S_client-server/q4sCLient
	    			./Q4SClient
	    			;;
				"F2")
	    			echo TODO\\n
	    			;;
    			"Quit")
    				tput clear
    				tput bold 
	    			echo -ne \\nQuitting ...
	    			tput sgr0
					#just to make sure & clean from previous failed runs
	    			sudo tc qdisc del dev ifb0 root netem 2>/dev/null
		    		sudo tc qdisc del dev eth0 ingress 2>/dev/null
					sudo tc qdisc del dev lo ingress 2>/dev/null
					delete_file temp_s >/dev/null
					delete_file temp_r >/dev/null

					tput bold 
					echo -e \\n\\nHave fun!\\n
					tput sgr0
					break
	    			;;
	    		*) 
					tput clear
	    			echo -ne Invalid option!\\n\\n
	    			;;
			esac
			break
		done
	done


