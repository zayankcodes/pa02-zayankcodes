#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include "utilities.h"
#include "movies.h"

bool parseLine(std::string &line, std::string &movieName, double &movieRating) {
    auto comma = line.find_last_of(',');
    movieName = line.substr(0, comma);
    movieRating = std::stod(line.substr(comma + 1));
    if (!movieName.empty() && movieName.front() == '"' && movieName.back() == '"')
        movieName = movieName.substr(1, movieName.size() - 2);
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " moviesFilename [prefixFilename]\n";
        return 1;
    }

    std::ifstream movieFile(argv[1]);
    if (!movieFile) {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return 1;
    }

    MovieDatabase db;
    std::string line, name;
    double rating;
    while (std::getline(movieFile, line) && parseLine(line, name, rating))
        db.addMovie(name, rating);
    movieFile.close();

    db.finalize();

    if (argc == 2) {
        db.printAll();
        return 0;
    }

    std::ifstream prefixFile(argv[2]);
    if (!prefixFile) {
        std::cerr << "Could not open file " << argv[2] << "\n";
        return 1;
    }

    std::vector<std::string> prefixes;
    while (std::getline(prefixFile, line))
        if (!line.empty())
            prefixes.push_back(line);
    prefixFile.close();

    for (auto &p : prefixes) {
        auto matches = db.getMoviesWithPrefix(p);
        if (matches.empty()) {
            std::cout << "No movies found with prefix " << p << "\n";
        } else {
            for (auto &m : matches) {
                std::cout << m.name << ", " << std::fixed << std::setprecision(1) << m.rating << "\n";
            }
            std::cout << "\n";
        }
    }

    for (auto &p : prefixes) {
        auto best = db.getBestMovieWithPrefix(p);
        if (best.rating >= 0.0) {
            std::cout << "Best movie with prefix " << p << " is: " << best.name
                      << " with rating " << std::fixed << std::setprecision(1) << best.rating << "\n";
        }
    }

    return 0;
}

/*
Part 3a: Time Complexity Analysis

n = #movies, m = #prefixes, kᵢ = matches at node i, l = max name length.

Preprocess (trie build + sorts):  O(n·l + Σ kᵢ log kᵢ)
Queries:    Σ over m of O(l + k)

→ Eliminates the per‐prefix O(log n·l + k·l + k log k·l) cost.

Part 3b: Space Complexity Analysis

Storing n movies + trie nodes + index lists:
  O(n·l + Σ kᵢ·l)  → worst‐case O(n·l·l) if every prefix is unique,
  but practically O(n·l).

Part 3c: Design Tradeoffs

• Target: minimize **query** time → paid a bit in build cost.  
• Achieved O(l + k) per lookup, at the cost of O(n·l + Σ kᵢ log kᵢ) build.  
• Harder: packing all sorted lists into the trie without exploding memory.
*/
