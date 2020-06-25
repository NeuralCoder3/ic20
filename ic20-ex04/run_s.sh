# run.sh
# Copyright (C) 2020 marcel <marcel@marcel-ge602pe>
#
# Distributed under terms of the MIT license.
#


make
echo "s q ratio MSE"
# outPath="kodak_test/s/kodim23_jpeg_"
outPath="kodak_test/s_q1/kodim23_jpeg_"
inImage="input/kodim23.ppm"
for s in 1 2 4 8 16 32 64 128 256 
do
    # ./ic20_jpeg_light
    output=$(./ic20_jpeg_light -i $inImage -o "${outPath}s${s}" -s $s -q 1 | grep -o "ratio: [.0-9]*:\|MSE: [.0-9]*" | tr -d -c "0-9. ")
    echo "$s$output"
done
