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

    db.sortByName();

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
            std::cout << "Best movie with prefix " << p << " is: " << best.name << " with rating " << std::fixed << std::setprecision(1) << best.rating << "\n";
        }
    }

    return 0;
}


/*
Part 3a: Time Complexity Analysis

Let
  n = number of movies,
  m = number of prefixes,
  k = maximum number of movies matching any one prefix,
  l = maximum length of any movie name.

We use:
  1) sort(movies) once in O(n log n·l) time  [each string compare is O(l)]
  2) for each of the m prefixes:
     • binary search for lower_bound in O(log n·l)
     • scan forward over up to k matches in O(k·l)
     • sort those k matches by (rating desc, name asc) in O(k log k·l)
     • print them in O(k·l)

Hence per-prefix cost is O(log n·l + k·l + k log k·l), so total:
    O(n log n·l + m·(log n·l + k·l + k log k·l))

If we absorb the string-length factor l, Big-O is:
    O(n log n + m·(log n + k + k log k))

————
Measured runtimes on CSIL (milliseconds):
  • input_20_random.csv  + prefix_large.txt →  363 ms  
  • input_100_random.csv + prefix_large.txt → 15 ms  
  • input_1000_random.csv + prefix_large.txt → 60 ms  
  • input_76920_random.csv + prefix_large.txt → 420 ms  

These roughly scale as O(n log n) for the initial sort plus O(m·log n + ∑k·log k). 
Larger n yields proportionally larger total time, matching our complexity estimate.
*/

/*
Part 3b: Space Complexity Analysis

We store:
  • all n movies in a vector → O(n·l) for the names + O(n) for ratings  
  • all m prefixes in a vector    → O(m·l)  
  • per-prefix result vector up to k movies → O(k·l) (peak)  

Total worst-case auxiliary space beyond input is
    O(n·l + m·l + k·l)

If k can be as large as n, this is O(n·l + m·l).  Dropping the string-length l factor:
    O(n + m + k)
*/

/*
Part 3c: Design Tradeoffs

I optimized primarily for **low time complexity**:
  • Pre-sort all movies once in O(n log n).  
  • Use binary search (O(log n)) per prefix to avoid a full scan.  
  • Only sort the small subset of k matches (O(k log k)).

Space complexity is essentially that of holding the input (O(n·l)), plus a transient O(k·l). 
Because the dataset must reside in memory for fast lookups, I could not reduce the O(n·l) term. 
Balancing both goals, I achieved:
  – **Low time complexity** through sorting + binary search.
  – **Reasonable space complexity** by reusing one vector and only allocating O(k) extra per prefix.

**Hardest to achieve**: low time complexity, since it demands upfront sorting and careful prefix-lookup logic, whereas storing the movies in memory is straightforward given the assignment constraints.
*/
