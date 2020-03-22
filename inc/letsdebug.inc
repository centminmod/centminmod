run_letsdebug() {
    letsdebug_domain=$1
    if [ ! -f /usr/bin/jq ]; then
        yum -q -y install jq
    fi
    letsdebug_id=$(curl -s --data "{\"method\":\"http-01\",\"domain\":\"$letsdebug_domain\"}" -H 'content-type: application/json' https://letsdebug.net | jq -r '.ID')
    sleep 6
    letsdebug_status=$(curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq -r '.status')
    if [[ "$letsdebug_status" != 'Complete' ]]; then
        sleep 4
        echo "waiting on letsdebug.net API check completion..."
        letsdebug_status=$(curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq -r '.status')
    fi
    if [[ "$letsdebug_status" != 'Complete' ]]; then
        sleep 4
        echo "waiting on letsdebug.net API check completion..."
        letsdebug_status=$(curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq -r '.status')
    fi
    if [[ "$letsdebug_status" != 'Complete' ]]; then
        sleep 4
        echo "waiting on letsdebug.net API check completion..."
        letsdebug_status=$(curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq -r '.status')
    fi
    echo
    curl -s -H 'accept: application/json' "https://letsdebug.net/$letsdebug_domain/${letsdebug_id}" | jq | tee "${CENTMINLOGDIR}/letsdebug-${letsdebug_domain}-${DT}.log"
    echo
}