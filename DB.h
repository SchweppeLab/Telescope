#ifndef _DB_H
#define _DB_H

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace db_ns {

#define DBASER_VER "1.1.0"
#define DBASER_DATE "19 SEPTEMBER 2024"
#define DBASER_ALPHABET 26

  struct DBVertex {
    int next[DBASER_ALPHABET];
    bool leaf = false;
    int p = -1;
    char pch;
    int link = -1;
    int go[DBASER_ALPHABET];
    size_t wordIndex = SIZE_MAX;

    DBVertex(int p = -1, char ch = '$') : p(p), pch(ch) {
      std::fill(std::begin(next), std::end(next), -1);
      std::fill(std::begin(go), std::end(go), -1);
    }
  };

  struct DBPeptide {
    std::string peptide;
    std::vector<size_t> proteins;
    DBPeptide(std::string s = "") : peptide(s) {}
  };


  //FASTA database structure
  typedef struct sDB {
    bool decoy;
    std::string description; //FASTA description (header - after first space)
    std::string name;        //FASTA name (header - before first space)
    std::string sequence;    //FASTA sequence
  } sDB;

  class DB {
  public:

    DB() { resetPeptides(); }

    sDB& operator[ ](const size_t& i) { return vDB[i]; }

    size_t addPeptide(std::string const& s);
    bool buildDB(std::string fname, std::string decoyStr = "");
    bool buildDB(const char* fname, std::string decoyStr = "");
    size_t find(std::string name);
    void findPeptides();
    DBPeptide& getPeptide(size_t index);
    void resetPeptides();
    size_t size() { return vDB.size(); }
    size_t sizePeptide() { return vPeptide.size(); }

  protected:
  private:

    //double        AA[128] = { 0 };
    std::vector<sDB> vDB;    //Entire FASTA database stored in memory
    std::vector<DBVertex> vTrie;
    std::vector<DBPeptide> vPeptide;
    std::map<std::string, size_t> mDB;
    std::map<std::string, size_t> mPep;

    int get_link(int v);
    int go(int v, char ch);
    void searchForPeps(size_t index);
  };

}

#endif
