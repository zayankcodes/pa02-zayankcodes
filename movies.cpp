#include "movies.h"
#include <algorithm>
#include <iostream>

void MovieDatabase::addMovie(const std::string& name, double rating) {
    movies.push_back({name, rating});
}

void MovieDatabase::sortByName() {
    std::sort(movies.begin(), movies.end(),
              [](auto& a, auto& b){ return a.name < b.name; });
}

void MovieDatabase::printAll() const {
    for (auto& m : movies)
        std::cout << m.name << ", " << m.rating << "\n";
}

std::vector<Movie> MovieDatabase::getMoviesWithPrefix(const std::string& prefix) const {
    std::vector<Movie> result;
    auto it = std::lower_bound(
        movies.begin(), movies.end(), prefix,
        [&](auto& m, const std::string& p){
            return m.name.compare(0, p.size(), p) < 0;
        });
    for (; it != movies.end(); ++it) {
        if (it->name.rfind(prefix, 0) == 0)
            result.push_back(*it);
        else
            break;
    }
    if (!result.empty()) {
        std::sort(result.begin(), result.end(),
                  [](auto& a, auto& b){
                      if (a.rating != b.rating) return a.rating > b.rating;
                      return a.name < b.name;
                  });
    }
    return result;
}

Movie MovieDatabase::getBestMovieWithPrefix(const std::string& prefix) const {
    Movie best{ "", -1.0 };
    auto cands = getMoviesWithPrefix(prefix);
    if (!cands.empty()) best = cands.front();
    return best;
}
