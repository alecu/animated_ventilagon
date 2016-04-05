import os
from itertools import izip_longest
from PIL import Image

C64_BLUE = "#6C5EB5"
DISPLAY_COLUMNS = 192
DISPLAY_ROWS = 32
SPRITE_ROWS = 32

USABLE_BITS = 12
MAX_VALUE = (1<<USABLE_BITS) - 1

def grouper(iterable, n, fillvalue=None):
  "Collect data into fixed-length chunks or blocks"
  # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx
  args = [iter(iterable)] * n
  return izip_longest(fillvalue=fillvalue, *args)

gamma = [int(pow(float(n)/255.0, 2.5) * float(MAX_VALUE) + 0.5) for n in range(256)]
print gamma

source_files = [
  ("scottpilgrim-src.png", 8),
  ("ramonaflowers-src.png", 8),
  ("mario-src.png", 7),
]

sources = []

for fn, num_frames in source_files:
  im = Image.open(fn).convert(mode="RGBA")
  total_w, h = im.size
  w = total_w / float(num_frames)
  new_size = (int(w * float(SPRITE_ROWS) / h + 0.5), SPRITE_ROWS)
  frames = [ im.crop((w*n,0,w*n+w-1,h)).resize(new_size, Image.LANCZOS) for n in range(num_frames)]
  sources.append(frames)
  print fn, im.mode, num_frames, len(frames), w

for frame_num in range(56):
  o = Image.new("RGBA", (DISPLAY_COLUMNS, DISPLAY_ROWS), C64_BLUE)
  for n, source in enumerate(sources):
    x = DISPLAY_COLUMNS * n / len(sources)
    frame = source[frame_num % len(source)]
    o.paste(frame, (x, 0), frame)

  o = o.transpose(Image.ROTATE_270)
  raw = o.convert("RGB").tobytes()
  
  #print " ".join("%02x" % ord(n) for n in raw[:10])

  with open("output/frame%03d.raw" % frame_num, "w") as f:
    for u,v in grouper(raw, 2):
      u, v = gamma[ord(u)], gamma[ord(v)]
      b1 = u >> 4
      b2 = (u << 4 & 0xF0) | (v >> 8)
      b3 = v & 0xFF
      f.write(chr(b1) + chr(b2) + chr(b3))
      #print "%02x %02x %02x" % (b1, b2, b3)
    
  o.save("output/frame%03d.png" % frame_num)

