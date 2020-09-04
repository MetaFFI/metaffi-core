# OpenFFI Usage Examples

Each example contains a readme.md file with details about the functions.\

## Running The Examples

Each example directory contains `run_example.sh` for Ubuntu and MacOS and `run_example.bat` for Windows. Running the example downloads example dependencies (if there are any), runs OpenFFI, build the example and executes the code.

## Real World Examples

These examples show how to use 3rd party libraries with OpenFFI:
* `RWgo2python3` - Call from Go to a function in [python-string-utils](https://pypi.org/project/python-string-utils/) library installed by *pip*
* `RWpython32go` - Call from Python3 to a function in [goutils](github.com/Masterminds/goutils) in github.com

## Local & Tests Examples

The following examples implement the same functions while using all possible combinations of the official supported languages:
* Go &rarr; Python3
* Python3 &rarr; Go
* Python3 &rarr; Python3
    * Important for testing purposes. To support cases of nested guest languages
* Go &rarr; Go
    * This example is for experimental purposes - test is expected to crash (or at least be not stable)
    * Go cannot load another Go C-Shared library as it causes Go to load Go-Runtime twice which **causes the application to crash**
    
### Foreign Functions

Functions are seperated into 2 modules to test and validate the usage of multiple modules within a single IDL:
* *GoUtils* or *PythonicUtils*
* *ComplexTypes*

#### *GoUtils* or *PythonicUtils*
* `void HelloWorld(void)`
    * Validates void parameter and return value
* `float Div(int32 x, int32 y)` 
    * Validates data types correctness
    * Tests error when y=0 (performs integer division, not float division)
* `string ConcatStrings(string[] arr_of_strings)`
    * Validates usage of arrays
    * Validates correct usage of strings

#### *ComplexTypes*

##### structs
OpenFFI (using the serializer) allows defining complex types that can be used a parameter and return value.

```
game_data
{
	string player = 1;
	int32 pawn_wins = 2;
	int32 boss_wins = 3;
}

game_score
{
	string player = 1;
	int32 score = 2;
}
```

##### functions
* `game_score CalculateScore(game_data gdata)`
    * Validate usage of complex types
    * Validates usage of nested complex type (in Protobuf)
* `(int32 total_score, game_score[] scores) GenScores(void)`
    * Validates multiple return values
    * Validates array of complex type
    * Validates usage of nested complex type in return value (in Protobuf)


    




