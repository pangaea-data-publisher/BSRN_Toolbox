#!/bin/bash
echo
echo BSRN Toolbox

echo - macdeployqt

cd ~/Development/GitHub/BSRN_Toolbox

rm -R '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app'
cp -R './build-BSRN_Toolbox-Desktop_Qt_5_7_0_clang_64bit-Release/BSRN_Toolbox.app' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app'
cp './trunk/Tools/f_check/BSRN_fcheck_MacOS' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/MacOS/BSRN_fcheck'
cp './trunk/Resources/Info.plist' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Info.plist'

/Developer/Qt/5.7/clang_64/bin/macdeployqt '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app'

echo - code signing

codesign --force --verify --sign  'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/MacOS/BSRN_fcheck'

codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtCore.framework'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtGui.framework'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtNetwork.framework'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtPrintSupport.framework'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtWidgets.framework'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/Frameworks/QtDBus.framework'

codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/bearer/libqcorewlanbearer.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/bearer/libqgenericbearer.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqdds.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqgif.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqicns.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqico.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqjpeg.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqmacjp2.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqtga.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqtiff.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqwbmp.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/imageformats/libqwebp.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/platforms/libqcocoa.dylib'
codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib'

codesign --force --verify --sign 'Developer ID Application: Alfred-Wegener-Institut fur Polar- und Meeresforschung (AWI)' '../../Distribution/BSRN_Toolbox/BSRN_Toolbox.app'

echo - mount package

cd ~/Development/Distribution/BSRN_Toolbox
hdiutil attach ~/Development/Distribution/BSRN_Toolbox_V2_OSX.dmg

rm -R '/Volumes/BSRN Toolbox/BSRN_Toolbox.app'
cp -R BSRN_Toolbox.app '/Volumes/BSRN Toolbox'

cd '/Volumes/BSRN Toolbox'
rm -rf .fseventsd
mkdir .fseventsd
touch .fseventsd/no_log
cd ~/Development/Distribution/BSRN_Toolbox

echo - verify package

codesign -d '/Volumes/BSRN Toolbox/BSRN_Toolbox.app'

echo
hdiutil detach '/Volumes/BSRN Toolbox'

echo - move application

rm -R /Applications/BSRN_Toolbox.app
cp -R BSRN_Toolbox.app /Applications
cd ~/Development/Distribution

echo - finished
