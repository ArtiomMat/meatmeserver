import os
import tkinter as tk
from PIL import Image, ImageTk, ImageFilter, ImageEnhance
import time


w,h = 0,0

dir_in = "data_femboys"
dir_in_files = os.listdir(dir_in)
dir_in_files.sort()
canvas_size = 600

x,y=0,0 # Actual x and y

start_x,start_y,end_x,end_y = 0,0,0,0

generate_rectangle,move_rectangle=False,False


move_x,move_y=0,0

cfg_str = dir_in+"\n"

def motion(event):
	global x, y
	global canvas_rect, generate_rectangle, canvas, move_rectangle, start_y, start_x, end_x, end_y
	
	x, y = event.x, event.y
	
	if generate_rectangle:
		# y = start_y+(x-start_x)
		canvas.coords(canvas_rect, start_x, start_y, x, y)
	elif move_rectangle:
		move_x = x
		move_y = y
		canvas.coords(canvas_rect, move_x, move_y, abs(end_x-start_x)+move_x, abs(end_y-start_y)+move_y)
		start_x,start_y,end_x,end_y = move_x, move_y, abs(end_x-start_x)+move_x, abs(end_y-start_y)+move_y
		
def on_press_l(e):
	global generate_rectangle,x,start_x,start_y,end_y,end_x
	end_x,end_y=0,0
	start_x,start_y = x,y
	generate_rectangle = True

def on_release_l(e):
	global generate_rectangle,x,end_x,end_y,start_x,start_y
	end_x,end_y = x,y
	if (end_x < start_x):
		tmp_x,tmp_y = start_x,start_y
		start_x,start_y=end_x,end_y
		end_x,end_y=tmp_x,tmp_y
	generate_rectangle = False

def on_press_r(e):
	global move_rectangle,move_x,move_y
	move_x,move_y=x,y
	move_rectangle = True

def on_release_r(e):
	global move_rectangle
	move_rectangle = False

i = 0
change_image = False
def on_key(event):
	global i, change_image, cfg_str
	if event.char == ' ':
		left,top,right,bottom = start_x/w, start_y/h, end_x/w, end_y/h
		cfg_str += dir_in_files[i] +" "+ str(left) +" "+ str(top) +" "+ str(right) +" "+ str(bottom) +"\n"
		i+=1
		change_image = True
	elif event.char == '\b':
		cfg_str += dir_in_files[i] +" BAD\n"
		i+=1
		change_image = True


def open_image(i):
	global w,h
	if i >= len(dir_in_files):
		print("DONE!")
		on_close()
		return
	pil_image = Image.open(dir_in+"/"+dir_in_files[i])
	w, h = pil_image.width, pil_image.height

	if w > h:
		pil_image = pil_image.resize((canvas_size, int(h*(canvas_size/w))))
	else:
		pil_image = pil_image.resize((int(w*(canvas_size/h)), canvas_size))

	w, h = pil_image.width, pil_image.height
	image = ImageTk.PhotoImage(pil_image)

	return image

def on_close():
	global loop_running
	cfg_f = open(dir_in+"_cfg.txt", "a")
	cfg_f.write(cfg_str)
	cfg_f.close()
	loop_running = False
	
######################### MAIN #########################

if os.path.isfile(dir_in+"_cfg.txt"):
	cfg_f = open(dir_in+"_cfg.txt", "r")
	if cfg_f.readable():
		cfg_str = ""
	s = cfg_f.read()
	i = max(0, s.count("\n")-1)

loop_running = True

root = tk.Tk()

root.wm_resizable(False,False)
root.protocol("WM_DELETE_WINDOW", on_close)

root.bind('<Motion>', motion)

root.bind("<ButtonPress-1>", on_press_l)
root.bind("<ButtonRelease-1>", on_release_l)

root.bind("<ButtonPress-3>", on_press_r)
root.bind("<ButtonRelease-3>", on_release_r)

root.bind("<Key>", on_key)

image = open_image(i)

canvas = tk.Canvas(root, width=canvas_size, height=canvas_size)
canvas.pack()
canvas_image = canvas.create_image(0,0,anchor="nw",image=image)
canvas_rect = canvas.create_rectangle(0,0,0,0, outline="red")


while loop_running:
	root.update()
	if change_image:
		img=open_image(i)
		canvas.itemconfig(canvas_image, image=img)
		change_image = False
	time.sleep(0.016)
