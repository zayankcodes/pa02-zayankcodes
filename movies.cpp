// movies.cpp
#include "movies.h"
#include <algorithm>
#include <iostream>
#include <functional>

MovieDatabase::MovieDatabase() : root(new Node) {}
MovieDatabase::~MovieDatabase() { deleteNode(root); }

void MovieDatabase::deleteNode(Node* n) {
    for (auto &kv : n->ch) deleteNode(kv.second);
    delete n;
}

void MovieDatabase::addMovie(const std::string &name, double rating) {
    int idx = movies.size();
    movies.push_back({name, rating});
    Node* cur = root;
    for (char c : name) {
        auto &child = cur->ch[c];
        if (!child) child = new Node;
        child->idx.push_back(idx);
        cur = child;
    }
  
}

void MovieDatabase::finalize() {

    std::function<void(Node*)> dfs = [&](Node* n) {
        auto cmp = [&](int a, int b){
            auto &A = movies[a], &B = movies[b];
            if (A.rating != B.rating) return A.rating > B.rating;
            return A.name < B.name;
        };
        std::sort(n->idx.begin(), n->idx.end(), cmp);
        for (auto &kv : n->ch) dfs(kv.second);
    };
    dfs(root);
}

void MovieDatabase::printAll() const {
  
    auto tmp = movies;
    std::sort(tmp.begin(), tmp.end(),
              [](auto &A, auto &B){ return A.name < B.name; });
    for (auto &m : tmp)
        std::cout << m.name << ", " << m.rating << "\n";
}

std::vector<Movie> MovieDatabase::getMoviesWithPrefix(const std::string &p) const {

    Node* cur = root;
    
    for (char c : p) {
        auto it = cur->ch.find(c);
        if (it == cur->ch.end()) return {};
        cur = it->second;
    }
    std::vector<Movie> out;
    out.reserve(cur->idx.size());
    for (int i : cur->idx) out.push_back(movies[i]);
    return out;
}

Movie MovieDatabase::getBestMovieWithPrefix(const std::string &p) const {

    auto v = getMoviesWithPrefix(p);
    if (v.empty()) return Movie{"", -1.0};
    return v.front(); 

}