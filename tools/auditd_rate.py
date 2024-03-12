#!/usr/bin/env python3
import re
from collections import defaultdict
from datetime import datetime
import os
import platform
import subprocess

# Attempt to import numpy for summary statistics
try:
    import numpy as np
    numpy_available = True
except ImportError:
    numpy_available = False

# Path to the audit log file
log_file_path = '/var/log/audit/audit.log'

# Regular expression to match the timestamp in audit log entries
timestamp_regex = re.compile(r'msg=audit\((\d+\.\d+):')

# Dictionary to hold the count of events per second
events_per_second = defaultdict(int)

# Check if the log file exists and is readable
if not os.path.isfile(log_file_path):
    print(f"Error: The log file {log_file_path} does not exist.")
elif not os.access(log_file_path, os.R_OK):
    print(f"Error: The log file {log_file_path} is not readable.")
else:
    try:
        # Process the log file
        with open(log_file_path, 'r') as log_file:
            for line in log_file:
                match = timestamp_regex.search(line)
                if match:
                    # Extract the timestamp, rounded to the nearest second
                    timestamp = round(float(match.group(1)))
                    # Increment the event count for this second
                    events_per_second[timestamp] += 1

        # Sort and display the results
        for timestamp in sorted(events_per_second):
            datetime_str = datetime.utcfromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')
            print(f"{datetime_str}: {events_per_second[timestamp]} events")

        # Summary statistics with numpy
        if numpy_available and events_per_second:
            print("\nSummary Statistics:")
            event_counts = list(events_per_second.values())
            print(f"Min events: {np.min(event_counts)}")
            print(f"Average events: {round(np.mean(event_counts), 2)}")  # Rounded
            print(f"Max events: {np.max(event_counts)}")
            # Rounded percentiles
            print(f"90th percentile: {round(np.percentile(event_counts, 90), 2)}")
            print(f"95th percentile: {round(np.percentile(event_counts, 95), 2)}")
            print(f"99th percentile: {round(np.percentile(event_counts, 99), 2)}")
        elif not numpy_available:
            print("\nNumPy is not installed. Summary statistics will not be displayed.")
            dist_version = int(platform.linux_distribution()[1].split('.')[0])
            if dist_version < 8:
                print("For CentOS 7/RHEL 7, you can install NumPy with: 'sudo yum install python36-numpy'")
            else:
                print("For EL8 (CentOS 8/RHEL 8) and EL9, use: 'sudo dnf install python3-numpy'")

    except Exception as e:
        print(f"An error occurred while processing the file: {e}")
