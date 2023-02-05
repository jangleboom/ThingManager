#!/bin/bash
export PATH=/opt/homebrew/bin:$PATH

rm cppcheck.txt
srcDir="$(pwd)/src"

if which cppcheck >/dev/null; then
    echo "running cppcheck"
    cppcheck -j 4 --enable=all --inline-suppr $srcDir 2>cppcheck.txt 1>/dev/null
    pwd=$(pwd)
    sed "s|\[|${pwd}/|" cppcheck.txt | sed 's|\]: |: warning: |'
else
    echo "warning: cppcheck not installed, install here: http://brewformulas.org/Cppcheck"
fi
