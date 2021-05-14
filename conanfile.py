from conans import ConanFile, CMake, tools
import os

class UnitConvertConan(ConanFile):
    generators = "cmake", "virtualenv"
    requires = 'boost/1.69.0','pybind11/2.3.0@conan/stable'
    default_options = {
        'boost:header_only': True,
        }


    def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()
