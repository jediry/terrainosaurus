#!/usr/bin/perl

package csv_tools;


BEGIN {
    use Exporter();
    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
    $VERSION        = 1.00;
    @ISA            = qw(Exporter);
    @EXPORT         = qw(&read_csv &write_matlab &normalize_dataset);
    %EXPORT_TAGS    = ( );
    @EXPORT_OK      = ( );
}


# The fields we don't want to include
%omit = (
#    'SumElevation' => 1,
#    'SumSlope' => 1,
#    'SumEdgeLength' => 1,
#    'SumEdgeScale' => 1,
#    'SumEdgeStrength' => 1,
#    'SumRidgeLength' => 1,
#    'SumRidgeScale' => 1,
#    'SumRidgeStrength' => 1,
);


# Load a CSV file into memory and return a structure describing the data
# contained therein. The structure has the following fields:
#   data    --  a 2D array (lines x columns) of the values
#   file    --  the filename of the source CSV file
#   labels  --  an array of text labels read from the file (or undef if not present)
#   lines   --  an integer specifying how many lines were in the file
#   columns --  an integer specifying the number of columns on a line
sub read_csv($) {
    my $file = $_[0];
    my $labels, $data = [], $columns = 0, $lines = 0;

    if (open INFILE, "$file") {         # Open the file

        while (<INFILE>) {                  # For each line
            chomp;                              # Take off the newline
            my $row = [split /,/, $_];
            $lines++;
            if (@$row != $columns) {            # Ensure consistent # fields
                if ($columns == 0) {                # First time
                    $columns = @$row;
                } else {
                    die "Inconsistent number of fields in file $file:\n" .
                        "\tLine 1 has $columns\n" .
                        "\tLine $lines has " . scalar(@$row) . "\n";
                }
            }

            if ($row->[0] =~ /[a-zA-Z]/) {  # Decide what this is
                $labels = $row;                 # It's column labels
                $lines--;                       # Oops...this wasn't a line
            } else {
                push(@$data, $row);             # It's a new row of data
            }
        }

        close INFILE;
        
#        print STDERR "Successfully read CSV file $file:\n";
#        print STDERR "\tlabels: " . join(',', @$labels) . "\n";
#        print STDERR "\tlines: " . $lines . "\n";
#        for (my $i = 0; $i < @data; ++$i) {
#            print STDERR "\tline $i: " . scalar(@$data[$i]) . " fields\n";
#        }
#        print STDERR "# columns is $columns\n\n";

    } else {                            # If there was an error
        warn "Unable to open file $file";   # Complain loudly
    }
    
    # Return a structure containing this data
    return {
        'file'      => $file,
        'labels'    => $labels,
        'data'      => $data,
        'lines'     => $lines,
        'columns'   => $columns,
    };
}


# Write two CSV files for each record, in the appropriate format for the
# Statistical Pattern Recognition toolbox for Matlab. The first file ends
# in ".data" and contains the data for all samples, with rows corresponding
# to fields and columns corresponding to samples:
#       Field1: S1F1, S2F1, S3F1...
#       Field2: S1F2, S2F2, S3F2...
#
# The second file ends in ".class" and contains the class membership of each
# sample as an integer.
#
# The argument is a reference to the hash of samples. Which class they belong
# to is controlled by the 'class' field of the samples.
sub write_matlab(%) {
    my $data = $_[0];
    my $columns = (values(%$data))[0]->{columns};
    foreach my $key (keys(%$data)) {
        my $basename = $key;
        $basename =~ s/\.dem.*$//;

        open DATAFILE, ">$basename.data" or
            die "Could not open data file $basename.data for writing";
        for (my $i = 0; $i < $columns; ++$i) {
            my $text;
            foreach my $sample (keys(%$data)) {
                for (my $line = 0; $line < $data->{$sample}->{lines}; ++$line) {
                    $text .= "$data->{$sample}->{data}[$line][$i],";
                }
            }
            chop($text);
            print DATAFILE "$text\n";
        }
        close DATAFILE;

        open CLASSFILE, ">$basename.class" or
            die "Could not open class file $basename.class for writing";
        my $text;
        foreach my $sample (keys(%$data)) {
            for (my $line = 0; $line < $data->{$sample}->{lines}; ++$line) {
                $text .= (2 - ($sample eq $key)) . ',';
            }
        }
        chop($text);
        print CLASSFILE "$text\n";
        close CLASSFILE;
    }
}


