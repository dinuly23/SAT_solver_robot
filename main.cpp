#include <iostream>
#include <string>
#include <algorithm>

#include "minisat/core/Solver.h"

#include "command.h"

int rows, columns, colors;

bool parse_args(int argc, char *argv[]) {
  if(argc != 4) {
    std::cout << "wrong number of arguments, aborting" << std::endl;
    return false;
  }
  try {
    rows = std::stoi(argv[1]);
    columns = std::stoi(argv[2]);
    colors = std::stoi(argv[3]);
  }
  catch(...) {
    std::cout << "arguments are not numbers or badly written numbers, aborting" << std::endl;
    return false;
  }
  if(!(rows >= 2 && columns >= 2 && colors >= 2)) {
    std::cout << "some argument numbers are less than 2, aborting" << std::endl;
    return false;
  }
  return true;
}



using namespace std;

int balanced_row,balanced_column,balanced_number,balanced_color,shuffled=0,i_nbalanced,j_nbalanced;
string esc_start="\033[",esc_end="m*\033[0m  ";
Minisat::Solver s;
Minisat::vec<Minisat::Lit> px;
Minisat::vec<Minisat::Lit> assump; 
vector <int> v;
Minisat::vec<Minisat::Var> x;
vector<unsigned long long> all;
unsigned long long one;
string esc_colors[72];
Command command;

void dfs(int num,int sum) {
  if (sum>=balanced_color)
      return;
  if (num==balanced_number) {
    for (int i1=0;i1<=rows-balanced_row;i1++)
      for (int j1=0;j1<=columns-balanced_column;j1++)
        for (int c=0;c<colors;c++) {
          for (int i=0;i<balanced_row;i++)
            for (int j=0;j<balanced_column;j++)
              px.push(Minisat::mkLit(x[(i1+i)*columns*colors+(j1+j)*colors+c],v[i*balanced_column+j]));
          px.push(~assump[assump.size()-1]);
          s.addClause(px);
          px.clear();
        }
  }
  else {
    v[num]=0;
    dfs(num+1,sum);
    v[num]=1;
    dfs(num+1,sum+1);
  }
}

void balanced(int row,int col) {
  balanced_number=col*row;
  balanced_column=col;
  balanced_row=row;
  if (balanced_number%colors)
    throw "cannot equal colors in rule balanced";
  balanced_color=balanced_number/colors;
  v.resize(balanced_number);
  dfs(0,0);
  v.clear();
}

void dfs_fill(int num,int sum) {
  if (sum>balanced_color)
      return;
  if (num==balanced_number) {
    if (sum==balanced_color) {
      all.push_back(one);
    }
  }
  else {
    dfs_fill(num+1,sum);
    one^=1<<num;
    dfs_fill(num+1,sum+1);
    one^=1<<num;
  }
}

void dfs_nbalanced(int color) {
  if (color==colors)
    s.addClause(px);
  else {
    for (int i1=0;i1<(int)all.size();++i1) {
      for (int i=0;i<balanced_row;i++)
        for (int j=0;j<balanced_column;j++) {
          px.push(Minisat::mkLit(x[(i_nbalanced+i)*columns*colors+(j_nbalanced+j)*colors+color],all[i1]&(1<<(i*balanced_column+j))));
        }
      dfs_nbalanced(color+1);
      for (int i=0;i<balanced_row;i++)
        for (int j=0;j<balanced_column;j++) {
          px.pop();
        }
    }
  }
}

void nbalanced(int row,int col) {
  balanced_number=col*row;
  balanced_column=col;
  balanced_row=row;
  balanced_color=balanced_number/colors;
  one=0;
  dfs_fill(0,0);
  for (i_nbalanced=0;i_nbalanced<=rows-balanced_row;i_nbalanced++)
    for (j_nbalanced=0;j_nbalanced<=columns-balanced_column;j_nbalanced++) {
      px.push(~assump[assump.size()-1]);
      dfs_nbalanced(0);
      px.clear();
    }
  all.clear();
}

void dfs_ebalanced(int num,int sum) {
  if (sum>=balanced_color)
      return;
  if (num==balanced_number) {
      for (int i1=0;i1<rows-balanced_row;i1++)
        for (int j1=0;j1<columns-balanced_column;j1++)
          for (int c=0;c<colors;c++) {
            for (int i=0;i<balanced_number-1;++i)
              px.push(Minisat::mkLit(x[(i1+command.rule.v[i].x)*columns*colors+(j1+command.rule.v[i].y)*colors+c],v[i+1]));
            px.push(Minisat::mkLit(x[i1*columns*colors+j1*colors+c],v[0]));
            px.push(~assump[assump.size()-1]);
            s.addClause(px);
            px.clear();
          }
  }
  else {
    v[num]=0;
    dfs_ebalanced(num+1,sum);
    v[num]=1;
    dfs_ebalanced(num+1,sum+1);
  }
}

