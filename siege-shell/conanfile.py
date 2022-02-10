from conans import ConanFile, CMake, tools
import glob
import os.path
import sys


class LocalConanFile(ConanFile):
    name = "siege-shell"
    version = "0.5.1"
    url = "https://github.com/3space-studio/3space-studio"
    license = "MIT"
    author = "Matthew Rindel (matthew@thesiegehub.com)"
    build_requires = "cmake/3.22.0"
    settings = "os", "compiler", "build_type", "arch"
    requires = "3space/0.5.1", "wxwidgets/3.1.5@bincrafters/stable"
    generators = "cmake_find_package"

    def build(self):
        cmake = CMake(self)
        print(f"Python executable path is {sys.executable}")
        cmake.definitions["PYTHON_EXECUTABLE"] = sys.executable
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        cmake.build()

    def package(self):
        cmake = CMake(self)
        print(f"Python executable path is {sys.executable}")
        cmake.definitions["PYTHON_EXECUTABLE"] = sys.executable
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        cmake.install()

    def imports(self):
        tools.rmdir("cmake")
        tools.mkdir("cmake")
        [tools.rename(file, f"cmake/{file}") for file in glob.glob("*.cmake")]


