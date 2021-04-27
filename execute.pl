#!/usr/bin/perl
use strict;
use warnings;
use File::Path;
use Cwd 'abs_path';

# Create directories to store generated oracles, datasets, inferred trees, 
# temporary DOE restuls and final average DOE results
my $oracle_dir = "oracle"; 
my $data_dir = "data";
my $tree_dir = "tree";
my $rez_dir = "rez";
my $doe_dir = "doe";
mkdir $oracle_dir unless -d $oracle_dir;
mkdir $data_dir unless -d $data_dir;
mkdir $tree_dir unless -d $tree_dir;
mkdir $rez_dir unless -d $rez_dir;
mkdir $doe_dir unless -d $doe_dir;

# Find the absolute paths for the created directories 
my $oracle_abs_path = abs_path($oracle_dir);
my $data_abs_path = abs_path($data_dir);
my $tree_abs_path = abs_path($tree_dir);
my $rez_abs_path = abs_path($rez_dir);

my $doefilepath="$doe_dir/";

my $features = 10; # Set the number of features
my $values = 2; # Set the number of values per feature

# Set the type of generating dataset
# 0 means completely random dataset, 1 means uniquely random dataset
my $opt = 0; 

my $ini = 200; # Set the minimum size for generated datasets
my $inf = 5000; # Set the maximum size for generated datasets
my $step = 200; # Set the incrementation value for the size of generated datasets
my $in;

my $depi = 5; # Set the minimum depth for generated oracles
my $depf = 9; # Set the maximum depth for generated oracles
my $dep;

my $sna=2529184780; # Set an arbitrary seed number for randomization
my $snm;

my $phi=0; 
my $phf=100; # Set the number of trials per configuration
my $pH;

my $tl = 0; # A counter for the total number of trails

my $prog = "./src/generator"; # Call the generater program 
my $prog2 = "java -classpath '/home/Téléchargements/weka-3-8-5/weka.jar' weka.classifiers.trees.J48 -U -O"; # Call a decision tree inference program
my $prog3 = "./src/average"; # Call the average program

for ($dep = $depi; $dep <= $depf; $dep++) {
    open(my $doefile, '>>', $doefilepath . "f$features". "_v$values" . "_d$dep" . ".csv") or die "Could not open file: $!\n";
    print $doefile "Dataset Size, Average DOE\n";
    for ($in=$ini; $in <= $inf; $in=$in+$step) {
        for ($pH=$phi;$pH<$phf;$pH++) {
    		$snm=$sna+$pH*10000; # A simple algorithm to change seed numbers, can be changed
            $tl = $tl + 1;
            print "Trial $tl started...\n";
            # Generate a oracle based on current inputs and create a random dataset from the oracle
            system("$prog -s $snm -f $features -v $values -d $dep -n $in -t $oracle_abs_path -a $data_abs_path -o $opt"); 
            # Apply the selected decision algorithm to the dataset and save the inferred tree
            system("$prog2 -t $data_abs_path/train-s$snm-f$features-v$values-d$dep-n$in.csv > $tree_abs_path/tree-s$snm-f$features-v$values-d$dep-n$in");
            # Compare the inferred tree with the oracle and calculate DOE value
            system("$prog -s $snm -f $features -v $values -d $dep -n $in -t $oracle_abs_path -p $tree_abs_path/tree-s$snm-f$features-v$values-d$dep-n$in -z $rez_abs_path");
            print "Trial $tl finish!\n";
        }
        # Calculate an average DOE value for each configuration
        open(my $program3, '-|', "$prog3 $rez_abs_path/prob-f$features-v$values-d$dep-n$in") or die "Could not open file: $!\n";
        while(my $line = <$program3>) {
            print $doefile "$in, $line";
        }
    }
    close($doefile);
}
print "Done!\n";

# Remove all generated directories and files to clean up the workspace
# Note that the "doe" directory is not deleted because we want to save the results
rmtree $oracle_dir;
rmtree $data_dir;
rmtree $tree_dir;
rmtree $rez_dir;
if(-e $oracle_dir and -e $data_dir and -e $tree_dir and -e $rez_dir) {
    print "Failed to clean up directories.\n";
} else {
    print "Successfully cleaned up directories.\n";
}