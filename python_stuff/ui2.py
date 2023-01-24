import os
import tkinter as tk
from PIL import Image, ImageTk, ImageFilter, ImageEnhance
import time

#################################################

dir_in = "./data_femboys"
dir_in_files = os.listdir(dir_in)

dir_out = dir_in+"_filtered"
image_out_size = 64

image_index = -1 # Don't worry about it man, keep going, it's -1 for a reason.
image_original: Image.Image
image_ratio = 1
image_zoomed: Image.Image
# The image that is previewed in the tk window & exported(after resizing ofc)
image_preview: Image.Image
image_preview_size = 300

image_tk: ImageTk.PhotoImage
image_tk_label: tk.Label

# x is equal to 0 at cursor_padding and is equal to 1 at width-cursor_padding
# Same with y.
cursor_padding = 30

zoom_factor = 30
zoom_level = 0 # How zoomed we are essentially.

# Saved to put it in the cfg file.
x_norm,y_norm = 0,0
# The final x and y in image_zoomed space.
x_final,y_final = 0,0

cfg_str = dir_in+"\n"

#################################################

def update_tk():
	global image_preview
	global image_tk_label
	global image_tk

	image_preview = image_zoomed.crop((x_final, y_final,image_preview_size+x_final, image_preview_size+y_final))
	image_tk = ImageTk.PhotoImage(image_preview)
	image_tk_label.configure(image=image_tk)

def open_next_image():
	global image_index
	global image_original
	global image_preview
	global image_zoomed
	global image_tk
	global image_ratio

	image_index+=1

	image_original = Image.open(dir_in+"/"+dir_in_files[image_index])
	image_original = image_original

	w, h = image_original.width, image_original.height
	image_ratio = w/h
	
	# Confining image to image_preview_size
	w, h = image_original.width, image_original.height
	if w > h:
		image_original = image_original.resize((int(w*(image_preview_size/h)), image_preview_size))
	else:
		image_original = image_original.resize((image_preview_size, int(h*(image_preview_size/w))))

	image_zoomed = image_original.copy()

	
	image_preview = image_original.crop((0,0,image_preview_size,image_preview_size))

	image_tk = ImageTk.PhotoImage(image_preview)


# Returns value from 0 to 1 proportional to padding and the preview size.
def normalize(v):
	global image_preview_size, cursor_padding
	if v > image_preview_size-cursor_padding:
		v = image_preview_size-cursor_padding
	elif v < cursor_padding:
		v = cursor_padding
	
	# v = ((v-cursor_padding)/(image_preview_size-cursor_padding*2)) * (full_dimention-image_preview_size)
	v = (v-cursor_padding)/(image_preview_size - 2*cursor_padding)

	return v

def on_motion(event):
	global image_preview, image_zoomed
	global x_norm,y_norm
	global x_final,y_final

	x_norm = normalize(event.x)
	y_norm = normalize(event.y)

	x_final = x_norm*(image_zoomed.width-image_preview_size)
	y_final = y_norm*(image_zoomed.height-image_preview_size)

	x_final = int(x_final)
	y_final = int(y_final)

def on_wheel(event):
	global image_zoomed
	global zoom_level

	# Cross platform method for both Linux and Windows
	if event.num == 5 or event.delta == -120:
		zoom_level -= 1
	elif event.num == 4 or event.delta == 120:
		zoom_level += 1
	
	# Limit zoom level
	if zoom_level < 0:
		zoom_level = 0
	
	z = zoom_factor*zoom_level
	image_zoomed = image_original.resize((int(image_original.width+z*image_ratio),image_original.height+z))

def on_lclick(event):
	global cfg_str
	# The cropped box of the original image.
	box = (
		(x_final)/image_original.width, 
		(y_final)/image_original.height,
		(image_preview_size+x_final)/image_original.width, 
		(image_preview_size+y_final)/image_original.height
	)
	add = dir_in_files[image_index] + str(box) + "\n"
	cfg_str += add
	print(add)

def on_close():
	global loop_running
	cfg_f = open(dir_out+"/cfg.txt", "w+")
	cfg_f.write(cfg_str)
	cfg_f.close()

	loop_running = False

def on_key(event):
	if event.char == ' ':
		open_next_image()

#################################################
loop_running = True
def main():
	global image_tk_label
	global image_tk
	global loop_running
	
	root = tk.Tk()
	root.wm_resizable(False,False)
	root.bind('<Motion>', on_motion)
	# For Windows
	root.bind('<MouseWheel>', on_wheel)
	# For Linux
	root.bind("<Button-4>", on_wheel)
	root.bind("<Button-5>", on_wheel)

	root.bind("<Key>", on_key)
	root.bind("<Button-1>", on_lclick)
	root.protocol("WM_DELETE_WINDOW", on_close)
	
	open_next_image()

	image_tk_label = tk.Label(root, image=image_tk)
	image_tk_label.pack()
	
	canvas = tk.Canvas(root, width=image_preview_size, height=image_preview_size)
	canvas.pack()
	
	while loop_running:
		update_tk()
		root.update()
		
		time.sleep(0.016)
main()
