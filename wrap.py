text = """If you are transitioning from convolutional to neural, don't consider the transitional layer as
the translation between convolutional to neural, this is done automatically when the layer is ran,
in other words, this transitional layer is just another hidden layer that fully connects to the
pervious convolutions, not a layer to which every pixel from to convolutions is copied to.

You can use MLE_HP_AUTO to automate the amount of neurons that will be used for this layer, it
makes the layer size around half of the previous one, though it depends on future layers.

You can use MLE_HP_LAST to set the amount of neurons to that of the last layer, if it's
a convolutional layer, the total amount of the convolutional pixels will be added up, this
calculation can get expensive the deeper your convolutions are(the more CNN layers there are)."""

text = text.replace("\n", " ")
text = text.replace("  ", "\n\n")

n = ""
l = 0
for w in text.split(" "):
	if l >= 80:
		n += w + '\n'
		l = 0
	else:
		n += w + ' '
	l += len(w) + len(' ')
print(n)	
