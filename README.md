![Okto programming language logo](https://git.maxwellj.xyz/repo-avatars/051ee58b3e6ca69fa4057c945d2ae46e6749c607092e93f5679bd8c6d338b1bc)

# oktolang - A silly little programming language

## What is Okto?

Okto is an interpreted programming language written in C++. It's a strongly typed language which tells you exactly where you messed up if there's an error. Both a REPL and file input are supported. 

## Why is Okto?

No reason.

Maybe one day the NodeJS community will find it and obsess over it in it's unoptimisedness. But that remains to be seen.

## How do I Okto?

Okto uses features from C++ 17, so make sure you have an updated compiler. I recommend gcc. Here are some instructions:

### Linux (and most other Unix-like)

First, install `gcc` or another C++ compiler.

Clone the repository, CD into it and run:

```bash
g++ src/main.cpp -o okto
```

If needed, statically link with `-static`. 

Run `./okto` to run, and test with `./okto example.okto` (assuming you did clone the repository)

### MacOS

First, install Xcode and accept the agreement. 

Then, clone the repository CD into it and run:

```bash
g++ src/main.cpp -o okto -std=c++17
```

If needed, statically link with `-static`.

Run `./okto` to run, and test with `./okto example.okto` (assuming you did clone the repository)

### Windows

There are three ways to compile for Windows: through Linux (I personally recommend, can be done through WSL), through MSYS2 or Cygwin, or through Windows.

#### Through Linux

Install your distro's mingw GCC package. On Arch Linux, it's `mingw-w64-gcc`. Compile with: 

```bash
x86_64-w64-mingw32 src/main.cpp -o okto.exe -static
```

If you're not using WSL, you may want to use Wine to test the build. At the time of writing, all Okto features should work when run with Wine. 

#### Through MSYS2/Cygwin

Refer to the documentation for these platforms. Steps should be similar to compiling for Linux.

#### Through Windows

Search for a package including g++ through `winget`, and install your preferred option. Compile with:

```powershell
g++ src/main.cpp -o okto
```

If needed, statically link with `-static` **This is highly recommended if distributing to other Windows machines.**.

Now we can start writing our code!

### The Basics of Okto

#### Syntax

Okto's syntax is still developing, and may change at any time. So if you update and your code breaks, check back here.

Functions usually work like this:

`functionName arg1 arg2 arg3`

A bit like Python 2 or shell.

Some built in functions include:

`log`: Log something to the console. Usage: `log (string, decimal, integer)` Example: `log "Hello, World!"`

`type`: Check the type of something. If it doesn't have a type, it's unknown. Usage: `type (function, variable, string, decimal, integer, incrementor, modifier, operator, comparitor, equals)` Example: `type 432`

`run`: Run a command on the system. May not work on every OS. Usage: `run (string)`

`in`: Take input from the console. For now, this doesn't do much, but this will change soon. Usage: `in (optional: string)` Example: `in "What's your name? "`

`exit`: Exit the program. Usage: `exit (optional: integer)`

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

#### Using variables

Use variables in your code like `$variable`, but replace variable with your actual variable's name. Make sure to use correct types in places! Incorporate a variable inside a string with `"${variable}"`

#### Updating Variables

Right now only integers and decimals can be changed. I'll refer to these as numbers.

Increment numbers with `myVariable ++` and decrement with `myVariable --`.

Add stuff together in number variables with `myVariable = $myOtherVariable + 7`. You can also subtract, multiply and divide with -, * and / respectively. For now you can't math outside of variables as it's still a prototype feature.