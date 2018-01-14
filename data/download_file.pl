#!/usr/bin/perl

use strict;


my $PROG_DIR       = $0;
$PROG_DIR =~ s/\/[^\/]+$//;
my $DATA_DIR       = "/mnt/data/scratch/gis";
my $SDTS_DIR       = "$DATA_DIR/sdts";
my $DEM_DIR        = "$DATA_DIR/dem";
my $CACHE_DIR      = "$DATA_DIR/web_cache";
my $COOKIE_FILE    = "$CACHE_DIR/cookies.txt";
my $TEMP_DIR       = "/tmp";
my $FILENAME_DB    = "$DATA_DIR/download.db";
my $AGENT_STRING   = "Mozilla/5.0 (compatible; Konqueror/3.3; Linux) (KHTML,like Gecko)";
#my $AGENT_STRING   = "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7) Gecko/20040626 Firefox/0.9.1";
my $TIMEOUT        = 1800;
my $USERNAME       = "JediRy";
my $PASSWORD       = "gocorel";
my $PREFERRED_LOD  = 30;

my $SESSION_LEVEL      = 1;
my $SESSION_ID         = '';
my $LAST_ACTIVITY_TIME = ''; #time();
my $BROWSER_ID         = '';


# Mapping of words to canonical direction names
my %directionNames = (
    'north'     => 'north',
    'northwest' => 'northwest',
    'west'      => 'west',
    'southwest' => 'southwest',
    'south'     => 'south',
    'southeast' => 'southeast',
    'east'      => 'east',
    'northeast' => 'northeast',
    'around'    => 'around',
    'n'         => 'north',
    'nw'        => 'northwest',
    'w'         => 'west',
    'sw'        => 'southwest',
    's'         => 'south',
    'se'        => 'southeast',
    'e'         => 'east',
    'ne'        => 'northeast',
    'a'         => 'around',
);


# Create a ReadLine interface to get nice terminal interface capabilities
use Term::ReadLine;
my $term = new Term::ReadLine or die "Unable to create ReadLine object\n";


# Start with the terminal in a reasonable read mode
use Term::ReadKey;
ReadMode(1);


# Find a parent process that is an instance of Konqueror, since that's who
# we'll be talking to via DCOP to do the actual web stuff.
my $ppid = getppid();
$BROWSER_ID = int(`ps -p $ppid --no-headers -o ppid`);
`ps -p $BROWSER_ID --no-headers -o args` =~ /konqueror/ or
    die "Grandparent process is not an instance of Konqueror. This program "
      . "must be launched from the Konqueror embedded terminal emulator\n";

# Load the cell DB and enter the command loop
my %cells = load_cell_db($FILENAME_DB);
print "Loaded data for " . keys(%cells) . " cells\n";
while (1) {
    my $line = $term->readline('gis> ');
    process_command($line);
    print "\n";
}

#### End of main script ####


sub process_command($) {
    my @cmd = split / /, $_[0], 2;

    # Quit
    if ($cmd[0] eq 'quit') {
        quit();

    # Just login
    } elsif ($cmd[0] eq 'login') {
        return login($USERNAME, $PASSWORD);
        
    # Get a cell
    } elsif ($cmd[0] eq 'get') {
        my $cells = find_cell_records($cmd[1]);
        my $lod = $PREFERRED_LOD;

        foreach my $cell (@$cells) {
            if (! -e dem_filename($cell, $lod)) {
                if (! -e sdts_filename($cell, $lod)) {
                    ($lod = download_sdts($cell, $lod)) or next;
                } else {
                    print sdts_filename($cell, $lod) . " has already been downloaded\n";
                }
                convert_sdts_to_dem($cell, $lod) or next;
            } else {
                print dem_filename($cell, $lod) . " has already been converted\n";
            }
        }
        return 1;

    # View a DEM in 3D
    } elsif ($cmd[0] eq 'view') {
        my $cells = find_cell_records($cmd[1]);
        my $lod = $PREFERRED_LOD;

        foreach my $cell (@$cells) {
            if (! -e dem_filename($cell, $lod)) {
                if (! -e sdts_filename($cell, $lod)) {
                    ($lod = download_sdts($cell, $lod)) or next;
                } else {
                    print sdts_filename($cell, $lod) . " has already been downloaded\n";
                }
                convert_sdts_to_dem($cell, $lod) or next;
            } else {
                print dem_filename($cell, $lod) . " has already been converted\n";
            }
            view_dem($cell, $lod);
        }
        return 1;

    # Set the preferred LOD
    } elsif ($cmd[0] eq 'lod') {
        if ($cmd[1] =~ /^[13]0$/) {
            $PREFERRED_LOD = $cmd[1];
            print "Setting default LOD to $cmd[1]\n";
        } else {
            print "$cmd[1] is not a legal LOD\n";
        }
        
    # Execute javascript
    } elsif ($cmd[0] eq 'js') {
        return execute_javascript($cmd[1]);

    # Silently ignore this, since Konqueror insists on sending it
    } elsif ($cmd[0] eq 'cd') {
        return 1;

    # Ignore empty lines
    } elsif ($cmd[0] eq '') {
        return 1;

    # Uh, Bob?
    } else {
        print "Unrecognized command $cmd[0]\n";
        return undef;
    }
}

