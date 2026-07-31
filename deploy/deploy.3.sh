ROOT=$(pwd)
NEON_WEBSITE=../../neon-website
RELEASE_FOLDER=Neon_$1

echo "Copying Neon release files..."
cp -r $RELEASE_FOLDER $NEON_WEBSITE/releases/

cd $NEON_WEBSITE/gen-tools

echo "Adding new version to download page..."
python3 publish.py $1
python3 downloads.py

cd $ROOT
zip -r $RELEASE_FOLDER.zip $RELEASE_FOLDER/*
mv $RELEASE_FOLDER.zip ../distribution/
rm -r $RELEASE_FOLDER

echo "Neon website is ready for next update. Don't forget to git push the website for final step!"
