countcpus() {

if [ -f /usr/local/nginx/conf/nginx.conf ]; then

NOCPUS=$(grep "processor" /proc/cpuinfo |wc -l)
NGINXCONFCPU='/usr/local/nginx/conf/nginx.conf'
WORKERCHECKA=$(grep 'worker_processes  1;' $NGINXCONFCPU)
WORKERCHECKB=$(grep 'worker_processes 1;' $NGINXCONFCPU)

if [[ "$NOCPUS" -le "2" ]]; then
        NOCPUS=$NOCPUS

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le 4 && "$NOCPUS" -gt 2 ]]; then    
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "6" && "$NOCPUS" -gt "4" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" = "8" || "$NOCPUS" = "7" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.333 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "12" && "$NOCPUS" -gt "8" ]]; then
        NOCPUS=$(echo "$NOCPUS"/3 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "15" && "$NOCPUS" -ge "13" ]]; then
        NOCPUS=$(echo "$NOCPUS"/3.25 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "16" && "$NOCPUS" -gt "12" ]]; then
        NOCPUS=$(echo "$NOCPUS"/4 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "23" && "$NOCPUS" -ge "17" ]]; then
        NOCPUS=$(echo "$NOCPUS"/3.5 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "31" && "$NOCPUS" -ge "24" ]]; then
        NOCPUS=$(echo "$NOCPUS"/4 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "47" && "$NOCPUS" -ge "32" ]]; then
        NOCPUS=$(echo "$NOCPUS"/4 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "64" && "$NOCPUS" -ge "48" ]]; then
        NOCPUS=$(echo "$NOCPUS"/4 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -le "256" && "$NOCPUS" -ge "65" ]]; then
        NOCPUS=$(echo "$NOCPUS"/8 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi
if [[ "$NOCPUS" -ge "257" ]]; then
        NOCPUS=$(echo "$NOCPUS"/12 | bc)

        echo
	if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes  1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
		sed -i "s/worker_processes 1;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
	fi
fi

fi
}