sub quit() {
#    my $kid = waitpid(-1, WNOHANG);

#    print "Exiting...$kid\n";
    exit(0);
}


# Only login if too much time has passed
sub ensure_login($$) {
    my ($username, $password) = @_;

    my $rc = 1;
    if (time() - $LAST_ACTIVITY_TIME > $TIMEOUT) {
        $rc = login($username, $password);
    }
    return $rc;
}

# Do the actual login
sub login($$) {
    my ($username, $password) = @_;

    print STDERR "Logging in to Geocomm (press ESC to skip)\n";

    # This mechanism is web-browser driven (via DCOP), since I didn't
    # feel morally right in flouting Thinkburst Media's policies (which
    # forbid using an automated mechanism, since they sell advertising).
    # My solution is to automate most of the process (login and cell lookup)
    # and to manually click-to-download, since I'll still see their ads, but
    # won't have to slog thru all the tedium of click-click-click and
    # renaming all those nasty files.
    my $loginURL   = "https://secure.thinkburst.com/accounts/login.php?type=";
    my $successURL = "https://secure.thinkburst.com/accounts/status.php";
    open_url($loginURL);

    # Switch to reading single characters
    ReadMode(3);
    my $char;
    while ($char ne chr(27) and current_url() eq $loginURL) {
        $char = ReadKey(-1);
        sleep(1);
    }

    # Restore the original terminal settings
    ReadMode(1);

    # Update the timestamp and report the results
    if ($char eq chr(27)) {
        $LAST_ACTIVITY_TIME = time();
        print STDERR "Login skipped (assuming valid cookie)\n";
        return 1;
    } elsif (current_url() eq $successURL) {
        $LAST_ACTIVITY_TIME = time();
        print STDERR "Logged in successfully\n";
        return 1;
    } else {
        print STDERR "Login error\n";
        return undef;
    }


    # This is the old mechanism, based on wget. It would be more efficient,
    # but I somehow feel honor-bound to obey the terms of service,
    # especially since they're providing me with free data. #$%@ conscience!
#        wget("http://data.geocomm.com/catalog/", "$CACHE_DIR/get_cookies.html", 0,
#             "--save-headers");
#        my @cookies = `grep Set-Cookie $CACHE_DIR/get_cookies.html`;
#        foreach (@cookies) {
#            /TBMCOOKIE=([^;]+);/  and $SESSION_LEVEL=$1;
#            /TBMSESSION=([^;]+);/ and $SESSION_ID=$1;
#        }
#        print STDERR "Session: $SESSION_LEVEL $SESSION_ID\n";
#        wget("https://secure.thinkburst.com/accounts/login.php?type=", "$CACHE_DIR/fake_login.html", 1, undef);
#        wget("https://secure.thinkburst.com/accounts/login.php", "$CACHE_DIR/do_login.html", 1,
#             "--post-data=\"username=$username&password=$password\" --save-headers");
#        my @cookies = `grep Set-Cookie $CACHE_DIR/do_login.html`;
#        foreach (@cookies) {
#            /TBMCOOKIE=([^;]+);/  and $SESSION_LEVEL=$1;
#            /TBMSESSION=([^;]+);/ and $SESSION_ID=$1;
#        }
#        $rc = $SESSION_LEVEL == 2;
}


