import os
from PIL import Image

dout = "data_femboys_cropped"
s = open("data_femboys_cfg.txt").read()

lines = s.split('\n')
d = lines[0]
for i in range(1, len(lines)-1):
	words = lines[i].split(' ')
	if words[1] == 'BAD':
		print(words[0]+" is bad.")
		continue
	img = Image.open(d+"/"+words[0])
	w,h = img.width, img.height
	left,up,right,down = float(words[1]), float(words[2]), float(words[3]), float(words[4])
	c = 0.1
	fw,fh = w*c, w*c
	img = img.crop((left*w-fw,up*h-fh,right*w+fw,down*h+fh)).resize((64,64)).convert("L")
	img.save(dout+"/"+str(i-1)+".jpg")
