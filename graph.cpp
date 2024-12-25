#include <queue>
#include <random>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <omp.h>
#include "graph.h"

using std::string;
using std::ifstream;
using std::stringstream;
using std::vector;
using std::unordered_map;

VertexRefWMut Graph::AddVertex(vid_t vid) {
    auto vertex = Vertex(vid);
    return vertices_.emplace(vid, vertex).first->second;
}

std::optional<VertexRefWMut> Graph::GetVertexMut(vid_t vid) {
    if (auto it = vertices_.find(vid); it != vertices_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<VertexRefW> Graph::GetVertex(vid_t vid) const {
    return const_cast<Graph*>(this)->GetVertexMut(vid);
}

bool Graph::AddEdge(Vertex& v1, const Vertex& v2) {
    return v1.adjacent.insert(v2.vid).second;
}

/* Create a graph using data saved in file with the address *address*. */
Graph Graph::Create(string address) {
    auto graph = Graph();
    ifstream file(address);
    stringstream ss;
    string data;
    while (file) {
        getline(file, data);
        int del_pos = data.find(' ');
        auto vid = std::stoi(data.substr(0, del_pos));
        auto uid = std::stoi(data.substr(del_pos + 1, data.size() - del_pos - 1));
        graph.AddVertex(vid);
        graph.AddVertex(uid);
        graph.AddEdge(graph.GetVertexMut(vid).value(), graph.GetVertexMut(uid).value());
    }
    file.close();
    return graph;
}

const Vertex& Graph::RandomSrc() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vertices_.size() - 1);
    auto itr = vertices_.begin();
    std::advance(itr, dis(gen));
    return itr->second;
}

//void SerialBFS(const Graph& graph, const Vertex& start) {
//    std::queue<VertexRefW> queue;
//    std::unordered_set<vid_t> visited;
//    queue.emplace(start);
//    while (!queue.empty()) {
//        auto vertex = queue.front();
//        queue.pop();
//        if (visited.contains(vertex.get().vid)) continue;
//        // visit(vertex);
//        // std::cout << vertex.get().vid << " ";
//        visited.insert(vertex.get().vid);
//        for (auto vid: vertex.get().adjacent) {
//            auto v = graph.GetVertex(vid);
//            if (v.has_value()) {
//                queue.push(v.value());
//            }
//        }
//    }
//}

void Graph::SerialBFS(const Vertex& start) {
    unordered_map<vid_t, int> levels = {};
    for (const auto& v : vertices_)
        levels.insert({v.first, -1});
    levels[start.vid] = 0;
    int level = 1;
    vector<vid_t> frontier = {};
    vector<vid_t> next_frontier = {};
    frontier.push_back(start.vid);
    // std::cout << start.vid << " ";
    while (!frontier.empty()) {
        for (auto itr = frontier.begin(); itr != frontier.end(); itr++) {
            auto uid = *itr;
            auto u = this->GetVertex(uid);
            for (auto vid : u->get().adjacent) {
                if (levels[vid] == -1) {
                    next_frontier.push_back(vid);
                    levels[vid] = level;
                    // std::cout << vid << " ";
                }
            }
        }
        frontier = next_frontier;
        next_frontier.clear();
        level++;
    }
}

void Graph::ParallelBFS(const Vertex& start, int num_threads) {
    unordered_map<vid_t, int> levels = {};
    for (const auto& v : vertices_)
        levels.insert({v.first, -1});
    levels[start.vid] = 0;
    int level = 1;
    vector<vid_t> frontier = {};
    vector<vid_t> next_frontier = {};
    frontier.push_back(start.vid);
    // std::cout << start.vid << " ";
    while (!frontier.empty()) {
        #pragma omp parallel num_threads(num_threads)
        {
            std::vector<vid_t> local_frontier = {};
            #pragma omp for nowait
            for (auto itr = frontier.begin(); itr != frontier.end(); itr++) {
                auto uid = *itr;
                auto u = this->GetVertex(uid);
                for (auto vid : u->get().adjacent) {
                    if (levels[vid] == -1) {
                        int insert;
                        #pragma omp atomic capture
                        {
                            insert = levels[vid];
                            levels[vid] = level;
                        }
                        if (insert == -1) {
                            local_frontier.push_back(vid);
                            // std::cout << vid << " ";
                        }
                    }
                }
            }
            #pragma omp critical
            {
                for (auto vid : local_frontier) {
                    next_frontier.push_back(vid);
                }
            }
        }
        frontier = next_frontier;
        next_frontier.clear();
        level++;
    }
}