sub load_cell_db($) {
    my %cells;

    open CELL_DB, "<$_[0]" or die "Unable to open cell database $_[0]\n";
    
    while (<CELL_DB>) {
        my @fields = split /\|/;
        my $key = "$fields[0], $fields[1]";
        $cells{$key} = {
            'name'      => $fields[0],
            'state'     => $fields[1],
            'county'    => $fields[2],
            'latitude'  => $fields[3],
            'longitude' => $fields[4],
            'catalog-id'=> $fields[5],
            'cell-id'   => $fields[6],
        };
    }
    close(CELL_DB);

    return %cells;
}

# This function takes a query string describing the desired cell, constructs
# a query
sub find_cell_records($) {
    my ($criteria) = @_;

    # Construct a query hash based on the criteria string
    my %query;
    my @words = split_respecting_quotes($criteria);
    while (@words) {
        my $word = shift(@words);
        if (exists($directionNames{lc($word)})) {           # Adjacent cell
            $word = $directionNames{lc($word)};
            my $requery = '"' . join('" "', @words) . '"';
            my $ref= find_cell_records($requery);           # Find reference cell
            my @cellIDs;
            if (! defined($ref->[0])) {
                print "Unable to resolve reference cell @words\n";
                return [];
            } elsif ($word eq 'north') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  0,  1));
            } elsif ($word eq 'northwest') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1,  1));
            } elsif ($word eq 'west') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1,  0));
            } elsif ($word eq 'southwest') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1, -1));
            } elsif ($word eq 'south') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  0, -1));
            } elsif ($word eq 'southeast') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1, -1));
            } elsif ($word eq 'east') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1,  0));
            } elsif ($word eq 'northeast') {
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1,  1));
            } elsif ($word eq 'around') {
                push(@cellIDs, $ref->[0]{'cell-id'});
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  0,  1));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1,  1));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1,  0));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  1, -1));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'},  0, -1));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1, -1));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1,  0));
                push(@cellIDs, cell_relative_to($ref->[0]{'cell-id'}, -1,  1));
            }
                

            # If we got it, put it in the query
            if (! @cellIDs) {
                print "Unable to identify cell(s) $word of " . format_cell($ref->[0]) . "\n";
                return [];
            } else {
#                print "Getting " . @cellIDs . " cells\n";
                $query{'cell-id'} = \@cellIDs;
            }
            
            # Don't parse the rest of the line, since the recursive call got it
            last;
        
        } elsif ($word =~ /^([0-9]+)d(?:([0-9]+)m)?\/([0-9]+)d(?:([0-9]+)m)?$/) {   # Lat/Long in deg/min
            my ($lat, $latSub, $long, $longSub) = ($1, int($2 / 7.5), $3, int($4 / 7.5));
            defined($latSub)  or $latSub  = 0;
            defined($longSub) or $longSub = 0;
            my $id = sprintf("%02d%03d%c%d", $lat, $long, $latSub + ord('a'), $longSub + 1);
            $query{'cell-id'} = $id;
        } elsif ($word =~ /^([0-9]+)(\.[0-9]+)?\/([0-9]+)(\.[0-9]+)?$/) {   # Lat/Long in decimal
            my ($lat, $latSub, $long, $longSub) = ($1, int($2 * 8), $3, int($4 * 8));
            defined($latSub)  or $latSub  = 0;
            defined($longSub) or $longSub = 0;
            my $id = sprintf("%02d%03d%c%d", $lat, $long, $latSub + ord('a'), $longSub + 1);
            $query{'cell-id'} = $id;
        } elsif ($word =~ /^\d{5}[A-Ha-h][1-8]$/) {     # Bare cell ID
            $query{'cell-id'} = $word;
        } elsif ($word =~ /^[A-Z]{2}$/) {               # State name
            $query{'state'} = $word;
        } else {                                        # Substring search
            $query{'name'} = $word;
        }
    }
    my @matches = find_matching_cells(\%cells, \%query);
    print "  " . @matches . " matching cell(s) found\n";

    # (Possibly) ask the user which one he wanted
    my $cells = choose_cells(@matches);

    if (@{$cells} > 0) {
        print "  Selecting " . @{$cells} . " cells\n";
    } else {
        print "  No cells selected\n";
    }
    return $cells;
}

