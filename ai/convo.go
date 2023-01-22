package ai

import (
	"encoding/base64"
	"image/png"
	"os"
	"strings"
)

type image struct {
	pixels []byte // Raw pixel data
}

type cUnit struct {
	image
}

func newImage(fp string) *image {
	i := new(image)
	// Reading the file
	file, _ := os.Open(fp)
	fileInfo, _ := file.Stat()
	fileSize := fileInfo.Size()
	var rawData []byte = make([]byte, fileSize)
	file.Read(rawData)

	reader := base64.NewDecoder(base64.StdEncoding, strings.NewReader(string(rawData)))
	iRaw, _ := png.Decode(reader)

	mono := true
	for y := 0; y < iRaw.Bounds().Dy(); y++ {
		for x := 0; x < iRaw.Bounds().Dx(); x++ {
			r, g, b, a := iRaw.At(x, y).RGBA()
		}
	}

	return i
}
