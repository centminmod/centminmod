countcpus() {

if [ -f /usr/local/nginx/conf/nginx.conf ]; then

NOCPUS=$(grep -c "processor" /proc/cpuinfo)
NGINXCONFCPU='/usr/local/nginx/conf/nginx.conf'
WORKERCHECKA=$(grep 'worker_processes  1;' $NGINXCONFCPU)
WORKERCHECKB=$(grep 'worker_processes 1;' $NGINXCONFCPU)

if [[ "$NOCPUS" -le "2" ]]; then
        NOCPUS=$NOCPUS

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le 4 && "$NOCPUS" -gt 2 ]]; then    
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "6" && "$NOCPUS" -gt "4" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [ "$NOCPUS" = "7" ]; then
        NOCPUS=$(echo "$NOCPUS"/2.333 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [ "$NOCPUS" = "8" ]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
  if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  fi
fi
if [[ "$NOCPUS" -le "10" && "$NOCPUS" -gt "8" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
  if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  fi
fi
if [ "$NOCPUS" = "11" ]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
  if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  fi
fi
if [ "$NOCPUS" = "12" ]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "15" && "$NOCPUS" -ge "13" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.1 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "16" && "$NOCPUS" -gt "12" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "23" && "$NOCPUS" -ge "17" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.1 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "31" && "$NOCPUS" -ge "24" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.4 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "47" && "$NOCPUS" -ge "32" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.6 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "64" && "$NOCPUS" -ge "48" ]]; then
        NOCPUS=$(echo "$NOCPUS"/2.666 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "127" && "$NOCPUS" -ge "65" ]]; then
        NOCPUS=$(echo "$NOCPUS"/4.7083 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi
if [[ "$NOCPUS" -le "191" && "$NOCPUS" -ge "128" ]]; then
        NOCPUS=$(echo "$NOCPUS"/6.4 +10 | bc)

        echo
  if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  fi
fi
if [[ "$NOCPUS" -le "256" && "$NOCPUS" -ge "192" ]]; then
        NOCPUS=$(echo "$NOCPUS"/6.4 +10 | bc)

        echo
  if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
    sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
  fi
fi
if [[ "$NOCPUS" -ge "257" ]]; then
        NOCPUS=$(echo "$NOCPUS"/6.5 +10 | bc)

        echo
    if [[ ! -z "$WORKERCHECKA" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*;/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    elif [[ ! -z "$WORKERCHECKB" ]]; then
        #echo "set cpu worker_processes to $NOCPUS"
        sed -i "s/worker_processes .*/worker_processes $NOCPUS;/g" $NGINXCONFCPU
    fi
fi

fi

if [[ "$NGINXCPU_AUTOTUNE_NEW" = [yY] ]]; then
    NOCPUS=$(grep -c "processor" /proc/cpuinfo)
    NOCPUS_NGINX=$(($NOCPUS/2))
    
    if [[ "$NOCPUS_NGINX" -ge '6' && "$NOCPUS_NGINX" -lt '8' ]]; then
    # optimise for up to 12-15 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '8' && "$NOCPUS_NGINX" -lt '16' ]]; then
    # optimise for up to 16-31 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '16' && "$NOCPUS_NGINX" -lt '24' ]]; then
    # optimise for upto 32-47 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '24' && "$NOCPUS_NGINX" -lt '28' ]]; then
    # optimise for upto 48-55 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '28' && "$NOCPUS_NGINX" -lt '32' ]]; then
    # optimise for upto 56-63 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '32' && "$NOCPUS_NGINX" -lt '48' ]]; then
    # optimise for upto 64-95 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '48' && "$NOCPUS_NGINX" -lt '64' ]]; then
    # optimise for 96-127 cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    elif [[ "$NOCPUS_NGINX" -ge '64' ]]; then
    # optimise for 128+ cpu cores
    # NOCPUS_NGINX=$(($NOCPUS_NGINX/1))
    sed -i '/worker_cpu_affinity/d' $NGINXCONFCPU
    sed -i "s/worker_processes .*;/worker_processes $NOCPUS_NGINX;\nworker_cpu_affinity auto;/g" $NGINXCONFCPU
    # grep worker_ $NGINXCONFCPU
    # for p in $(pidof nginx); do taskset -c -p $p; done
    fi
fi

}