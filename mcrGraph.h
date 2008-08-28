// -*-C++-*-

#ifndef MCRGRAPH_H
#define MCRGRAPH_H

#include <string>
#include <iterator>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <ctime>            // std::time
#include <iostream>

// integer types

#include <boost/cstdint.hpp>

// graph

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map.hpp>
#include <boost/graph/properties.hpp>

using boost::adjacency_list;
using boost::graph_traits;
using boost::property;
using boost::property_map;
using boost::vertex_name_t;
using boost::vertex_name;
using boost::edge_weight_t;
using boost::edge_weight;

// Properties for graphs

enum vertex_flags_t { vertex_flags};  // flags for vertex (for knowing
                                      // wether a node is word or
                                      // synset)
enum vertex_gloss_t { vertex_gloss};  // gloss of node
enum edge_id_t      { edge_id };      // relation id
enum edge_rtype_t   { edge_rtype };  // relation type

namespace boost {
  BOOST_INSTALL_PROPERTY(vertex, flags);
  BOOST_INSTALL_PROPERTY(vertex, gloss);
  BOOST_INSTALL_PROPERTY(edge, id);
  BOOST_INSTALL_PROPERTY(edge, rtype);
}

typedef adjacency_list <
  boost::listS,
  boost::vecS,
  boost::bidirectionalS,
  property<vertex_name_t, std::string,
		   property<vertex_gloss_t, std::string,
					property<vertex_flags_t, unsigned char> > >,
  property<edge_weight_t, float,
		   property<edge_rtype_t, boost::uint32_t> >
  > McrGraph;


typedef graph_traits<McrGraph>::vertex_descriptor Mcr_vertex_t;
typedef graph_traits<McrGraph>::edge_descriptor Mcr_edge_t;
typedef graph_traits < McrGraph >::vertices_size_type Mcr_vertex_size_t;

class Mcr {

  enum {
    is_word = 1
  } vflags;

public:

  typedef McrGraph boost_graph_t; // the underlying graph type

  //Singleton
  static Mcr & instance();


  // 2 functions for creating Mcr graphs
  //
  // 1. create_from_txt
  //    Create graph by reading a textfile with synset relations (synsFile)
  //    Exclude relations not in rels_source set
  //
  // 2. create_from_binfile
  //    Load a binary snapshot of the graph into memory

  static void create_from_txt(const std::string & synsFile,
							  const std::set<std::string> & rels_source);

  static void create_from_binfile(const std::string & o);


  // write_to_binfile
  // Write mcr graph to a binary serialization file

  void write_to_binfile (const std::string & str) const;

  // add_from_txt
  // add relations from synsFile to the graph

  void add_from_txt(const std::string & synsFile); // Add a textfile with relations btw. synsets

  // add_relSource
  // add a new relation source

  void add_relSource(const std::string & str) { relsSource.insert(str); }

  // Add tokens and link them to their synsets, according to the loaded dictionary.

  void add_dictionary(bool with_weight); // Adds all words of the current dictionary
  void add_token(const std::string & str, bool with_weight); // Add just a word (lemma)

  // graph
  // Get the underlying boost graph

  McrGraph & graph() {return g;}

  // Add nodes and relations to the graph

  Mcr_vertex_t find_or_insert_synset(const std::string & str);
  Mcr_vertex_t find_or_insert_word(const std::string & str);

  Mcr_edge_t find_or_insert_edge(Mcr_vertex_t u, Mcr_vertex_t v, float w );

  // Add relation type to edge

  void edge_add_reltype(Mcr_edge_t e, const std::string & rel);  

  // Ask for a node

  std::pair<Mcr_vertex_t, bool> get_vertex_by_name(const std::string & str) const;

  // ask for node properties

  std::string  get_vertex_name(Mcr_vertex_t u) const {return get(vertex_name, g, u);}
  std::string  get_vertex_gloss(Mcr_vertex_t u) const {return get(vertex_gloss, g, u);}

  // ask for edge preperties

  std::vector<std::string> get_edge_reltypes(Mcr_edge_t e) const;

  // Nodes can be synsets or words

  bool vertex_is_synset(Mcr_vertex_t u) const;
  bool vertex_is_word(Mcr_vertex_t u) const;

  // Add a comment to graph


  void display_info(std::ostream & o) const;
  size_t size() const {return num_vertices(g); }

  void add_comment(const std::string & str);
  const std::vector<std::string> & get_comments() const;

  Mcr_vertex_t get_random_vertex() const;

  // Graph algorithms

  bool bfs (Mcr_vertex_t source_synset, std::vector<Mcr_vertex_t> & synv) const ;

  bool dijkstra (Mcr_vertex_t src, std::vector<Mcr_vertex_t> & parents) const;

  void pageRank_ppv(const std::vector<float> & ppv_map,
		    std::vector<float> & ranks,
		    bool use_weight);

  void ppv_weights(const std::vector<float> & ppv);

  std::ostream & dump_graph(std::ostream & o) const;

private:
  // Singleton
  static Mcr * p_instance;
  static Mcr * create();

  // Private methods
  Mcr() : coef_status(0) {};
  Mcr(const Mcr &) {};
  Mcr &operator=(const Mcr &);
  ~Mcr() {};

  Mcr_vertex_t InsertNode(const std::string & name, unsigned char flags);

  void read_from_txt(const std::string & relFile);

  void read_from_stream (std::ifstream & o);
  std::ofstream & write_to_stream(std::ofstream & o) const;

  // Private members
  McrGraph g;
  std::set<std::string> relsSource;
  std::map<std::string, Mcr_vertex_t> synsetMap; // synset name to vertex id
  std::map<std::string, Mcr_vertex_t> wordMap; // synset name to vertex id

  // Registered relation types

  std::vector<std::string> rtypes;

  std::vector<std::string> notes;        // Command line which created the graph
  // Aux variables

  std::vector<float> out_coefs;          // aux. vector of out-degree coefficients
  char coef_status;                      // 0 invalid
                                         // 1 calculated without weights
                                         // 2 calculated with weights

};


#endif 