# This function takes a string and returns a list of the whitespace tokens in it,
# except that it does not split around spaces inside of double quotes.
# The surrounding double quotes are removed from around the quoted string.
sub split_respecting_quotes($) {
    my $string = $_[0];

    my $inQuotes = 0;
    my @chars = unpack('(a)*', $string);
    for (my $i = 0; $i < @chars; ++$i) {
        if ($chars[$i] eq '"') {
            $inQuotes = ! $inQuotes;
            $chars[$i] = '';
        } elsif ($chars[$i] eq ' ' and $inQuotes) {
            $chars[$i] = '#sp#';
        }
    }
    $string = pack('(a*)*', @chars);
    my @tokens = split(/\s+/, $string);
    for (my $i = 0; $i < @tokens; $i++) {
        $tokens[$i] =~ s/#sp#/ /g;          # Replace quoted spaces
        if ($tokens[$i] =~ /^\s*$/) {       # Delete empty tokens
            splice(@tokens, $i, 1);
            redo;
        }
    }
#    print "New string is $string\n";
#    foreach my $tok (@tokens) {
#        print "Token: '$tok'\n";
#    }

    return @tokens;
}


# This function takes a string representing a USGS quadrangle ID, in the
# format described below and calculates the ID of another cell which is some
# relative distance away from the reference cell. The distance is specified
# using the 'dw' and 'dn' parameters, which encode the number of grid cells
# to walk in the westerly and northerly directions, respectively. A few
# examples:
#
#   To pick the cell immediately south of the reference, dw = 0, dn = -1
#   To pick the cell three steps north and one step east, dw = -1, dn = 3
# 
# http://www5.egi.utah.edu/Geospatial_Data/FishLakeValley/24kquadindex.htm
# The meaning of USGS quadrangle IDs:
# 
# The quadrangle identification number. The first two digits represent the
# latitude in degrees north, the next three digits represent the longitude in
# degrees west, and a hyphen, and the last two characters reveal the grid cell
# code within each one-degree rectangle. To understand the grid cell code within
# the one-degree rectangle, imagine a rectangle the shape of a door. Divide the
# "door" into eight rows (A-H) and columns (1-8), the lower right corner grid
# cell code is "A1" and the upper left corner grid cell code is "H8". Each grid
# cell represents a USGS 1:24,000 topographic quadrangle, hence sixty-four per
# one-degree rectangle. Each USGS 1:100,000 topographic quadrangle is one-half
# by one degree with only two fitting within the one-degree rectangle-the upper
# half and the lower half of the "door". The lower quadrangle's grid cell code
# is "A1" and the upper quadrangle's grid cell code is "E1" (these are the only
# grid cell codes for USGS 1:100,000 topographic quadrangles). Each USGS
# 1:250,000 topographic quadrangle is one by two degrees, which, is two "doors"
# side by side. The quadrangle's grid cell code is "A1" (this is the only grid
# cell code for USGS 1:250,000 topographic quadrangles).
#
#     USGS 1:24,000         USGS 1:100,000        USGS 1:250,000  
#    _ _ _ _ _ _ _ _       _______________       _______________  
#   |_|_|_|_|_|_|_|_|H    |               |H    |               |H
#   |_|_|_|_|_|_|_|_|G    |       E1      |G    |               |G
#   |_|_|_|_|_|_|_|_|F    |               |F    |    left or    |F
#   |_|_|_|_|_|_|_|_|E    |_______________|E    |   right half  |E
#   |_|_|_|_|_|_|_|_|D    |               |D    |     of A1     |D
#   |_|_|_|_|_|_|_|_|C    |       A1      |C    |               |C
#   |_|_|_|_|_|_|_|_|B    |               |B    |               |B
#   |_|_|_|_|_|_|_|_|A    |_______________|A    |_______________|A
#    8 7 6 5 4 3 2 1       8 7 6 5 4 3 2 1       8 7 6 5 4 3 2 1  
#
# Note: We differ from this in that we don't include the hypen.
sub cell_relative_to($$$) {
    my ($refID, $dw, $dn) = @_;

    # Make sure we can parse this
    if (lc($refID) =~ /^(\d\d)(\d\d\d)([a-h])([1-8])$/) {
        my ($lat, $long, $subLat, $subLong) = ($1, $2, ord($3) - ord('a'), $4 - 1);
        
        my ($newLat, $newLong)       = ($lat, $long);
        my ($newSubLat, $newSubLong) = ($subLat + $dn, $subLong + $dw);
        while ($newSubLat >= 8) {
            $newSubLat -= 8;
            $newLat++;
        }
        while ($newSubLong >= 8) {
            $newSubLong -= 8;
            $newLong++;
        }
        while ($newSubLat < 0) {
            $newSubLat += 8;
            $newLat--;
        }
        while ($newSubLong < 0) {
            $newSubLong += 8;
            $newLong--;
        }
        my $cellID = sprintf("%02i%03i%c%i", $newLat, $newLong, $newSubLat + ord('a'), $newSubLong + 1);
#        print "New cell is $cellID\n";
        return $cellID;

    } else {
        print "Cannot parse cell ID $refID\n";
        return undef;
    }
}


