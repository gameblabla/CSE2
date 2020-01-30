Being a pure, plain decompilation of the original `Doukusu.exe` file (v1.0.0.6),
there should not be much to say about the project's philosophies:

## Goal
The end-goal is for the project to be able to produce an `.exe` file that is
identical to the original. This means that there should be no custom code,
decompiled code should ideally be made to produce the same assembly code as the
original, bugs should be left intact, etc.

Another goal of the project is to document Cave Story's inner-working, so code
should be reasonably-annotated.

## Accuracy to the original source code
I do aim to make the decompiled code _functionally_-accurate to the original,
down to generating the same assembly code, but I do not aim for
_visually_-accurate code.

Despite this, I personally do try to preserve the original function/variable
names, variable-declaration locations, and source file naming.

Part of the reason why I do not aim for visually-accurate source code is that we
know from the [Organya source code release](https://github.com/shbow/organya/)
what Pixel's code-style looked like, and I find it **extremely** hard to read.

## Language
Cave Story's developer, Pixel, primarily speaks Japanese, but his code's
variable/function names are mostly written in English (with rare instances of
Romanised Japanese).

The Organya source code release indicates Pixel wrote his comments in Japanese,
however, in this project, I prefer them to be written in English.

The English employed in this project is a mix of American English, Canadian
English, and British English.
