#!/usr/bin/env python

import json
import sys

data = sys.stdin.readlines()
print data
str = "".join(data)
print str
j = json.loads(str)
print json.dumps(j, sort_keys=False, indent=4, separators=(',', ': '))