void ebalanced() {
  balanced_number=command.rule.v.size()+1;
  balanced_column=0;
  balanced_row=0;
  for (int i=0;i<balanced_number-1;++i) {
    balanced_column=max(balanced_column,command.rule.v[i].y);
    balanced_row=max(balanced_row,command.rule.v[i].x);
  }
  if (balanced_number%colors)
    throw "cannot equal colors in rule balanced";
  balanced_color=balanced_number/colors;
  v.resize(balanced_number);
  dfs_ebalanced(0,0);
  v.clear();
}

void dfs_enbalanced(int color) {
  if (color==colors)
    s.addClause(px);
  else {
    for (int i1=0;i1<(int)all.size();++i1) {
      for (int i=0;i<balanced_number-1;++i) {
        px.push(Minisat::mkLit(x[(i_nbalanced+command.rule.v[i].x)*columns*colors+(j_nbalanced+command.rule.v[i].y)*colors+color],all[i1]&(1<<(i+1))));
      }
      px.push(Minisat::mkLit(x[i_nbalanced*columns*colors+j_nbalanced*colors+color],all[i1]&(1<<0)));
      dfs_enbalanced(color+1);
      for (int i=0;i<balanced_number;++i) {
        px.pop();
      }
    }
  }
}

void enbalanced() {
  balanced_number=command.rule.v.size()+1;
  balanced_column=0;
  balanced_row=0;
  for (int i=0;i<balanced_number-1;++i) {
    balanced_column=max(balanced_column,command.rule.v[i].y);
    balanced_row=max(balanced_row,command.rule.v[i].x);
  }
  balanced_color=balanced_number/colors;
  one=0;
  dfs_fill(0,0);
  for (i_nbalanced=0;i_nbalanced<rows-balanced_row;i_nbalanced++)
    for (j_nbalanced=0;j_nbalanced<columns-balanced_column;j_nbalanced++) {
      px.push(~assump[assump.size()-1]);
      dfs_enbalanced(0);
      px.clear();
    }
  all.clear();
}

void mixed(int row,int col) {
  for (int i1=0;i1<=rows-row;i1++)
    for (int j1=0;j1<=columns-col;j1++) {
      for (int c=0;c<colors;c++) {
        for (int i=0;i<row;i++) {
          for (int j=0;j<col;j++) {
            px.push(Minisat::mkLit(x[(i1+i)*columns*colors+(j1+j)*colors+c],1));
          }
        }
        px.push(~assump[assump.size()-1]);
        s.addClause(px);
        px.clear();
      }
    }
}

void emixed() {
  balanced_column=0;
  balanced_row=0;
  balanced_number=command.rule.v.size();
  for (int i=0;i<balanced_number;++i) {
    balanced_column=max(balanced_column,command.rule.v[i].y);
    balanced_row=max(balanced_row,command.rule.v[i].x);
  }
  for (int i1=0;i1<rows-balanced_row;i1++)
    for (int j1=0;j1<columns-balanced_column;j1++) {
      for (int c=0;c<colors;c++) {
        for (int i=0;i<balanced_number;++i)
          px.push(Minisat::mkLit(x[(i1+command.rule.v[i].x)*columns*colors+(j1+command.rule.v[i].y)*colors+c],1));
        px.push(Minisat::mkLit(x[i1*columns*colors+j1*colors+c],1));
        px.push(~assump[assump.size()-1]);
        s.addClause(px);
        px.clear();
      }
    }
}

void rich(int row,int col) {
  for (int i1=0;i1<=rows-row;i1++)
    for (int j1=0;j1<=columns-col;j1++) {
      for (int c=0;c<colors;c++) {
        for (int i=0;i<row;i++) {
          for (int j=0;j<col;j++) {
            px.push(Minisat::mkLit(x[(i1+i)*columns*colors+(j1+j)*colors+c]));
          }
        }
        px.push(~assump[assump.size()-1]);
        s.addClause(px);
        px.clear();
      }
    }
}

void erich() {
  balanced_column=0;
  balanced_row=0;
  balanced_number=command.rule.v.size();
  for (int i=0;i<balanced_number;++i) {
    balanced_column=max(balanced_column,command.rule.v[i].y);
    balanced_row=max(balanced_row,command.rule.v[i].x);
  }
  for (int i1=0;i1<rows-balanced_row;i1++)
    for (int j1=0;j1<columns-balanced_column;j1++) {
      for (int c=0;c<colors;c++) {
        for (int i=0;i<balanced_number;++i)
          px.push(Minisat::mkLit(x[(i1+command.rule.v[i].x)*columns*colors+(j1+command.rule.v[i].y)*colors+c]));
        px.push(Minisat::mkLit(x[i1*columns*colors+j1*colors+c]));
        px.push(~assump[assump.size()-1]);
        s.addClause(px);
        px.clear();
      }
    }
}


