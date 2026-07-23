#!/bin/bash
#TODO! Clean all the folders where we go for building things, and remove the useless ones

if [[ $1 ==  "" ]]; then
    echo "Please specify a version"
    exit
fi

VERSION=$1
DEPLOY=$(pwd)
ROOT=$DEPLOY/..
BUILD_DIR=$DEPLOY/Neon$VERSION

function init_project {
    echo "Creating build dir..."
    rm -r $BUILD_DIR 2> /dev/null
    mkdir $BUILD_DIR
}

function produce_docs {
    echo "Compiling Neon documentation..."
    cd $ROOT/doc
    rm *.pdf 2> /dev/null
    snap run typst compile documentation_fr.typ
    snap run typst compile documentation_en.typ
    mv *.pdf $BUILD_DIR/
}


function produce_neon_exe_linux {
    echo "Compiling Neon for Linux..."
    cd $ROOT
    make -f Makefile.linux_amd64 clean > /dev/null
    mkdir obj obj/extern obj/assets 2> /dev/null
    make -f Makefile.linux_amd64 -j 8
    cp bin/neon $BUILD_DIR/neon_${VERSION}_amd64
}

function produce_neon_setup_linux {
    echo "Creating installation package for Linux..."
    cd $ROOT/build/linux/lpm
    rm *.lpk 2> /dev/null
    lpm build neon.yaml
    mv Neon_installer.lpk $BUILD_DIR/Neon_installer_${VERSION}_amd64.lpk
}

function produce_ti_ez80_archive {
    local FOLDER_NAME=Neon_${VERSION}_TI_EZ80
    local FILE_CONVERSION=file_conversion
    local EXAMPLES=examples

    # Create directories
    mkdir $BUILD_DIR/$FOLDER_NAME

    cd $BUILD_DIR/$FOLDER_NAME
    mkdir $FILE_CONVERSION
    mkdir $FILE_CONVERSION/$EXAMPLES

    cp $DEPLOY/sources/how-to-use.md $BUILD_DIR/$FOLDER_NAME/$FILE_CONVERSION/
    cp $DEPLOY/sources/how-to-install.md $BUILD_DIR/$FOLDER_NAME/
    
    # Build Neon for TI_EZ80
    cd $ROOT
    make -f Makefile.ez80 clean > /dev/null
    make -f Makefile.ez80 -j 8
    ./appvars_split.sh
    mv bin/Neon.*.8xv $BUILD_DIR/$FOLDER_NAME/

    cd $ROOT/build/ti_ez80/app_tools/installer
    make clean
    make
    cp bin/NEONINST.8xp $BUILD_DIR/$FOLDER_NAME/

    # Now, handle ne -> 8xv conversion and example files
    cd $ROOT/build/ti_ez80/file_conversion

    for file in ne-to-8xv.py convbin.elf convbin.exe; do
        cp $file $BUILD_DIR/$FOLDER_NAME/$FILE_CONVERSION/$file
    done

    cd sources
    for file in {apples,LAUNCHER,line,mandbrot,palette,primes,rgb,text}.ne; do
        cp $file $BUILD_DIR/$FOLDER_NAME/$FILE_CONVERSION/$EXAMPLES/$file
    done

    # Create an archive for the whole directory
    cd $BUILD_DIR
    zip -r ${FOLDER_NAME}.zip $FOLDER_NAME/*
    rm -r $FOLDER_NAME
}

function produce_vscode_extension {
    echo "Packaging VSCodium extension..."
    cd $ROOT/syntax_highlighting/VScode/neon-syntax-highlighting
    vsce package
    mv neon-syntax-highlighting-0.0.1.vsix $BUILD_DIR/
}


init_project
produce_docs
produce_neon_exe_linux
produce_neon_setup_linux
produce_ti_ez80_archive
produce_vscode_extension

echo "Now please restart on windows and run deploy.2.bat"
