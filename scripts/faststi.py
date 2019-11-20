import os
import sys
import argparse
import ctypes
import pandas

# These are hardcoded from fsti_type in fsti-defs.h
LONG = 8
DBL = 13
STR = 15

# These are defined in fsti-pythoncalls.c
NEW = 0
REPLACE = 1
NEW_REPLACE = 2


class Value(ctypes.Union):
    _fields_ = [
        ("longint", ctypes.c_long,),
        ("dbl", ctypes.c_double,),
        ("string", ctypes.c_char_p),
    ]

class Variant(ctypes.Structure):
    _fields_ = [
        ("value", Value,),
        ("field_type", ctypes.c_int),
    ]

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


class Simulation:

    def __init__(self, dll, address):
        self.dll = dll
        self._address = address

    def get_id(self):
        if self._address:
            self.dll.fsti_py_simulation_id.argtypes = [ctypes.c_void_p, ]
            return self.dll.fsti_py_simulation_id(self._address)
        else:
            return -1

    def get_parm(self, key, index=0):
        self.dll.fsti_py_config_get.argtypes = [ctypes.c_void_p,
                                                ctypes.c_char_p,
                                                ctypes.c_size_t,]
        self.dll.fsti_py_config_get.restype = ctypes.POINTER(Variant)
        var = self.dll.fsti_py_config_get(self._address,
                                          key.encode('utf-8'),
                                          index)[0]
        # These types are encoded in fsti_type in fsti_defs.h
        if var.field_type == LONG:
            return var.value.longint
        elif var.field_type == DBL:
            return var.value.dbl
        elif var.field_type == STR:
            return var.value.string.decode("utf-8")

    def set_parm(self, key, values):
        self.dll.fsti_py_config_set.argtypes = [ctypes.c_void_p,
                                                ctypes.c_char_p,
                                                ctypes.c_size_t,
                                                ctypes.c_char_p,
                                                ctypes.c_int,
                                                ctypes.c_char_p,]
        vals = ";".join(str(i) for i in values).encode("utf-8")
        self.dll.fsti_py_config_set(self._address,
                                    key.encode("utf-8"),
                                    len(values),
                                    vals,
                                    REPLACE,
                                    "".encode("utf-8"))


    def run(self):
        self.dll.fsti_py_simulations_exec.argtypes = \
                [ctypes.c_int, ctypes.c_void_p * n]
        arr = (ctypes.c_void_p * 1)([self._id,])
        self.dll.fsti_py_simulations_exec(1, arr)
        self._address = arr[0]

    def address(self):
        return self._address

    def is_alive(self):
        if self._address:
            return True
        else:
            return False

    def set_freed(self):
        self._address = 0

    def __del__(self):
        if self._address:
            self.dll.fsti_py_simulation_free.argtypes = [ctypes.c_void_p, ]
            self.dll.fsti_py_simulation_free(self._address)

class SimulationSet:

    def __init__(self, config_filename=None, libname="libfaststi.so",
                 datapath=None):
        if datapath:
            os.environ["FSTI_DATA"] = datapath
        self.dll = ctypes.CDLL(libname)
        self.dataset = None
        self._simset = self.dll.fsti_py_simset_init()
        self.config = Config(self.dll, self._simset, config_filename)
        self.dll.fsti_py_simulations_get.argtypes = [ctypes.c_void_p,]
        self.dll.fsti_py_simulations_get.restype = \
                                        ctypes.POINTER(ctypes.c_void_p)
        sim_arr = self.dll.fsti_py_simulations_get(self._simset)

        self.simulations =  [Simulation(self.dll, sim_arr[i])
                             for i in range(1, sim_arr[0] + 1)]

    global lines

    def results_as_dataframe(self,
                             filename="results.csv",
                             sort=True,
                             sep=";"):
        results = open(filename)
        lines = results.readlines()
        results.close()
        header = "name" + sep
        if sort:
            lines.sort()
            if "header" in lines[-1]:
                lines.insert(0, lines[-1])
                lines.pop()
        if header not in lines[0]:
            lines.insert(0,"name;sim;num;date;description;value\n")
        print(lines[0])
        results = open(filename, "w")
        results.writelines(lines)
        results.close()
        return pandas.read_csv(filename, sep)

    def run(self, lo=0, hi=None):
        if hi == None:
            hi = len(self.simulations)
        n = hi - lo
        if n > 0 and self.simulations[lo].is_alive():
            sep = self.simulations[lo].get_parm("csv_delimiter")
            filename = self.simulations[lo].get_parm("results_file")
            self.dll.fsti_py_simulations_exec.argtypes = \
                    [ctypes.c_int, ctypes.c_void_p * n]
            addresses = [s.address() for s in self.simulations[lo:hi]]
            arr = (ctypes.c_void_p * n)(*addresses)
            self.dll.fsti_py_simulations_exec(n, arr)
            for i in range(lo,hi):
                self.simulations[i].set_freed()
            if filename:
                self.dataset = self.results_as_dataframe(filename, True, sep)
        return self.dataset


    def __del__(self):
        self.dll.fsti_py_simset_free(self._simset)


def main():
    parser = argparse.ArgumentParser(description="Run simulations of "
                                     "sexually transmitted infection "
                                     "epidemics.")
    parser.add_argument('--config', '-c', required=True,
                        help='Simulation set configuration file')
    parser.add_argument('--data_path', '-d',
                        help='Path to data files')

    args = parser.parse_args()

    filename = args.config
    data_path = args.data_path
    simset = SimulationSet(config_filename=filename, datapath=data_path)
    simset.run()

if __name__ == '__main__':
    main()
