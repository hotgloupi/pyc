from . import main

import sys

ret = 1

try:
    ret = main()
except KeyboardInterrupt:
    ret = 127
except Exception as e:
    print("Fatal Error:", e)
    import traceback
    traceback.print_exc()
    ret = 1

sys.exit(ret)
