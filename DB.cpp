#include "DB.h"

//return index if peptide added, otherwise SIZE_MAX (because peptide already in trie).
size_t db_ns::DB::addPeptide(std::string const& s) {
  std::map<std::string, size_t>::iterator it;
  it = mPep.find(s);
  size_t index = 0;
  if (it == mPep.end()) {
    index = vPeptide.size();
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

bool db_ns::DB::buildDB(std::string fname, std::string decoyStr) {
  return buildDB(fname.c_str(), decoyStr);
}

bool db_ns::DB::buildDB(const char* fname, std::string decoyStr) {
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

  std::map<std::string, size_t>::iterator it;
  for (size_t a = 0;a < vDB.size();a++) {
    it = mDB.find(vDB[a].name);
    if (it == mDB.end()) mDB.insert(std::pair<std::string, size_t>(vDB[a].name, a));
    else std::cout << vDB[a].name << " found multiple times in FASTA file." << std::endl;
  }

  //cout << "  Total Proteins: " << vDB.size() << endl;
  return true;
}

size_t db_ns::DB::find(std::string name) {
  std::map<std::string, size_t>::iterator it;
  it = mDB.find(name);
  if (it == mDB.end()) return SIZE_MAX;
  else return it->second;
}

void db_ns::DB::findPeptides() {
  for (size_t a = 0;a < vPeptide.size();a++) vPeptide[a].proteins.clear();
  for (size_t a = 0; a < vDB.size(); a++) searchForPeps(a);
}

int db_ns::DB::get_link(int v) {
  if (vTrie[v].link == -1) {
    if (v == 0 || vTrie[v].p == 0) vTrie[v].link = 0;
    else vTrie[v].link = go(get_link(vTrie[v].p), vTrie[v].pch);
  }
  return vTrie[v].link;
}

db_ns::DBPeptide& db_ns::DB::getPeptide(size_t index) {
  return vPeptide[index];
}

int db_ns::DB::go(int v, char ch) {
  int c = ch - 'A';
  if (vTrie[v].go[c] == -1) {
    if (vTrie[v].next[c] != -1) vTrie[v].go[c] = vTrie[v].next[c];
    else vTrie[v].go[c] = v == 0 ? 0 : go(get_link(v), ch);
  }
  return vTrie[v].go[c];
}

void db_ns::DB::resetPeptides() {
  std::vector<db_ns::DBPeptide> vPeptideTmp;
  vPeptide.swap(vPeptideTmp); //for clear and reallocate
  mPep.clear();
  std::vector<DBVertex> vTrieTmp;
  vTrie.swap(vTrieTmp);
  vTrie.emplace_back(1);
}

void db_ns::DB::searchForPeps(size_t index) {
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
