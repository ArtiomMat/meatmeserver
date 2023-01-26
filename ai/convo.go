package ai

import (
	"encoding/base64"
	"image/jpeg"
	"os"
	"strings"
)

type channel struct {
}

type Image interface {

}

type RawImage struct {
	width, height uint16
	pixels []float32 // Raw pixel data
	channels []channel
}

func (Image *i) Open(fp string) {
	// Reading the file into a raw image struct
	file, _ := os.Open(fp)
	fileInfo, _ := file.Stat()
	fileSize := fileInfo.Size()
	var rawData []byte = make([]byte, fileSize)
	file.Read(rawData)

	reader := base64.NewDecoder(base64.StdEncoding, strings.NewReader(string(rawData)))
	iRaw, _ := jpeg.Decode(reader)

	// Parsing iRaw
	mono := true
	transparent := false
	for y := 0; y < iRaw.Bounds().Dy(); y++ {
		for x := 0; x < iRaw.Bounds().Dx(); x++ {
			r, g, b, a := iRaw.At(x, y).RGBA()

			if r != g && r != b {

			}
		}
	}

	return i
}

type cUnit struct {
	image
}

