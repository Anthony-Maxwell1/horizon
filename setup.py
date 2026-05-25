import os, platform
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

if (platform.node() == "PW0CV1AM"):
    Import("env")
    os.environ["PATH"] = r"C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + os.environ["PATH"]

os_name = platform.system()

if os_name == "Linux":
    flags = "-I/usr/include/SDL2"
elif os_name == "Darwin":
    flags = "-I/opt/homebrew/Cellar/SDL2/2.32.10/include -L/opt/homebrew/Cellar/SDL2/2.32.10/lib"
elif os_name == "Windows":
    flags = "-IC:/SDL2/x86_64-w64-mingw32/include -LC:/SDL2/x86_64-w64-mingw32/lib -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -mconsole"
else:
    flags = ""

if flags:
    env.Append(BUILD_FLAGS=[flags])