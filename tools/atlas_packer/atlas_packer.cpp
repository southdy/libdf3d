#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <df3d/df3d.h>
#include <RectangleBinPack/MaxRectsBinPack.h>

Json::Value ReadInput(const char *filename)
{
    std::ifstream f(filename);
    std::string tmp((std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>());
    auto jsonRoot = df3d::JsonUtils::fromString(tmp);
    if (jsonRoot.isNull())
        throw std::runtime_error("Atlas Packer: Failed to parse input params");
    return jsonRoot;
}

int main(int argc, const char **argv) try
{
    if (argc != 3)
        throw std::runtime_error("Invalid input. Usage: atlas_packer.exe params.json output.json");

    auto jsonRoot = ReadInput(argv[1]);

    int width = jsonRoot["size"][0].asInt();
    int height = jsonRoot["size"][1].asInt();

    rbp::MaxRectsBinPack packer(width, height, false);

    std::vector<rbp::Rect> resultRects;

    for (auto it : jsonRoot["inputRects"])
    {
        int rectWidth = it[0].asInt();
        int rectHeight = it[1].asInt();

        auto rect = packer.Insert(rectWidth, rectHeight, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);
        if ((rect.height > 0) && (rect.width == rectWidth) && (rect.height == rectHeight))
            resultRects.push_back(rect);
        else
            throw std::runtime_error("Atlas Packer: failed to pack");
    }

    Json::Value result;
    result["rects"] = Json::Value(Json::arrayValue);
    for (auto r : resultRects)
    {
        Json::Value jsonRect;
        jsonRect["x"] = r.x;
        jsonRect["y"] = r.y;
        jsonRect["w"] = r.width;
        jsonRect["h"] = r.height;

        result["rects"].append(jsonRect);
    }

    std::ofstream outf(argv[2]);
    Json::StreamWriterBuilder b;
    b["indentation"] = "  ";
    outf << Json::writeString(b, result);

    return outf.bad() ? 1 : 0;
}
catch (std::exception &e)
{
    std::cerr << "An error occurred:\n" << e.what() << "\n";

    return 1;
}
