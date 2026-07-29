rm -r Neon_$1
NEON_WEBSITE=../../neon-website

cd $NEON_WEBSITE/gen-tools
python3 publish.py undo $1