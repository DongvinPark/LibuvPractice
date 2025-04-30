# Libuv Practice

## Project Structure
- Please see branch list of this repository.
<br><br/>

## Run Commands
- Linux
```text
./fast_build_run_for_linux.sh
```
- macOS
```text
./macos_fast_build_run.sh
```
<br><br/>

## Setting Up IDE and Install 'Libuv' and 'glib'
### CLion IDE를 사용했습니다.
### Linux
- 둘 다 터미널 명령어로 설치하면 됩니다.
- 그 후, CLion에서 본 프로젝트를 열고 실행하면 됩니다.
### macOS
- homebrew를 먼저 설치해줍니다.
- 그 후, libuv와 glib를 homebrew 명령어로 설치해줍니다.
- 그 후, CLion에서 본 프로젝트를 열고 실행하면 됩니다.
### Window
- 우선 Visual Stuido 커뮤니티 에디션을 설치해줍니다.
- 그 후, 파워셸을 관리자 권한으로 연 다음, C 드라이브의 루트 디렉토리로 이동합니다.
- 그 후, 아래의 명령어를 실행하여 vcpkg를 설치해준 후, vcpkg 디렉토리로 이동합니다.
- **_vcpkg를 반드시 C 드라이브의 루트 리렉토리에 설치해야 본 프로젝트의 CMakeLists.txt 파일을 통한 빌드가 정상 진행됩니다._**
```text
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```
- 그 다음, 아래의 명령어를 실행해서 CMake 프로젝트에 vckpg를 이용하도록 설정해줍니다.
```text
.\vcpkg integrate install
```
- 그러면 아래와 같은 결과물이 뜹니다. 해당 라인들 중에서, "-DCMAKE ..." 라고 표시돼 있는 부분을 다른 곳에 Control C,V 해서 기록해 둡니다.
```text
PS C:\vcpkg> .\vcpkg integrate install
>>
Applied user-wide integration for this vcpkg root.
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"

All MSBuild C++ projects can now #include any installed libraries. Linking will be handled automatically. Installing new libraries will make them instantly available.
```
- CLion에서 본 프로젝트를 열고, 아래와 같이 IDE 메뉴를 탐색합니다.
```text
File >> Setting >> Build, Execution, Deployment >> CMake 
```
- CMake 메뉴 내에서, ToolChain은 visual sutio로, generator는 use default(ninja)로 설정해줍니다. 그 후, apply를 클릭하고 창을 닫습니다.
- 그 후, 다시 파워셸을 관리자 권한으로 킨 다음 C >> vcpkg 디렉토리로 이동합니다.
- 아래의 명령어를 실행해서 glib를 설치해줍니다. 이 작업은 30분에서 최대 50 분 정도 걸릴 수 있습니다.
```text
.\vcpkg install glib
```
- 설치를 마쳤다면 본 프로젝트를 CLion으로 열어준 뒤, 아래의 순서로 Invalid Caches... 메뉴에 진입합니다. 
```text
File >> Invalid Caches...
```
- 그러면 'Invalid Caches' 창이 뜨는데, 해당 창 내부에 존재하는 모든 체크박스를 전부 체크한 후 'Invalid and Restart'를 클릭하여 본 프로젝트를 다시 열어줍니다.
- 그 후, 프로젝트를 실행하면 됩니다.