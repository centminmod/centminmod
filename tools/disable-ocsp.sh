#!/bin/bash

disable_ocsp_stapling() {
    local nginx_conf_dir="/usr/local/nginx/conf/conf.d"
    local files_modified=0
    local backup_timestamp=$(date +%Y%m%d_%H%M%S)
    
    echo "Searching for Nginx vhosts with OCSP stapling enabled..."
    
    # Find all .conf files that contain ssl_stapling on or ssl_stapling_verify on
    while IFS= read -r -d '' file; do
        echo "Processing: $file"
        
        # Check if file contains the target settings in 'on' state
        if grep -q "ssl_stapling on\|ssl_stapling_verify on" "$file"; then
            echo "  Found OCSP stapling enabled in: $(basename "$file")"
            
            # Create backup with comment header
            local backup_file="${file}.bak.${backup_timestamp}"
            
            # Add comment header to backup file
            cat > "$backup_file" << EOF
# BACKUP CREATED: $(date '+%Y-%m-%d %H:%M:%S %Z')
# REASON: Automatic backup before disabling OCSP stapling
# CONTEXT: Let's Encrypt ending OCSP support - https://letsencrypt.org/2024/12/05/ending-ocsp/
# CHANGES: ssl_stapling on -> ssl_stapling off, ssl_stapling_verify on -> ssl_stapling_verify off
# ORIGINAL FILE: $file
# SCRIPT: disable_ocsp_stapling function
#
EOF
            
            # Append original file content to backup
            cat "$file" >> "$backup_file"
            
            echo "  Created backup with header: $(basename "$backup_file")"
            
            # Replace ssl_stapling on with ssl_stapling off
            sed -i 's/ssl_stapling on;/ssl_stapling off;/g' "$file"
            
            # Replace ssl_stapling_verify on with ssl_stapling_verify off
            sed -i 's/ssl_stapling_verify on;/ssl_stapling_verify off;/g' "$file"
            
            echo "  Updated OCSP stapling settings to 'off'"
            ((files_modified++))
        else
            echo "  No OCSP stapling 'on' settings found in: $(basename "$file")"
        fi
        
    done < <(find "$nginx_conf_dir" -name "*.conf" -type f -print0)
    
    if [ $files_modified -gt 0 ]; then
        echo ""
        echo "Modified $files_modified file(s). Testing Nginx configuration..."
        
        # Test nginx configuration
        if nginx -t >/dev/null 2>&1; then
            echo "Nginx configuration test passed. Reloading Nginx..."
            systemctl reload nginx >/dev/null 2>&1
            
            if [ $? -eq 0 ]; then
                echo "Nginx reloaded successfully!"
                echo ""
                echo "Summary:"
                echo "- Files modified: $files_modified"
                echo "- OCSP stapling disabled for all affected vhosts"
                echo "- Nginx configuration tested and reloaded"
                echo "- Backups created with explanatory headers"
            else
                echo "Error: Failed to reload Nginx. Please check manually."
                return 1
            fi
        else
            echo "Error: Nginx configuration test failed. Please check the configuration manually."
            echo "Backups were created with timestamp suffix for rollback if needed."
            return 1
        fi
    else
        echo "No files required modification. All vhosts already have OCSP stapling disabled or don't use it."
    fi
}

# Call the function
disable_ocsp_stapling