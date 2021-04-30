from pymem  import *
from pymem.process import *

print("---Sirumal's Killing Floor 2 Power---")

try:
    pm = pymem.Pymem("KFGame.exe")
except:
    sys.exit("Target process was not found!")

gameModule = module_from_name(pm.process_handle, "KFGame.exe").lpBaseOfDll

def GetPtrAddr(base, offsets):
    addr = pm.read_int(base)
    for i in offsets:
        if i != offsets[-1]:
            addr = pm.read_int(addr + i)
    return addr + offsets[-1]
    
while True:
    bullets = input("How many bullets do you want?\n")
    pm.write_int(GetPtrAddr(gameModule + 0x0220A068, [0x420,0x40,0xD00,0x40,0x49C,0x244,0x5FC]), bullets)
