#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include "rule.h"

enum CT {c_bad, c_exit, c_solve, c_back, c_rule};

struct Command {
public:
  CT type;
  Rule rule;
};

class CommandParser {
  int rows, columns, colors;
  
  void get_type(std::istream & is, Command & com);
  void get_cell(std::istream & is, Command & com);
  void get_rectangle(std::istream & is, Command & com);
  void get_offset_vector(std::istream & is, Command & com);
  Command parse(std::istream & is);
public:
  CommandParser(int rows, int columns, int colors) : rows(rows), columns(columns), colors(colors) {}
  
  Command parse(const std::string & line);
};

#endif // COMMAND_H
