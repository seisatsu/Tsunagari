#!/usr/bin/env perl
#
# gen-depends.pl
#   by Paul Merrill <napalminc@gmail.com>
#
# Generate GNU make dependencies, but block blacklisted headers.
#
use warnings;
use IPC::Open2;

# Add to this list if we have more headers to blacklist.
my @unwanted_deps = (
	"/usr/include"
);


sub wanted_deps {
	my @deps = @_;
	return grep {
		my $dep = $_;
		scalar(grep { $dep =~ m/$_/ } @unwanted_deps) == 0;
	} @deps;
}

sub write_deps {
	my ($srcfl, @deps) = @_;
	my @wanted = wanted_deps(@deps);
	print OUT "$srcfl: @wanted\n";
}

sub gen_deps {
	open(OUT, ">depends.txt") or die "cannot open 'depends.txt': $!";
	print "Writing to depends.txt...\n";

	my $first = 1; # We don't have a previous entry when we read the first.
	my $srcfl = "";
	my @deps;

	open2(MAKE_OUT, _, "make depend") or die "cannot run 'make depend': $!";
	while (<MAKE_OUT>) {
		chomp($_);
		if (/^(.+\.o): ?(.*?)( \\)?$/) {
			if (!$first) {
				write_deps($srcfl, @deps);
			}
			$first = 0;

			$srcfl = $1;
			print " DEPENDS $srcfl\n";

			@deps = split(/ /, $2);
		}
		elsif (/^ (.+?)( \\)?$/) {
			push(@deps, split(/ /, $1));
		}
	}
	close(MAKE_OUT);

	if ($srcfl ne "") {
		write_deps($srcfl, @deps);
	}

	close(OUT);
}

gen_deps();

