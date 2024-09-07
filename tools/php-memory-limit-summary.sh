#!/bin/bash
###################################################################
# tool to summarize php error log memory_limit reached php files 
# for log entries for the past 24hrs
###################################################################
set -e

LOG_FILE="/var/log/php-fpm/www-php.error.log"
SUMMARY_FILE="/var/log/php-fpm/php_memory_limit_summary.log"
DATE_24_HOURS_AGO=$(date -d "24 hours ago" '+%Y-%m-%d %H:%M:%S')
TEMP_DIR=$(mktemp -d)
trap 'rm -rf "$TEMP_DIR"' EXIT

# Function to format bytes
format_bytes() {
    local bytes=$1
    if ((bytes >= 1073741824)); then
        printf "%.2f GB" "$(echo "scale=2; $bytes / 1073741824" | bc)"
    elif ((bytes >= 1048576)); then
        printf "%.2f MB" "$(echo "scale=2; $bytes / 1048576" | bc)"
    elif ((bytes >= 1024)); then
        printf "%.2f KB" "$(echo "scale=2; $bytes / 1024" | bc)"
    else
        printf "%d bytes" "$bytes"
    fi
}

# Process log file
awk -v date_24h_ago="$DATE_24_HOURS_AGO" '
    $1" "$2 >= date_24h_ago {
        if ($0 ~ /PHP Fatal error.*Allowed memory size of .* bytes exhausted/) {
            match($0, /Allowed memory size of ([0-9]+) bytes exhausted/, arr)
            exhausted_size = arr[1]
            match($0, /tried to allocate ([0-9]+) bytes/, arr)
            allocate_size = arr[1]
            match($0, /in (.*) on line/, arr)
            file_name = arr[1]
            gsub(/\//, "_", file_name)
            print exhausted_size, allocate_size > "'$TEMP_DIR'/"file_name".tmp"
        }
    }
' "$LOG_FILE"

# Generate summary
{
    echo "Summary for the last 24 hours:"
    echo "-----------------------------------"
    for tmp_file in "$TEMP_DIR"/*.tmp; do
        if [[ -f "$tmp_file" ]]; then
            file_name=$(basename "$tmp_file" .tmp | tr '_' '/')
            echo "File: $file_name"
            
            count=$(wc -l < "$tmp_file")
            total_exhausted=0
            total_allocate=0
            min_exhausted=""
            max_exhausted=0
            min_allocate=""
            max_allocate=0
            
            while read -r exhausted_size allocate_size; do
                total_exhausted=$((total_exhausted + exhausted_size))
                total_allocate=$((total_allocate + allocate_size))
                
                if [[ -z "$min_exhausted" ]] || ((exhausted_size < min_exhausted)); then
                    min_exhausted=$exhausted_size
                fi
                if ((exhausted_size > max_exhausted)); then
                    max_exhausted=$exhausted_size
                fi
                
                if [[ -z "$min_allocate" ]] || ((allocate_size < min_allocate)); then
                    min_allocate=$allocate_size
                fi
                if ((allocate_size > max_allocate)); then
                    max_allocate=$allocate_size
                fi
            done < "$tmp_file"
            
            avg_exhausted=$((total_exhausted / count))
            avg_allocate=$((total_allocate / count))
            
            echo "  Count: $count"
            printf "  Exhausted Memory - Min: %s, Max: %s, Avg: %s\n" \
                "$(format_bytes "$min_exhausted")" \
                "$(format_bytes "$max_exhausted")" \
                "$(format_bytes "$avg_exhausted")"
            printf "  Tried to Allocate - Min: %s, Max: %s, Avg: %s\n" \
                "$(format_bytes "$min_allocate")" \
                "$(format_bytes "$max_allocate")" \
                "$(format_bytes "$avg_allocate")"
            echo ""
        fi
    done
} > "$SUMMARY_FILE"

# Display the summary
cat "$SUMMARY_FILE"