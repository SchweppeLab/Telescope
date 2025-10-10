#ifndef _DBASER_H
#define _DBASER_H

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace dbaser {

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

class DBaser {
public:

  DBaser() { resetPeptides(); }

  sDB& operator[ ](const size_t& i) { return vDB[i]; }

  size_t addPeptide(std::string const& s);
  bool buildDB(std::string fname, std::string decoyStr = "");
  bool buildDB(const char* fname, std::string decoyStr="");
  size_t find(std::string name);
  void findPeptides();
  DBPeptide& getPeptide(size_t index);
  void resetPeptides();
  size_t size() { return vDB.size();}
  size_t sizePeptide() { return vPeptide.size();}

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

//return index if peptide added, otherwise SIZE_MAX (because peptide already in trie).
size_t DBaser::addPeptide(std::string const& s) {
  std::map<std::string,size_t>::iterator it;
  it=mPep.find(s);
  size_t index=0;
  if(it==mPep.end()){
    index= vPeptide.size();
    mPep.insert(std::pair<std::string, size_t>(s, index));
    vPeptide.emplace_back(s);
  } else return it->second;

  int v = 0;
  for (char ch : s) {
    int c = ch - 'A';
    if (vTrie[v].next[c] == -1) {
      vTrie[v].next[c] = (int)vTrie.size();
      vTrie.emplace_back(v, ch);
    }
    v = vTrie[v].next[c];
  }
  vTrie[v].leaf = true;
  vTrie[v].wordIndex = index;
  return index;
}

bool DBaser::buildDB(std::string fname, std::string decoyStr) {
  return buildDB(fname.c_str(),decoyStr);
}

bool DBaser::buildDB(const char* fname, std::string decoyStr) {
  char  str[10240];
  char* tok;
  FILE* f;
  sDB  d;
  char  c;

  d.name = "NIL";
  vDB.clear();
  mDB.clear();

  f = fopen(fname, "rt");
  if (f == NULL) return false;
  while (!feof(f)) {
    if (fgets(str, 10240, f) == NULL) continue;
    if (strlen(str) > 0) {
      tok = strtok(str, "\r\n");
      if (tok == NULL) continue;
      strcpy(str, tok);
    }
    if (str[0] == '>') {
      if (d.name.compare("NIL") != 0) {
        if (!decoyStr.empty() && d.name.find(decoyStr) != std::string::npos) d.decoy = true;
        else d.decoy = false;
        vDB.push_back(d);
      }
      std::string st = &str[1];
      size_t sp = st.find_first_of(' ');
      d.name = st.substr(0, sp);
      d.description = st.substr(sp + 1, st.size());
      d.sequence = "";
    } else {
      for (unsigned int i = 0; i < strlen(str); i++) {
        c = toupper(str[i]);
        //if (AA[c] == 0) {
        //  cout << "  WARNING: " << &d.name[0] << " has an unexpected amino acid character or errant white space: '" << c << "'" << endl;
        //}
        if (c == ' ' || c == '\t') continue;
        //if (AA[c] == 0) {
        //  cout << "  WARNING: Mass of '" << c << "' is currently set to 0. Consider revising with the aa_mass parameter." << endl;
        //}
        d.sequence += c;
      }
    }
  }
  fclose(f);
  if (d.sequence.length() > SIZE_MAX) {
    std::cout << "  WARNING: " << &d.name[0] << " has a sequence that is too long. It will be skipped." << std::endl;
  } else {
    if (!decoyStr.empty() && d.name.find(decoyStr) != std::string::npos) d.decoy = true;
    else d.decoy = false;
    vDB.push_back(d);
  }

  std::map<std::string,size_t>::iterator it;
  for(size_t a=0;a<vDB.size();a++){
    it=mDB.find(vDB[a].name);
    if(it==mDB.end()) mDB.insert(std::pair<std::string,size_t>(vDB[a].name,a));
    else std::cout << vDB[a].name << " found multiple times in FASTA file." << std::endl;
  }

  //cout << "  Total Proteins: " << vDB.size() << endl;
  return true;
}

size_t DBaser::find(std::string name){
  std::map<std::string, size_t>::iterator it;
  it = mDB.find(name);
  if(it==mDB.end()) return SIZE_MAX;
  else return it->second;
}

void DBaser::findPeptides(){
  for(size_t a=0;a<vPeptide.size();a++) vPeptide[a].proteins.clear();
  for (size_t a = 0; a < vDB.size(); a++) searchForPeps(a);
}

int DBaser::get_link(int v) {
  if (vTrie[v].link == -1) {
    if (v == 0 || vTrie[v].p == 0) vTrie[v].link = 0;
    else vTrie[v].link = go(get_link(vTrie[v].p), vTrie[v].pch);
  }
  return vTrie[v].link;
}

DBPeptide& DBaser::getPeptide(size_t index){
  return vPeptide[index];
}

int DBaser::go(int v, char ch) {
  int c = ch - 'A';
  if (vTrie[v].go[c] == -1) {
    if (vTrie[v].next[c] != -1) vTrie[v].go[c] = vTrie[v].next[c];
    else vTrie[v].go[c] = v == 0 ? 0 : go(get_link(v), ch);
  }
  return vTrie[v].go[c];
}

void DBaser::resetPeptides(){
  std::vector<DBPeptide> vPeptideTmp;
  vPeptide.swap(vPeptideTmp); //for clear and reallocate
  mPep.clear();
  std::vector<DBVertex> vTrieTmp;
  vTrie.swap(vTrieTmp);
  vTrie.emplace_back(1);
}

void DBaser::searchForPeps(size_t index){
  int currentState = 0;
  for (size_t i = 0; i < vDB[index].sequence.size(); ++i) {
    currentState = go(currentState, vDB[index].sequence[i]);
    if (currentState == 0) continue; // Nothing new, let's move on to the next character.
    else if (vTrie[currentState].leaf) {
      //cout << pep[vTrie[currentState].wordIndex].pep << " found at: " << i - pep[vTrie[currentState].wordIndex].pep.size() + 1 << endl;
      vPeptide[vTrie[currentState].wordIndex].proteins.push_back(index);
    }
  }
}

}

#endif
