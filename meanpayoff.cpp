// Author: Romain Brenguier <rbrengui@cs.ox.ac.uk>
// A mean payoff solver using Gain and Bias equations.
// This assumes the library Z3 is installed on your computer.

#include<vector>
#include<set>
#include<list>
#include<iostream>
#include<fstream>
#include<string>
#include"z3++.h"

using namespace z3;


/* Encoding of Mean payoff games: 
   States are represented by integers.
   Even states are minimizer states.
   Odd states are maximizer states.
   The initial state is 0.
*/ 
class Transition {
  int source;
  int dest;
  int weight;

public : 
  Transition(int s,int d, int w) {
    dest=d ;
    source=s;
    weight=w;
  }
  int get_source(){ return source; }
  int get_dest(){ return dest; }
  int get_weight(){ return weight; }
};

class Game{
  std::vector<std::list<Transition> > succ;

public:
  
  void add_transition(Transition t) {
    int s = t.get_source();
    succ.resize(s+1);
    succ[s].push_back(t);
  }

  void add_transition(int s,int d, int w=0) {
    add_transition(Transition(s,d,w));
  }


  std::set<int> states() {
    std::set<int> s;
    for(unsigned i=0;i<succ.size();i++) {
      s.insert(i);
    }
    return s;
  }

  std::list<Transition> successors(int state) {
    return succ[state];
  }
};


expr fold(func_decl f, std::list<expr> e){
  std::list<expr>::iterator it = e.begin();
  expr res = *it;
  for(++it; it != e.end(); ++it)
    res = f(res, *it);
  return res;
}


expr disj(std::list<expr> e){
  std::list<expr>::iterator it = e.begin();
  expr res(*it);
  for(++it; it != e.end(); ++it)
    res = res || *it;
  return res;
}

expr conj(std::list<expr> e){
  std::list<expr>::iterator it = e.begin();
  expr res(*it);
  for(++it; it != e.end(); ++it)
    res = res && *it;
  return res;
}

void solve_game(Game g){
    context c;

    sort R = c.real_sort();
    sort state_sort = c.int_sort();
    func_decl mp = c.function("mp",1,&state_sort,R);
    func_decl bias = c.function("bias",1,&state_sort,R);

    func_decl max = c.function("max",R,R,R);
    func_decl min = c.function("min",R,R,R);
    expr x = c.real_const("x");
    expr y = c.real_const("y");

    std::set<int> states = g.states();
    solver s(c);    
    //    s.add(forall(x,y,max(x,y) == (ite(x < y,y,x))));
    //s.add(forall(x,y,min(x,y) == (ite(x < y,x,y))));
    

    for(std::set<int>::iterator it = states.begin();
	it != states.end(); ++it)
      {
	std::list<expr> e;
	std::list<expr> f;
	std::list<Transition> successors = g.successors(*it);
	for(std::list<Transition>::iterator sit=successors.begin(); sit!=successors.end(); ++sit)
	  {
	    if (*it % 2 == 0)
	      e.push_back(mp(*it) <= mp(sit->get_dest()));
	    else 
	      e.push_back(mp(*it) >= mp(sit->get_dest()));

	    // TO CHANGE: We need to take the max/min of the bias
	    f.push_back((mp(*it) == mp(sit->get_dest())) && (mp(*it) + bias(*it)) == sit->get_weight() + bias(sit->get_dest()));
	  }

	// if (*it % 2 == 0) s.add(mp(*it) == fold(min,e)); else s.add(mp(*it) == fold(max,e));

	s.add(conj(e));

	if(! f.empty()) 
	  s.add(disj(f));

      }
    

    //std::cout << "Equations : " << std::endl << s.to_smt2() << "\n";
    std::cout << "Equations : " << std::endl << s<< "\n";
    switch (s.check()) {
    case unsat:   std::cout << "equations are not satisfiable\n"; break;
    case sat:     std::cout << "equations are satisfiable\n"; break;
    case unknown: std::cout << "unknown\n"; break;
    }


    model m = s.get_model();
    std::cout << "Model: "<< std::endl << m << std::endl;


    std::cout << std::endl << " Results: " << std::endl;
    set_param("pp.decimal", true);
    for(std::set<int>::iterator it=states.begin(); it!= states.end(); ++it)
      std::cout << "State: "<< *it << "  MP: " << m.eval(mp(*it)) << "  Bias: " << m.eval(bias(*it)) << std::endl;

        
}

int main(int argc, char* argv[]) {
  Game g;

  if(argc < 2) {
    std::cout << "usage: meanpayoff <file>" << std::endl;
    std::cout << "using a dummy game instead" << std::endl;
    g.add_transition(0,1,1);
    g.add_transition(1,0,2);
  }
  else {
    std::ifstream file(argv[1]);
    if(file.is_open()) {
      int a,b,c,line=1;
      while(file >> a >> b >> c) {
	line++;
	g.add_transition(a,b,c);
      }
      file.close();
      std::cout << line-1 << " lines read" << std::endl;
    }
    else std::cout << "Unable to open file";
  }
	
  solve_game(g);
  return 0;

}
