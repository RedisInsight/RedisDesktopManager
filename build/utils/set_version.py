from __future__ import print_function
from sys import argv, exit


if len(argv) != 2:
    print("Usage: %version%")
    exit(1)

print(
    """
#ifndef RDM_VERSION
    #define RDM_VERSION "{0}"
#endif // !RDM_VERSION
""".format(argv[1])
)
