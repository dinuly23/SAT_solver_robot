#ifndef RULE_H
#define RULE_H

#include <vector>

enum RT {r_unruly, r_color, r_ncolor, r_rich, r_erich, r_mixed, r_emixed, r_balanced, r_ebalanced, r_nbalanced, r_enbalanced};

struct Coord {
  int x;
  int y;
};

struct Rule {
  RT type;
  Coord c;
  std::vector<Coord> v;
  int color;
};

#endif // RULE_H
