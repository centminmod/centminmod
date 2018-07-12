centminlog() {

CENTMINLOGDIR='/root/centminlogs'

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p $CENTMINLOGDIR
fi
}