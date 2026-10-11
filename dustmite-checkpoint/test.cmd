@echo off
"D:\a\importC\importC\dmd-src\generated\windows\release\64\dmd.exe" -m64 -allinst -P=/I. -P=/FIimportc_msvc_builtins.h -P=-DSLJIT_HAVE_CONFIG_PRE=1 -c -oftest_obj.obj sljitTest.c >nul 2>nul
if errorlevel 1 exit /b 1
"D:\a\importC\importC\dmd-src\generated\windows\release\64\dmd.exe" -m64 -allinst -P=/I. -P=/FIimportc_msvc_builtins.h -P=-DSLJIT_HAVE_CONFIG_PRE=1 -L/DEFAULTLIB:mincore.lib -L/STACK:0x400000 -ofout.exe sljitLir_reduced.c sljitMain.c msvc_intrinsic_shims.c test_obj.obj >nul 2>nul
if errorlevel 1 exit /b 1
out.exe -v >nul 2>nul
set RC=%errorlevel%
if %RC% LSS 0 exit /b 0
if %RC% GTR 138 exit /b 0
exit /b 1
