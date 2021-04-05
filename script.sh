#!/bin/bash
# Name: Testcode voor de eerste deadline.
#
# Voer dit bestand uit in de root van je project. 
# De map waarin het bestand sources en de map src zit
#
# By Robbert Gurdeep Singh
################################################################################
TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

# Get sources

 
if [ -f "sources" ];
then
    sources=$(cat sources | tr "\t" " " | grep "^\s*ukkonen")
    sources=$(echo ${sources#*:} | tr " " "\n" | tr "\n" " ")

    # Get Makefile and execute it
    gsed -n '/#MAKEFILE\x2DSTART/,/#EXPECTED\x2DSTART/p' $0 \
        | tail -n+2 | ghead -n-1 \
        | make -B ukkonen OUTDIR="$TMPDIR" SOURCES="${sources#*:}" -f - -C src
    makeExit=$?
else
   echo "Het sources bestand kon niet gevonden worden  "
   makeExit=4  
fi


if [ $makeExit -ne 0 ]; then
    echo "compilatie gefaald"
else
    # If make succeeded, comtinue to test the output
    echo -n "abacabc_" | "$TMPDIR/ukkonen" > $TMPDIR/output.txt
    < $TMPDIR/output.txt tr -d " \t" \
        | sort -k1,1 -t'|' -n \
        | sed 's/[@|=]/ & /g;/^$/d' \
        | grep -v '^$' > $TMPDIR/output_clean.txt
    sed -n '/#EXPECTED\x2DSTART/,$p' $0 \
        | tail -n+2  \
        | tr -d " \t" \
        | sed 's/[@|=]/ & /g;/^$/d' > $TMPDIR/expected.txt

    diff -w -s -a $TMPDIR/output_clean.txt $TMPDIR/expected.txt
    diffExit=$?
    if [ $diffExit -ne 0 ]; then
        echo "Jouw output is fout"
    else
        echo "";
        echo "COMPILATIE EN MINIMALE TEST GESLAAGD"
        echo "OK"
        exit 0
    fi
fi
echo ""
echo "Bronbestanden voor ukkonen: $sources"
echo ""
echo "Jouw code kan niet ontvangen worden!"
echo "Zorg ervoor dat volgende twee exitcodes 0 zijn."
echo "COMPILATIE exitcode = $makeExit"
echo "MINIMALE TEST exitcode = ${diffExit:-?}"
echo "FOUT"
exit 1
#MAKEFILE-START
flags = -g -std=c11 -Wall -lm 

all: ukkonen

ukkonen: $(SOURCES)
	gcc $(flags) -o $(OUTDIR)/$@ $^

#EXPECTED-START
0 @  -  = 95:1,7-7 | 97:2,0-0 | 98:7,1-1 | 99:10,3-3
1 @ 7-7
2 @ 0-0 = 98:3,1-1 | 99:6,3-7
3 @ 0-1 = 97:4,2-7 | 99:5,6-7
4 @ 0-7
5 @ 4-7
6 @ 2-7
7 @ 1-1 = 97:8,2-7 | 99:9,6-7
8 @ 1-7
9 @ 5-7
10 @ 3-3 = 95:11,7-7 | 97:12,4-7
11 @ 6-7
12 @ 3-7
