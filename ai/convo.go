package ai

import (
	"encoding/base64"
	"image/jpeg"
	"os"
	"strings"
)

type image struct {
	pixels []float32 // Raw pixel data
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
	iRaw, _ := jpeg.Decode(reader)

	mono := true
	transparent := false
	for y := 0; y < iRaw.Bounds().Dy(); y++ {
		for x := 0; x < iRaw.Bounds().Dx(); x++ {
			r, g, b, a := iRaw.At(x, y).RGBA()

			if r == g && r == b {
				if a < 255 {

				}

			}
		}
	}

	return i
}
