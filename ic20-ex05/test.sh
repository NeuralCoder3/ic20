make > /dev/null
declare -A pairs=( [f]=dither [n]=random [r]=regular )
if [[ $1 == 'mask' ]]; then
    echo "Create Masks"
else
    echo "d kind k MSE"
fi
for d in 0.01 0.05 0.1 0.2 0.5; do
    [[ -d "./mask/$d" ]] || mkdir "./mask/$d"
    [[ -d "./test/$d" ]] || mkdir "./test/$d"
    for opt in "${!pairs[@]}"; do
        name=${pairs[$opt]}
        if [[ $1 == 'mask' ]]; then
            if [[ $opt == 'n' ]]; then
                ./mask_generator -i input/landscape.pgm -m "mask/$d/$name.pgm" -d $d > /dev/null
            elif [[ $opt == 'f' ]]; then
                ./mask_generator -i input/landscape.pgm -m "mask/$d/$name.pgm" -d $d -$opt -I > /dev/null
            else
                ./mask_generator -i input/landscape.pgm -m "mask/$d/$name.pgm" -d $d -$opt > /dev/null
            fi
        else
            [[ -d "./test/$d/$name" ]] || mkdir "./test/$d/$name"
            for k in 1 5 10 100 1000; do
                echo -n "$d $name $k "
                ./inpainting -i input/landscape.pgm -m "mask/$d/$name.pgm" -o "test/$d/$name/$k.pgm" -k $k | grep -o "iterations: [.0-9]*" | tr -d -c "0-9."
                echo ""
            done
        fi
    done
done 


# MSE after 100 iterations: 651.105042
