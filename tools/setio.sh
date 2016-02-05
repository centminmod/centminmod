#!/bin/bash
# https://community.centminmod.com/threads/help-test-innodbio-sh-for-mysql-tuning.6012/
# for centminmod.com /etc/my.cnf
VER=0.1
DEBUG='n'
CPUS=$(grep "processor" /proc/cpuinfo |wc -l)
TIME='n'

if [ ! -f /usr/bin/fio ]; then
  yum -q -y install fio
fi

if [ ! -d /root/tools/fio ]; then
  mkdir -p /root/tools/fio
fi

if [ ! -f /proc/user_beancounters ]; then
  if [[ ! -f /usr/bin/lscpu ]]; then
    yum -q -y install util-linux-ng
  fi
fi

baseinfo() {
  echo
  echo "--------------------------------------------------------------------"
  echo "System Info ($VER)"
  echo "--------------------------------------------------------------------"  
  if [ ! -f /proc/user_beancounters ]; then
    lscpu
  else
    CPUNAME=$(cat /proc/cpuinfo | grep "model name" | cut -d ":" -f2 | tr -s " " | head -n 1)
    CPUCOUNT=$(cat /proc/cpuinfo | grep "model name" | cut -d ":" -f2 | wc -l)
    echo "CPU: $CPUCOUNT x$CPUNAME"
  fi
  uname -r
  cat /etc/redhat-release
  echo "--------------------------------------------------------------------"
  df -hT
  echo "--------------------------------------------------------------------"
  echo
}

fiosetup() {
  cd /root/tools/fio
  if [[ ! -f /root/tools/fio/reads.ini || ! -f /root/tools/fio/reads.ini || ! -f /root/tools/fio/reads-16k.ini || ! -f /root/tools/fio/writes-16k.ini ]]; then
    rm -rf reads.ini writes.ini reads-16k.ini writes-16k.ini
    wget -q https://gist.github.com/centminmod/5edc872cbd97b213aed5/raw/c6b2e25f860fc4f0e06011c910b2778addeff693/reads.ini
    wget -q https://gist.github.com/centminmod/5edc872cbd97b213aed5/raw/c6b2e25f860fc4f0e06011c910b2778addeff693/writes.ini
    cp reads.ini reads-16k.ini
    cp writes.ini writes-16k.ini
    sed -i 's|bs=4k|bs=16k|' reads-16k.ini
    sed -i 's|ba=4k|ba=16k|' reads-16k.ini
    sed -i 's|bs=4k|bs=16k|' writes-16k.ini
    sed -i 's|ba=4k|ba=16k|' writes-16k.ini
  fi
}

