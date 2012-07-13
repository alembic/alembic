@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

if /i "%1" == "db:" (
	set DB=_db
	set config=Debug
	shift
) ELSE (
	set DB=
	set config=RelWithDebInfo
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set arch=x64
	set Generator="Visual Studio 9 2008 Win64"
) ELSE (
	set SYS=x86
	set arch=Win32
	set Generator="Visual Studio 9 2008"
)

set outDir=%ALEMBIC_OUT%\%SYS%\hdf5
if NOT exist "%outDir%" md %outDir%

REM *********************************************************************
REM Do NOT call windows\copy_hdf.bat
REM *********************************************************************
:call %ALEMBIC_ROOT%\contrib\hdf5-1.8.9\windows\copy_hdf.bat

REM ******************************************************************************************
REM We always use the MT libraries, so comment out the next line if you want MTd libs for debug
REM ******************************************************************************************
set Luxology_flags=-D CMAKE_C_FLAGS=" /MT " -D CMAKE_CXX_FLAGS=" /MT "

@echo on
pushd %outDir%
cmake -G %Generator% -D HDF5_BUILD_HL_LIB:BOOL=ON %Luxology_flags% %ALEMBIC_ROOT%\contrib\hdf5-1.8.9
vcbuild /nologo %1 %2 %3 %4 %5 %6 HDF5.sln "%config%|%arch%"
@popd
