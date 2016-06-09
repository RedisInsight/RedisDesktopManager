#!/bin/bash
# RDM dmg/deb build script
# ========================
# Should be defined:
# $QTDIR
# $APP_VERSION

# On OSX use coreutils package to fix absense of readlink command
if [[ $OSTYPE == darwin* ]]; then
  brew install coreutils
  PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"
fi

# Import common functions
DIR=$(dirname "$(readlink -f "$0")") && RDM_DIR=$DIR/../
. $RDM_DIR/build/common_functions


if [[ $OSTYPE == darwin* ]]; then
    QMAKE_EXTRA_PARAMS=
    RDM_OS="osx"
else
    QMAKE_EXTRA_PARAMS="QMAKE_LFLAGS_RPATH=\"\""
    RDM_OS="linux"
fi


function set_application_version {
    print_title "Set version $1"
    python $DIR/utils/set_version.py $1 > $RDM_DIR/src/version.h
    python $DIR/utils/set_version.py $1 > $RDM_DIR/3rdparty/crashreporter/src/version.h
    print_line
}


function get_build_env_info {
    print_title "Build environment"
    echo '# Used Qt:'
    qmake -v
    which qmake
    print_line
}


function clean_build_dir {
    print_title "Clean build dir"
    rm -f redis-desktop-manager-*.gz
    rm -f redis-desktop-manager-*.tar
    rm -fR $RDM_DIR/redis-desktop-manager-*/
    rm -fR $RDM_DIR/bin/$RDM_OS/release/*
    print_line
}


function build_crash_reporter {
    print_title "Build Crash Reporter"
    cd $RDM_DIR/3rdparty/crashreporter
    qmake CONFIG+=release DESTDIR=$RDM_DIR/bin/$RDM_OS/release $QMAKE_EXTRA_PARAMS
    make -s -j 2
    print_line
}

function build_debian_package {
    print_title "Build Debian package"

    # Build deps
    # ========================================================================
    print_title "Build Dependencies"
    cd $RDM_DIR/src
    chmod +x ./configure
    ./configure

    # Prepare directory for build
    # ========================================================================
    print_title "copy source dir :"
    cd $RDM_DIR

    BUILD_DIR=./redis-desktop-manager-$APP_VERSION
    rm -fR $BUILD_DIR/*
    mkdir $BUILD_DIR

    cp -Rf ./src/* $BUILD_DIR
    chmod +x $BUILD_DIR/configure
    chmod +x $BUILD_DIR/resources/rdm.sh
    mkdir $BUILD_DIR/debian
    cp -Rf ./build/debian/* $BUILD_DIR/debian
    print_line

    print_title "replace tag in debian/changelog:"
    sed -i -e "s,0.0.0,$APP_VERSION,g" $BUILD_DIR/debian/changelog
    print_line

    print_title "copy libs:"
    DEPS_LIB=$BUILD_DIR/lib
    PLUGINS=$DEPS_LIB/plugins/

    mkdir $DEPS_LIB
    mkdir -p $PLUGINS

    #external deps
    cp -R $QTDIR/qml $PLUGINS


    print_title "pack source:"
    rm -fR $BUILD_DIR/Debug
    rm -fR $BUILD_DIR/Release
    rm -fR $BUILD_DIR/GeneratedFiles

    cd $RDM_DIR

    tar czvf redis-desktop-manager-$APP_VERSION.tar.gz $BUILD_DIR
    cp redis-desktop-manager-$APP_VERSION.tar.gz redis-desktop-manager_$APP_VERSION.orig.tar.gz
    print_line

    # Build package
    # ========================================================================
    print_title "Build package:"
    cd $BUILD_DIR

    DEBEMAIL="u.glide@gmail.com"
    DEBFULLNAME="Igor Malinovskiy"
    export DEBEMAIL DEBFULLNAME

    #debuild -uc -us
    dpkg-buildpackage -b
    
    print_title "rename package"    
    
    PLATFORM=$(uname -m)
    BUILDERNAME=$(lsb_release -si)$(lsb_release -sr)

    if [ "$PLATFORM" != 'x86_64' ]; then
        PLATFORM='i386'
    else
        PLATFORM='amd64'
    fi

    PACKAGESUFIX=$APP_VERSION"_"$PLATFORM

    FILENAME="redis-desktop-manager_"$PACKAGESUFIX".deb"
    NEW_FILE="redis-desktop-manager_"$BUILDERNAME"_"$PACKAGESUFIX".deb"

    echo "# New Package Name : $NEW_FILE"

    cp $FILENAME $NEW_FILE

    rm -f $FILENAME
}

function build_dmg {
    print_title "Build OSX dmg"
    
    # Build app
    # ========================================================================
    print_title "Build Application"
    cd $RDM_DIR/src
    
    #replace tag in Info.plist:
    cp resources/Info.plist.sample resources/Info.plist
    sed -i "s/0.0.0/$APP_VERSION/g" resources/Info.plist
    
    sh ./configure
    qmake CONFIG-=debug CONFIG+=release CONFIG+=app_bundle
    make -s -j 2    
    
    # Build dmg
    # ========================================================================    
    print_title "Create release bundle"

    cd $RDM_DIR
    
    BUNDLE_PATH=$RDM_DIR/bin/$RDM_OS/release/ 
    BUILD_DIR=$BUNDLE_PATH/rdm.app/Contents/

    cp -f ./src/resources/Info.plist $BUILD_DIR/
    cp -f ./src/resources/rdm.icns $BUILD_DIR/Resources/
    
    cd $BUNDLE_PATH
    
    pushd $QTDIR/bin # Workaround for macdeployqt bug
    macdeployqt $BUNDLE_PATH/rdm.app -dmg -executable=$BUNDLE_PATH/rdm.app/Contents/MacOS/crashreporter -qmldir=$RDM_DIR/src/resources/qml/ -verbose=3
    popd    
    mv rdm.dmg redis-desktop-manager-$APP_VERSION.dmg
}


function export_debug_symbols {
    print_title "Export debug symbols"
    
    cd $RDM_DIR/3rdparty/gbreakpad
    if [[ $OSTYPE == darwin* ]]; then
        xcodebuild -sdk macosx10.11 -project src/tools/mac/dump_syms/dump_syms.xcodeproj -configuration Release ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES MACOSX_DEPLOYMENT_TARGET=10.11 GCC_VERSION=com.apple.compilers.llvm.clang.1_0
        cd src/tools/mac/dump_syms/build/Release
        BINARY_PATH="$RDM_DIR/bin/$RDM_OS/release/rdm.app/Contents/MacOS/rdm"
    else
        ./configure && make
        cd src/tools/linux/dump_syms
        BINARY_PATH="$RDM_DIR/bin/$RDM_OS/release/rdm"
    fi
    ./dump_syms $BINARY_PATH > $RDM_DIR/bin/$RDM_OS/release/rdm.sym
}

# Build package:
get_build_env_info
clean_build_dir
set_application_version $APP_VERSION
build_crash_reporter

if [[ $OSTYPE == darwin* ]]; then
   build_dmg
else
   build_debian_package
fi

export_debug_symbols
