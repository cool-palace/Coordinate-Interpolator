#include "Interpolator.h"

using dirit = std::filesystem::directory_iterator;

bool Interpolator::is_valid_folder_name(const std::string &folder) {
    std::regex pattern(R"(^\d{8}$)");
    return std::regex_match(folder, pattern);
}

bool Interpolator::is_valid_track_folder_name(const std::string &folder, const std::string &date) {
    if (folder.substr(0, 8) != date) return false;
    std::regex pattern(R"(^\d{8}_Track\d{2}$)");
    return std::regex_match(folder, pattern);
}

bool Interpolator::is_valid_text_file_name(const std::string &filename, const std::string &prefix) {
    if (filename.substr(0, 16) != prefix) return false;
    std::regex pattern(R"(^\d{8}_Track\d{2}_C1_EO.txt$)");
    return std::regex_match(filename, pattern);
}

std::vector<TrackData> Interpolator::parse_file(const std::string &filepath) {
    std::vector<TrackData> data;
    std::ifstream file(filepath);
    std::string line;
    char delimiter = ';';

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        TrackData entry;
        std::getline(ss, entry.image_name, delimiter);
        ss >> entry.latitude >> delimiter >> entry.longitude >> delimiter >> entry.altitude >> delimiter;
        std::getline(ss, entry.timestamp, delimiter);

        data.push_back(entry);
    }
    return data;
}

void Interpolator::interpolate(std::vector<TrackData> &data) {
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i].latitude == 0.0 && data[i].longitude == 0.0) {
            std::cout << "Null values detected in: " << data[i].image_name << '\n';
            size_t start = i - 1;
            size_t end = i + 1;
            while (end < data.size() && data[end].latitude == 0.0 && data[end].longitude == 0.0) {
                ++end;
            }
            double lat_increment = (data[end].latitude - data[start].latitude) / (end - start);
            double lon_increment = (data[end].longitude - data[start].longitude) / (end - start);
            double alt_increment = (data[end].altitude - data[start].altitude) / (end - start);

            for (size_t j = start + 1; j < end; ++j) {
                data[j].latitude = data[start].latitude + lat_increment * (j - start);
                data[j].longitude = data[start].longitude + lon_increment * (j - start);
                data[j].altitude = data[start].altitude + alt_increment * (j - start);
            }
        }
    }
}

void Interpolator::process_folder(const std::filesystem::path &path, const std::string &prefix) {
    for (const auto& entry : dirit(path)) {
        std::string dir_name = entry.path().filename().string();
        if (entry.is_directory() && is_valid_track_folder_name(dir_name, prefix)) {
            for (const auto& track_dir : dirit(entry.path())) {
                std::string filename = track_dir.path().filename().string();
                std::string filepath = track_dir.path().string();
                if (is_valid_text_file_name(filename, dir_name)) {
                    std::cout << "Processing file: " << filename << "\n";
                    std::vector<TrackData> track_data = parse_file(filepath);
                    std::cout << "Track data size: " << track_data.size() << "\n";
                    interpolate(track_data);
                    write_file(filepath, track_data);
                }
            }
        }
    }
}

void Interpolator::write_file(const std::string &filepath, const std::vector<TrackData> &data) {
    std::ofstream outfile(filepath);

    for (const auto& entry : data) {
        outfile << entry.image_name << ";"
                << std::fixed << std::setprecision(9) << entry.latitude << ";"
                << entry.longitude << ";"
                << entry.altitude << ";"
                << entry.timestamp << ";\n";
    }
}

void Interpolator::process() {
    for (const auto& entry : dirit(std::filesystem::current_path())) {
        if (entry.is_directory()) {
            std::string folder = entry.path().filename().string();
            if (is_valid_folder_name(folder)) {
                std::cout << "Processing folder: " << folder << "\n";
                process_folder(entry.path(), folder);
            }
        }
    }
}
