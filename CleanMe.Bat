del /F /S *.sdf
del /F /S /A:H *.suo
rmdir /S /Q ExternLibs

call :SubFolderClean Eat
call :SubFolderClean Engine
call :SubFolderClean File_System
call :SubFolderClean GL3W
call :SubFolderClean GLFW
call :SubFolderClean Math
call :SubFolderClean Memory_System
call :SubFolderClean Object_Management
call :SubFolderClean PCS_Tree
call :SubFolderClean Time

goto:eof

:SubFolderClean
	pushd %~1
	rmdir /S /Q Debug
	rmdir /S /Q Release
	del /F /S *.sdf
	del /F /S /A:H *.suo
	rmdir /S /Q obj
	popd