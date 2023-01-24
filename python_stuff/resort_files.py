import os
from PIL import Image

d_in = './data_femboys_raw_in'
d_out = './data_femboys_raw'
files = os.listdir(d_in)

i = 0
for f in files:
	
	img = Image.open(d_in + '/' + f)
	# if img.mode == 'RGBA':
	# 	background = Image.new('RGBA', img.size, (255,255,255))
	# 	img = Image.alpha_composite(img, background)
	img.convert('RGB').save(d_out + '/' +str(i)+'.jpg')

	i+=1

