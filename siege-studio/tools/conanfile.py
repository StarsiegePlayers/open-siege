from conans import ConanFile, CMake, tools
import os.path
import sys


class LocalConanFile(ConanFile):
    settings = "os", "compiler", "build_type", "arch", "arch_build"
    build_requires = "cmake/3.22.0"

    def requirements(self):
        args = sys.argv[3:]
        for index, value in enumerate(args):
            if value == "--profile":
                profile = args[index + 1]
                args[index + 1] = os.path.abspath(profile) if os.path.exists(profile) else profile
        targets = ["dts-to-json", "json-to-dts", "dts-to-obj", "unvol"]

        settings = ' '.join(args)

        for target in targets:
            commands = [f"cd {target}", f"conan install . {settings}"]
            self.run(" && ".join(commands), run_environment=True)


    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        cmake.install()