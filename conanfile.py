from conans import ConanFile, CMake, tools
import os

class UnitConvertConan(ConanFile):
    generators = "cmake", "virtualenv"
    requires = 'boost/1.72.0','pybind11/2.9.1'
    default_options = {
        'boost:header_only': True,
        }


    def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()
