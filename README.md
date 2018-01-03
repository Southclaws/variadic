# Variadic functions in PAWN.

## Introduction
Variadic functions are a feature in most scripting languages, where a scripter can specify an indefinite amount of arguments towards a function. However, in PAWN, this isn't possible without playing around with the assembly (later versions of PAWN have dealt with this), but this include introduces a brand new way of handling them!

## More information
Have you ever wrote a function like this before?

```pawn
printex(str[], ...)
{
    new output[128];

    format(output, sizeof(output), str, ...);

    return print(output);
}
```

And then you figured out that you can't do it this way! The reason being is because variadic parameters don't have a referable location in ordinary PAWN code, so assembly is required, but that can be very complicated and one mistake could corrupt the entire script.

However, this include allows scripters to dynamically push arguments and format them. The pushed arguments are stored within an array by address, so they can be accessed later with the corresponding functions.

## Example
An equivalent of the function in the introduction is possible using this include:

```pawn
#include <a_samp>
#include <variadic>

var printex(str[])
{
    new output[128];

    format(output, sizeof(output), str, @variadic[1]);
    return print(output);
}
```

You can also use `@variadic[]` with `CallLocalFunction` and such to call local functions with the corresponding pushed arguments.

Alternatively, you can also use `stock`, but make sure you place `...` after the parameters.

## String arguments
You can retrieve string arguments easily.

**Getting:**

```pawn
stock MyFunction(a[], b[])
{
    new addr = GetVariadicAddress(0); // Address of "a[]".

    print(GetVariadicString(addr)); // Print the result.
}
```

**Setting:**

```pawn
stock MyFunction(a[], b[])
{
    new addr = GetVariadicAddress(1); // Address of "b[]".

    SetVariadicString(addr, "Hi there.");

    print(GetVariadicString(addr)); // Print the result.
}
```

## Functions
```pawn
// Get the address of a function argument.
native GetVariadicAddress(argument);

// Get the string from the specified argument address. You can return it or store it inside "dest".
native GetVariadicString(address, dest[] = "", size = sizeof(dest));

// Set the string from the specified argument address to the specified string.
native SetVariadicString(address, str[]);

// Checks if an argument is packed.
native IsArgumentPacked(address);

// Gets the length of a variadic string.
native GetVariadicLength(address);

// Delete an argument from the specified argument address.
native DeleteArgument(address);

// Popped the pushed arguments.
native PopArguments();
```

## Obsolete functions
These functions are obsolete since version 2. However, I am simply keeping them for another feature I'm adding to this include, so the best thing to do is ignore them.

```pawn
// Push all arguments from "start" to "end" (obsolete).
native PushArguments(start, end);

// "Format" the variadic arguments.
native VariadicFormat(output[], len, const str[]);
  	
// Call a local function with the variadic arguments.
native CallLocalVariadic(function[], const specifiers[]);

// Call a remote function with the variadic arguments.
native CallRemoteVariadic(function[], const specifiers[]);
```

Alternatively, you can use `@variadic[]` instead of these functions - but it's up to you.
