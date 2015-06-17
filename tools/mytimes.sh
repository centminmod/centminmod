#!/bin/bash
TIMEZONES='UTC Australia/Brisbane America/Los_Angeles America/Chicago America/New_York Europe/London'

for t in $TIMEZONES; do
    echo -e "$(TZ=$t date)\t[$t]";
done