# This function takes a reference to an array of cells and returns a reference
# to one or more cells, or undef if none is chosen. If the array has more than one
# valid cell (i.e., for which we can get data), it asks the user to choose
# from the list. If there is only one valid cell, it simply returns that cell.
# If the list is empty, contains no valid cells, or the user cancels, undef is
# returned.
sub choose_cells(@) {
    my @matches = @_;

    # First, strip out any invalid cells
    my $count = @matches;
    for (my $i = 0; $i < $count; $i++) {
        if (! defined($matches[$i]->{'catalog-id'})) {
            splice(@matches, $i, 1);    # Take it out
            redo;                       # Don't increment $i
        }
    }

    if (@matches <= 1) {                # Zero or one valid cells -- no need to ask
        return \@matches;
        
    } else {                            # Have to ask the user
        # We use one-based indexing for our user's convenience.
        # Anything for YOU, user!
        my $index;

        # Print all the options
        for (my $i = 0; $i < @matches; ++$i) {
            print "    " . ($i + 1) . ": ";
            print format_cell($matches[$i]);
            print "\n";
        }
        
        # Ask Mr. User which one he wants
        my $rl = new Term::ReadLine;
        do {
            print "  Please type an integer in the range 1.." . @matches
                  . ", 'a' for all, or 'q' to quit\n";
            my $response = $rl->readline("choice> ");
            $response ne 'q' or return [];
            $response ne 'a' or return \@matches;
            $index = int($response);
        } while (! ($index > 0 and $index <= @matches));
        $index--;   # Go back to a zero-based system

        # Return the one he chose
        return [ $matches[$index] ];
    }
}


sub find_matching_cells(%%) {
    my ($cells, $search) = @_;
    print "Search\n";
    foreach my $key (keys(%$search)) {
        print "$key -> $search->{$key}\n";
    }
    my @results;
    foreach my $cell (values(%$cells)) {
        foreach my $field (keys(%$search)) {
            # See if it's an array
            if (ref($search->{$field}) eq 'ARRAY') {
                foreach my $item (@{$search->{$field}}) {
                    if ($cell->{$field} =~ /$item/) {
                        push(@results, $cell);
                    }
                }

            # Ok, it's a scalar then
            } elsif ($cell->{$field} =~ /$search->{$field}/) {
                push(@results, $cell);
            }
        }
    }

    return @results;
}


sub format_cell(%) {
    my ($cell) = @_;
    return "$cell->{'cell-id'} - $cell->{name}, $cell->{state}";
}

# Return the base filename (w/o extension or path)
sub basename(%$) {
    my ($cell, $lod) = @_;
    return format_cell($cell) . " (${lod}m)";
}

# Return the SDTS filename for a cell
sub sdts_filename(%$) { return "$SDTS_DIR/" . basename(@_) . ".sdts.tgz"; }

# Return the DEM filename for a cell
sub dem_filename(%$) { return "$DEM_DIR/" . basename(@_) . ".dem"; }


