#!/usr/bin/env python
#ckwg +28
# Copyright 2011-2013 by Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
#  * Neither name of Kitware, Inc. nor the names of any contributors may be used
#    to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


def test_import():
    try:
        import sprokit.pipeline.pipeline
    except:
        test_error("Failed to import the pipeline module")


def test_create():
    from vital.config import config
    from sprokit.pipeline import pipeline

    c = config.empty_config()

    pipeline.Pipeline()
    pipeline.Pipeline(c)


def test_api_calls():
    from vital.config import config
    from sprokit.pipeline import edge
    from vital.modules import modules
    from sprokit.pipeline import pipeline
    from sprokit.pipeline import process
    from sprokit.pipeline import process_cluster
    from sprokit.pipeline import process_factory

    p = pipeline.Pipeline()

    proc_type1 = 'numbers'
    proc_type2 = 'print_number'
    proc_type3 = 'orphan_cluster'

    proc_name1 = 'src'
    proc_name2 = 'snk'
    proc_name3 = 'orp'

    port_name1 = 'number'
    port_name2 = 'number'

    modules.load_known_modules()

    proc1 = process_factory.create_process(proc_type1, proc_name1)

    conf_name = 'output'

    c = config.empty_config()

    c.set_value(conf_name, 'test-python-pipeline-api_calls-print_number.txt')
    proc2 = process_factory.create_process(proc_type2, proc_name2, c)

    proc3 = process_factory.create_process(proc_type3, proc_name3)

    p.add_process(proc1)
    p.add_process(proc2)
    p.add_process(proc3)
    p.connect(proc_name1, port_name1,
              proc_name2, port_name2)
    p.process_names()
    p.process_by_name(proc_name1)
    p.cluster_names()
    p.cluster_by_name(proc_name3)
    p.connections_from_addr(proc_name1, port_name1)
    p.connection_to_addr(proc_name2, port_name2)

    p.disconnect(proc_name1, port_name1,
                 proc_name2, port_name2)
    p.remove_process(proc_name1)
    p.remove_process(proc_name3)

    # Restore the pipeline so that setup_pipeline works.
    p.add_process(proc1)
    p.connect(proc_name1, port_name1,
              proc_name2, port_name2)

    p.setup_pipeline()

    p.upstream_for_process(proc_name2)
    p.upstream_for_port(proc_name2, port_name2)
    p.downstream_for_process(proc_name1)
    p.downstream_for_port(proc_name1, port_name1)
    p.sender_for_port(proc_name2, port_name2)
    p.receivers_for_port(proc_name1, port_name1)
    p.edge_for_connection(proc_name1, port_name1,
                          proc_name2, port_name2)
    p.input_edges_for_process(proc_name2)
    p.input_edge_for_port(proc_name2, port_name2)
    p.output_edges_for_process(proc_name1)
    p.output_edges_for_port(proc_name1, port_name1)

    p.is_setup()
    p.setup_successful()

    c = config.empty_config()

    p.reconfigure(c)

    p.reset()


if __name__ == '__main__':
    import os
    import sys

    if not len(sys.argv) == 4:
        test_error("Expected three arguments")
        sys.exit(1)

    testname = sys.argv[1]

    os.chdir(sys.argv[2])

    sys.path.append(sys.argv[3])

    from sprokit.test.test import *

    run_test(testname, find_tests(locals()))
