setmycnf() {
  if [ -f "$(which figlet)" ]; then
    figlet -ckf standard "Set /etc/my.cnf"
  fi
  MYTOTALMEM=$(cat /proc/meminfo | grep MemTotal | awk '{print $2}')
  MYCPUS=$(cat "/proc/cpuinfo" | grep "processor"|wc -l)
  if [[ "$(expr $MYTOTALMEM \<= 2099000)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-min.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-min.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \> 2100001)" = "1" && "$(expr $MYTOTALMEM \<= 4190000)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 4190001)" = "1" && "$(expr $MYTOTALMEM \<= 8199999)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-4gb.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-4gb.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 8200000)" = "1" && "$(expr $MYTOTALMEM \<= 15999999)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-8gb.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-8gb.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 16000000)" = "1" && "$(expr $MYTOTALMEM \<= 31999999)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-16gb.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-16gb.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 32000000)" = "1" && "$(expr $MYTOTALMEM \<= 63999999)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-32gb.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-32gb.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 64000000)" = "1" && "$(expr $MYCPUS \<= 8)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-64gb-8c.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-64gb-8c.cnf /etc/my.cnf
  fi
  if [[ "$(expr $MYTOTALMEM \>= 64000000)" = "1" && "$(expr $MYCPUS \>= 12)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-64gb.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-64gb.cnf /etc/my.cnf
  elif [[ "$(expr $MYTOTALMEM \>= 64000000)" = "1" && "$(expr $MYCPUS \>= 8)" = "1" ]]; then
    echo -e "\nCopying MariaDB my-mdb10-64gb-8c.cnf file to /etc/my.cnf\n" | tee "${CENTMINLOGDIR}/setmycnf_${SCRIPT_VERSION}_${DT}.log"
    \cp -f $CUR_DIR/config/mysql/my-mdb10-64gb-8c.cnf /etc/my.cnf
  fi
  mysql_charset_default
  echo
  cat /etc/my.cnf
  echo
}