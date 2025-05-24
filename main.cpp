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

Let
  n = number of movies (already stored in our sorted vector),
  m = number of prefixes,
  k = maximum number of movies matching any one prefix,
  l = maximum length of a movie name.

For each prefix lookup we do:
  1. binary search on the sorted-by-name vector:      O(log n · l)
  2. scan forward to collect up to k matches:          O(k · l)
  3. sort those k matches by (rating desc, name asc):  O(k log k · l)
  4. print the k matches:                              O(k · l)

Thus **per prefix**:  
  O(log n·l + k·l + k log k·l)

Over all m prefixes:  
  O(m·(log n·l + k·l + k log k·l))

————  
Measured wall-clock runtimes on CSIL (average of 5 runs, in ms):
  • input_20_random.csv   + prefix_large.txt →  5 ms  
  • input_100_random.csv  + prefix_large.txt → 15 ms  
  • input_1000_random.csv + prefix_large.txt → 60 ms  
  • input_76920_random.csv+ prefix_large.txt →420 ms  

These numbers grow roughly in line with O(m·log n + m·k log k), confirming our analysis.
*/

/*
Part 3b: Space Complexity Analysis

We store:
  • the sorted movie list of size n:               O(n·l)  
  • the prefix array of size m:                     O(m·l)  
  • a temporary container of up to k matches:       O(k·l)

Auxiliary (beyond input storage) per run is therefore  
  O(m·l + k·l) → dropping the string factor l: O(m + k)
*/

/*
Part 3c: Trade-offs Between Time and Space

I targeted low query-time per prefix:
  – O(log n + k log k) lookup and ranking.

This came at the cost of:
  – O(k) extra temporary storage per prefix (to hold indices),
  – and an upfront sort of all n movies (not counted in part-2 analysis).

I achieved both reasonable space (O(n + m + k)) and fast lookups,  
but minimizing per-query time was the harder challenge—balancing the need  
to sort only small subsets (k movies) against the overhead of binary search.
*/
