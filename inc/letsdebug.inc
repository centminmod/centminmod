run_letsdebug() {
    letsdebug_domain=$1
    if [ ! -f /usr/bin/jq ]; then
        yum -q -y install jq
    fi
    letsdebug_id=$(curl -s --data "{\"method\":\"http-01\",\"domain\":\"$letsdebug_domain\"}" -H 'content-type: application/json' https://letsdebug.net | jq -r '.ID')
    sleep 6
    echo
    curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq | tee "${CENTMINLOGDIR}/letsdebug-${letsdebug_domain}-${DT}.log"
    echo
}