#!/usr/bin/perl

# Configuration
$DATA_DIR       = '/mnt/data/scratch/gis';
$CELL_DB        = "$DATA_DIR/24kgrid.dbf";
$CACHE_DIR      = "$DATA_DIR/web_cache";
$OUTPUT_DB      = "$DATA_DIR/download.db";

# Attributes of each cell
%cells;


# Read the cell grid database
use XBase;

print "Reading cell attributes from $CELL_DB\n";
my $db = new XBase("$CELL_DB") or die $XBase->errstr;
my $records = $db->get_all_records("NAME", "STATE", "LONG", "LAT");
#my $records = [];
foreach (@$records) {
    my ($name, $state, $lat, $long) = @$_;
    $cells{"$name, $state"} = {
        'name'      => $name,
        'state'     => $state,
        'longitude' => $long,
        'latitude'  => $lat,
    };
#    print STDERR "Record $name, $state\tat $lat/$long\n";
}
$db->close();

# Now scan the web cache for USGS IDs for each cell
print "Scanning downloaded files and matching cells to filenames\n";
@cellFiles = `find '$CACHE_DIR' -name "group4-3.html"`;

foreach (@cellFiles) {
    chomp;
    my $file = $_;
    my $county = `grep -m 1 County $file`;
    chomp($county);
    $county =~ s/^.*>(.*) County<.*$/\1/;
    my @matches = `grep -B 3 GCI-DEM24 $file`;
    my $mode = 0;
    my $name, $state, $cellID, $catalogID;
    foreach (@matches) {
        if ($mode == 0) {
            if (/<b>([^,]+), (..) \(([0-9a-z]+)\)<\/b>/) {
                $name = $1;
                $state = $2;
                $cellID = $3;
                $mode = 1;
            }
        } elsif ($mode == 1) {
            if (/CATALOGID.*="(GCI-DEM24[^"]+)"/) {
                $catalogID = $1;
                my $key = "$name, $state";
                if (! exists($cells{$key})) {
                    print STDERR "$file: No cell named $key exists...creating\n";
                    $cells{$key} = {
                        'name'  =>  $name,
                        'state' =>  $state,
                    };
                }
                $cells{$key}{'county'} = $county;
                $cells{$key}{'catalog-id'} = $catalogID;
                $cells{$key}{'cell-id'} = $cellID;
                $mode = 0;
            }
        }
    }
}
@cellFiles = ();


# Finally, dump it all out to the file
print "Generating filename database $OUTPUT_DB\n";
open(OUTFILE, ">$OUTPUT_DB") or die "Unable to open output file\n";
foreach (values(%cells)) {
    print OUTFILE "$_->{name}|$_->{state}|$_->{county}|$_->{latitude}|"
                . "$_->{longitude}|$_->{'catalog-id'}|$_->{'cell-id'}|\n";
}
close OUTFILE;
print keys(%cells) . " records written\n";

