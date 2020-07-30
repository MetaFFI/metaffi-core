package main

import (
	"fmt"
	"strings"
)

func F1(p1 []string) string{
	res := strings.Join(p1, ",")
	fmt.Println(res)

	return res
}

func F2(p21 string, p1 Params1) (string, Return1){

	str := p21 + strings.Join(p1.P1, ",")
	r1 := Return1{
		R1:	"Inner String!",
	}

	return str, r1
}