# This subroutine takes a hash mapping labels -> CSV structures and returns
# a similar hash with all fields that are degenerate across the dataset
# removed (i.e., where all samples have identical values) and all
# non-degenerate fields normalized according to their order of magnitude
# (e.g., if the largest value for a fields is 1.3e20, all values will be
# divided by 1.0e20).
sub normalize_dataset(%) {
    my $data = $_[0];

    # These will be our references for detecting conflicting column
    # definitions and degenerate fields
    my $ref = (values(%$data))[0];
    my @ref = values(%$data);
    my $refColumns  = (values(%$data))[0]->{columns};
    my @refData     = @{(values(%$data))[0]->{data}[0]};
    my $refFile, @refLabels;
    foreach my $value (values(%$data)) {
        if (defined($value->{labels})) {
            $refFile    = $value->{file};
            @refLabels  = @{$value->{labels}};
            last;
        }
    }


    # Check each record, ensuring that column labels are consistent,
    # checking for degenerate columns, and finding the order of magnitude
    # for each column.
    my @max = (0) x $refColumns, @min = (1.0e300) x $refColumns;
    my @degenerate = (1) x $refColumns;
    foreach my $key (keys(%$data)) {
        my $record = ${$data}{$key};

        # Check the column count
        if ($refColumns != $record->{columns}) {
            die "Disagreement over the number of columns:\n" .
                "\t$refFile says $refColumns\n" .
                "\t$key says $record->{columns}\n";
        }

        # If this record has labels, make sure they're consistent
        if (defined($record->{labels}) and defined($refLabels)) {
            for (my $i = 0; $i < $refColumns; ++$i) {
                if ($refLabels[$i] ne $record->{labels}->[$i]) {
                    die "Disagreement over the meaning of column $i:\n" .
                        "\t$refFile says '$refLabels[$i]'\n" .
                        "\t$key says '$record->{labels}[$i]'\n";
                }
            }
        }

        # Any fields in this record that differ from the reference data are
        # non-degenerate, so flag them so. Also, we're looking for the largest
        # value in each field across the whole dataset so we can establish the
        # order of magnitude for that field.
        #
        # Since a record may have multiple rows, we must scan all of them.
        for (my $line = 0; $line < $record->{lines}; ++$line) {
            for (my $i = 0; $i < $refColumns; ++$i) {
                if ($refData[$i] != $record->{data}[$line][$i]) {
                    $degenerate[$i] = 0;
                }
                my $size = abs($record->{data}[$line][$i]);
                if ($size > $max[$i]) {
                    $max[$i] = abs($record->{data}[$line][$i]);
                } elsif ($size < $min[$i] and $size > 0) {
                    $min[$i] = abs($record->{data}[$line][$i]);
                }
            }
        }
    }

    # Calculate the order of magnitude for each non-degenerate column as the
    # smallest power of ten larger than the largest value for that column.
    # This has the effect of compressing each column into the range [0,1)
    my @minOrder, @maxOrder, @orderRange, @nondegenerateLabels;
    my $nondegenerateColumnCount = 0;
    for (my $i = 0; $i < $refColumns; ++$i) {
        if ($degenerate[$i] or $omit{$refLabels[$i]}) {
            $minOrder[$i] = $maxOrder[$i] = 1;
        } else {
            $nondegenerateColumnCount++;
            push(@nondegenerateLabels, $refLabels[$i]);
            $minOrder[$i] = 10 ** int(log($min[$i]) / log(10) + ($min[$i] > 1.0));
            $maxOrder[$i] = 10 ** int(log($max[$i]) / log(10) + ($max[$i] > 1.0));
            $orderRange[$i] = int(log($maxOrder[$i] / $minOrder[$i]) / log(10));
        }
    }
    
    printf STDERR "%-25s%-10s%-10s%-10s%-10s%-10s\n", "column", "min", "max", "min-order", "max-order", "orders";
    for (my $i = 0; $i < $refColumns; ++$i) {
        if ($omit{$refLabels[$i]} == 1) {
            printf STDERR "%-25s%-20s\n", $refLabels[$i], "     << omitted>>";
        } elsif ($degenerate[$i]) {
            printf STDERR "%-25s%-20s\n", $refLabels[$i], "   << degenerate >>";
        } else {
            printf STDERR "%-25s%-10.3g%-10.3g%-10.1g%-10.1g%10d\n",
                $refLabels[$i], $min[$i], $max[$i], $minOrder[$i], $maxOrder[$i], $orderRange[$i];
        }
    }


    # Finally, prune and normalize the data
    my %normalized;
    foreach my $key (keys(%$data)) {
        my $oldRecord = $data->{$key};
        my $newRecord = {
            'file'      => $oldRecord->{file},
            'lines'     => $oldRecord->{lines},
            'columns'   => $nondegenerateColumnCount,
            'labels'    => $nondegenerateLabels,
            'data'      => [ ]
        };
        for (my $line = 0; $line < $oldRecord->{lines}; ++$line) {
            my $written = 0;
            my @row = ();
            for (my $i = 0; $i < $refColumns; ++$i) {
                if (! $degenerate[$i] and $omit{$refLabels[$i]} != 1) {
                    if ($orderRange > 2) {
                        push(@row, log($oldRecord->{data}[$line][$i]));
                    } else {
                        push(@row, $oldRecord->{data}[$line][$i] / $maxOrder[$i]);
                    }
                    $written++;
                }
            }
            push(@{$newRecord->{data}}, [ @row ]);
        }

        $normalized{$key} = $newRecord;
    }

    # Return what we built
    return \%normalized;
}


return 1;   # Truth!
