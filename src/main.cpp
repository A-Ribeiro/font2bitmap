// #include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit/ITKCommon/ITKCommon.h>

#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include <ITKWrappers/FT2.h>

#include "ImageRescaler.h"

#ifdef _WIN32
#include "win32/getopt.h"
#pragma warning(disable : 4996)
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
        int readed_size = (int)fread(&buffer[0], sizeof(uint8_t), buffer.size(), in);
        fclose(in);
    }
    return ITKCommon::StringUtil::utf8_to_utf32(buffer);
}

std::string charToUTF8_Cpp_Literal(char32_t char_code)
{
    std::string result = "\"";
    auto utf8_str = ITKCommon::StringUtil::utf32_to_utf8(std::u32string(U"") + char_code);
    for (auto chr : utf8_str)
        result += ITKCommon::PrintfToStdString("\\x%.2x", (uint8_t)chr);
    result += "\"";
    return result;
}

struct ParametersSet
{
    bool i, o, s, c, p, t, a;
    std::string inputFile;
    std::string outputFile;
    int characterSize;
    std::string characterUtf8Charset;
    int spaceBetweenChar;
    float outlineThickness;
    int faceToSelect;
    std::string addIniImageChar;
};

int main(int argc, char *argv[])
{
    fprintf(stdout, "font2bitmap - alessandroribeiro.thegeneralsolution.com\n");

    ParametersSet parameters = {0, 0, 0, 0, 0, 0, 0, "", "", 0, "", 0, 0.0f, 0, ""};
    int c;
    opterr = 0;

    char validOp[] = {"i:o:s:c:p:t:f:a:"};
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
            parameters.outlineThickness = (float)atof(optarg);
            break;
        case 'f':
            parameters.faceToSelect = atoi(optarg);
            break;
        case 'a':
            parameters.a = true;
            parameters.addIniImageChar = optarg;
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
                "\nUsage: font2bitmap -i ... -f ... -o ... -s ... -c ... -p ... -t ... -a ...\n\n"
                "-i : input file(.ttf|.otf|freetype compatible font)\n"
                "-f : face in font to be selected (default: 0)\n"
                "-o : output file without extension\n"
                "-s : character size in pixels to be exported (minimum:14).\n"
                "-c : text file with all characters to be generated in utf8.\n"
                "-p : size in pixels of the space between the characters exported.\n"
                "-t : outline thickness real number.\n"
                "-a : add ini file describing image to char generation.\n\n");
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
        fontWriter.initFromAtlas(&atlas, (float)parameters.characterSize, spaceWidth, newLineHeight);

        // generate all characters
        int64_t max_1char = -1;
        int64_t max_2chars = -1;
        int64_t max_3chars = -1;
        int64_t max_4chars = -1;
        for (int i = 0; i < utf32data.length(); i++)
        {
            if (utf32data[i] < 0x80)
            {
                if ((int64_t)utf32data[i] > max_1char)
                    max_1char = (int64_t)utf32data[i];
            }
            else if (utf32data[i] < 0x800)
            {
                if ((int64_t)utf32data[i] > max_2chars)
                    max_2chars = (int64_t)utf32data[i];
            }
            else if (utf32data[i] < 0x10000)
            {
                if ((int64_t)utf32data[i] > max_3chars)
                    max_3chars = (int64_t)utf32data[i];
            }
            else if (utf32data[i] < 0x110000)
            {
                if ((int64_t)utf32data[i] > max_4chars)
                    max_4chars = (int64_t)utf32data[i];
            }

            if (utf32data[i] == U' ')
                continue;

            auto *glyph = ft2.generateGlyph(utf32data[i]);
            if (glyph == NULL)
            {
                printf("Glyph not found: %u [0x%.8x]\n", (uint32_t)utf32data[i], (uint32_t)utf32data[i]);
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

        if (max_1char < 0)
            max_1char = 0;
        else
            max_1char = std::min<int64_t>(max_1char+1, 127);
        if (max_2chars < 0)
            max_2chars = 0x80;
        else
            max_2chars = std::min<int64_t>(max_2chars+1, 2047);
        if (max_3chars < 0)
            max_3chars = 0x800;
        else
            max_3chars = std::min<int64_t>(max_3chars+1, 65535);
        if (max_4chars < 0)
            max_4chars = 0x10000;
        else
            max_4chars = std::min<int64_t>(max_4chars+1, 1114111);

        // add custom character
        {
            printf("    1char (<128) starts from: %u [const char* var = %s;]\n", (uint32_t)max_1char, charToUTF8_Cpp_Literal((char32_t)max_1char).c_str());
            printf("    2chars (<2048) starts from: %u [const char* var = %s;]\n", (uint32_t)max_2chars, charToUTF8_Cpp_Literal((char32_t)max_2chars).c_str());
            printf("    3chars (<65536) starts from: %u [const char* var = %s;]\n", (uint32_t)max_3chars, charToUTF8_Cpp_Literal((char32_t)max_3chars).c_str());
            printf("    4chars (<1114112) starts from: %u [const char* var = %s;]\n", (uint32_t)max_4chars, charToUTF8_Cpp_Literal((char32_t)max_4chars).c_str());

            if (parameters.a)
            {
                using namespace ITKCommon;
                using namespace ITKCommon::FileSystem;
                std::unique_ptr<FILE, void (*)(FILE *)> file(File::fopen(parameters.addIniImageChar.c_str(), "rb"), [](FILE *f)
                                                             { if (f) File::fclose(f); });
                if (file != nullptr)
                {
                    char line[1024];
                    char input_image[1024] = "";              // = ./letter_a.png
                    double height_scale_related_to_font_size; // = 1.0
                    double x_start_percent;                   // = 0.0
                    double x_advance_percent;                 // = 1.0
                    double y_baseline_percent;                //= 0.5
                    uint32_t output_char_code;                //= 65536
                    char var_name[1024] = "";

                    ITKCommon::Matrix<MathCore::vec4u8> blank_img(MathCore::vec2i(parameters.characterSize, parameters.characterSize));
                    blank_img.clear(MathCore::vec4u8(0));

                    printf("Ini file processing from: %s\n", parameters.addIniImageChar.c_str());
                    std::string all_chars_inserted = "";
                    while (fgets(line, 1024, file.get()) != nullptr)
                    {
                        for (auto &_char : line)
                            if (_char == '\n' || _char == '\r')
                                _char = '\0';
                        if (strlen(line) == 0 || line[0] == ';')
                            continue;

                        if (sscanf(line, "input_image=%s", &input_image) == 1 ||
                            sscanf(line, "height_scale_related_to_font_size=%lf", &height_scale_related_to_font_size) == 1 ||
                            sscanf(line, "x_start_percent=%lf", &x_start_percent) == 1 ||
                            sscanf(line, "x_advance_percent=%lf", &x_advance_percent) == 1 ||
                            sscanf(line, "y_baseline_percent=%lf", &y_baseline_percent) == 1 ||
                            sscanf(line, "var_name=%s", &var_name) == 1)
                            continue;
                        else if (sscanf(line, "output_char_code=%u", &output_char_code) == 1)
                        {

                            printf("    Reading char: %u (0x%.8x)\n", output_char_code, output_char_code);
                            printf("        const char* var = %s;\n", charToUTF8_Cpp_Literal((char32_t)output_char_code).c_str());

                            all_chars_inserted += ITKCommon::PrintfToStdString("const char* %s = %s;\n",
                                                                               var_name,
                                                                               charToUTF8_Cpp_Literal((char32_t)output_char_code).c_str());

                            printf("        input_image: %s\n", input_image);
                            printf("        height_scale_related_to_font_size: %f\n", height_scale_related_to_font_size);
                            printf("        x_start_percent: %f\n", x_start_percent);
                            printf("        x_advance_percent: %f\n", x_advance_percent);
                            printf("        y_baseline_percent: %f\n", y_baseline_percent);

                            vec2i font_size = vec2i((int)((float)parameters.characterSize * (float)height_scale_related_to_font_size + 0.5f));

                            ImageRescaler rescaler(input_image);
                            rescaler.rescale(font_size.x, font_size.y);

                            auto *atlasElementFace = atlas.addElement(UInt32toStringHEX(output_char_code), font_size.x, font_size.y);
                            atlasElementFace->copyFromRGBABuffer((uint8_t *)rescaler.output_image.array, font_size.x * 4);

                            // auto* atlasElementStroke = atlas.addElement(UInt32toStringHEX(output_char_code) + std::string("s"), parameters.characterSize, parameters.characterSize);
                            auto *atlasElementStroke = atlas.addElement(UInt32toStringHEX(output_char_code) + std::string("s"), 0, 0);
                            // atlasElementStroke->copyFromRGBABuffer((uint8_t *)blank_img.array, font_size.x * 4);

                            float advance_x = (float)rescaler.output_image.size.x * (float)x_advance_percent;
                            int16_t top_origin = (int16_t)((float)rescaler.output_image.size.y * (1.0f - (float)y_baseline_percent) + 0.5f);
                            int16_t left_origin = (int16_t)((float)rescaler.output_image.size.x * (float)x_start_percent + 0.5f);

                            fontWriter.setCharacter(
                                output_char_code,
                                advance_x,   // glyph->advancex,
                                top_origin,  // glyph->normalRect.top,
                                left_origin, // glyph->normalRect.left,
                                atlasElementFace,
                                0, // glyph->strokeRect.top,
                                0, // glyph->strokeRect.left,
                                atlasElementStroke);

                            continue;
                        }

                        printf("    reading line with no valid content: %s\n", line);
                    }

                    printf("All Chars Inserted:\n%s\n", all_chars_inserted.c_str());
                }
            }
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