# Download the SDTS archive at the specified URL and save it with the given
# filename, inside the SDTS directory.
sub download_sdts($$) {
    my ($cell, $lod) = @_;
    my ($url, $file) = @_;

    # Make sure we have a valid catalog id
    if ($cell->{'catalog-id'} eq '') {
        print "Cell $cell->{name}, $cell->{state} does not have a catalog ID\n";
        return undef;
    } else {
        print "Downloading SDTS " . format_cell($cell) . "\n";
    }

    # Make sure we're logged in to the website
    ensure_login($USERNAME, $PASSWORD);

    # Send the browser to the download page. We have to rewrite one of
    # the forms on their catalog page, because it looks like they're
    # checking the "Referer:" header. Yay for DHTML.
    open_url("http://data.geocomm.com/catalog/US/51058/1362/group4-3.html");
    my $javascript =
        "document.forms[3].innerHTML = \""
            . "<input type=hidden name=CATALOGID value=$cell->{'catalog-id'}>"
            . "<input type=hidden name=download_x value=" . int(rand(14) + 1) . ">"
            . "<input type=hidden name=download_y value=" . int(rand(11) + 3) . ">\";"
            . "document.forms[3].submit();";
    sleep 3;
    execute_javascript($javascript) or return undef;

    # Now, pull up the cell on the website. The user can drag the version he
    # wants onto the terminal
    # Read a character at a time from the keyboard, looking for Escape or for
    # a gzip/tar archive filename to be completed. The user can create this by
    # DnD-ing the download URL onto this window.
    my $url = '';
    my $other = ($lod == 30 ? 10 : 30);

    # Switch to reading single characters
    ReadMode(3);

    my $print = 1;          # Print message the first time
    my $char;
    while (! ($url =~ /\.TAR\.GZ$/)) {
        if ($print) {
            $print = 0;     # Don't print again
            print "Drag the URL for the ${lod}M DEM onto this window, "
                . "or press TAB to switch to ${other}M.\n"
                . "Press ESC to cancel.\n";
        }
        $char = ReadKey(0);
        if ($char eq chr(27)) {     # Escape means cancel
            last;
        } elsif ($char eq chr(9)) { # Tab means switch
            my $tmp = $lod;
            $lod = $other;
            $other = $tmp;
            $print = 1;
            $url = '';
        } elsif ($char eq "'") {    # Ignore quotes
            next;
        } else {
            $url .= $char;
        }
    }

    # Restore the original terminal settings
    ReadMode(1);

    # Make sure we've not already downloaded this
    my $filename = sdts_filename($cell, $lod);
    if (-e $filename) {
        print "This cell has already been downloaded to $filename\n";
        return 1;
    }

    print "[DOWNLOAD] " . basename($cell, $lod);

    if ($char eq chr(27) or $url eq '') {
        print " cancelled\n";
        return undef;
    } else {
        print " from $url...";
        if (! wget($url, $filename, 0, '-q')) {
            print "FAILED\n";
            return undef;
        } else {
            print "ok\n";
            $LAST_ACTIVITY_TIME = time();   # Update the timestamp
            return $lod;                    # Which did we end up getting?
        }
    }

#        my $tempFile = "$CACHE_DIR/post_download.html";
#        open(POSTFILE, ">$tempFile")
#            or die "Unable to write post-hack HTML file\n";
#        my ($randX, $randY) = (rand(14) + 1, rand(10) + 4);
#        print POSTFILE <<EOF;
#            Referer: http://data.geocomm.com/catalog/US/51058/1362/group4-3.html
#            <html>
#              <body onload="document.forms[0].submit()">
#                <form name="hack_form" action="http://download.geocomm.com/download.php" method="post">
#                  <input type="hidden" name="CATALOGID" value="$cell->{catalog-id}"/>
#                  <input type="hidden" name="download_x" value="$randX"/>
#                  <input type="hidden" name="download_y" value="$randY"/>
#                </form>
#              </body>
#            </html>
#EOF
#        close(POSTFILE);
#        open_url("file:$tempFile");
}

sub convert_sdts_to_dem($$) {
    my ($sdts_file, $dem_file, $basename) = (sdts_filename(@_), dem_filename(@_), basename(@_));

    if (-e $dem_file) {
        print "This cell has already been extracted to $dem_file\n";

    } else {
        print "[CONVERT]  $basename SDTS -> DEM...";
        my $cmd = "tar -zxvf \"$sdts_file\" -C \"$TEMP_DIR\" \"*.DDF\"";
        my $ddf_file = `$cmd | grep CEL0`;
        $? == 0 or die "FAILED\nExtracting $sdts_file to $TEMP_DIR failed\n";
        chdir($TEMP_DIR) or die "FAILED\nFailed to change directory to $TEMP_DIR\n";
        $ddf_file =~ /^(\d\d\d\d)CEL0/;
        my $tileID = $1;
        $cmd = "sdts2dem $tileID \"$DEM_DIR/$basename\" L0 >/dev/null 2>&1";
        system($cmd) == 0 or die "FAILED\nConverting to DEM failed\n";
        system("rm -f \"$TEMP_DIR\"/$tileID*") == 0 or die "FAILED\nCleanup SDTS failed\n";
        print "ok\n[VERIFY]   $basename...";
        if (system("$PROG_DIR/verify_dem \"$dem_file\" 2>/dev/null") != 0) {
            system("rm -f \"$dem_file\"") == 0 or die "FAILED\nDelete DEM failed\n";
            print "FAILED\nDEM is corrupt...deleting\n";
            return undef;
        }
        print "ok\n";
    }

    return 1;
}


