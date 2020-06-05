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

# `enhanced-lite` branch
This branch deviates the most from the `accurate` branch, focussing less on
authenticity, and more on improving the base engine.

## Goal
This branch is intended to benefit modders, by providing a feature-rich base for
mods, while still keeping the original game's 'feel' (think of the Boom project
rather than Chocolate Doom or GZDoom).

That said, I want to avoid the codebase becoming an over-engineered mess of
seldom-used and niche features - only features with a strong reason to be
included should be added.

Priority features include ones that already exist in the modding community as
"EXE hacks", such as...
* Common custom TSC commands ('<MIM', '<PHY')
* Custom weapons
* A money system
* The "Cave Story Mod Loader" (widescreen, support for Ogg Vorbis music)

Note that such features should only be added if they see widespread use.

## Target audience
While this branch is meant as a base for mods, it is not intended for modders
unfamiliar with programming. This means features that only exist to benefit
non-programmers will not be accepted, as they will likely clutter the codebase,
making it harder for programmers to make modifications.

## Accuracy to the original source code
Despite this branch being less accuracy-focussed, I still want modifications to
the vanilla code to be minimal - that means no rewriting code to "look nicer" or
add micro-optimisations.

Keeping the vanilla code intact guarantees CSE2E will accurately-reproduce
Cave Story's gameplay, avoiding an NXEngine-scenario where the game is riddled
with inaccuracies, both small and large. This also has the benefit of making it
easier to merge changes from the other branches.

## Enhancements unrelated to modding
Some features have made their way into this branch, despite not actually being
of any direct use to modders - one example of this is the option to disable the
pixel-alignment effect.

The reason for allowing these features is that, while they are of no direct
benefit to modders, they are of benefit _to players._ Thus, a mod with these
features is better than a mod without them. Likewise, the input-rebinding menu
falls under a similar use.

The line is drawn when a feature comes at a _detriment_ to modders - for
example, having an option to select an alternative soundtrack (such as the
Nicalis ones) is nice for the player, but will complicate modding, as, if a mod
includes custom music, the modder will either have to disable the soundtracks
they don't support, or they'll have to create multiple versions of each song.

# `enhanced` branch
This branch is built upon the `enhanced-lite` branch, and follows similar ideals
to said branch. However, this branch targets players rather than modders,
allowing a greater range of features to be added.
