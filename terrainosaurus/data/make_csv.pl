#!/usr/bin/perl

use csv_tools;

$data = { };
@classes = (0);
$name;
@ignore = ();

# Read each CSV file specified on the command line
while (@ARGV) {
    $file = shift;
    if ($file eq '--') {
        push(@classes, 0);
    } elsif ($file eq '-i') {           # Modify ignored lines
        my $lines = shift;
        @ignore = split /,/, $lines;
    } elsif ($file eq '-n') {           # Set output file prefix
        $name = shift;
    } else {
        $data->{$file} = read_csv($file, \@ignore);
        $data->{$file}->{class} = @classes;
        $classes[$#classes] += @{$data->{$file}->{data}};
    }
}

# Print out which lines we're ignoring from the file
print STDERR "Ignoring lines: ";
if (@ignore == 0) {
    print STDERR "<none>";
}
for (my $i = 0; $i < @ignore; ++$i) {
    if ($i % 10 == 0) {
        print STDERR "\n\t";
    }
    printf STDERR "%4d ", $ignore[$i];
}
print STDERR "\n\n";


# Print out the number of samples for each class
print STDERR @classes . " classes found:\n\t";
for (my $i = 0; $i < @classes; ++$i) {
    printf STDERR "%4d ", $i;
}
print STDERR "\n\t " . ("-----" x @classes) . "\n\t";
for (my $i = 0; $i < @classes; ++$i) {
    printf STDERR "%4d ", $classes[$i];
}
print STDERR "\n\n";


# Trim out degenerate fields and normalize orders of magnitude
$data = normalize_dataset($data);


# Now output CSV files in the matlab format for each terrain-type
write_matlab($data, $name);
