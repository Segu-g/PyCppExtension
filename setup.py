from setuptools import setup, Extension
from typing import List, Dict
from pathlib import Path
import winreg

def get_VC_IncludePath() -> List[Path]:
    pathes: List[Path] = []
    reg_path = r"Software\Microsoft\Windows Kits\Installed Roots"
    key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, reg_path)
    data, reg_type = winreg.QueryValueEx(key, 'KitsRoot10')
    include_path = Path(data) / "Include"
    ver_path = list(include_path.iterdir())
    ver_path.sort()
    lastest = ver_path[-1]
    for child in ["ucrt", "um", "shared", "winrt", "cppwinrt"]:
        pathes.append(lastest/child)
    return pathes

def main() -> None:
    modules: List[Extension] = []
    modules.append(
        Extension("mdl.cext", ["./mdl/cext.cpp", "./mdl/extension_utils/error_wrapper.cpp"]))
    setup(name = "mdl",
          version = "0.0.1",
          author = "Segu",
          packages=['mdl'],
          package_dir={'mdl': 'mdl'},
          ext_modules = modules)


if __name__ == "__main__":
    main()
