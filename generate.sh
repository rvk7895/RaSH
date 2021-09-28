#!/bin/bash
mkdir -p testing/dir1/dir2
mkdir -p testing/dir3
touch testing/dir1/a.txt testing/dir1/b.c testing/dir1/.hidden.txt
touch testing/dir1/dir2/c.txt testing/dir1/dir2/d.txt 
touch testing/dir3/e.txt testing/dir3/f.py 
echo "This is a test for cat" > testing/dir1/a.txt

