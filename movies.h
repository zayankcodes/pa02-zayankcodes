// movies.h
#ifndef MOVIES_H
#define MOVIES_H
#include <string>
#include <vector>
#include <unordered_map>

struct Movie { std::string name; double rating; };

class MovieDatabase {
public:
    MovieDatabase();
    ~MovieDatabase();
    void addMovie(const std::string&, double);
    void finalize();                        // build & sort the trie
    void printAll() const;                  // as before
    std::vector<Movie> getMoviesWithPrefix(const std::string&) const;
    Movie               getBestMovieWithPrefix(const std::string&) const;
private:
    struct Node {
        std::vector<int>        idx;        // indices into movies[]
        std::unordered_map<char,Node*> ch;
    };
    Node*                root;
    std::vector<Movie>   movies;
    void deleteNode(Node*);
};
#endif
