//+build !guest

package main

import "fmt"
import "os"

func main(){

	// call foreign functions via OpenFFI

    // Prints hello world.
    fmt.Println("Calling WordsCount('one,two,three,four can I have a little more?'):")
    res, err := WordsCount("one,two,three,four can I have a little more?")  // -- calling hello_world
    if err != nil{
        fmt.Printf("Failed to call WordsCount() - %v\n", err)
        os.Exit(1)
    }

    if res != 10{
        fmt.Printf("Test failed. Expected 10, received %v\n", res)
        os.Exit(1)
    }

    fmt.Println(res)
}