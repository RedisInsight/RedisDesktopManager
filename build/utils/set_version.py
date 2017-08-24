import sys
import fileinput

if len(sys.argv) != 2 :
    print "Usage: %version%"
    quit()	

print """
#ifndef RDM_VERSION
    #define RDM_VERSION "{0}"
#endif // !RDM_VERSION
""".format(sys.argv[1])