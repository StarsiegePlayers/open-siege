from conans import ConanFile, CMake, tools
import os.path

# conan install . -s arch=x86

class LocalConanFile(ConanFile):
    build_requires = "cmake/3.17.3"
    settings = "os", "compiler", "build_type", "arch"
    requires = "detours/4.0.1@microsoft/stable", "nlohmann_json/3.8.0", "catch2/2.13.4"
    generators = "cmake_find_package"

    def build(self):
        self.run(f"{tools.vcvars_command(self)} && make-proxy-lib.bat")
        cmake = CMake(self)
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        cmake.build()
        cmake.test()


