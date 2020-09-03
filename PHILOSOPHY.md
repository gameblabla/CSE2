# `accurate` branch
Being a pure, plain decompilation of the original `Doukutsu.exe` file (v1.0.0.6),
there should not be much to say about this branch's philosophies:

## Goal
The end-goal is for the project to be able to produce an `.exe` file that is
identical to the original. This means that there should be no custom code,
decompiled code should ideally be made to produce the same assembly code as the
original, bugs should be left intact, etc.

Another goal of the project is to document Cave Story's inner-working, so code
should be reasonably-annotated. Likewise, bugs should be documented, and fixes
provided wrapped in `#ifdef FIX_BUGS` conditions.

## Accuracy to the original source code
Personally, I do aim to make the decompiled code _functionally_-accurate to the
original, down to generating the same assembly code, but I do not aim for
_visually_-accurate code.

Despite this, I do try to preserve the original function/variable names,
variable-declaration locations, and source file naming.

Part of the reason why I do not aim for visually-accurate source code is that we
know from the [Organya source code release](https://github.com/shbow/organya/)
what Pixel's code-style looked like, and I find it **extremely** hard to read.

## Language
Cave Story's developer, Pixel, primarily speaks Japanese, but his code's
variable/function names are mostly written in English (with a few rare instances
of Romanised Japanese).

The Organya source code release indicates Pixel wrote his comments in Japanese,
however, in this project, I prefer them to be written in English.

The English employed in this project is a mix of American English, Canadian
English, and British English.

# `portable` branch
This branch takes a different direction to the `accurate` branch, but they still
share some core philosophies:

## Goal
The goal is still to accurately-reproduce Cave Story's original code, but
modified to the extent that it is easy to port to other platforms. This branch
is intended for purists that don't want to use Windows or its deprecated APIs.

Notably, this means bugs should still be left intact. However, bugs and other
coding errors that affect portability should be fixed.

For comparison, I believe this branch shares some parallels with the
Chocolate Doom project, except perhaps more strict. See the link below for their
list of philosophies which may be applicable here:

https://github.com/chocolate-doom/chocolate-doom/blob/master/PHILOSOPHY.md

## Custom code
Cave Story's original source code was written in C++, but stylised like C89
(with a number of exceptions). Custom code added to the project should follow
suit, but the C-style doesn't have to be C89 - personally, I prefer to write in
the subset of C99 that C++98 allows.

I prefer compiler-specific code to be avoided, since more-portable code benefits
all compilers, and keeps the codebase free of clutter.
