#!/usr/bin/perl

use csv_tools;


# Read each CSV file specified on the command line
$data = { };
$class = 0;
while (@ARGV) {
    $file = shift;
    $data->{$file} = read_csv($file);
    $data->{$file}->{class} = $class++;
}


# Trim out degenerate fields and normalize orders of magnitude
$data = normalize_dataset($data);


# Now output each CSV files in the matlab format for each terrain-type
foreach $file (keys(%$data)) {
    write_matlab($data);
}
