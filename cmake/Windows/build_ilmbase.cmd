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
	set config=Release
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set arch=x64
) ELSE (
	set SYS=x86
	set arch=Win32
)

REM ***************************************************************
REM IlmBase SLN & VCPROJ files *must* be modified for static libs
REM ***************************************************************

set srcRoot=%ALEMBIC_ROOT%\contrib\ilmbase-1.0.2
set srcDir=%srcRoot%\vc\vc8\IlmBase
set outDir=%ALEMBIC_OUT%\%SYS%\ilmbase
set outLib=%outDir%\lib%DB%
set IncDir=%outDir%\include\OpenEXR

if NOT exist "%IncDir%"		md %IncDir%
if NOT exist "%outLib%"		md %outLib%

@echo on
vcbuild /nologo %1 %2 %3 %4 %5 %6 %srcDir%\IlmBase.sln "%config%|%arch%"
copy %srcDir%\%arch%\%config%\*.lib %outLib%
copy %srcRoot%\Half\half*.h 	%IncDir%
copy %srcRoot%\Iex\*.h 		%IncDir%
copy %srcRoot%\IMath\*.h 	%IncDir%
copy %srcRoot%\IlmThread\*.h 	%IncDir%
