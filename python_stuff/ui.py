# Import Module
import os
from tkinter import *
from PIL import Image, ImageTk, ImageFilter, ImageEnhance
import numpy as np

wanted_size = 64

use_x = 0
cunt = 25

x = 0
y = 0

real_x = 0
real_y = 0

i = 0

cropped_image = Image.Image

def swap_images():
	global l
	l.destroy()
	l = Label(image=img)
	l.image = img
	l.pack()

def motion(event):
	x, y = event.x, event.y
	global real_x
	global real_y
	real_x = x
	real_y = y
	if x > wanted_size-cunt:
		x = wanted_size-cunt
	elif x < cunt:
		x = cunt
	
	if y > wanted_size-cunt:
		y = wanted_size-cunt
	elif y < cunt:
		y = cunt
	
	global img
	global use_x
	global l
	global new_image

	w, h = image.width, image.height
	x = ((x-cunt)/(wanted_size-cunt*2)) * (w-wanted_size)
	y = ((y-cunt)/(wanted_size-cunt*2)) * (h-wanted_size)

	new_image = image.crop((
		0+x*use_x, 0+y*(not use_x),
		wanted_size+x*use_x, wanted_size+y*(not use_x)))
	img = ImageTk.PhotoImage(new_image)
	swap_images()

fstr = str(wanted_size)+'\n'

# TODO: SAVE THE CONFIGURATION OF EACH SAVED IMAGE, SO THAT IF YOU DECIDE TO CHANGE THE SIZE OF
# AI INPUT IMAGES MIDWAY, YOU DONT HAVE TO REDO EVERYTHING
def on_lclick(event):
	open_next_image(1)
	swap_images()

def on_close():
	global fstr
	global dname
	print(fstr)
	f = open(dname+'/cfg.txt', 'w')
	f.write(fstr)
	f.close()
	root.destroy()

def on_rclick(event):
	open_next_image(0)
	swap_images()

def on_up(event):
	global image
	image = image.crop((10,10,image.width-10,image.height-10)).resize((wanted_size, wanted_size))
def on_down(event):
	global image
	image = image.crop((-10,-10,image.width+10,image.height+10)).resize((wanted_size, wanted_size))
	

# Create Tkinter Object
root = Tk()

root.protocol("WM_DELETE_WINDOW", on_close)
root.bind('<Motion>', motion)
root.bind("<Button-1>", on_lclick)
root.bind("<Button-3>", on_rclick)
root.bind("<Up>", on_up)
root.bind("<Down>", on_down)

dname = 'data_femboys'
dname_out = dname+'_filtered'

if not os.path.isdir(dname_out):
	os.mkdir(dname_out)

def save_with_changes(img: Image.Image, contrast:float,  angle:float, num:int):
	enhancer = ImageEnhance.Contrast(img)
	new = enhancer.enhance(contrast).rotate(angle)
	if angle != 0:
		new = new.resize((wanted_size+7, wanted_size+7))
		new = new.crop((7,7, wanted_size-7, wanted_size-7))
		new = new.resize((wanted_size, wanted_size))
	
	new.save(dname_out+"/"+str(i)+"_"+str(num)+".jpeg")
	
	for x in range(new.width):
		for y in range(new.height):
			v = new.getpixel((x,y))+np.random.randint(-25, 25)
			if v > 255:
				v = 255
			elif v < 0:
				v = 0
			new.putpixel((x,y), (v))

	new.save(dname_out+"/"+str(i)+"_"+str(num)+"_n.jpeg")

def save_image(img: Image.Image):
	# save_with_changes(img, 1.0, 0,		0)
	# save_with_changes(img, 1.0, 25,		1)
	# save_with_changes(img, 1.0, -25,	2)

	# save_with_changes(img, 1.5, 0,		3)
	# save_with_changes(img, 1.5, 25,		4)
	# save_with_changes(img, 1.5, -25,	5)

	# save_with_changes(img, 0.8, 0,		6)
	# save_with_changes(img, 0.8, 25,		7)
	# save_with_changes(img, 0.8, -25,	8)
	img.save(dname_out+"/"+str(i)+".jpeg")

def open_next_image(is_ok):
	global i
	global fstr
	global x
	global y
	global cropped_image

	x = 0
	y = 0
	real_x = 0
	real_y = 0
	if is_ok:
		fstr += str(i) + ' ' + str(real_x) + ' ' + str(real_y)+'\n'
		# new_image.save(dname_out+"/"+str(i)+".jpg")
		save_image(new_image)
	i+=1
	open_image(dname+"/"+str(i)+".jpg")
	cropped_image = image.copy()

# Importing the image
def open_image(fp):
	global image
	global use_x
	global fstr
	image = Image.open(fp)
	w, h = image.width, image.height
	image = image.convert('L')
	if w > h:
		use_x = 1
		image = image.resize((int(w*(wanted_size/h)), wanted_size))
	else:
		use_x = 0
		image = image.resize((wanted_size, int(h*(wanted_size/w))))
	if w == h:
		fstr += str(i) + ' 0 0\n'
		# image.save(dname_out+str(i)+".jpg")
		save_image(image)
		open_next_image(0)
	#landscape image

open_image(dname+"/"+str(i)+".jpg")

img = ImageTk.PhotoImage(image.crop((0+x,0+y, wanted_size+x, wanted_size+y)))
l = Label(image=img)
l.image = img
l.pack()

# Execute Tkinterdef task():
def task():
	global x
	global y

	#x+=1
	# do something
	root.update()

while 1:
   task()
