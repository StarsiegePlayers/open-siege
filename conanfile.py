from conans import ConanFile, CMake, tools
import os.path
import sys
import shutil
import json

class LocalConanFile(ConanFile):
    settings = "os", "compiler", "build_type", "arch", "arch_build"
    build_requires = "cmake/3.22.0"

    def requirements(self):
        self.run("conan install cmake/3.22.0@/ -g json")

        with open("conanbuildinfo.json", "r") as info:
            data = json.load(info)
            deps_env_info = data["deps_env_info"]
            os.environ["PATH"] += os.pathsep + deps_env_info["PATH"][0] + os.sep
            os.environ["CMAKE_ROOT"] = deps_env_info["CMAKE_ROOT"]
            os.environ["CMAKE_MODULE_PATH"] = deps_env_info["CMAKE_MODULE_PATH"]

        if os.path.exists("./cmake"):
            shutil.rmtree("./cmake")

        profile = "default"

        if "--profile" in sys.argv:
            profile = sys.argv[sys.argv.index("--profile") + 1]
            profile = os.path.abspath(profile) if profile != "default" else profile

        commands = [
            "cd 3space-studio",
            "pip3 install -r requirements.txt",
            "cd ..",
            "cd siege-shell",
            "pip3 install -r requirements.txt"
        ]

        self.run(" && ".join(commands), run_environment=True)

        targets = ["tools", "siege-shell", "3space-studio", "extender"]

        settings = f"--profile {profile} -s build_type={self.settings.build_type} -s arch={self.settings.arch} --build=missing"

        commands = [
            "cd 3space",
            f"conan install . {settings}",
            "conan export .",
            "cd ..",
            "mkdir cmake",
            "cd cmake",
            f"conan install 3space/0.5.1@/ -g cmake_find_package {settings}"
        ]
        self.run(" && ".join(commands), run_environment=True)


        for target in targets:
            commands = [
                f"cd {target}",
                f"conan install . {settings}"
            ]
            self.run(" && ".join(commands), run_environment=True)


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["CONAN_CMAKE_SYSTEM_PROCESSOR"] = self.settings.arch
        cmake.definitions["CI"] = os.environ["CI"] if "CI" in os.environ else "False"
        cmake.definitions["PYTHON_EXECUTABLE"] = sys.executable
        cmake.configure(source_folder=os.path.abspath("."), build_folder=os.path.abspath("build"))
        return cmake

    def build(self):
        self._configure_cmake().build()

    def package(self):
        git = tools.Git(folder="wiki")
        git.clone("https://github.com/StarsiegePlayers/3space-studio.wiki.git", shallow=True)
        tools.rmdir("wiki/.git")
        self.copy(pattern="LICENSE", src=self.source_folder, dst="licenses")
        self.copy(pattern="README.md", src=self.source_folder, dst="res")
        self.copy(pattern="wiki/*.md", src=self.source_folder, dst="res")
        self.copy(pattern="game-support.md", src=self.source_folder, dst="res")
        tools.rmdir("wiki")
        self._configure_cmake().install()
