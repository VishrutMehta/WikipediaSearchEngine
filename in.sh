#!/bin/bash
echo $1
rm -r Wiki_Split_Files
mkdir Wiki_Split_Files
cd Wiki_Split_Files
split -b 2M --suffix-length=4 $1
cd ..
python quantizing.py "Wiki_Split_Files/"
ls Wiki_Split_Files > file_names
g++ create_index.cpp -O3 -w
python script.py
g++ merge.cpp -O3 -w
./a.out 403
g++ query.cpp -O3 -w
./a.out
