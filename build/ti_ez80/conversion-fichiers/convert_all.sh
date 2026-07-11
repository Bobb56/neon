#!/bin/bash
for filename in Sources/*; do
    # Compute file name without Sources/
    output_name=$(echo $filename | sed 's:Sources/::')
    python3 ne-to-8xv.py ${filename} ${output_name/.ne/}
    mv ${output_name/.ne/.8xv} Converted/${output_name/.ne/.8xv}
done
