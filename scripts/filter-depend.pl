#!/usr/bin/env perl
#
# filter-depend.pl
#   by Paul Merrill <napalminc@gmail.com>
#
# Read an list of GNU make dependencies from STDIN.
# Remove dependencies not matching a @whitelist pattern.
# Print what's left to STDOUT.
#
use warnings;
use strict;

my @whitelist = (
	"^[^/]" # Only track inner-project dependencies.
);

# Output is wrapped to this many columns.
my $width = 78;


# Read a line that's been split with backslashes (\) over multiple lines.
sub read_wrapped_line {
	my (@tokens, $line, $more);
	do {
		($line, $more) = <> =~ /^(.*?)(\\)?$/;
		push(@tokens, split(/\s+/, $line));
	} while ($more);
	return @tokens;
}

# Return true if a string matches any pattern in @whitelist.
sub whitelisted {
	my $s = shift;
	return grep { $s =~ m/$_/ } @whitelist;
}

# Given a list of tokens, return only those that are whitelisted.
sub filter {
	return grep { whitelisted($_) } @_;
}

# Word wrap a string to $outputwidth columns. If split over multiple lines, all
# but the last line are suffixed with a backslash and all but the first line
# are indented with a space.
sub wrap {
	my ($space, $token, @rest) = @_;
	my $len = length($token);
	if ($space-2 < $len) {
		return "\\\n " . wrap($width-1, $token, @rest);
	} elsif (@rest) {
		return "$token " . wrap($space-$len-1, @rest);
	} else {
		return "$token\n";
	}
}

# Prefer files in the current directory, then files with only one "/", etc.
sub by_locality {
	my ($c, $d) = (0, 0);
	$c++ while ($a =~ /\//g);
	$d++ while ($b =~ /\//g);
	$c <=> $d or $a cmp $b
}

# Process STDIN one line at a time.
until (eof()) {
	my ($target, @deps) = read_wrapped_line();
	print wrap($width, $target, sort by_locality filter(@deps));
}

