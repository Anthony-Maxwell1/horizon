import os, platform
if (platform.node() != "PW0CV1AM"): quit()

Import("env")
os.environ["PATH"] = r"C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + os.environ["PATH"]