#!/usr/bin/perl

$DATA_DIR       = "/mnt/data/scratch/gis";
$SDTS_DIR       = "$DATA_DIR/sdts";
$DEM_DIR        = "$DATA_DIR/dem";
$CACHE_DIR      = "$DATA_DIR/web_cache";
$COOKIE_FILE    = "$CACHE_DIR/cookies.txt";
$TEMP_DIR       = "/tmp";
$FILENAME_DB    = "$DATA_DIR/download.db";
#$AGENT_STRING   = "Mozilla/5.0 (compatible; Konqueror/3.3; Linux) (KHTML,like Gecko)";
$AGENT_STRING   = "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7) Gecko/20040626 Firefox/0.9.1";
$TIMEOUT        = 3600;
$USERNAME       = "JediRy";
$PASSWORD       = "gocorel";

$SESSION_LEVEL      = 1;
$SESSION_ID         = "";
$LAST_ACTIVITY_TIME = 0;
$BROWSER_ID         = "";

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
%cells = load_cell_db($FILENAME_DB);
print "Loaded data for " . keys(cells) . " cells\n";
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
    } elsif ($cmd[0] eq "login") {
        return login($USERNAME, $PASSWORD);
        
    # Get a cell
    } elsif ($cmd[0] eq "get") {
#        ensure_login($USERNAME, $PASSWORD);
        my $filename = get_dem($cmd[1], $sessionID);
        if (defined($filename)) {
            return convert_dem($filename);
        } else {
            return undef;
        }
        
    # Execute javascript
    } elsif ($cmd[0] eq "js") {
        return executeJavascript($cmd[1]);

    # Silently ignore this, since Konqueror insists on sending it
    } elsif ($cmd[0] eq "cd") {
        return 1;

    # Ignore empty lines
    } elsif ($cmd[0] = '') {
        return 1;

    # Uh, Bob?
    } else {
        print "Unrecognized command $cmd[0]\n";
        return undef;
    }
}

sub quit() {
    my $kid = waitpid(-1, WNOHANG);

    print "Exiting...$kid\n";
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

    print STDERR "Logging in to Geocomm\n";

    # This mechanism is web-browser driven (via DCOP), since I didn't
    # feel morally right in flouting Thinkburst Media's policies (which
    # forbid using an automated mechanism, since they sell advertising).
    # My solution is to automate most of the process (login and cell lookup)
    # and to manually click-to-download, since I'll still see their ads, but
    # won't have to slog thru all the tedium of click-click-click and
    # renaming all those nasty files.
    my $loginURL = "https://secure.thinkburst.com/accounts/login.php?type=";
    my $successURL = "https://secure.thinkburst.com/accounts/status.php";
    openURL($loginURL);
    while (currentURL() eq $loginURL) {
        sleep(1);
    }
    $rc = currentURL() eq $successURL;


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

    # Report on the success of the operation and return
    if ($rc == 1) {
        $LAST_ACTIVITY_TIME = time();   # Update the timestamp
        print STDERR "Logged in successfully\n";
        return 1;
    } else {
        print STDERR "Login error\n";
        return 0;
    }
}


# Loads a URL into the linked browser window
sub openURL($) {
    my ($url) = @_;

    # First set the current URL to nothing, so we'll know when we've finished
    # loading (this is necessary if loading the same URL).
    my $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 openURL 'about:blank'";
    system($cmd) == 0 or print "Error in DCOP call: $cmd\n" and return 0;
    while (currentURL() ne 'about:blank') {  # Wait for the browser to catch up
        sleep 1;
    }

    # Run the command and return error on failure
    $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 openURL $url";
    system($cmd) == 0 or print "Error in DCOP call: $cmd\n" and return 0;
    while (currentURL() ne $url) {  # Wait for the browser to catch up
        sleep 1;
    }

    return 1;
}

sub currentURL() {
    my $cmd = "dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 currentURL";
    my $url = `$cmd`;
    chomp($url);
    return $url;
}

sub executeJavascript($) {
    my ($cmd) = @_;
    my $partID = `dcop konqueror-$BROWSER_ID konqueror-mainwindow#1 currentPart`;
    chomp($partID);
    $partID =~ s/^.*(html-widget.*)\).*$/\1/;
    $partID =~ /^html-widget/
        or print STDERR "DCOP error: Unable to get current part\n"
        and return 0;
#    print "dcop konqueror-$BROWSER_ID $partID evalJS '$cmd'\n";
    system("dcop konqueror-$BROWSER_ID $partID evalJS '$cmd'") == 0
        or print STDERR "DCOP error: Unable to evalJS\n"
        and return 0;
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
    return system($cmd) == 0;
}

