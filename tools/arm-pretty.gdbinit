python 
import sys 
import gdb

# sys.path.append('/home/elco/repos/firmware/tools')
# from prettyprinters import printers
      
sys.path.append('/home/elco/opt/gcc-arm-none-eabi-9-2020-q2-update/share/gcc-arm-none-eabi')

# Load the pretty-printers.
from libstdcxx.v6.printers import register_libstdcxx_printers
register_libstdcxx_printers (gdb.current_objfile ())

end

set print pretty on