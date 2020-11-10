#include <string>
#include <iostream>
#include <cstdio>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

using namespace boost;
using namespace std;

#define API extern "C" BOOST_SYMBOL_EXPORT

// Classifier file is saved into this array so library is a single file.
extern "C" const char faceCascadeData[];
extern "C" const size_t faceCascadeData_len;

cv::CascadeClassifier faceCascade;

API vector<cv::Rect> findFaces(const string&& param) noexcept;

// Initialize cascade classifier and ensure its validity
bool checkCascade() noexcept
{
    if(!faceCascade.empty())
        return true;
    try
    {   // Old classifiers can be read only from filesystem, so writing it to file.
        const char *filename = "temporary_cascade_file.xml";
        FILE *f = fopen(filename, "w");
        fwrite(faceCascadeData, 1, faceCascadeData_len, f);
        fclose(f);
        faceCascade.load(filename);
        remove(filename);
    }
    catch(...)
    {
        return false;
    }
    return !faceCascade.empty();
}

// Return positions of faces if any
vector<cv::Rect> findFaces(const string&& filename) noexcept
{
    vector<cv::Rect> faces;
    if(!checkCascade())
    {
        cout << "Critical failure: could not load cascade classifier.\n";
        return faces;
    }
    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    if(img.empty())
        return faces;
    cv::equalizeHist(img, img);
    faceCascade.detectMultiScale(img, faces);
    cout << filename << " -> " << faces.size() << " face(s) detected." << endl;
    return faces;
}
