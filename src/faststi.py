import os
import ctypes

class Config:

    def __init__(self, dll, simset, filename=None):
        self.dll = dll
        self._parameters = {}
        self._simset = simset
        if filename:
            self.load(filename)

    def load(self, filename):
        load_config_file = self.dll.fsti_simset_load_config_file
        load_config_file.argtypes = [ctypes.c_void_p, ctypes.c_char_p,]
        load_config_file(self._simset, filename.encode('utf-8'))

    def set(self, key, value):
        self._parameters[key] = value

    def get(self, key):
        return self._parameters[key]

    def __del__(self):
        pass


class Simulation:

    def __init__(self):
        self.config = Config()

    def run(self):
        pass

    def __del__(self):
        pass

class SimulationSet:

    def __init__(self, config_filename=None, libpath=None,
                 libname="libfaststi.so", datapath=None):
        if datapath:
            os.environ["FSTI_DATA"] = datapath
        self.dll = ctypes.CDLL(os.path.join(libpath, libname))
        self._simset = self.dll.fsti_py_simset_init()
        self.config = Config(self.dll, self._simset, config_filename)
        self._simulations = []

    def get_simulations(self):
        self.dll.fsti_py_simulations_get.argtypes = [ctypes.c_void_p,]
        self.dll.fsti_py_simulations_get.restype = \
                                        ctypes.POINTER(ctypes.c_void_p)
        sim_arr = self.dll.fsti_py_simulations_get(self._simset)
        print("N: ", sim_arr[0])

    def run(self):
        print("Running simulation set", self.dll, self._simset)
        self.dll.fsti_simset_exec(self._simset)
        print("Run complete", self.dll, self._simset)

    def __del__(self):
        self.dll.fsti_py_simset_free(self._simset)


filename = "/home/nathan/workspace/C/faststi/simulations/examples/eg2.ini"
datadir = "/home/nathan/workspace/C/faststi/data"
simset = SimulationSet(config_filename=filename, libpath="./.libs/", datapath=datadir)
print("Running:")
# simset.run()
simset.get_simulations()
print("Deleting")
del simset
print("All good")
