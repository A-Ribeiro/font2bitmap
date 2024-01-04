#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include <ITKWrappers/FT2.h>

#ifdef _WIN32
#include "win32/getopt.h"
#else
#include <getopt.h>
#endif

std::string UInt32toString(uint32_t i)
{
    char result[16];
    sprintf(result, "%u", i);
    return result;
}

std::string UInt32toStringHEX(uint32_t i)
{
    char result[16];
    sprintf(result, "%x", i);
    return result;
}

std::u32string readUTF32fromFile(const char *filename)
{
    FILE *in = fopen(filename, "rb");
    std::string buffer;
    if (in)
    {
        fseek(in, 0, SEEK_END);
        buffer.resize(ftell(in));
        fseek(in, 0, SEEK_SET);
        int readed_size = (int)fread(buffer.data(), sizeof(uint8_t), buffer.size(), in);
        fclose(in);
    }
    return ITKCommon::StringUtil::utf8_to_utf32(buffer);
}

struct ParametersSet
{
    bool i, o, s, c, p, t;
    std::string inputFile;
    std::string outputFile;
    int characterSize;
    std::string characterUtf8Charset;
    int spaceBetweenChar;
    float outlineThickness;
    int faceToSelect;
};

int main(int argc, char *argv[])
{
    fprintf(stdout, "font2bitmap - alessandroribeiro.thegeneralsolution.com\n");

    ParametersSet parameters = {0, 0, 0, 0, 0, 0, "", "", 0, "", 0, 0.0f, 0};
    int c;
    opterr = 0;

    char validOp[] = {"i:o:s:c:p:t:f:"};
    while ((c = getopt(argc, argv, validOp)) != -1)
    {
        switch (c)
        {
        case 'i':
            parameters.i = true;
            parameters.inputFile = optarg;
            break;
        case 'o':
            parameters.o = true;
            parameters.outputFile = optarg;
            break;
        case 's':
            parameters.s = true;
            parameters.characterSize = atoi(optarg);
            break;
        case 'c':
            parameters.c = true;
            parameters.characterUtf8Charset = optarg;
            break;
        case 'p':
            parameters.p = true;
            parameters.spaceBetweenChar = atoi(optarg);
            break;
        case 't':
            parameters.t = true;
            parameters.outlineThickness = atof(optarg);
            break;
        case 'f':
            parameters.faceToSelect = atoi(optarg);
            break;
        case '?':
            if (optopt == 'i')
                fprintf(stderr, "Option -i requires an argument <input file>.\n");
            else if (optopt == 'o')
                fprintf(stderr, "Option -o requires an argument <output file>.\n");
            else if (optopt == 's')
                fprintf(stderr, "Option -s requires an argument <character size in pixels>.\n");
            else if (optopt == 'c')
                fprintf(stderr, "Option -c requires an argument <input charset utf8>.\n");
            else if (optopt == 'p')
                fprintf(stderr, "Option -p requires an argument <pixel between char>.\n");
            else if (optopt == 't')
                fprintf(stderr, "Option -t requires an argument <outline thickness>.\n");
            else if (optopt == 'f')
                fprintf(stderr, "Option -f requires an argument <face to select>.\n");
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            break;
        default:
            // error -- unknown parameter
            break;
        }
    }

    if (!parameters.i && !parameters.o && !parameters.s && !parameters.c && !parameters.p && !parameters.t)
    {
        fprintf(stdout,
                "\nUsage: font2bitmap -i ... -f ... -o ... -s ... -c ... -p ... -t ...\n\n"
                "-i : input file(.ttf|.otf|freetype compatible font)\n"
                "-f : face in font to be selected (default: 0)\n"
                "-o : output file without extension\n"
                "-s : character size in pixels to be exported (minimum:14).\n"
                "-c : text file with all characters to be generated in utf8.\n"
                "-p : size in pixels of the space between the characters exported.\n"
                "-t : outline thickness real number.\n\n");
    }
    else if (!parameters.i)
    {
        fprintf(stderr, "Missing option -i <input file>.\n");
    }
    else if (!parameters.o)
    {
        fprintf(stderr, "Missing option -o <output file>.\n");
    }
    else if (!parameters.s)
    {
        fprintf(stderr, "Missing option -s <character size in pixels>.\n");
    }
    else if (!parameters.c)
    {
        fprintf(stderr, "Missing option -c <input charset utf8 file>.\n");
    }
    else if (!parameters.p)
    {
        fprintf(stderr, "Missing option -p <pixel between char>.\n");
    }
    else if (!parameters.t)
    {
        fprintf(stderr, "Missing option -t <outline thickness>.\n");
    }
    else
    {

        std::u32string utf32data = readUTF32fromFile(parameters.characterUtf8Charset.c_str());

        // configura FT2 and Atlas
        ITKWrappers::FT2::FT2 ft2;

        ft2.readFromFile_PX(parameters.inputFile,
                            parameters.faceToSelect,
                            parameters.outlineThickness,
                            parameters.characterSize,
                            parameters.characterSize);
        float newLineHeight = (float)parameters.characterSize;
        float spaceWidth = (float)parameters.characterSize;

        ITKWrappers::FT2::FT2Glyph *glyph_white_space = ft2.generateGlyph(U' ');
        if (glyph_white_space != NULL)
        {
            spaceWidth = glyph_white_space->advancex;
            ft2.releaseGlyph(&glyph_white_space);
        }

        ITKExtension::Atlas::Atlas atlas(parameters.spaceBetweenChar, parameters.spaceBetweenChar);

        ITKExtension::Font::FontWriter fontWriter;
        fontWriter.initFromAtlas(&atlas, parameters.characterSize, spaceWidth, newLineHeight);

        // generate all characters
        for (int i = 0; i < utf32data.length(); i++)
        {
            if (utf32data[i] == U' ')
                continue;

            auto *glyph = ft2.generateGlyph(utf32data[i]);
            if (glyph == NULL)
            {
                printf("Glyph not found: %u\n", utf32data[i]);
                continue;
            }

            auto *atlasElementFace = atlas.addElement(
                UInt32toStringHEX(utf32data[i]),
                glyph->normalRect.w,
                glyph->normalRect.h);

            atlasElementFace->copyFromRGBABuffer(glyph->bitmapRGBANormal, glyph->normalRect.w * 4);

            auto *atlasElementStroke = atlas.addElement(
                UInt32toStringHEX(utf32data[i]) + std::string("s"),
                glyph->strokeRect.w,
                glyph->strokeRect.h);

            atlasElementStroke->copyFromRGBABuffer(glyph->bitmapRGBAStroke, glyph->strokeRect.w * 4);

            fontWriter.setCharacter(
                utf32data[i],
                glyph->advancex,
                glyph->normalRect.top,
                glyph->normalRect.left,
                atlasElementFace,
                glyph->strokeRect.top,
                glyph->strokeRect.left,
                atlasElementStroke);

            ft2.releaseGlyph(&glyph);
        }

        atlas.organizePositions(false);

        atlas.savePNG((parameters.outputFile +
                       std::string("-") +
                       UInt32toString(parameters.characterSize) +
                       std::string("-atlas.png"))
                          .c_str());
        atlas.savePNG_Alpha((parameters.outputFile +
                             std::string("-") +
                             UInt32toString(parameters.characterSize) +
                             std::string("-atlas-gray.png"))
                                .c_str());

        fontWriter.saveGlyphTable(
            (parameters.outputFile +
             std::string("-") +
             UInt32toString(parameters.characterSize) +
             std::string(".asbgt2"))
                .c_str());

        fontWriter.save(
            (parameters.outputFile +
             std::string("-") +
             UInt32toString(parameters.characterSize) +
             std::string(".basof2"))
                .c_str());
    }

    return 0;
}
