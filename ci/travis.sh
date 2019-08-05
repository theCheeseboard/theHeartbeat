if [ $STAGE = "script" ]; then
    if [ $TRAVIS_OS_NAME = "linux" ]; then
        echo "[TRAVIS] Preparing build environment"
        source /opt/qt510/bin/qt510-env.sh
        
        echo "[TRAVIS] Building and installing the-libs"
        git clone https://github.com/vicr123/the-libs.git
        cd the-libs
        git checkout blueprint
        qmake
        make
        sudo make install INSTALL_ROOT=/
        cd ..
        
        echo "[TRAVIS] Running qmake"
        qmake
        echo "[TRAVIS] Building project"
        make
        echo "[TRAVIS] Installing into appdir"
        make install INSTALL_ROOT=~/appdir
        echo "[TRAVIS] Getting linuxdeployqt"
        wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
        chmod a+x linuxdeployqt-continuous-x86_64.AppImage
        echo "[TRAVIS] Building AppImage"
        ./linuxdeployqt-continuous-x86_64.AppImage ~/appdir/usr/share/applications/*.desktop -appimage -extra-plugins=iconengines/libqsvgicon.so,imageformats/libqsvg.so
    fi
elif [ $STAGE = "after_success" ]; then
    if [ $TRAVIS_OS_NAME = "linux" ]; then
        echo "[TRAVIS] Publishing AppImage"
        wget -c https://github.com/probonopd/uploadtool/raw/master/upload.sh
        cp com.vicr123.theHeartbeat*.AppImage theHeartbeat-linux.AppImage
        cp com.vicr123.theHeartbeat*.AppImage.zsync theHeartbeat-linux.AppImage.zsync
        bash upload.sh theHeartbeat-linux.AppImage*
    fi
fi