#!/usr/bin/env perl
#
# gen-depend.pl
#   by Paul Merrill <napalminc@gmail.com>
#
# Filter GNU make dependencies with a whitelist.
#
use warnings;
use strict;


my $termwidth = 78;

# Allow only inner-project dependencies.
my @whitelist = (
	"^[^/]"
);


# Word wrap a string to $termwidth columns. If split over multiple lines, all
# but the last line are suffixed with a backslash (\).
sub wrap {
	my ($width, $token, @remaining) = @_;
	return "" unless $token;
	my $len = length($token);
	if ($len+2 <= $width) {
		return "$token" . (@remaining ?
			" " .wrap($width-$len-1, @remaining) :
			"");
	} elsif ($len >= $termwidth) {
		return "\\\n $token " . wrap($termwidth-$len-2, @remaining);
	} else {
		return "\\\n " . wrap($termwidth-1, $token, @remaining);
	}
}

# Given a target and its dependencies, return a string describing only the
# dependencies that match a pattern in @whitelist.
sub proper_deps {
	my ($target, @deps) = @_;
	@deps = grep {
		my $dep = $_;
		grep { $dep =~ m/$_/ } @whitelist;
	} @deps;
	return wrap($termwidth, "$target:", @deps) . "\n";
}

# Read the dependencies for one target.
sub get_target {
	my $in = shift;
	my ($target, $deps, $more) = <$in> =~ /^(.+\.o): (.*?)( \\)?$/;
	print " DEPEND $target\n";
	my @deps = split(/ /, $deps);
	while ($more) {
		($deps, $more) = <$in> =~ /^ (.+?)( \\)?$/;
		push(@deps, split(/ /, $deps));
	}
	return ($target, @deps);
}

# Call 'make depend' and filter its output.
sub gen_deps {
	my @targets;
	open(IN, "-|", "make depend") or die "cannot run 'make depend': $!";
	push(@targets, proper_deps(get_target(*IN))) until (eof(IN));
	close(IN);
	return @targets;
}

sub main {
	open(OUT, ">depend.txt") or die "cannot open depend.txt: $!";
	print "Writing to depend.txt...\n";
	print OUT gen_deps();
	close(OUT);
}

main();

