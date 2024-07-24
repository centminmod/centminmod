<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Function to convert human-readable sizes to bytes
function humanToBytes($size) {
    $units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB'];
    $number = substr($size, 0, -2);
    $suffix = strtoupper(substr($size, -2));

    if (is_numeric(substr($suffix, 0, 1))) {
        return preg_replace('/[^\d]/', '', $size);
    }

    $exponent = array_flip($units)[$suffix] ?? null;
    if ($exponent === null) {
        return null;
    }

    return $number * (1024 ** $exponent);
}

// Set the memory limit from command line argument or use default
$memoryLimit = isset($argv[1]) ? $argv[1] : '1024M';
$memoryLimitBytes = humanToBytes($memoryLimit);

ini_set('memory_limit', $memoryLimit);

echo "Current memory limit: " . ini_get('memory_limit') . "\n";

// Array to store data
$data = [];

// Consume memory until we reach close to the limit
try {
    while (true) {
        $data[] = str_repeat('A', 1024 * 1024); // Allocate 1MB per iteration
        if (count($data) % 10 == 0) {
            $usedMemory = memory_get_usage(true);
            $percentUsed = ($usedMemory / $memoryLimitBytes) * 100;
            echo "Memory used: " . number_format($usedMemory) . " bytes (" . number_format($percentUsed, 2) . "%)\n";
        }
    }
} catch (Error $e) {
    echo "Caught error: " . $e->getMessage() . "\n";
}

echo "This line should be reached after memory exhaustion.\n";