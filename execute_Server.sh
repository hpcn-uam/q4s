#!/bin/bash
tput bold
cd Q4S_client-server/q4sServer
option=""
	while [ "$option" != Quit ]
	do
		tput bold 
		echo Select runnning mode:
		tput sgr0
		options=("Compile" "Run" "Quit")
		PS3='Select mode: '
		select option in "${options[@]}"
		do
			case $option in
				"Compile")
					echo "Please set the flags:   Save: SAVE_INFO Show: SHOW_INFO Debug: SHOW_INFO2 "
					read SAVE_INFO SHOW_INFO SHOW_INFO2
	    			make SAVE_INFO=$SAVE_INFO SHOW_INFO=$SHOW_INFO SHOW_INFO2=$SHOW_INFO2
	    			;;
				"Run")
					./Q4SServer
	    			echo TODO\\n
	    			;;
    			"Quit")
    				tput clear
    				tput bold 
	    			echo -ne \\nQuitting ...
	    			;;
	    		*) 
					tput clear
	    			echo -ne Invalid option!\\n\\n
	    			;;
			esac
			break
		done
	done


