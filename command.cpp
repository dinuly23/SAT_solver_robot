#include "command.h"

#include <sstream>

void CommandParser::get_type(std::istream & is, Command & com) {
  std::string name;
  std::getline(is, name, ' ');
  if(is.bad()) com.type = c_bad;
  else if(name == "exit") com.type = c_exit;
  else if(name == "solve") com.type = c_solve;
  else if(name == "back") com.type = c_back;
  else {
    com.type = c_rule;
    if(name == "unruly") com.rule.type = r_unruly;
    else if(name == "color") com.rule.type = r_color;
    else if(name == "ncolor") com.rule.type = r_ncolor;
    else if(name == "rich") com.rule.type = r_rich;
    else if(name == "erich") com.rule.type = r_erich;
    else if(name == "mixed") com.rule.type = r_mixed;
    else if(name == "emixed") com.rule.type = r_emixed;
    else if(name == "balanced") com.rule.type = r_balanced;
    else if(name == "ebalanced") com.rule.type = r_ebalanced;
    else if(name == "nbalanced") com.rule.type = r_nbalanced;
    else if(name == "enbalanced") com.rule.type = r_enbalanced;
    else com.type = c_bad;
  }
}

void CommandParser::get_cell(std::istream & is, Command & com) {
  int x, y, c;
  is >> x;
  is >> y;
  is >> c;
  if(is.bad() || x >= rows || y >= columns || c >= colors) com.type = c_bad;
  else {
    com.rule.c = {x,y};
    com.rule.color = c;
  }
}

void CommandParser::get_rectangle(std::istream & is, Command & com) {
  int x, y;
  is >> x;
  is >> y;
  if(is.bad() || x <= 0 || x > rows || y <= 0 || y > columns) com.type = c_bad;
  else com.rule.c = {x,y};
}

void CommandParser::get_offset_vector(std::istream & is, Command & com) {
  int x, y;
  while(is.good()) {
    is >> x;
    is >> y;
    if(is.bad() || x >= rows || y >= columns) {
      com.type = c_bad;
      return;
    }
    com.rule.v.push_back({x, y});
  }
}

Command CommandParser::parse(std::istream & is) {
  Command res;
  get_type(is, res);
  if(res.type == c_bad || res.type == c_exit || res.type == c_solve || res.type == c_back || (res.type == c_rule && res.rule.type == r_unruly)) return res;
  else if(res.rule.type == r_color || res.rule.type == r_ncolor) get_cell(is, res);
  else if(res.rule.type == r_rich || res.rule.type == r_balanced || res.rule.type == r_nbalanced || res.rule.type == r_mixed) get_rectangle(is, res);
  else if(res.rule.type == r_erich || res.rule.type == r_ebalanced || res.rule.type == r_enbalanced || res.rule.type == r_emixed) get_offset_vector(is, res);
  else res.type = c_bad;
  return res;
}

Command CommandParser::parse(const std::string & line) {
  std::stringstream ss(line);
  return parse(ss);
}
