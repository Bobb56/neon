#!/bin/bash
for filename in sources/*; do
    # Compute file name without sources/
    output_name=$(echo $filename | sed 's:sources/::')
    python3 ne-to-8xv.py ${filename} ${output_name/.ne/}
    mv ${output_name/.ne/.8xv} converted/${output_name/.ne/.8xv}
done
