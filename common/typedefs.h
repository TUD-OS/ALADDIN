#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/topological_sort.hpp>

typedef uint64_t Addr;

// Typedefs for Boost::Graph.

typedef boost::property<boost::vertex_index_t, unsigned> VertexProperty;
typedef boost::property<boost::edge_name_t, uint8_t> EdgeProperty;
typedef boost::adjacency_list<boost::listS,
                              boost::vecS,
                              boost::bidirectionalS,
                              VertexProperty,
                              EdgeProperty> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iter;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iter;
typedef boost::property_map<Graph, boost::edge_name_t>::type EdgeNameMap;
typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexNameMap;

// Other convenience typedefs.
typedef std::unordered_map<std::string, unsigned> unrolling_config_t;

#endif
