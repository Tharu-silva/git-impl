#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>
#include <vector>
#include <algorithm>

#define BUFF_SIZE 1024

std::vector<char> decompress_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> compressed_data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    
    // Prepare output buffer
    std::vector<char> decompressed_data(compressed_data.size() * 4); // Initialize with larger size
    uLongf decompressed_size = decompressed_data.size();
    
    // Decompress
    int result = uncompress(
        (Bytef*)decompressed_data.data(),
        &decompressed_size,
        (Bytef*)compressed_data.data(),
        compressed_data.size()
    );
    
    if (result != Z_OK) {
        throw std::runtime_error("Decompression failed");
    }
    
    // Resize to actual decompressed size
    decompressed_data.resize(decompressed_size);
    return decompressed_data;
}


int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }
    
   std::string command = argv[1];
    if (command == "init") 
    {
        try {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");
            std::ofstream headFile (".git/HEAD");
            if (headFile.is_open()) 
            {
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            } else 
            {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }
            std::cout << "Initialized git directory\n";
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    } else if (command == "cat-file")
    {
        if (argc <= 3)
        {
            std::cerr << "Invalid arguments, arguments required cat-file -p <blob_sha1>\n";
            return EXIT_FAILURE;
        } 

        const std::string flag = argv[2];
        if (flag != "-p")
        {
            std::cerr << "Invalid flag, expected `-p`";
            return EXIT_FAILURE;
        }

        //Find the file location 
        std::string obj_dir {"./.git/objects"};
        std::string file_hash {argv[3]};
        std::string file_dir {file_hash.substr(0, 2)}; 
        std::string file_name {file_hash.substr(2)};
        std::string blob_path {obj_dir + "/" + file_dir + "/" + file_name};

        //Decompress
        std::cout << "Reading from " << blob_path << '\n';
        std::vector<char> decompressed = decompress_file(blob_path);
        auto it {std::find(decompressed.begin(), decompressed.end(), '\0') + 1};
        auto content_start {std::distance(decompressed.begin(), it)};
        std::cout.write(decompressed.data() + content_start, decompressed.size());
    } 
    else 
    {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
