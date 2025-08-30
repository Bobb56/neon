set chemin=%CD%
cd ..\..\
xcopy src build\ti_ez80\src /E /I /Y
pushd C:\CEdev
cedev.bat "%chemin%"
popd