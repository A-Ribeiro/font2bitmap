> For this is how God loved the world:  
he gave his only Son, so that everyone  
who believes in him may not perish  
but may have eternal life.  
  \
John 3:16

## Font tool: font2bitmap - OpenGL Font 2.0

The command line tool __font2bitmap__ now generates the new format(Binary ASilva OpenGL Font, version 2.0 - basof2) and the old at the same time.

The input is any Freetype2 supported font format (.ttf, .otf, etc...).

The parameters to the command line tool:

* __-i:__ input file(.ttf, .otf, freetype compatible font)
* __-f:__ face in font to be selected (default: 0)
* __-o:__ output file without extension
* __-s:__ character size in pixels to be exported (minimum:14).
* __-c:__ text file with all characters to be generated in utf8.
* __-p:__ size in pixels of the space between the characters exported.
* __-t:__ outline thickness real number.

Example:

```bash
# Generates a font file called output-20.basof2
# parameters: -s 20 the square pixel size is 20 pixels.
#             -c charset.utf8 the input character list utf-8 file
#             -p 6 the minimum distance in pixels from each 
#                  character from another in the exported atlas.
#             -t 0.33333 the thickness of the outline
font2bitmap -i "input.ttf" -o "output" -s 20 -c charset.utf8 -p 6 -t 0.33333
```

## How to Clone?

This library uses git submodules.

You need to fetch the repo and the submodules also.

### a) Clone With Single Command

__HTTPS__

```bash
git clone --recurse-submodules https://github.com/A-Ribeiro/font2bitmap.git
```

__SSH__

```bash
git clone --recurse-submodules git@github.com:A-Ribeiro/font2bitmap.git
```

### b) Clone With Multiple Commands

__HTTPS__

```bash
git clone https://github.com/A-Ribeiro/font2bitmap.git
cd OpenGLStarter
git submodule init
git submodule update
```

__SSH__

```bash
git clone git@github.com:A-Ribeiro/font2bitmap.git
cd OpenGLStarter
git submodule init
git submodule update
```

## Related Links

https://github.com/A-Ribeiro/aRibeiroCore

https://github.com/A-Ribeiro/aRibeiroPlatform

https://github.com/A-Ribeiro/aRibeiroData

https://github.com/A-Ribeiro/aRibeiroWrappers

https://github.com/A-Ribeiro/aRibeiroTests

## Tools

https://github.com/A-Ribeiro/assimp2bams

https://github.com/A-Ribeiro/font2bitmap

## Authors

***Alessandro Ribeiro da Silva*** obtained his Bachelor's degree in Computer Science from Pontifical Catholic 
University of Minas Gerais and a Master's degree in Computer Science from the Federal University of Minas Gerais, 
in 2005 and 2008 respectively. He taught at PUC and UFMG as a substitute/assistant professor in the courses 
of Digital Arts, Computer Science, Computer Engineering and Digital Games. He have work experience with interactive
software. He worked with OpenGL, post-processing, out-of-core rendering, Unity3D and game consoles. Today 
he work with freelance projects related to Computer Graphics, Virtual Reality, Augmented Reality, WebGL, web server 
and mobile apps (andoid/iOS).

More information on: https://alessandroribeiro.thegeneralsolution.com/en/
