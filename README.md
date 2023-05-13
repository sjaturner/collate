# Collate lines based on keys for space separated input

There must be something else which can do this, I just have not seen
it yet.  Also, this would probably be much nicer in Python and faster -
because one could use efficient dictionaries.

It's easier to demonstrate than explain further ...

    :; cat input
    123 a b 4 5 6
    234 a b 5 6 7
    345 c d 1 2
    456 c e 4 5
    :; make collate
    gcc -Wall -Wextra    collate.c   -o collate
    :; ./collate 2 3 < input
    a b 123 4 5 6 234 5 6 7
    c d 345 1 2
    c e 456 4 5

Note that the first two input lines of input have the same character in
fields 2 and 3, where the field numbering is consistent with, say, cut.

The arguments supplied to ./collate denote the columns with the match keys.
So the first two lines have a key match. So they will be collated. 

The output function emits the key first then the non-key parts of the match.

Here is a more practical example, perhaps. I generated a time series data set.

    :; for val in $(seq 1 20) ; do echo $((RANDOM % 4)) $val ; sleep 0.0$(( RANDOM % 100)) ; done  | ts %.s | tee timeseries
    1683997937.021725 2 1
    1683997937.053262 1 2
    1683997937.085353 0 3
    1683997937.151615 1 4
    1683997937.203902 1 5
    1683997937.288180 3 6
    1683997937.322388 1 7
    1683997937.368589 0 8
    1683997937.460893 1 9
    1683997937.483167 0 10
    1683997937.530633 0 11
    1683997937.608837 3 12
    1683997937.655120 3 13
    1683997937.742387 3 14
    1683997937.837610 1 15
    1683997937.923997 0 16
    1683997937.996324 2 17
    1683997938.063396 1 18
    1683997938.124650 2 19
    1683997938.136805 1 20

Collate that on column 2

    :; cat timeseries | ./collate 2
    2 1683997937.021725 1 1683997937.996324 17 1683997938.124650 19
    1 1683997937.053262 2 1683997937.151615 4 1683997937.203902 5 1683997937.322388 7 1683997937.460893 9 1683997937.837610 15 1683997938.063396 18 1683997938.136805 20
    0 1683997937.085353 3 1683997937.368589 8 1683997937.483167 10 1683997937.530633 11 1683997937.923997 16
    3 1683997937.288180 6 1683997937.608837 12 1683997937.655120 13 1683997937.742387 14

Hmmn, not quite good enough for Octave, yet ...

But we can work out the longest line:

    :; cat timeseries | ./collate 2 | awk '{if(NF > maxnf) maxnf=NF;} END{print maxnf;}'
    17

So, plugging 17 into nanpad gives a file which Octave will like:

    :; cat timeseries | ./collate 2 | awk -vnanpad=17 '{for(x = 1; x <= nanpad; ++x){if (x <= NF) {printf("%s ",$x);} else {printf("NaN ")}};printf("\n");}'
    2 1683997937.021725 1 1683997937.996324 17 1683997938.124650 19 NaN NaN NaN NaN NaN NaN NaN NaN NaN NaN
    1 1683997937.053262 2 1683997937.151615 4 1683997937.203902 5 1683997937.322388 7 1683997937.460893 9 1683997937.837610 15 1683997938.063396 18 1683997938.136805 20
    0 1683997937.085353 3 1683997937.368589 8 1683997937.483167 10 1683997937.530633 11 1683997937.923997 16 NaN NaN NaN NaN NaN NaN
    3 1683997937.288180 6 1683997937.608837 12 1683997937.655120 13 1683997937.742387 14 NaN NaN NaN NaN NaN NaN NaN NaN
