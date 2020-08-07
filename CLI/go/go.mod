module github.com/GreenFuze/OpenFFI/CLI/go

go 1.12

replace github.com/GreenFuze/OpenFFI/CLI/utils/go => ../utils/go

require (
	github.com/GreenFuze/OpenFFI/CLI/utils/go v0.0.0-20200807141947-6a5af47915ab
	github.com/Masterminds/sprig v2.22.0+incompatible
	github.com/antchfx/jsonquery v1.1.4
	github.com/golang/protobuf v1.4.2
	github.com/yoheimuta/go-protoparser v3.4.0+incompatible
)
