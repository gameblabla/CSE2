## About

clownaudio is a stereo sound engine, capable of playing and mixing sounds in a
variety of formats.

Supported formats include...
* Ogg Vorbis
* MP3
* Opus
* FLAC
* WAV
* Various tracker formats ('.it', '.xm', '.mod', etc.)
* PxTone Music
* PxTone Noise
* SNES SPC

clownaudio is a 'full stack' sound engine, meaning that it handles everything
from decoding to playback - the user merely has to provide it with sound data to
process.

That said, clownaudio's internals are modular, meaning you can easily extract
its mixer and use it as part of your own audio system if needed.


## Decoding backends

In order to support a range of audio formats, clownaudio leverages numerous
open-source libraries, dubbed 'decoding backends'. These libraries include...

| Library     | Format     | Licence             | Included in-tree |
|-------------|------------|---------------------|------------------|
| libvorbis   | Ogg Vorbis | BSD                 | No               |
| stb_vorbis  | Ogg Vorbis | Public-domain/MIT-0 | Yes              |
| dr_mp3      | MP3        | Public-domain/MIT-0 | Yes              |
| libopus     | Opus       | BSD                 | No               |
| libFLAC     | FLAC       | BSD                 | No               |
| dr_flac     | FLAC       | Public-domain/MIT-0 | Yes              |
| dr_wav      | WAV        | Public-domain/MIT-0 | Yes              |
| libsndfile  | Various (includes Ogg Vorbis, FLAC, WAV, AIFF, and others) | LGPL 2.1 | No |
| libopenmpt  | Various (includes .mod, .it, .xm, .s3m, .mptm, and **many** others) | BSD | No |
| libxmp-lite | .mod, .it, .xm, .s3m | MIT       | Yes              |
| PxTone      | PxTone Music/PxTone Noise | Custom (appears to be permissive) | Yes |
| snes_spc    | SNES SPC   | LGPL 2.1            | Yes              |

clownaudio aims to be bloat-free and dependency-free: each decoding backend can
be toggled at compile-time, and an effort is made to provide multiple backends
for individual formats, to allow the user a choice between standard libraries
(such as libvorbis), and lighter in-tree libraries (such as stb_vorbis).


## Playback backends

In addition, clownaudio utilises one of the following libraries to provide
audio-playback:

| Library   | Licence             | Included in-tree |
|-----------|---------------------|------------------|
| Cubeb     | ISC                 | No               |
| miniaudio | Public-domain/MIT-0 | Yes              |
| PortAudio | MIT                 | No               |
| SDL1.2    | LGPL 2.1            | No               |
| SDL2      | zlib                | No               |


## Building

clownaudio is built with CMake - both shared and static libraries can be
produced. Built libraries can be accessed via `pkg-config` and CMake's
`find_package`.

As an alternative, CMake projects can embed clownaudio directly, using
`add_subdirectory`.

Example `Makefile` and `CMakeLists.txt` files for a demo tool are provided: the
`Makefile` demonstrates linking clownaudio with `pkg-config`, and the
`CMakeLists.txt` file demonstrates linking with `find_package` and embedding
with `add_subdirectory`.

`PKGBUILD` files for MSYS2 and Arch Linux are provided in the `packages`
directory.


## Licensing

clownaudio itself is under the zlib licence.

Be aware that libraries used by the various backends are subject to
their own licences.
