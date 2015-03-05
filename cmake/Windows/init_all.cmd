@echo off

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

REM -------------------------------------------------------------------
REM  be sure to change vcproj for ilmbase to do static libs
REM -------------------------------------------------------------------
call %~dp0\build_boost.cmd %*
call %~dp0\build_hdf.cmd %*
call %~dp0\build_ilmbase.cmd %*
call %~dp0\init_Alembic.cmd %*