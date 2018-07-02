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
	sudo tc qdisc del dev eth0 root
	killall screen	    			
	#cd ../Q4S_client-server/q4sServer
	screen -S screenServer -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh
	#screen -S screenServer -d -m ./Q4SServer 
	#cd ../../test
	for ms_delay in 0 5 10; 
	do
		for jitter_coef in 3 2 1; 
		do
			jitter_max=$(($ms_delay/$jitter_coef))
			echo $jitter_max
			for lossprcent in 10 5 1; 
			do
				for bandwidth_max in 50 28 20; 
				do
					FileName="client_lt_"$ms_delay"_jt_"$jitter_max"_pl_"$lossprcent"_bw_"$bandwidth_max".txt"
	    			echo $FileName

					sleep 2
					echo sudo tc qdisc add dev eth0 root netem delay $ms_delay"ms" $jitter_max"ms" loss random $lossprcent"%" rate $bandwidth_max"Mbit"

					sudo tc qdisc add dev eth0 root netem delay $ms_delay"ms" $jitter_max"ms" loss random $lossprcent"%" rate $bandwidth_max"Mbit"
	    			cd ../Q4S_client-server/q4sCLient
	    			
	    			screen -S screenClient -d -m ./Q4SClient 

	    			#screen -S screenClient -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh 
	    			sleep 60
	    			screen -S screenClient -X at '#' stuff ^C
	    			
					cd ../../test
	    			sleep 6
					scp measured/measure_client.txt "measured/F1/client_lan/client_lt_"$ms_delay"_jt_"$jitter_max"_pl_"$lossprcent"_bw_"$bandwidth_max".txt"
					sudo tc qdisc del dev eth0 root
				done
			done
		done
	done	    			
	screen -S screenServer -X at '#' stuff ^C

	#scp measured/measure_server.txt measured/static_server.txt
	# tc qdisc change dev eth1 root netem loss 1%
	# tc qdisc change dev eth1 root netem delay 80ms 10ms

	tput bold 
    echo -e \\n[OK!]
	tput sgr0
	echo -e \\n\\n

} 
function dynamic_network(){
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
	sudo tc qdisc del dev eth0 root
	killall screen	    			
	cd ../Actuador
	screen -S screenActuador -d -m python actuator_restAPI.py
	cd ../Q4S_client-server/q4sServer

	screen -S screenServer -d -m ./Q4SServer 
	screen -S screenClient -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh 
	sleep 30
	cd ../../test
	for ms_delay in 10 5 1 9 4 0 7 2 3 2 0 1 4; 
	do
		for jitter_coef in 1 2 3; 
		do
			jitter_max=$(($ms_delay/$jitter_coef))
			for lossprcent in 10 5 1; 
			do

					
					if ! screen -list | grep -q "screenClient";
					then 
						echo end session
						sudo tc qdisc del dev eth0 root
						time_Now=$(date +"%s")
						Features='0,0,0,'$time_Now
	    				echo $Features	>> real/netem_params_actuador.txt
						#cat measured/measure_server.txt>>dynamic_measurement_server.txt
						screen -S screenClient -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh 
						sleep 30
						time_Now=$(date +"%s")
						Features='0,0,0,'$time_Now
	    				echo $Features	>> real/netem_params_actuador.txt

					else 
						sleep 60
						sudo tc qdisc del dev eth0 root
						echo $time_Now
						time_Now=$(date +"%s")
						Features=$ms_delay','$jitter_max','$lossprcent','$time_Now
	    				echo $Features	>> real/netem_params_actuador.txt

					fi


					sudo tc qdisc add dev eth0 root netem delay $ms_delay"ms" $jitter_max"ms" loss random $lossprcent"%" & time_Now=$(date +"%s")
					echo $time_Now
					Features=$ms_delay','$jitter_max','$lossprcent','$time_Now
	    			echo $Features	>> real/netem_params_actuador.txt
					#screen -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh 

					

					echo $Features
	    			#cd ../Q4S_client-server/q4sCLient
	    			#cd ../q4sCLient
	    			#screen -S screenClient -d -m ssh root@192.168.1.101 ./../home/hpcn/q4s/test/setnetcondF1.sh 
	    			sleep 60
	    			#screen -S screenClient -X at '#' stuff ^C
	    			

	    			sleep 6
					#scp measured/measure_server.txt "measured/server_lt_"$ms_delay"_jt_"$jitter_max"_pl_"$lossprcent"_bw_"$bandwidth_max".txt"
					#sudo tc qdisc del dev eth0 root
				
			done
		done
	done
	# tc qdisc change dev eth1 root netem loss 1%
	# tc qdisc change dev eth1 root netem delay 80ms 10ms
	screen -S screenClient -X at '#' stuff ^C
	screen -S screenServer -X at '#' stuff ^C
	scp measured/measure_server.txt measured/dynamic_measurement_server_actuator.txt
	scp measured/dynamic_actuator.txt measured/dynamic_actuator.txt

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
	    			
	    			;;
				"F2")
	    			#echo TODO\\n
	    			dynamic_network
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


