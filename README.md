# oktolang - A silly little programming language

## What is Okto?

Okto is an interpreted programming language written in C++. It's a strongly typed language which tells you exactly where you messed up if there's an error. Both a REPL and file input are supported. 

## Why is Okto?

No reason.

Maybe one day the NodeJS community will find it and obsess over it in it's unoptimisedness. But that remains to be seen.

## How do I Okto?

First, download the source code and your favourite C++ compiler. Next, clone the repo (`git clone https://git.maxwellj.xyz/max/okto`) and CD into it. Compile the code (if the GCC is your poison, `g++ src/okto -o okto`) and run the binary.

Now we can start writing our code!

### The Basics of Okto

#### Syntax

Okto's syntax is still developing, and may change at any time. So if you update and your code breaks, check back here.

Functions usually work like this:

`functionName arg1 arg2 arg3`

A bit like Python 2 or shell.

Some built in functions include:

`log`: Log something to the console. Usage: `log (variable, string, decimal, integer)` Example: `log "Hello, World!"`
`type`: Check the type of something. If it doesn't have a type, it's unknown. Usage: `type (function, variable, string, decimal, integer, incrementor, modifier, operator, comparitor, equals)` Example: `type 432`

Okto is strongly typed, which means we need to learn about all the different types in the language! At present there are three types in Okto: strings, integers and decimals. Defining variables uses a C/C++ like syntax.

#### Strings

Strings are a bunch of letters enclosed in double quotes, like `"this"`.

Define a string in Okto with `str myString = "Hello World!"`

#### Integers

Integers are whole numbers, like `7`.

Define an integer in Okto with `int myInteger = 7`

#### Decimals

Decimals are non-whole numbers, like `3.14159`.

Define a decimal in Okto with `dec myDecimal = 3.14159`