###############################################################################
###               Interface functions for external programs                 ###
###############################################################################

# Opens a DEM in the terrain viewer
sub view_dem($) {
    my ($cell, $lod) = @_;

    my $pid = fork();
    if ($pid == 0) {
        my $filename = dem_filename($cell, $lod);
        print "[VIEW]   " . basename($cell, $lod) . "\n";
        exec("$PROG_DIR/view_dem", $filename)
            or die "Unable to exec $PROG_DIR/view_dem $filename\n";
    } else {
        if (waitpid($pid, 0) > 0) {
            print "ok\n";
        } else {
            print "??\n";
        }
    }
}

# Loads a URL into the linked browser window
sub open_url($) {
    my ($url) = @_;

    # First set the current URL to nothing, so we'll know when we've finished
    # loading (this is necessary if loading the same URL).
    my $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 openURL 'about:blank'";
    system($cmd) == 0 or print "Error in DCOP call: $cmd\n" and return undef;
    while (current_url() ne 'about:blank') {  # Wait for the browser to catch up
        sleep 1;
    }

    # Run the command and return error on failure
    $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 openURL $url";
    system($cmd) == 0 or print "Error in DCOP call: $cmd\n" and return undef;
    while (current_url() ne $url) {  # Wait for the browser to catch up
        sleep 1;
    }

    return 1;
}

sub current_url() {
    my $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 currentURL";
    my $url = `$cmd`;
    chomp($url);
    return $url;
}

sub execute_javascript($) {
    my ($cmd) = @_;
    my $partID = `dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 currentPart`;
    chomp($partID);
    $partID =~ s/^.*(html-widget.*)\).*$/\1/;
    $partID =~ /^html-widget/
        or print STDERR "DCOP error: Unable to get current part\n"
        and return undef;
#    print "dcop konqueror-$BROWSER_ID $partID evalJS '$cmd'\n";
    system("dcop konqueror-$BROWSER_ID $partID evalJS '$cmd'") == 0
        or print STDERR "DCOP error: Unable to evalJS\n"
        and return undef;
    return 1;
}

sub wget($$$$) {
    my ($url, $file, $session, $extra) = @_;
    my $agent = "--user-agent=\"$AGENT_STRING\"";
    my $downloadTo;
    if (defined($file)) {
        $downloadTo = "-O \"$file\"";
    } else {
        my $basename = $url;
        $basename =~ s/^.*\/([^\/]+)\$/\1/;
        $downloadTo = "-R \"$basename\"";
    }
    my $cookies;
    if ($session) {
        $cookies = "--cookies=on --header=\"Cookie: TBMCOOKIE=$SESSION_LEVEL; TBMSESSION=$SESSION_ID;\"";
    }
    my $wait = "--random-wait --wait=2";
    my $cmd = "wget $agent $cookies $extra $wait $downloadTo $url";
    system($cmd) == 0 or return undef;
    return 1;
}





