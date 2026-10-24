centminlog() {

CENTMINLOGDIR='/root/centminlogs'

if [ ! -d "$CENTMINLOGDIR" ]; then
  mkdir -p -m 700 $CENTMINLOGDIR
fi
}