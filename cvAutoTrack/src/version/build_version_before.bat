echo build version.h before build

if exist src\version\Version.h (
	del src\version\Version.h
)
if exist src\version\version.ver (
	del src\version\version.ver
)
if exist src\version\version.branch (
	del src\version\version.branch
)
if exist src\version\version_hash.hash (
	del src\version\version_hash.hash
)
if exist src\version\version_next.number (
	del src\version\version_next.number
)

git describe --tags >> src\version\version.ver
git branch --show-current >>  src\version\version.branch
git log -n1 --format=format:"%%h" >> src\version\version_hash.hash
powershell src\version\GetNextBuildVersion.ps1 >> src\version\version_next.number

for /f %%x in (src\version\version.ver) do (
	set version=%%x
)
for /f  "tokens=1,2,3 delims=.-" %%a  in  ("%version%")  do (
	set v1=%%a
	set v2=%%b
	set /a v3=%%c+1
)
for /f %%x in (src\version\version_next.number) do (
	set /a v4=%%x-1
)
for /f %%x in (src\version\version.branch) do (
	set v5=%%x
)
for /f %%x in (src\version\version_hash.hash) do (
	set v6=%%x
)

echo #pragma once>>src\version\Version.h
echo namespace TianLi::Version>>src\version\Version.h
echo {>>src\version\Version.h
echo    const int version_major = %v1%;>>src\version\Version.h
echo    const int version_minor = %v2%;>>src\version\Version.h
echo    const int version_revision = %v3%;>>src\version\Version.h
echo    const int version_build = %v4%;>>src\version\Version.h
echo    const std::string version_hash = "%v6%";>>src\version\Version.h
echo    const std::string build_version = "%v1%.%v2%.%v3%.%v4%-%v5%-%v6%";>>src\version\Version.h
echo %#ifndef _DEBUG>>src\version\Version.h
echo    const std::string build_time = "%date%";>>src\version\Version.h
echo #else>>src\version\Version.h
echo    const std::string build_time = "%date% %time%";>>src\version\Version.h
echo %#endif>>src\version\Version.h
echo }>>src\version\Version.h
echo //该文件自动生成，无需更改 >>src\version\Version.h

echo build version : %v1%.%v2%.%v3%.%v4%-%v5%-%v6% 

del src\version\version.ver
del src\version\version.branch
del src\version\version_hash.hash
del src\version\version_next.number
