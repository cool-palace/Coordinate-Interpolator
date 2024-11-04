#ifndef COORDINATE_INTERPOLATOR_INTERPOLATOR_H
#define COORDINATE_INTERPOLATOR_INTERPOLATOR_H
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <regex>

struct TrackData {
    std::string image_name;
    double latitude;
    double longitude;
    double altitude;
    std::string timestamp;
};

class Interpolator {
public:
    static void process();
private:
    static bool is_valid_folder_name(const std::string& folder);
    static bool is_valid_track_folder_name(const std::string& folder, const std::string& date);
    static bool is_valid_text_file_name(const std::string& filename, const std::string& prefix);
    static void process_folder(const std::filesystem::path& path, const std::string& prefix);
    static std::vector<TrackData> parse_file(const std::string& filepath);
    static void interpolate(std::vector<TrackData>& data);
    static void write_file(const std::string& filepath, const std::vector<TrackData>& data);
};


#endif //COORDINATE_INTERPOLATOR_INTERPOLATOR_H
