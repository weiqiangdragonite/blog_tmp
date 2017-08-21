/*
 *
 */

package main

import (
    "fmt"
    "image"
    "image/color"
    "image/gif"
    "io"
    "math"
    "math/rand"
    "os"
    "net/http"
    "log"
    "time"
    "strconv"
)

var palette = []color.Color{color.White, color.Black}
const (
    white_index = 0
    black_index = 1
)

func main() {
    rand.Seed(time.Now().UTC().UnixNano())
    http.HandleFunc("/", handler)
    log.Fatal(http.ListenAndServe("0.0.0.0:8000", nil))
}

func handler(w http.ResponseWriter, r *http.Request) {
    if err := r.ParseForm(); err != nil {
        log.Print(err)
        os.Exit(1)
    }
    for k, v := range r.Form {
        var ccy int
        if k == "cycles" {
            ccy, _ := strconv.Atoi(v[0])
        } else {
            ccy := 5
        }
        fmt.Printf("Form[%q] = %q, ccy = %d\n", k, v, ccy)
    }
    //fmt.Printf("cycles = %d\n", cycles)
    lissajous(w)
}

func lissajous(out io.Writer) {
    const (
        cycles = 5
        res = 0.001
        size = 100
        nframes = 64
        delay = 8
    )

    freq := rand.Float64() * 3.0
    anim := gif.GIF{LoopCount: nframes}
    phase := 0.0
    for i := 0; i < nframes; i++ {
        rect := image.Rect(0, 0, 2*size+1, 2*size+1)
        img := image.NewPaletted(rect, palette)
        for t := 0.0; t < float64(cycles)*2*math.Pi; t += res {
            x := math.Sin(t)
            y := math.Sin(t*freq + phase)
            img.SetColorIndex(size+int(x*size+0.5), size+int(y*size+0.5), black_index)
        }
        phase += 0.1
        anim.Delay = append(anim.Delay, delay)
        anim.Image = append(anim.Image, img)
    }
    gif.EncodeAll(out, &anim)
}

