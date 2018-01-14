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
#   data    --  a 2D array (rows x columns) of the values
#   file    --  the filename of the source CSV file
#   labels  --  an array of text labels read from the file (or undef if not present)
#   rows    --  an integer specifying how many rows of data were in the file
#               (labels do not count as a row)
#   columns --  an integer specifying the number of columns in a row
# The first argument is the filename. The second is an optional list of lines
# to in the file to ignore (starting from one). Note that this is the actual
# line number in the file, not the logical "row" number. So if the first line
# in the file is the column labels and you ignore line #1, it will be as
# though the file contained no labels. If this argument is undefined, then
# no lines will be ignored.

sub read_csv($@) {
    my $file = $_[0];
    my @ignore = @{$_[1]};
    my $labels, $data = [], $columns = 0, $lines = 0;

    if (open INFILE, "$file") {         # Open the file

LINE:   while (<INFILE>) {                  # For each line
            chomp;                              # Take off the newline
            $lines++;

            # See if this is a line we're supposed to ignore
            for (my $i = 0; $i < @ignore; ++$i) {
                if ($ignore[$i] == $lines) {
                    next LINE;
                }
            }

            my $row = [split /,/, $_];
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
        'rows'      => scalar(@$data),
        'columns'   => $columns,
    };
}


# Write a pair of CSV files in the appropriate format for the Statistical
# Pattern Recognition toolbox for Matlab. The first file ends in ".data"
# and contains the data for all samples, with rows corresponding to fields
# and columns corresponding to samples:
#       Field1: S1F1, S2F1, S3F1...
#       Field2: S1F2, S2F2, S3F2...
#
# The second file ends in ".class" and contains the class membership of each
# sample as an integer.
#
# The first argument is a reference to the hash of samples.
#
# The second argument controls whether to generate a 2-class or an N-class
# dataset. If undefined, then N sets of .data/.class/.label files are created, each
# named based on one of the datasets, which is made class #1, and everything
# else class #2. Otherwise, then only one set files is created, named
# $name.data and $name.class, with each dataset in its own class.
sub write_matlab(%$) {
    my ($data, $name) = @_;
    my $labels = (values(%$data))[0]->{'labels'};

    if (! defined($name)) {
        foreach my $key (keys(%$data)) {
            my $basename = $key;
            $basename =~ s/\.dem.*$//;
            my %classes;
            foreach my $sample (keys(%$data)) {
                # Class 1 only if we came from $basename.dem...2 otherwise
                $classes{$sample} = 2 - ($sample eq $key);
            }
            write_csv_set($labels, $data, \%classes, $basename);
        }

    } else {
        my $basename = $name;
        my %classes;
        my $label = 1;
        foreach my $sample (keys(%$data)) {
            # Copy over the class label from the input
            $classes{$sample} = $data->{$sample}->{class};
        }
        write_csv_set($labels, $data, \%classes, $basename);
    }
}


sub write_csv_set(@%%$) {
    my ($labels, $data, $classes, $basename) = @_;

    my $columns = @$labels;

    open LABELFILE, ">$basename.label" or
        die "Could not open lable file $basename.label for writing";
    my $text;
    foreach my $label (@$labels) {
        $text .= "$label,";
    }
    chop($text);
    print LABELFILE "$text\n";
    close LABELFILE;

    open DATAFILE, ">$basename.data" or
        die "Could not open data file $basename.data for writing";
    for (my $i = 0; $i < $columns; ++$i) {
        my $text;
        foreach my $sample (keys(%$data)) {
            for (my $row = 0; $row < $data->{$sample}->{'rows'}; ++$row) {
                $text .= "$data->{$sample}->{data}[$row][$i],";
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
        for (my $row = 0; $row < $data->{$sample}->{'rows'}; ++$row) {
            $text .= "$classes->{$sample},";
        }
    }
    chop($text);
    print CLASSFILE "$text\n";
    close CLASSFILE;
}



# This subroutine takes a hash mapping labels -> CSV structures and returns
# a similar hash with all fields that are degenerate across the dataset
# removed (i.e., where all samples have identical values) and all
# non-degenerate fields normalized according to their order of magnitude
# (e.g., if the largest value for a fields is 1.3e20, all values will be
# divided by 1.0e20).
# HACK -- now we're normalizing to [0,1]
sub normalize_dataset(%) {
    my $data = $_[0];

    # These will be our references for detecting conflicting column
    # definitions and degenerate fields
    my $ref = (values(%$data))[0];
    my @ref = values(%$data);
    my $refColumns  = (values(%$data))[0]->{'columns'};
    my @refData     = @{(values(%$data))[0]->{'data'}[0]};
    my $refFile, @refLabels;
    foreach my $value (values(%$data)) {
        if (defined($value->{'labels'})) {
            $refFile    = $value->{'file'};
            @refLabels  = @{$value->{'labels'}};
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
        for (my $row = 0; $row < $record->{rows}; ++$row) {
            for (my $i = 0; $i < $refColumns; ++$i) {
                if ($refData[$i] != $record->{data}[$row][$i]) {
                    $degenerate[$i] = 0;
                }
                my $size = abs($record->{data}[$row][$i]);
                if ($size > $max[$i]) {
                    $max[$i] = abs($record->{data}[$row][$i]);
                } elsif ($size < $min[$i] and $size > 0) {
                    $min[$i] = abs($record->{data}[$row][$i]);
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
    print STDERR ('-' x 78) . "\n";
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
            'rows'      => $oldRecord->{rows},
            'class'     => $oldRecord->{class},
            'columns'   => 0,
            'labels'    => [ ],
            'data'      => [ ]
        };
        # Keep only the lables for the columns we use
        for (my $i = 0; $i < $refColumns; ++$i) {
            if (! $degenerate[$i] and $omit{$refLabels[$i]} != 1) {
                push(@{$newRecord->{labels}}, $refLabels[$i]);
                $newRecord->{columns}++;
            }
        }
        # Condense the data
        for (my $row = 0; $row < $oldRecord->{rows}; ++$row) {
            my $written = 0;
            my @rowData = ();
            for (my $i = 0; $i < $refColumns; ++$i) {
                if (! $degenerate[$i] and $omit{$refLabels[$i]} != 1) {
                    if (1) {
                        push(@rowData, $oldRecord->{data}[$row][$i] / $max[$i]);
                    } elsif ($orderRange > 2) {
                        push(@rowData, log($oldRecord->{data}[$row][$i]));
                    } else {
                        push(@rowData, $oldRecord->{data}[$row][$i] / $maxOrder[$i]);
                    }
                    $written++;
                }
            }
            push(@{$newRecord->{data}}, [ @rowData ]);
        }
        $normalized{$key} = $newRecord;
    }

    # Return what we built
    return \%normalized;
}


return 1;   # Truth!
