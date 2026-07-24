for source in documentation_*.typ; do
  echo "Compiling $source..."
  snap run typst compile $source
done
echo "Done!"
