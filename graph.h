#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>

using vid_t = uint64_t;

struct Vertex {
    const vid_t vid;
    std::unordered_set<vid_t> adjacent;
};

using VertexRefW = std::reference_wrapper<const Vertex>;
using VertexRefWMut = std::reference_wrapper<Vertex>;

class Graph {
public:
    VertexRefWMut AddVertex(vid_t vid);

    std::optional<VertexRefWMut> GetVertexMut(vid_t vid);

    std::optional<VertexRefW> GetVertex(vid_t vid) const;

    bool AddEdge(Vertex& v1, const Vertex& v2);

    static Graph Create(std::string address);

    const Vertex& RandomSrc();

    void SerialBFS(const Vertex&);

    void ParallelBFS(const Vertex&, int num_threads);

private:
    std::unordered_map<vid_t, Vertex> vertices_;
};

// void SerialBFS(const Graph& graph, const Vertex& start);
