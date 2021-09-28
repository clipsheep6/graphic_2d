#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import optparse
import subprocess
import sys


def parse_args(args):
    parser = optparse.OptionParser()
    parser.add_option("--output")
    opts, _ = parser.parse_args(args)
    return opts

if __name__ == '__main__':
    options = parse_args(sys.argv[1:])
    parse_scripts = subprocess.check_call(
        ["cp", "-f", "/usr/include/FlexLexer.h", options.output])
