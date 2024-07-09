SET mypath=%~dp0
echo %mypath%

cd %mypath%\..\external\GKLib
mkdir build-Win
cmake -DCMAKE_INSTALL_PREFIX=%mypath%\..\external\GKLib\build-Win .
cmake --build . --config Release
cmake --install .

xcopy %mypath%\..\external\GKLib\win32 %mypath%\..\external\GKLib\build-Win\include\win32 /E /H /C /R /Q /Y

cd %mypath%\..\external\METIS
mkdir build-Win
cmake -DCMAKE_INSTALL_PREFIX=%mypath%\..\external\METIS\build-Win -DGKLIB_PATH=%mypath%\..\external\GKLib\build-Win -DIDX64=ON -DREAL64=ON -DREALTYPEWIDTH=64 -DIDXTYPEWIDTH=64 .
cmake --build . --config Release
cmake --install .