void print_answer() {
  if (!shuffled) {
    for (int i=0;i<8;++i) {
      esc_colors[i]=to_string(30+i);
      for (int j=0;j<8;++j) {
        esc_colors[(i+1)*8+j]=to_string(30+i)+';' + to_string(40+j);
      }
    }
    srand(time(0));
    random_shuffle(esc_colors+8,esc_colors+72);
    random_shuffle(esc_colors,esc_colors+8);
    shuffled=1;
  }
  for(int i=0; i<rows; i++){
    for(int j=0; j<columns; j++){
      for (int c=0;c<colors;++c) {
        if(s.modelValue(x[i*columns*colors+j*colors+c])==Minisat::l_True) {
          string str=esc_start + esc_colors[c] + esc_end;
          cout << str;
        }
      }
    }
    cout<<"\n";
  }
}

int main(int argc, char *argv[])
{
  if(!parse_args(argc, argv)) return 0;
  std::string line;
  CommandParser parser(rows, columns, colors);
  x.growTo(rows*columns*colors);
  for(int i=0;i<rows*columns*colors;i++)
          x[i]=s.newVar();
  for(int i=0; i<rows; i++) {             //каждая клетка имеет один цвет
    for(int j=0; j<columns;j++) {
      for(int c=0; c<colors;c++)
        px.push(Minisat::mkLit(x[i*columns*colors+j*colors+c]));
      s.addClause(px);
      px.clear();
      for(int c1=0; c1<colors;c1++){
        for(int c2=0; c2<colors;c2++){
          if(c1!=c2){
            px.push(Minisat::mkLit(x[i*columns*colors+j*colors+c1],1));
            px.push(Minisat::mkLit(x[i*columns*colors+j*colors+c2],1));
            s.addClause(px);
            px.clear();
          }
        }
      }
    }
  }
  while(true) {
      try {
      std::cout << "enter command: ";
      std::getline(std::cin, line);
      if(!std::cin.good()) {
        std::cout << "input stream broken, aborting" << std::endl;
        return 0;
      }
      command = parser.parse(line);
      if (command.type==c_exit) {
        cout << "Goodbye\n";
        break;
      }
      else if (command.type==c_solve) {
        //~ s.toDimacs("1.txt");
        if (!s.solve(assump)) {
          cout << "With this set of rules unruly is unsolvable\n";
          continue;
        }
        print_answer();
      }
      else if (command.type==c_bad) {
        cout << "Invalid command\n";
      }
      else if (command.type==c_back) {
        if(assump.size()>=1){
          s.releaseVar(~assump[assump.size()-1]);
          assump.pop();
        }
        else cout<< "No instructions\n";
      }
      else if (command.type==c_rule) {
        Minisat::Lit l=Minisat::mkLit(s.newVar());
        assump.push(~l);
        if (command.rule.type==r_color) {
          s.addClause(Minisat::mkLit(x[command.rule.c.x*columns*colors+command.rule.c.y*colors+command.rule.color]),~assump[assump.size()-1]);
        }
        else if (command.rule.type==r_ncolor) {
          s.addClause(Minisat::mkLit(x[command.rule.c.x*columns*colors+command.rule.c.y*colors+command.rule.color],1),~assump[assump.size()-1]);
        }
        else if (command.rule.type==r_balanced) {
          balanced(command.rule.c.x,command.rule.c.y);
        }
        else if (command.rule.type==r_nbalanced) {
          nbalanced(command.rule.c.x,command.rule.c.y);
        }
        else if (command.rule.type==r_mixed) {
          mixed(command.rule.c.x,command.rule.c.y);
        }
        else if (command.rule.type==r_rich) {
          rich(command.rule.c.x,command.rule.c.y);
        }
        else if (command.rule.type==r_unruly) {
          balanced(1,columns);
          balanced(rows,1);
          mixed(1,3);
          mixed(3,1);
        }
        else if (command.rule.type==r_ebalanced) {
          ebalanced();
        }
        else if (command.rule.type==r_enbalanced) {
          enbalanced();
        }
        else if (command.rule.type==r_emixed) {
          emixed();
        }
        else if (command.rule.type==r_erich) {
          erich();
        }
      }
    }
    catch(const char * e) {
      cout << e << endl;
    }
  }
  return 0;
}