sub get_dem($) {
    # Find the cell the user wants
    my $cell = get_cell_record($_[0]);
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
        openURL("http://data.geocomm.com/catalog/US/51058/1362/group4-3.html");
        my $javascript =
            "document.forms[3].innerHTML = \""
                . "<input type=hidden name=CATALOGID value=$cell->{'catalog-id'}>"
                . "<input type=hidden name=download_x value=" . int(rand(14) + 1) . ">"
                . "<input type=hidden name=download_y value=" . int(rand(11) + 3) . ">\";"
                . "document.forms[3].submit();";
        sleep 2;
        executeJavascript($javascript) or return undef;

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
#        openURL("file:$tempFile");

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
#            download_file($thirty. $filename);
#            convert_dem($filename);
#        }
#    } elsif ($ten) {
#        print "  Cell '$cell->{name}, $cell->{state}' has only 10m resolution data.\n";
#    } else {
#        print "  Cell '$cell->{name}, $cell->{state}' does not have any downloadable data.\n";
#    }
}


sub get_urls($$) {
    my ($catalogID) = @_;
    my $url = "http://download.geocomm.com/download.php";
    my $extra = "--post-data=\"CATALOGID=$catalogID&download_x=" . int(rand(16) + 2) . "&download_y=" . int(rand(16) + 1) . "\" ";
    my $thirty, $ten;

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
                    print STDERR "Found filename $1 for unknown LOD $lod\n";
                }
            }
        }
    }

    print "$thirty\n$ten\n";
    return ($thirty, $ten);
}


sub format_cell(%) {
    my ($cell) = @_;
    return "$cell->{'cell-id'} - $cell->{name}, $cell->{state}";
}

sub filename_for(%$) {
    my ($cell, $lod) = @_;
    return "$cell->{'cell-id'} - $cell->{name}, $cell->{state} ($lod).sdts.tgz";
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

sub get_cell_record($) {
    my ($criteria) = @_;

    # Choose which cell we're talking about
    my @matches = find_matching_cells(\%cells, { 'name' => $criteria });
    print "  " . @matches . " matching cell(s) found\n";
    my $index = 1;  # One-based for our users...anything for YOU, user!
    if (@matches > 1) {
        $index = 0;
        for (my $i = 0; $i < @matches; ++$i) {
            print "  " . ($i + 1) . ": ";
            print format_cell($matches[$i]);
            print "\n";
        }
        do {
            if ($index != 0) {  # Don't print the first time
                print "  Please type an integer in the range 1.." . @matches . "\n";
            }
            my $rl = new Term::ReadLine;
            $index = int($rl->readline("choice> "));
        } while (! ($index > 0 and $index <= @matches));
        $index--;   # Go back to a zero-based system

        # Return the choice the user selected
        return $matches[$index];

    } elsif (@matches == 0) {
        return undef;
    }
}

sub find_matching_cells(%%) {
    my ($cells, $search) = @_;
    my @results;
    foreach my $cell (values(%$cells)) {
        foreach my $field (keys(%$search)) {
            if ($cell->{$field} =~ /$search->{$field}/) {
                @results = (@results, $cell);
            }
        }
    }

    return @results;
}

sub download_file($$) {
    my ($url, $file) = @_;

    wget($url, "$SDTS_DIR/$file", 1, undef)  == 0 or die "Downloading $url failed\n";
}

sub convert_dem($) {
    my $tgz_file = $_[0];
    $tgz_file =~ /^(.*)\.tgz/;
    my $basename = $1;
    my $dem_file = "$basename.dem";
    if (-e "DEM_DIR/$basename.dem") {
        print "This cell has already been extracted to $DEM_DIR/$basename.dem\n";

    } else {
        print "Converting $tgz_file to $basename.dem\n";
        my $cmd = "tar -zxvf \"$SDTS_DIR/$tgz_file\" -C \"$TEMP_DIR\" \"*.DDF\"";
        my $ddf_file = `$cmd | grep CEL0`;
        $? == 0 or die "Extracting SDTS $tgz_file to $TEMP_DIR failed\n";
        chdir($TEMP_DIR) or die "Failed to change directory to $TEMP_DIR\n";
        $ddf_file =~ /^(\d\d\d\d)CEL0/;
        my $cell = $1;
        $cmd = "sdts2dem $cell \"$DEM_DIR/$basename\" L0 >/dev/null 2>&1";
        system($cmd) == 0 or die "Converting to DEM failed\n";
        system("rm -f \"$TEMP_DIR\"/$cell*") == 0 or die "Cleanup SDTS failed\n";
        print "Conversion complete\n";
    }

    return 1;
}
