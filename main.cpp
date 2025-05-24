#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Movie { std::string name; double rating; };

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

    std::vector<Movie> movies;
    std::string line, name;
    double rating;
    while (std::getline(movieFile, line) && parseLine(line, name, rating))
        movies.push_back({name, rating});
    movieFile.close();

    std::sort(movies.begin(), movies.end(),
              [](auto &a, auto &b){ return a.name < b.name; });

    if (argc == 2) {
        for (auto &m : movies)
            std::cout << m.name << ", "
                      << std::fixed << std::setprecision(1) << m.rating
                      << "\n";
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

    auto findRange = [&](const std::string &p) {
        auto lo = std::lower_bound(movies.begin(), movies.end(), p,
            [&](const Movie &mov, const std::string &pref){
                return mov.name.compare(0, pref.size(), pref) < 0;
            });
        auto hi = std::upper_bound(movies.begin(), movies.end(), p,
            [&](const std::string &pref, const Movie &mov){
                return mov.name.compare(0, pref.size(), pref) > 0;
            });
        return std::pair{lo, hi};
    };

    for (auto &p : prefixes) {
        auto [lo, hi] = findRange(p);
        size_t k = hi - lo;
        if (k == 0) {
            std::cout << "No movies found with prefix " << p << "\n";
        } else {
            std::vector<int> idx;
            idx.reserve(k);
            for (auto it = lo; it != hi; ++it)
                idx.push_back(int(it - movies.begin()));
            std::sort(idx.begin(), idx.end(),
                [&](int a, int b) {
                    if (movies[a].rating != movies[b].rating)
                        return movies[a].rating > movies[b].rating;
                    return movies[a].name < movies[b].name;
                });
            for (int i : idx)
                std::cout << movies[i].name << ", "
                          << std::fixed << std::setprecision(1)
                          << movies[i].rating << "\n";
            std::cout << "\n";
        }
    }

    for (auto &p : prefixes) {
        auto [lo, hi] = findRange(p);
        size_t k = hi - lo;
        if (k > 0) {
            std::vector<int> idx;
            idx.reserve(k);
            for (auto it = lo; it != hi; ++it)
                idx.push_back(int(it - movies.begin()));
            std::sort(idx.begin(), idx.end(),
                [&](int a, int b) {
                    if (movies[a].rating != movies[b].rating)
                        return movies[a].rating > movies[b].rating;
                    return movies[a].name < movies[b].name;
                });
            int best = idx[0];
            std::cout << "Best movie with prefix " << p
                      << " is: " << movies[best].name
                      << " with rating "
                      << std::fixed << std::setprecision(1)
                      << movies[best].rating << "\n";
        }
    }

    return 0;
}

/*
Part 3a: Time Complexity Analysis

n = number of movies, m = number of prefixes, k = max matches/prefix, l = max name length.

• Sort movies by name: O(n · log n · l)
• Per prefix:
    – binary-search range: O(log n · l)
    – collect k indices:    O(k)
    – sort indices:         O(k · log k)
    – print k results:      O(k · l)

Total: O(n log n·l + m·(log n·l + k log k + k·l))

Part 3b: Space Complexity Analysis

O(n·l) for movie list + O(m·l) for prefixes + O(k) auxiliary per prefix → O(n·l + m·l + k·l).

Part 3c: Design Tradeoffs

I optimized for low runtime by:
 – One global sort by name
 – Fast binary-search per prefix
 – Only integer-index sorting (no Movie copies)
This minimizes both CPU and memory churn, with per-query cost O(log n + k log k + k).
*/
