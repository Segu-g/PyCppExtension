from setuptools import setup, Extension
from typing import List, Dict
from pathlib import Path


def main() -> None:
    setup_mdl()
    setup_anothermdl()


def get_VC_IncludePath() -> List[Path]:
    pathes: List[Path] = []
    reg_path = r"Software\Microsoft\Windows Kits\Installed Roots"
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, reg_path)
        data, reg_type = winreg.QueryValueEx(key, 'KitsRoot10')
        include_path = Path(data) / "Include"
        ver_path = list(include_path.iterdir())
        ver_path.sort()
        lastest = ver_path[-1]
        for child in ["ucrt", "um", "shared", "winrt", "cppwinrt"]:
            pathes.append(lastest/child)
        return pathes
    except importError as exception:
        print(exception)
        return pathes

def setup_mdl() -> None:
    modules: List[Extension] = []
    modules.append(
        Extension("mdl.cext",
                  sources =["./mdl/cext.cpp",
                            "./extension_utils/error_wrapper.cpp"],
                  language = "c++",
                  extra_compile_args=['-std=c++2a'],))
    setup(name = "mdl",
          version = "0.0.1",
          author = "Segu",
          packages=['mdl'],
          package_dir={'mdl': 'mdl'},
          ext_modules = modules)

def setup_anothermdl() -> None:
    setup(name = "anothermdl",
          version = "0.0.1",
          author = "Segu",
          ext_modules = [Extension("anothermdl",
                    sources =["./anothermdl/anothermdl.cpp",
                              "./extension_utils/error_wrapper.cpp"],
                    language = "c++",
                    include_dirs = ["./mdl"],
                    extra_compile_args=['-std=c++2a'])])


if __name__ == "__main__":
    main()