# Not used
sub get_dem($) {
    # Find the cell the user wants
    my $cell = find_cell_record($_[0]);
    if (! defined($cell)) {
        return undef;
    }
    
    # Make sure we've not already downloaded this
    my $filename = filename_for($cell, 30);
    if (-e "$SDTS_DIR/$filename") {
        print "This cell has already been downloaded to $SDTS_DIR/$filename\n";
        return $filename;
    }

    # Now, pull up the cell on the website. The user can drag the version he
    # wants onto the terminal
    if ($cell->{'catalog-id'} ne '') {
        # Send the browser to the download page. We have to rewrite one of
        # the forms on their catalog page, because it looks like they're
        # checking the "Referer:" header. Yay for DHTML.
        open_url("http://data.geocomm.com/catalog/US/51058/1362/group4-3.html");
        my $javascript =
            "document.forms[3].innerHTML = \""
                . "<input type=hidden name=CATALOGID value=$cell->{'catalog-id'}>"
                . "<input type=hidden name=download_x value=" . int(rand(14) + 1) . ">"
                . "<input type=hidden name=download_y value=" . int(rand(11) + 3) . ">\";"
                . "document.forms[3].submit();";
        sleep 3;
        execute_javascript($javascript) or return undef;

        # Read a character at a time from the keyboard, looking for Escape or for
        # a gzip/tar archive filename to be completed. The user can create this by
        # DnD-ing the download URL onto this window.
        print "Drag the URL for the 30m DEM onto this window, or press ESC to cancel.\n";
        my $url = '';

        # Switch to reading single characters
        ReadMode(3);
        my $char = ReadKey(0);
        while ($char ne chr(27) and ! ($url =~ /\.TAR\.GZ$/)) {
            if ($char ne "'") {
                $url .= $char;
            }
            $char = ReadKey(0);
        }

        # Restore the original terminal settings
        ReadMode(1);
        
        if ($char eq chr(27)) {
            print "Cancelled\n";
            return undef;
        } else {
            print "Downloading $url to $filename\n";
            wget($url, "$SDTS_DIR/$filename", 0, undef) or return undef;
            return $filename;
        }
        
#        my $tempFile = "$CACHE_DIR/post_download.html";
#        open(POSTFILE, ">$tempFile")
#            or die "Unable to write post-hack HTML file\n";
#        my ($randX, $randY) = (rand(14) + 1, rand(10) + 4);
#        print POSTFILE <<EOF;
#            Referer: http://data.geocomm.com/catalog/US/51058/1362/group4-3.html
#            <html>
#              <body onload="document.forms[0].submit()">
#                <form name="hack_form" action="http://download.geocomm.com/download.php" method="post">
#                  <input type="hidden" name="CATALOGID" value="$cell->{catalog-id}"/>
#                  <input type="hidden" name="download_x" value="$randX"/>
#                  <input type="hidden" name="download_y" value="$randY"/>
#                </form>
#              </body>
#            </html>
#EOF
#        close(POSTFILE);
#        open_url("file:$tempFile");

        $LAST_ACTIVITY_TIME = time();   # Update the timestamp
    } else {
        print "Cell $cell->{name}, $cell->{state} does not have a catalog ID\n";
        return undef;               # I've FAILED you, master!
    }
    
#    # We might have more than one LOD we could download
#    my ($thirty, $ten) = get_urls($cell->{'catalog-id'});
#    if ($thirty) {
#        my $pid = fork();
#        defined($pid) or die "Ack! fork() failed!\n";
#        if ($pid == 0) {    # I'm the kid -- go run wget
#            my $filename = filename_for($cell, 30);
#            download_sdts($thirty. $filename);
#            convert_sdts_to_dem($filename);
#        }
#    } elsif ($ten) {
#        print "  Cell '$cell->{name}, $cell->{state}' has only 10m resolution data.\n";
#    } else {
#        print "  Cell '$cell->{name}, $cell->{state}' does not have any downloadable data.\n";
#    }
}


# Not used
sub get_urls($$) {
    my ($catalogID) = @_;
    my $url = "http://download.geocomm.com/download.php";
    my $extra = "--post-data=\"CATALOGID=$catalogID&download_x=" . int(rand(16) + 2) . "&download_y=" . int(rand(16) + 1) . "\" ";
    my ($thirty, $ten);

    if (wget($url, "$CACHE_DIR/temp.html", 1, $extra) != 0) {
        print STDERR "Download of $url failed\n";
    } else {
        my @lines = `grep DEM\.SDTS\.TAR\.GZ $CACHE_DIR/temp.html`;
        for (@lines) {
            if (/(http:.*\d+\.DEM\.SDTS\.TAR\.GZ)">.*\((\d\d) meter\)/) {
                if ($2 == 10) {
                    $ten = $1;
                } elsif ($2 == 30) {
                    $thirty = $1;
                } else {
                    print STDERR "Found filename $1 for unknown LOD $2\n";
                }
            }
        }
    }

    print "$thirty\n$ten\n";
    return ($thirty, $ten);
}


