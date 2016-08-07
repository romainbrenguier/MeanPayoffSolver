// Author: Guillermo A. Perez <gperezme@ulb.ac.be>
// A discounted-sum game solver. (Requires that Z3 be installed
// on your computer.)

#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "z3++.h"

using namespace z3;

const char* VERSION = "Discounted-Sum Game Solver 1.0";

void usage() {
    std::cout << VERSION << std::endl
<< "usage:" << std::endl
<<"./discountedsum <filename> <discount_factor>"
<< std::endl
<< "filename                           the path to the file containing the"
<< std::endl
<< "                                   transitions of the ds-game"
<< std::endl
<< "discount_factor                    a real number 0 <= x < 1"
<< std::endl;
}

/**
 * Enconding of discounted-sum games played on automata: states and actions are
 * represented as integers. States are owned by maximizer while actions are
 * owned by minimizer. The initial state is always 0
 */

class Transition {
    int source;
    int action;
    int dest;
    int weight;

    public: 
        Transition(int s, int a, int d, int w) {
            dest = d;
            action = a;
            source = s;
            weight = w;
        }
        int getSource() { return source; }
        int getAction() { return action; }
        int getDest() { return dest; }
        int getWeight() { return weight; }
};

class Game {
    std::vector<std::vector<std::list<Transition> > > succ;

    public:
        void addTransition(Transition t) {
            int s = t.getSource();
            succ.resize(s + 1);
            int a = t.getAction();
            succ[s].resize(a + 1);
            succ[s][a].push_back(t);
        }

        void addTransition(int s, int a, int d, int w = 0) {
            addTransition(Transition(s, a, d, w));
        }

        std::set<int> getStates() {
            std::set<int> s;
            for(unsigned i = 0; i < succ.size(); i++) {
                s.insert(i);
            }
            return s;
        }

        std::set<int> availableActions(int state) {
            std::set<int> s;
            std::vector<std::list<Transition> > succ_state = succ[state];
            for(unsigned i = 0; i < succ_state.size(); i++) {
                if(succ_state[i].size() > 0)
                    s.insert(i);
            }
            return s;
        }

        std::list<Transition> post(int state, int action) {
            return succ[state][action];
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

void solveGame(Game g, float discount_factor){
    char disc_str [100];
    sprintf(disc_str, "%f", discount_factor);

    context c;

    sort R = c.real_sort();
    sort state_sort = c.int_sort();
    func_decl ds = c.function("ds", 1, &state_sort, R);

    func_decl max = c.function("max", R, R, R);
    func_decl min = c.function("min", R, R, R);
    expr lambda = c.real_const("lambda");
    expr x = c.real_const("x");
    expr y = c.real_const("y");

    std::set<int> states = g.getStates();
    solver s(c);
    s.add(lambda == c.real_val(disc_str));
    s.add(forall(x, y, max(x, y) == (ite(x < y, y, x))));
    s.add(forall(x, y, min(x, y) == (ite(x < y, x, y))));
    
    for(std::set<int>::iterator it = states.begin(); it != states.end(); ++it) {
        std::list<expr> state_expressions;
        std::set<int> actions = g.availableActions(*it);

        for(std::set<int>::iterator ait = actions.begin();
                ait != actions.end(); ++ait) {
            std::list<expr> state_action_expressions;
            std::list<Transition> successors = g.post(*it, *ait);

            for(std::list<Transition>::iterator sit = successors.begin();
                    sit != successors.end(); ++sit)
                state_action_expressions.push_back(sit->getWeight() +
                                                   (lambda * ds(sit->getDest())));

            state_expressions.push_back(fold(min, state_action_expressions));
	    }

        s.add(ds(*it) == fold(max, state_expressions));
    }
    
    std::cout << "Equations : " << std::endl << s << "\n";
    switch(s.check()) {
        case unsat:   std::cout << "equations are not satisfiable\n"; break;
        case sat:     std::cout << "equations are satisfiable\n"; break;
        case unknown: std::cout << "unknown\n"; break;
    }

    model m = s.get_model();
    std::cout << "Model: " << std::endl << m << std::endl;

    std::cout << std::endl << " Results: " << std::endl;
    set_param("pp.decimal", true);
    for(std::set<int>::iterator it = states.begin(); it!= states.end(); ++it)
        std::cout << "DS(" << *it << ") = " << m.eval(ds(*it)) << std::endl;
        
}

int main(int argc, char* argv[]) {
    Game g;
  
    if (argc < 3) {
        usage();
        exit(1);
    } else {
        std::cout << "Reading file " << argv[1] << std::endl;
        std::ifstream file(argv[1]);
        if(file.is_open()) {
            int a, b, c, d, line = 0;
            while(file >> a >> b >> c >> d) {
    	        line++;
    	        g.addTransition(a, b, c, d);
            }
            file.close();
            std::cout << line << " lines read" << std::endl;
        }
        else std::cout << "Unable to open file";
        std::cout << "Discount factor: " << std::atof(argv[2]) << std::endl;
    }
  	
    solveGame(g, std::atof(argv[2]));
    return 0;

}
