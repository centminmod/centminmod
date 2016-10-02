#!/bin/bash
TIMEZONES='UTC Australia/Brisbane America/Los_Angeles America/Chicago America/New_York America/Montreal Europe/London Europe/Berlin Asia/Bangkok Asia/Ho_Chi_Minh Asia/Jakarta Asia/Kuala_Lumpur Asia/Singapore'

for t in $TIMEZONES; do
    echo -e "$(TZ=$t date)\t[$t]";
done