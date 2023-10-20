#
# Copyright 2023
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Carnegie Robotics, LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import datetime as dt
import os
import random

nfns = 4  # Functions per class
nargs = 4  # Arguments per function


def generate_dummy_code_pybind11(nclasses=10):
    decl = ""
    bindings = ""

    for cl in range(nclasses):
        decl += f"class cl{cl:03};\n"
    decl += "\n"

    for cl in range(nclasses):
        decl += f"class {cl:03} {{\n"
        decl += "public:\n"
        bindings += f'    py::class_<cl{cl:03}>(m, "cl{cl:03}")\n'
        for fn in range(nfns):
            ret = random.randint(0, nclasses - 1)
            params = [random.randint(0, nclasses - 1) for i in range(nargs)]
            decl += f"    cl{ret:03} *fn_{fn:03}("
            decl += ", ".join(f"cl{p:03} *" for p in params)
            decl += ");\n"
            bindings += f'        .def("fn_{fn:03}", &cl{cl:03}::fn_{fn:03})\n'
        decl += "};\n\n"
        bindings += "        ;\n"

    result = "#include <pybind11/pybind11.h>\n\n"
    result += "namespace py = pybind11;\n\n"
    result += decl + "\n"
    result += "PYBIND11_MODULE(example, m) {\n"
    result += bindings
    result += "}"
    return result


def generate_dummy_code_boost(nclasses=10):
    decl = ""
    bindings = ""

    for cl in range(nclasses):
        decl += f"class cl{cl:03};\n"
    decl += "\n"

    for cl in range(nclasses):
        decl += "class cl%03i {\n" % cl
        decl += "public:\n"
        bindings += f'    py::class_<cl{cl:03}>("cl{cl:03}")\n'
        for fn in range(nfns):
            ret = random.randint(0, nclasses - 1)
            params = [random.randint(0, nclasses - 1) for i in range(nargs)]
            decl += f"    cl{ret:03} *fn_{fn:03}("
            decl += ", ".join(f"cl{p:03} *" for p in params)
            decl += ");\n"
            bindings += f'        .def("fn_{fn:03}", &cl{cl:03}::fn_{fn:03}, py::return_value_policy<py::manage_new_object>())\n'
        decl += "};\n\n"
        bindings += "        ;\n"

    result = "#include <boost/python.hpp>\n\n"
    result += "namespace py = boost::python;\n\n"
    result += decl + "\n"
    result += "BOOST_PYTHON_MODULE(example) {\n"
    result += bindings
    result += "}"
    return result


for codegen in [generate_dummy_code_pybind11, generate_dummy_code_boost]:
    print("{")
    for i in range(0, 10):
        nclasses = 2**i
        with open("test.cpp", "w") as f:
            f.write(codegen(nclasses))
        n1 = dt.datetime.now()
        os.system(
            "g++ -Os -shared -rdynamic -undefined dynamic_lookup "
            "-fvisibility=hidden -std=c++14 test.cpp -I include "
            "-I /System/Library/Frameworks/Python.framework/Headers -o test.so"
        )
        n2 = dt.datetime.now()
        elapsed = (n2 - n1).total_seconds()
        size = os.stat("test.so").st_size
        print("   {%i, %f, %i}," % (nclasses * nfns, elapsed, size))
    print("}")
