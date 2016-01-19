del /F /S *.sdf
del /F /S /A:H *.v12.suo
rmdir /S /Q ExternLibs
rmdir /S /Q Debug
rmdir /S /Q Release

call :SubFolderClean Archive\Archive_Reader
call :SubFolderClean Archive\Chunkify
call :SubFolderClean Engine
call :SubFolderClean File_System
call :SubFolderClean Display\FLTK
call :SubFolderClean Display\GL3W
call :SubFolderClean Display\GLFW
call :SubFolderClean Display\GUI
call :SubFolderClean Math
call :SubFolderClean Memory_System
call :SubFolderClean Model_Converter
call :SubFolderClean Object_Management
call :SubFolderClean PCS_Tree
call :SubFolderClean Shared
call :SubFolderClean Time
call :SubFolderClean Archive\Unify


goto:eof

:SubFolderClean
	pushd %~1
	rmdir /S /Q Debug
	rmdir /S /Q Release
	del /F /S *.sdf
	del /F /S /A:H *.suo
	del /F /S *.out
	rmdir /S /Q obj
	popd