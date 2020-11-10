#include <cmath>
#include <iostream>
#include <string>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace boost;

dll::shared_library sharedLib;
vector<cv::Rect>(*findFaces)(const string&&);
cv::FileStorage outJSON;
const cv::Mat scaleMat({2, 3}, {0.5f, 0.f, 0.f, 0.f, 0.5f, 0.f}); // Halves image dimensions

void initialize() noexcept
{
    dll::fs::error_code err;
    sharedLib.load("FindFaces", dll::load_mode::append_decorations, err);
    if(err)
    {
        cout << "Failed to load the library.\n";
        exit(1);
    }
    if(sharedLib.has("findFaces") == false)
    {
        cout << "The library doesn't have required function.\n";
        exit(1);
    }
    findFaces = &sharedLib.get<vector<cv::Rect>(const string&&)>("findFaces");
    try
    {
        (*findFaces)("");
    }
    catch(...)
    {
        cout << "Something is wrong with the library function.\n";
        exit(1);
    }
}

// Recursively check all files in the directory and its subdirs
void walkDirectory(const filesystem::path &root, const filesystem::path &p)
{
    if(p.string() == "out")
        return;
    filesystem::create_directory(root / "out" / p);
    for (auto &x : filesystem::directory_iterator(root / p))
    {
        auto filepath = x.path();
        const filesystem::path relativePath = p / filepath.filename();
        if(filesystem::is_directory(filepath))
        {
            walkDirectory(root, relativePath);
            continue;
        }
        cv::Mat img = cv::imread(filepath.string(), cv::IMREAD_COLOR);
        if(img.empty())
            continue;
        auto faces = (*findFaces)(filepath.string());
        outJSON.startWriteStruct("", cv::FileNode::MAP);
        outJSON << "image" << relativePath.string();
        outJSON << "output" << ("out" / relativePath).string() + ".jpg";
        outJSON.startWriteStruct("faces", cv::FileNode::SEQ);
        for (auto &faceRect : faces) // Blur all faces
        {
            outJSON << faceRect;
            cv::Mat face(img, faceRect);
            const int w = max(5, faceRect.width/5);
            const int h = max(5, faceRect.height/5);
            cv::Mat kernel = cv::Mat::ones(h, w, CV_32F) / (float)(w*h);
            cv::filter2D(face, face, -1, kernel);
            face.copyTo(img(faceRect));
        }
        outJSON.endWriteStruct();
        outJSON.endWriteStruct();
        cv::Mat imgScaled(img.size()/2, img.type());
        cv::warpAffine(img, imgScaled, scaleMat, imgScaled.size());
        cv::imwrite((root / "out" / relativePath).string() + ".jpg", imgScaled);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " {directory path}" << endl;
        return 1;
    }

    initialize();

    filesystem::path root(argv[1]);
    if(root.is_relative())
        root = filesystem::current_path() / root;
    outJSON.open((root / "result.json").string(), outJSON.WRITE | outJSON.FORMAT_JSON);
    if(outJSON.isOpened() == false)
    {
        cout << "Cannot create result.json\n";
        return 1;
    }
    try
    {
        if (!filesystem::is_directory(root))
        {
            cout << "Specified path is not an existing directory.\n";
            return 1;
        }
        root.make_preferred();
        outJSON << "root" << root.string();
        outJSON.startWriteStruct("files", cv::FileNode::SEQ);
        walkDirectory(root, "");
    }
    catch (const filesystem::filesystem_error& ex)
    {
      cout << ex.what() << endl;
      return 1;
    }
    return 0;
}
