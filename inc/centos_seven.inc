# example usage
# https://community.centminmod.com/threads/work-log-for-dealing-with-centos-7-0-changes-systemd.738/

seven_function() {

CMSDEBUG=n

if [[ "$CMSDEBUG" = [yY] ]]; then
  if [[ "$CENTOS_SEVEN" != '7' ]]; then
            echo "Detected 6.5"
  else
            echo "Detected 7.0"
  fi
fi

}