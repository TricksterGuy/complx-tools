import pyLC3
import sys

from pathlib import Path

state = pyLC3.pylc3.LC3State(testing_mode=True)
filename = sys.argv[1]
print(state.load(filename, disable_plugins=True, process_debug_comments=False))
end = state.lookup('END')

base = Path(filename).with_suffix('')
new_filename = Path(filename).with_suffix('.cpp')
with open(new_filename, 'w') as f:
	f.write("#include \"lc3/lc3_os.hpp\"\n\nstd::array<uint16_t, 0x%03x> %s\n{{\n" % (end + 1, base))
	for addr in range(0, end + 1):
		data = state.get_memory(addr)
		if data < 0:
			data += 65536
		f.write("    0x%04X,\n" % data)
	f.write("}};\n\n")