fiocheck() {
  if [ -f /usr/bin/fio ]; then
    fiosetup
    cd /root/tools/fio
    FIOR=$(fio --minimal reads-16k.ini | awk -F ';' '{print $8}')
    FIOW=$(fio --minimal writes-16k.ini | awk -F ';' '{print $49}')
    FIOR=$((FIOR*100000))
    FIOW=$((FIOW*100000))
    rm -rf sb-io-test 2>/dev/null
    echo -n "Full Reads: "
    echo "$((FIOR/100000))"
    echo -n "Full Writes: "
    echo "$((FIOW/100000))"
    echo -n "innodb_io_capacity = "
    echo $((FIOW/30/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/40/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/50/100000))
    echo -n "innodb_io_capacity = "
    echo $((FIOW/70/100000))
    echo
    if [[ "$((FIOW/100000))" -ge '1600001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/40/100000))
    elif [[ "$((FIOW/100000))" -lt '160000' && "$((FIOW/100000))" -ge '1400001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/35/100000))
    elif [[ "$((FIOW/100000))" -lt '140000' && "$((FIOW/100000))" -ge '1200001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/30/100000))
    elif [[ "$((FIOW/100000))" -lt '120000' && "$((FIOW/100000))" -ge '100001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/25/100000))
    elif [[ "$((FIOW/100000))" -lt '100000' && "$((FIOW/100000))" -ge '80001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/20/100000))
    elif [[ "$((FIOW/100000))" -lt '80000' && "$((FIOW/100000))" -ge '60001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/20/100000))
    elif [[ "$((FIOW/100000))" -lt '60000' && "$((FIOW/100000))" -ge '40001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/15/100000))
    elif [[ "$((FIOW/100000))" -lt '40000' && "$((FIOW/100000))" -ge '20001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/10/100000))      
    elif [[ "$((FIOW/100000))" -lt '20000' && "$((FIOW/100000))" -ge '10001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/10/100000))
    elif [[ "$((FIOW/100000))" -lt '10000' && "$((FIOW/100000))" -ge '5001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/8/100000))
    elif [[ "$((FIOW/100000))" -lt '5000' && "$((FIOW/100000))" -ge '3001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/6/100000))
    elif [[ "$((FIOW/100000))" -lt '3000' && "$((FIOW/100000))" -ge '2001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/5/100000))
    elif [[ "$((FIOW/100000))" -lt '2000' && "$((FIOW/100000))" -ge '1001' ]]; then
      echo -n "innodb_io_capacity = "
      echo $((FIOW/4/100000))
    elif [[ "$((FIOW/100000))" -lt '1000' && "$((FIOW/100000))" -ge '501' ]]; then
      echo "innodb_io_capacity = 250"
    elif [[ "$((FIOW/100000))" -lt '500' && "$((FIOW/100000))" -ge '301' ]]; then
      echo "innodb_io_capacity = 200"
    elif [[ "$((FIOW/100000))" -lt '300' && "$((FIOW/100000))" -ge '201' ]]; then
      echo "innodb_io_capacity = 150"
    elif [[ "$((FIOW/100000))" -lt '200' && "$((FIOW/100000))" -ge '101' ]]; then
      echo "innodb_io_capacity = 100"
    elif [[ "$((FIOW/100000))" -lt '100' ]]; then
      echo "innodb_io_capacity = 100"
    fi
  fi
}

threadcal() {
  IOTHREADS=$((2*CPUS/4))
  if [ "$CPUS" -eq '1' ];then
    IOTHREADS=2
  fi
  if [ "$IOTHREADS" -lt '2' ];then
    IOTHREADS=2
  fi
  cat /etc/my.cnf | sed -e "s|innodb_read_io_threads = .*|innodb_read_io_threads = $IOTHREADS|g" | grep innodb_read_io_threads
  cat /etc/my.cnf | sed -e "s|innodb_write_io_threads = .*|innodb_write_io_threads = $IOTHREADS|g" | grep innodb_write_io_threads
}

infocheck() {
  baseinfo
  echo "--------------------------------------------------------------------"
  echo -n "$(fio -v)"; echo " calculated (IOPs)"
  echo "--------------------------------------------------------------------"
  echo
  if [[ "$TIME" = [yY] ]]; then
    time fiocheck
  else
    fiocheck
  fi
  echo
  if [[ "$TIME" = [yY] ]]; then
    time fiocheck
  else
    fiocheck
  fi
  echo
  echo "--------------------------------------------------------------------"
  threadcal
  echo "--------------------------------------------------------------------"
}

setbp() {
  INNODB_BPSIZE=$(mysqladmin var | grep 'innodb_buffer_pool_size' | tr -s ' ' | awk '{print $4}')
  INNODB_BPTHRESHOLD='1073741824'
  if [[ "$INNODB_BPSIZE" -le "$INNODB_BPTHRESHOLD" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 1|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 1|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 1;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  fi
  if [[ "$(echo "$INNODB_BPSIZE/$CPUS" | bc)" -le "$INNODB_BPTHRESHOLD" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 1|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 1|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 1;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*48" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*48" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 48|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 48|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 48;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*40" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*40" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 40|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 40|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 40;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*32" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*32" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 32|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 32|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 32;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*24" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*24" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 24|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 24|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 24;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*16" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*16" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 16|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 16|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 16;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*12" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*12" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 12|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 12|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 12;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*8" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*8" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 8|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 8|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 8;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*6" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*6" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 6|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 6|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 6;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*4" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*4" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 4|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 4|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 4;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  elif [[ "$(echo "$INNODB_BPSIZE/$CPUS*2" | bc)" -ge "$(echo "$INNODB_BPTHRESHOLD*$CPUS*2" | bc)" ]]; then
    sed -i "s|#innodb_buffer_pool_instances=.*|innodb_buffer_pool_instances = 2|g" /etc/my.cnf
    sed -i "s|innodb_buffer_pool_instances = .*|innodb_buffer_pool_instances = 2|g" /etc/my.cnf
    # mysql -e "SET GLOBAL innodb_buffer_pool_instances = 2;"
    # mysql -e "SHOW VARIABLES like '%innodb_buffer_pool_instances%'"
  fi
  # grep 'innodb_buffer_pool_instances =' /etc/my.cnf
}

setio() {
  if [ -f /usr/bin/fio ]; then
    fiosetup
    cd /root/tools/fio
    FIOR=$(fio --minimal reads-16k.ini | awk -F ';' '{print $8}')
    FIOW=$(fio --minimal writes-16k.ini | awk -F ';' '{print $49}')
    FIOR=$((FIOR*100000))
    FIOW=$((FIOW*100000))
    FIOR=$((FIOR/100000))
    FIOW=$((FIOW/100000))
    rm -rf sb-io-test 2>/dev/null

    echo -n "Full Reads: "
    echo "$FIOR"
    echo -n "Full Writes: "
    echo "$FIOW"

    if [[ "$FIOW" -ge '1600001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/40))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '160000' && "$FIOW" -ge '1400001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/35))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '140000' && "$FIOW" -ge '1200001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/30))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '120000' && "$FIOW" -ge '100001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/25))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '100000' && "$FIOW" -ge '80001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/20))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '80000' && "$FIOW" -ge '60001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/20))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '60000' && "$FIOW" -ge '40001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/15))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '40000' && "$FIOW" -ge '20001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/10))      
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '20000' && "$FIOW" -ge '10001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/10))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '10000' && "$FIOW" -ge '5001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/8))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '5000' && "$FIOW" -ge '3001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/6))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '3000' && "$FIOW" -ge '2001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/5))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '2000' && "$FIOW" -ge '1001' ]]; then
      echo -n "innodb_io_capacity = "
      FIOWSET=$((FIOW/4))
      FIOWSET=$(echo "$(echo "scale=2; $FIOWSET/10000*10000" | bc)/1" | bc)
      echo $FIOWSET
    elif [[ "$FIOW" -lt '1000' && "$FIOW" -ge '501' ]]; then
      FIOWSET=250
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '500' && "$FIOW" -ge '301' ]]; then
      FIOWSET=200
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '300' && "$FIOW" -ge '201' ]]; then
      FIOWSET=150
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '200' && "$FIOW" -ge '101' ]]; then
      FIOWSET=100
      echo "innodb_io_capacity = $FIOWSET"
    elif [[ "$FIOW" -lt '100' ]]; then
      FIOWSET=100
      echo "innodb_io_capacity = $FIOWSET"
    fi
  fi

  echo
  echo "/etc/my.cnf adjustment"
  # echo
  echo "existing value: "
  # grep 'innodb_io_capacity' /etc/my.cnf
  mysql -e "SHOW VARIABLES like '%innodb_io_capacity%'"

  # sed -e "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET|g" /etc/my.cnf | grep 'innodb_io_capacity'
  sed -i "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET|g" /etc/my.cnf
  echo "new value: "
  # grep 'innodb_io_capacity' /etc/my.cnf
  mysql -e "SET GLOBAL innodb_io_capacity = $FIOWSET;"
  IOMAX=$((FIOWSET*2))
  if [[ "$IOMAX" -gt "$FIOW" ]]; then
    IOMAX=$(echo "$IOMAX*0.66/1"|bc)
  fi
  IOMAXCHECK=$(grep 'innodb_io_capacity_max' /etc/my.cnf)
  if [[ -z "$IOMAXCHECK" ]]; then
    sed -i "s|innodb_io_capacity = .*|innodb_io_capacity = $FIOWSET\ninnodb_io_capacity_max = $IOMAX|g" /etc/my.cnf
  else
    sed -i "s|innodb_io_capacity_max = .*|innodb_io_capacity_max = $IOMAX|g" /etc/my.cnf
  fi
  # echo "new value: "
  # grep 'innodb_io_capacity_max' /etc/my.cnf
  mysql -e "SET GLOBAL innodb_io_capacity_max = $IOMAX;"
  mysql -e "SHOW VARIABLES like '%innodb_io_capacity%'"
}

case "$1" in
  check )
    infocheck
    ;;
  set )
    setbp
    setio
    ;;
  * )
    echo "$0 {check|set}"
    ;;
esac
exit