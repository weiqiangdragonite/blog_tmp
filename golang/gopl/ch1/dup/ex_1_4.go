/*
 *
 */

package main

import (
    "fmt"
    "os"
    "bufio"
)

func main() {
    counts := make(map[string]int)
    filename, emp := "", ""
    files := os.Args[1:]
    if len(files) != 0 {
        for _, arg := range files {
            f, err := os.Open(arg)
            if err != nil {
                fmt.Fprintf(os.Stderr, "dup2: %v\n", err)
                continue
            }
            countLines(f, counts)
            f.Close()
            if is_dup(counts) {
                filename = emp + arg
                emp = " "
            }
        }
    }
    fmt.Printf("Filename: %s\n", filename)
}

func is_dup(counts map[string]int) bool {
    for _, n := range counts {
        if n > 1 {
            return true
        }
    }
    return false
}

func countLines(f *os.File, counts map[string]int) {
    input := bufio.NewScanner(f)
    for input.Scan() {
        counts[input.Text()]++
    }
    // 忽略 input.Err() 中可能的错误
}

