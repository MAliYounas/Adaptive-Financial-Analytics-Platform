#ifndef GRAPH_H
#define GRAPH_H

#include "Vector.h"
#include "Queue.h"
#include "Stack.h"

using namespace std;

template<typename T>
class Graph {
private:
    bool is_directed_;
    int num_vertices_;
    Vector<Vector<bool>> adjacency_matrix_;
    Vector<Vector<int>> adjacency_list_;
    Vector<T> vertex_data_;

    void clear_graph() {
        adjacency_matrix_.clear();
        adjacency_list_.clear();
        vertex_data_.clear();
        num_vertices_ = 0;
    }

    void copy_graph(const Graph& other) {
        is_directed_ = other.is_directed_;
        num_vertices_ = other.num_vertices_;
        adjacency_matrix_ = other.adjacency_matrix_;
        adjacency_list_ = other.adjacency_list_;
        vertex_data_ = other.vertex_data_;
    }

public:
    explicit Graph(bool is_directed = false, int num_vertices = 0)
        : is_directed_(is_directed), num_vertices_(num_vertices) {
        if (num_vertices > 0) {
            for (int i = 0; i < num_vertices; ++i) {
                Vector<bool> row;
                for (int j = 0; j < num_vertices; ++j) {
                    row.push_back(false);
                }
                adjacency_matrix_.push_back(row);
                Vector<int> list;
                adjacency_list_.push_back(list);
                T default_value = T();
                vertex_data_.push_back(default_value);
            }
        }
    }

    Graph(const Graph& other) {
        copy_graph(other);
    }

    Graph(Graph&& other)
        : is_directed_(other.is_directed_),
          num_vertices_(other.num_vertices_),
          adjacency_matrix_(other.adjacency_matrix_),
          adjacency_list_(other.adjacency_list_),
          vertex_data_(other.vertex_data_) {
        other.adjacency_matrix_ = Vector<Vector<bool>>();
        other.adjacency_list_ = Vector<Vector<int>>();
        other.vertex_data_ = Vector<T>();
        other.num_vertices_ = 0;
    }

    Graph& operator=(const Graph& other) {
        if (this != &other) {
            clear_graph();
            copy_graph(other);
        }
        return *this;
    }

    Graph& operator=(Graph&& other) {
        if (this != &other) {
            clear_graph();
            is_directed_ = other.is_directed_;
            num_vertices_ = other.num_vertices_;
            adjacency_matrix_ = other.adjacency_matrix_;
            adjacency_list_ = other.adjacency_list_;
            vertex_data_ = other.vertex_data_;
            other.adjacency_matrix_ = Vector<Vector<bool>>();
            other.adjacency_list_ = Vector<Vector<int>>();
            other.vertex_data_ = Vector<T>();
            other.num_vertices_ = 0;
        }
        return *this;
    }

    ~Graph() {
        clear_graph();
    }

    void add_vertex(const T& data = T{}) {
        ++num_vertices_;
        vertex_data_.push_back(data);

        for (int i = 0; i < adjacency_matrix_.size(); ++i) {
            adjacency_matrix_[i].push_back(false);
        }

        Vector<bool> new_row;
        for (int i = 0; i < num_vertices_; ++i) {
            new_row.push_back(false);
        }
        adjacency_matrix_.push_back(new_row);

        Vector<int> new_list;
        adjacency_list_.push_back(new_list);
    }

    void add_edge(int from, int to) {
        if (from < 0 || from >= num_vertices_ || to < 0 || to >= num_vertices_) {
            throw "Vertex index out of range";
        }

        adjacency_matrix_[from][to] = true;
        adjacency_list_[from].push_back(to);

        if (!is_directed_) {
            adjacency_matrix_[to][from] = true;
            adjacency_list_[to].push_back(from);
        }
    }

    void remove_edge(int from, int to) {
        if (from < 0 || from >= num_vertices_ || to < 0 || to >= num_vertices_) {
            throw "Vertex index out of range";
        }

        adjacency_matrix_[from][to] = false;
        Vector<int>& list = adjacency_list_[from];
        for (int i = 0; i < list.size(); ++i) {
            if (list[i] == to) {
                list.erase(i);
                break;
            }
        }

        if (!is_directed_) {
            adjacency_matrix_[to][from] = false;
            Vector<int>& list2 = adjacency_list_[to];
            for (int i = 0; i < list2.size(); ++i) {
                if (list2[i] == from) {
                    list2.erase(i);
                    break;
                }
            }
        }
    }

    bool has_edge(int from, int to) {
        if (from < 0 || from >= num_vertices_ || to < 0 || to >= num_vertices_) {
            return false;
        }
        return adjacency_matrix_[from][to];
    }

    Vector<int> get_neighbors(int vertex) {
        if (vertex < 0 || vertex >= num_vertices_) {
            throw "Vertex index out of range";
        }
        return adjacency_list_[vertex];
    }

    void set_vertex_data(int vertex, const T& data) {
        if (vertex < 0 || vertex >= num_vertices_) {
            throw "Vertex index out of range";
        }
        vertex_data_[vertex] = data;
    }

    T& get_vertex_data(int vertex) {
        if (vertex < 0 || vertex >= num_vertices_) {
            throw "Vertex index out of range";
        }
        return vertex_data_[vertex];
    }

    int num_vertices() {
        return num_vertices_;
    }

    bool is_directed() {
        return is_directed_;
    }

    Vector<int> bfs(int start_vertex) {
        if (start_vertex < 0 || start_vertex >= num_vertices_) {
            throw "Vertex index out of range";
        }

        Vector<int> result;
        Vector<bool> visited;
        for (int i = 0; i < num_vertices_; ++i) {
            visited.push_back(false);
        }
        Queue<int> q;

        visited[start_vertex] = true;
        q.enqueue(start_vertex);

        while (!q.empty()) {
            int current = q.front();
            q.dequeue();
            result.push_back(current);

            Vector<int> neighbors = get_neighbors(current);
            for (int i = 0; i < neighbors.size(); ++i) {
                int neighbor = neighbors[i];
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.enqueue(neighbor);
                }
            }
        }

        return result;
    }

    Vector<int> dfs(int start_vertex) {
        if (start_vertex < 0 || start_vertex >= num_vertices_) {
            throw "Vertex index out of range";
        }

        Vector<int> result;
        Vector<bool> visited;
        for (int i = 0; i < num_vertices_; ++i) {
            visited.push_back(false);
        }
        Stack<int> s;

        s.push(start_vertex);

        while (!s.empty()) {
            int current = s.top();
            s.pop();

            if (!visited[current]) {
                visited[current] = true;
                result.push_back(current);

                Vector<int> neighbors = get_neighbors(current);
                for (int i = neighbors.size() - 1; i >= 0; --i) {
                    int neighbor = neighbors[i];
                    if (!visited[neighbor]) {
                        s.push(neighbor);
                    }
                }
            }
        }

        return result;
    }
};

#endif

