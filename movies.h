#ifndef MOVIES_H
#define MOVIES_H

#include <string>
#include <vector>

struct Movie {
    std::string name;
    double rating;
};

class MovieDatabase {
public:
    void addMovie(const std::string& name, double rating);
    void sortByName();
    void printAll() const;
    std::vector<Movie> getMoviesWithPrefix(const std::string& prefix) const;
    Movie getBestMovieWithPrefix(const std::string& prefix) const;
private:
    std::vector<Movie> movies;
};

#endif
