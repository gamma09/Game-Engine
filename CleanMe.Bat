del /F /S *.sdf
del /F /S /A:H *.v12.suo

rmdir /S /Q Shaders
rmdir /S /Q Debug
rmdir /S /Q Release
rmdir /S /Q Game\GeneratedFiles
rmdir /S /Q Editor\GeneratedFiles

call :SubFolderClean Archive_Reader
call :SubFolderClean Chunkify
call :SubFolderClean Unify

call :SubFolderClean DirectXTex

call :SubFolderClean Editor
call :SubFolderClean Engine
call :SubFolderClean File_System
call :SubFolderClean Game
call :SubFolderClean Math_Engine
call :SubFolderClean Memory_System
call :SubFolderClean Model_Converter
call :SubFolderClean Object_Management
call :SubFolderClean PCS_Tree

call :SubFolderClean Reflection_Headers
call :SubFolderClean Reflection_Tool
call :SubFolderClean TinyXML

call :SubFolderClean Shared
call :SubFolderClean Time



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