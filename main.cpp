#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctype.h>
#include <ctime>

using namespace std;

struct Part{
    std::string src; // 源
    std::string edge; // 边
    std::string dst; // 目的地
};

struct ObsPart {
    vector<string> src;
    string edge;
    vector<string> dst;
};

struct EstPart {
    vector<string> src1;
    vector<string> src2;
    string edge;
    vector<string> dst1;
    vector<string> dst2;
};

struct State{
    vector<string> state1;
    vector<string> state2;
};

void outputVectorString(vector<string>& s) {
    for(int i = 0; i < s.size(); ++i) {
        cout << " {" << s[i] << "} ";
    }
}

void outputObs(vector<ObsPart> & obs) {
    string t = "\t\t";
    cout << "src" << t << "edge" << t << "dst" <<"\n";
    for(int i = 0; i < obs.size(); ++i) {
        outputVectorString(obs[i].src);
        cout << t << obs[i].edge << t;
        outputVectorString(obs[i].dst);
        cout << "\n";
    }
}

bool checkInE(vector<string> & E, string &e) {
    for(auto & i : E) {
        if(i == e) {
            return true;
        }
    }
    return false;
}

vector<string> & getEFromNFA(vector<Part> & NFA, vector<string> & E) {
    for(int i = 0; i < NFA.size(); ++i) {
        if(!checkInE(E, NFA[i].edge)) {
            E.push_back(NFA[i].edge);
        }
    }
    return E;
}

// 根据 E 和 Euo 构造 Eo
vector<string> constructEoFromEEuo(std::vector<std::string> &Eo, std::vector<std::string> E, std::vector<std::string> Euo) {
    for(auto i = Euo.begin(); i != Euo.end(); ++i) {
        for(auto j = E.begin(); j != E.end(); ++j) {
            if((*i) == (*j)) {
                E.erase(j);
            }
        }
    }

    for(auto i = E.begin(); i != E.end(); ++i) {
        Eo.push_back(*i);
    }
    sort(Eo.begin(), Eo.end());
    //删除重复元素
    Eo.erase(std::unique(Eo.begin(), Eo.end()), Eo.end());
    return Eo;
}


// 构建字符集
void constructEera(std::vector<std::string> &E, std::string e) {
    E.push_back(e);
}

// 从文件中读取字符集E
vector<string>& inputEFromfile(std::vector<std::string> &E, std::string &filename) {
    std::ifstream infile(filename);
    if(!infile.is_open()) {
        std::cout << "faild to open file" << std::endl;
        return E;
    }
    char buf[0xFF];
    std::string e;
    int line = 0;
    int num = 99;
    while(infile.getline(buf, sizeof(buf))) {
        std::stringstream word(buf);
        if(line == 0 && line < num) {
            word >> num;
            line++;
        } else {
            word >> e;
            constructEera(E, e);
        }
    }
    return E;
}

// 输出 字符集
void outputE(std::vector<std::string> &E) {
    std::cout << "E contains ";
    for(int i = 0; i < E.size(); ++i) {
        std::cout << E[i] << " ";
    }
    std::cout << std::endl;
}

// 输出 Vector Part
void outputNFA(std::vector<Part> NFA) {
    for(int i = 0; i < NFA.size(); ++i) {
        std::cout << NFA[i].src << " " << NFA[i].edge << " " << NFA[i].dst << std::endl;
    }
}

// 判断 vector part 中 是否存在边 src edge dst
bool checkIfPartInNFA(std::vector<Part>& NFA, std::string & src, std::string & edge, std::string & dst) {
    for(int i = 0; i < NFA.size(); ++i) {
        if(NFA[i].src == src && NFA[i].edge == edge && NFA[i].dst == dst) {
            return true;
        }
    }
    return false;
}

//构造NFA
vector<Part> & constructNFA(std::vector<Part> &NFA, std::string src, std::string edge, std::string dst) {
    Part temp;
    temp.src = src;
    temp.edge = edge;
    temp.dst = dst;
    if(!checkIfPartInNFA(NFA, src, edge, dst)) {
        NFA.push_back(temp);
    }
    return NFA;
}

// 构建 Plant G
vector<Part>& constructPlantG(std::vector<Part> &NFA, std::string &fileName) {
    std::ifstream infile(fileName);
    if(!infile.is_open()) {
        std::cout << "faild to open file" << std::endl;
        return NFA;
    }
    char buf[0xFF];
    Part temp;
    int line = 0;
    int num = 99999;
    while(infile.getline(buf, sizeof(buf))) {
        std::stringstream word(buf);
        if(line == 0 && line < num) {
            word >> num;
            line++;
        }
        else{
            word >> temp.src;
            word >> temp.edge;
            word >> temp.dst;
            constructNFA(NFA, temp.src,temp.edge, temp.dst);
        }
    }

    // 检查输出结果是否正确
    outputNFA(NFA);
    return NFA;
}

int checkFlag(vector<bool> & t) {
    for(int i = 0; i < t.size(); ++i) {
        if(!t[i]) {
            return i;
        }
    }
    return -1;
}

bool checkInStateVector(vector<string> & E, string & e) {
    for(int i = 0; i < E.size(); ++i) {
        if(E[i] == e) return true;
    }
    return false;
}

vector<string> & closure(vector<string> &curState, vector<string> & dstState, vector<Part> & nfa) {

    vector<bool> stateFlag;
    for(int i = 0; i < curState.size(); ++i) {
        if(!checkInStateVector(dstState, curState[i])) {
            dstState.push_back(curState[i]);
            stateFlag.push_back(false);
        }
    }

    while(checkFlag(stateFlag) != -1) {
        int m = checkFlag(stateFlag);
        stateFlag[m] = true;
        for(int i = 0; i < nfa.size(); ++i) {
            if(dstState[m] == nfa[i].src && nfa[i].edge == "&") {
                if(!checkInStateVector(dstState, nfa[i].dst)) {
                    dstState.push_back(nfa[i].dst);
                    stateFlag.push_back(false);
                }
            }
        }
    }

    if(!dstState.empty()) return dstState;

    return curState;
}

vector<string> move(vector<string> & cur, string & input, vector<Part> & nfa) {
    vector<string>  dst;
    for(int i = 0; i < cur.size(); ++i) {
        for(int j = 0; j < nfa.size(); ++j) {
            if(cur[i] == nfa[j].src && nfa[j].edge == input) {
                if(!checkInStateVector(dst, nfa[j].dst)) {
                    dst.push_back(nfa[j].dst);
                }
            }
        }
    }
    return dst;
}

bool equalString(vector<string> & a, vector<string> & b) {
    if(a.size() != b.size()) return false;
    for(int i = 0; i < a.size(); ++i) {
        if(!checkInE(b, a[i])) {
            return false;
        }
    }
    return true;
}

bool checkInSet(vector<vector<string>> & set, vector<string> & e) {
    for(int i = 0; i < set.size(); ++i) {
        if(equalString(set[i], e)) return true;
    }
    return false;
}

vector<ObsPart>& constructObs(vector<ObsPart>& obs, ObsPart &temp) {
    for(int i = 0; i < obs.size(); ++i) {
        if(equalString(obs[i].src, temp.src) && equalString(obs[i].dst, temp.dst) && obs[i].edge == temp.edge) {
            return obs;
        }
    }
    obs.push_back(temp);
    return obs;
}

vector<ObsPart>& constructObs(vector<ObsPart>& obs, vector<Part> & nfa, vector<string> & Euo) {

    // 1.将nfa中不可观测的部分转换为 &
    for(int i = 0; i < Euo.size(); ++i) {
        for(int j = 0; j < nfa.size(); ++j) {
            if(nfa[j].edge == Euo[i]) {
                nfa[j].edge = "&";
            }
        }
    }
    // 2.构造集合E
    vector<string> E;
    for(int i = 0; i < nfa.size(); ++i) {
        if(!checkInE(E, nfa[i].edge) && nfa[i].edge != "&") {
            E.push_back(nfa[i].edge);
        }
    }

    // 3. 构造obs
    // 初始状态
    vector<string> initial;
    initial.push_back(nfa[0].src);
    vector<string> start;
    start = closure(initial, start, nfa);

    vector<bool> flag;
    flag.push_back(false);
    vector<vector<string>> set;
    set.push_back(start);
    while(checkFlag(flag) != -1) {
        int m = checkFlag(flag);
        flag[m] = true;
        for(int i = 0; i < E.size(); ++i) {
            vector<string> temp;
            vector<string> u = move(set[m], E[i], nfa);
            temp = closure(u, temp, nfa);
            if(!checkInSet(set, temp) && temp.size() != 0) {
                set.push_back(temp);
                flag.push_back(false);
            }
        }
    }

    for(int i = 0; i < set.size(); ++i) {
        for(int j = 0; j < E.size(); ++j) {
            ObsPart temp;
            temp.src = set[i];
            temp.edge = E[j];
            vector<string> u = move(set[i], E[j], nfa);
            temp.dst = closure(u, temp.dst, nfa);
            if(temp.dst.size() != 0) {
                obs = constructObs(obs, temp);
            }
        }
    }

    cout << "this is observer with edge " << obs.size() << " edges\n";
    outputObs(obs);
    return obs;
}

vector<ObsPart> & constructAttObs(vector<ObsPart> & attObs, vector<ObsPart> & Obs, vector<string> & Eins, vector<string> & Eera) {

    for(int i = 0; i < Obs.size(); ++i) {
        attObs = constructObs(attObs, Obs[i]);
    }

    for(int i = 0; i < Eera.size(); ++i) {
        for(int j = 0; j < attObs.size(); ++j) {
            if(attObs[j].edge == Eera[i]) {
                ObsPart temp;
                temp.src = attObs[j].src;
                temp.dst = attObs[j].dst;
                temp.edge = attObs[j].edge + "-";
                attObs = constructObs(attObs, temp);
            }
        }
    }

    for(int i = 0; i < Eins.size(); ++i) {
        for (int j = 0; j < Obs.size(); ++j) {
            ObsPart temp;
            temp.src = attObs[j].src;
            temp.dst = attObs[j].src;
            temp.edge = Eins[i] + "+";
            attObs = constructObs(attObs, temp);
        }
    }

    cout << "This is attacker observer with " << attObs.size() << " edges\n";
    outputObs(attObs);
    return attObs;
}

bool checkSrcEdgeInObs(vector<ObsPart> & obs, vector<string> & src, string & s) {
    for(int i  = 0; i < obs.size(); ++i) {
        if(equalString(obs[i].src, src) && obs[i].edge == s) {
            return true;
        }
    }
    return false;
}

// 构建 Ea 字符集
void constructEa(vector<string> &Ea, vector<string> &Eo, vector<string> &Eins,
                 vector<string> &Eera) {
    for(int i = 0; i < Eo.size(); ++i) {
        Ea.push_back(Eo[i]);
    }
    for(int i = 0; i < Eins.size(); ++i) {
        Ea.push_back(Eins[i] + "+");
    }
    for(int i = 0; i < Eera.size(); ++i) {
        Ea.push_back(Eera[i] + "-");
    }
    std::sort(Ea.begin(), Ea.end());
    Ea.erase(std::unique(Ea.begin(), Ea.end()), Ea.end());
}

vector<ObsPart> & constructOprObs(vector<ObsPart> & oprObs, vector<ObsPart> & obs, vector<string> &Eo, vector<string> & Eins, vector<string> & Eera) {
    vector<string> Ea;
    constructEa(Ea, Eo, Eins, Eera);

    // 先复制
    for(int i = 0; i < obs.size(); ++i) {
       oprObs  = constructObs(oprObs, obs[i]);
    }

    for(int i = 0; i < Eins.size(); ++i) {
        for (int j = 0; j < obs.size(); ++j) {
            if(obs[j].edge == Eins[i]) {
                ObsPart temp;
                temp.src = oprObs[j].src;
                temp.dst = oprObs[j].dst;
                temp.edge = Eins[i] + "+";
                oprObs = constructObs(oprObs, temp);
            }
        }
    }

    for(int i = 0; i < Eera.size(); ++i) {
        for(int j = 0; j < obs.size(); ++j) {
            ObsPart temp;
            temp.src = oprObs[j].src;
            temp.dst = oprObs[j].src;
            temp.edge = Eera[i] + "-";
            oprObs = constructObs(oprObs, temp);
        }
    }

    for(int i = 0; i < Ea.size(); ++i) {
        for(int j = 0; j < oprObs.size(); ++j) {
            if(!checkSrcEdgeInObs(oprObs, oprObs[j].src, Ea[i])) {
                ObsPart temp;
                temp.src = oprObs[j].src;
                temp.edge = Ea[i];
                temp.dst.emplace_back("bø");
                oprObs = constructObs(oprObs, temp);
            }
        }
    }

    cout << "this is oprObs with " << oprObs.size() << " edges\n";
    outputObs(oprObs);

    return oprObs;
}


vector<bool> & addStateToStateArr(std::vector<State> &stateArr, State state, std::vector<bool> &t) {
    for(int i = 0; i < stateArr.size(); ++i) {
        if(equalString(stateArr[i].state1, state.state1) && equalString(stateArr[i].state2, state.state2)) {
            return t;
        }
    }
    stateArr.push_back(state);
    t.push_back(false);
    return t;
}

bool equalEstPart(EstPart & p1, EstPart & p2) {
    if(equalString(p1.dst1, p2.dst1) &&
       equalString(p1.dst2, p2.dst2) &&
       equalString(p1.src1, p2.src1) &&
       equalString(p1.src2, p2.src2) &&
       p1.edge == p2.edge) {
        return true;
    }
    return false;
}

bool checkPartInEst(vector<EstPart> & estimator, EstPart & part) {
    for(int i = 0; i < estimator.size(); ++i) {
        if(equalEstPart(estimator[i], part)) {
            return true;
        }
    }
    return false;
}
// 想estimator中加入新的边
vector<EstPart> & addEstPartToEst(vector<EstPart> & estimator, EstPart & part) {
    if(!checkPartInEst(estimator, part)) {
        estimator.push_back(part);
    }
    return estimator;
}

// 输出Estimator
void outputEstimator(std::vector<EstPart> &estimator) {
    std::string t = "\t\t\t\t";
    std::cout << "the estimator with " << estimator.size() << " edges is\n";
    std::cout << "" << t << "src" << t << "edge" << t << "dst" << t << std::endl;
    for (int i = 0; i < estimator.size(); ++i) {
        cout << "[";
        outputVectorString(estimator[i].src1);
        cout << "] [";
        outputVectorString(estimator[i].src2);
        cout << "]" << t << estimator[i].edge << t;
        cout << "[";
        outputVectorString(estimator[i].dst1);
        cout << "] [";
        outputVectorString(estimator[i].dst2);
        cout << "]\n";
    }
}

/**
 *  Input Two DFAs G' = (X', E', δ', x0', Xm') and G′′ = (X′′,E′′,δ′′,x0′′,Xm′′)
 *  Output: A DFA G = (X,E,δ,x0,Xm) with L(G) = L(G′) ∥ L(G′′) and Lm(G) = Lm(G′) ∥ Lm(G′′).
 * **/

/**
 *  Input Two DFAs G' = (X', E', δ', x0') and G′′ = (X′′,E′′,δ′′,x0′′)
 *  Output: A DFA G = (X,E,δ,x0) with L(G) = L(G′) ∥ L(G′′) and Lm(G) = Lm(G′) ∥ Lm(G′′).
 * **/
vector<EstPart> & constructEstimator(vector<EstPart> & estimator, vector<ObsPart> & attObs, vector<ObsPart> & oprObs) {

    /**
    *  0. generate E1 and E2
    * **/
    vector<string> E1;
    for(int i = 0; i < oprObs.size(); ++i) {
        if(!checkInE(E1, oprObs[i].edge)) {
            E1.push_back(oprObs[i].edge);
        }
    }

    vector<string> E2;
    for(int i = 0; i < attObs.size(); ++i) {
        if(!checkInE(E2, attObs[i].edge)) {
            E2.push_back(attObs[i].edge);
        }
    }
    /**
     *  1. Let E = E′ ∪ E′′
     * **/
    vector<string> E;
    for(int i = 0; i < attObs.size(); ++i) {
        if(!checkInE(E, attObs[i].edge)) {
            E.push_back(attObs[i].edge);
        }
    }
    for(int i = 0; i < oprObs.size(); ++i) {
        if(!checkInE(E, oprObs[i].edge)) {
            E.push_back(oprObs[i].edge);
        }
    }

    /**
     *  2. Let x0 = (x0', x0'')
     * **/
    State x0;
    x0.state1 = oprObs[0].src;
    x0.state2 = attObs[0].src;

    /**
     *  3. Let X = ø and Xnew = {x0}
     *  (At the end of the algorithm X ⊆ X′×X′′
     *  will contain all states of G, while the set Xnew contains at each step the states of G still to be explored.)
     * **/

//    State x;
//    x.state1.push_back("ø");
//    x.state2.push_back("ø");

    vector<State> stateArr;
    stateArr.push_back(x0);

    /**
     *  4. Select a state x = (x', x'') ∈ Xnew
     *      (a) For all e ∈ E:
     *          i. Let δ(x, e) = (x̄', x'') if e ∈ E′ \ E′′, δ′(x′,e) = x̄'
     *                         = (x', x̄'') if e ∈ E′ \ E′′， δ''(x'', e) = x̄''
     *                         = (x̄', x̄'') if e ∈ E' ∩ E'', δ′(x′,e) = x̄', δ''(x'', e) = x̄''
     *                         undefined otherwise
     *         ii. if state x̄ = δ(x, e) is defined and x̄ does not belongs to X ∪ Xnew then
     *         Xnew = {x̄} ∪ Xnew
     *      (b) Let X = X ∪ {x} and Xnew = Xnew \ {x}.
     * **/
     vector<bool> stateFlag;
     stateFlag.push_back(false);
     while(checkFlag(stateFlag) != -1) {
         int m = checkFlag(stateFlag);
         stateFlag[m] = true;
         for(int i = 0; i < E.size(); ++i) {
             if(checkInE(E1, E[i]) == true && checkInE(E2, E[i]) == false) {
                 EstPart tempPart;
                 for(int j = 0; j < oprObs.size(); ++j) {
                     if(equalString(oprObs[j].src, stateArr[m].state1) && oprObs[j].edge == E[i]) {
                         tempPart.src1 = stateArr[m].state1;
                         tempPart.src2 = stateArr[m].state2;
                         tempPart.edge = E[i];
                         tempPart.dst1 = oprObs[j].dst;
                         tempPart.dst2 = stateArr[m].state2;
                     }
                     if(!tempPart.dst1.empty() && !tempPart.dst2.empty() && tempPart.edge != "") {
                         estimator = addEstPartToEst(estimator, tempPart);
                         State tempState;
                         tempState.state1 = tempPart.dst1;
                         tempState.state2 = tempPart.dst2;
                         stateFlag = addStateToStateArr(stateArr, tempState, stateFlag);
                     }
                 }
             } else if(checkInE(E1, E[i]) == false && checkInE(E2,E[i]) == false) {
                 EstPart tempPart;
                 for(int j = 0; j < attObs.size(); ++j) {
                     if(equalString(attObs[j].src, stateArr[m].state2) && attObs[j].edge == E[i]) {
                         tempPart.src1 = stateArr[m].state1;
                         tempPart.src2 = stateArr[m].state2;
                         tempPart.edge = E[i];
                         tempPart.dst1 = stateArr[m].state1;
                         tempPart.dst2 = attObs[j].dst;
                         if(!tempPart.dst1.empty() && !tempPart.dst2.empty() && tempPart.edge != "") {
                             estimator = addEstPartToEst(estimator, tempPart);
                             State tempState;
                             tempState.state1 = tempPart.dst1;
                             tempState.state2 = tempPart.dst2;
                             addStateToStateArr(stateArr, tempState, stateFlag);
                         }
                     }
                 }
             } else if(checkInE(E1, E[i]) == true && checkInE(E2, E[i])) {
                 EstPart tempPart;
                 for(int j = 0; j < oprObs.size(); ++j) {
                     if(equalString(oprObs[j].src, stateArr[m].state1) && oprObs[j].edge == E[i]) {
                         tempPart.src1 = stateArr[m].state1;
                         tempPart.src2 = stateArr[m].state2;
                         tempPart.edge = E[i];
                         tempPart.dst1 = oprObs[j].dst;
                     }
                 }
                 for(int k = 0; k < attObs.size(); ++k) {
                     if(equalString(attObs[k].src, stateArr[m].state2) && attObs[k].edge == E[i]) {
                         tempPart.dst2 = attObs[k].dst;
                     }
                 }
                 if(!tempPart.dst1.empty() && !tempPart.dst2.empty() && tempPart.edge != "") {
                     estimator = addEstPartToEst(estimator, tempPart);
                     State tempState;
                     tempState.state1 = tempPart.dst1;
                     tempState.state2 = tempPart.dst2;
                     addStateToStateArr(stateArr, tempState, stateFlag);
                 }
             }
         }
     }
     outputEstimator(estimator);
     return estimator;
}

bool checkSubInString(string &s) {
    for(int i = 0; i < s.size(); ++i) {
        if(s[i] == '-') {
            return true;
        }
    }
    return false;
}

bool checkPlusInString(string &s) {
    for(int i = 0; i < s.size(); ++i) {
        if(s[i] == '+') {
            return true;
        }
    }
    return false;
}






vector<ObsPart> & estimatorToObs(vector<ObsPart> & obs, vector<EstPart> & estimator) {

    vector<ObsPart> tempObs;


    for(int i = 0; i < estimator.size(); ++i) {
        ObsPart tempPart;
        tempPart.src = estimator[i].src1;
        if(checkSubInString(estimator[i].edge)) {
            tempPart.edge = "&";
        } else if(checkPlusInString(estimator[i].edge)) {
            tempPart.edge.push_back(estimator[i].edge[0]);
        } else {
            tempPart.edge = estimator[i].edge;
        }
        tempPart.dst = estimator[i].dst1;
        constructObs(tempObs, tempPart);
    }


    vector<Part> nfa;
    for(int i = 0; i < tempObs[0].src.size(); ++i) {

        nfa = constructNFA(nfa, tempObs[0].src[0], "&", tempObs[0].src[i]);
    }
    for(int i = 0; i < tempObs.size(); ++i) {
        for(int j = 0; j < tempObs[i].src.size(); ++j) {
            for(int k = 0; k < tempObs[i].dst.size(); ++k) {
                Part temp;
                temp.src = tempObs[i].src[j];
                temp.edge = tempObs[i].edge;
                temp.dst = tempObs[i].dst[k];
                nfa = constructNFA(nfa, temp.src, temp.edge, temp.dst);
            }
        }
    }

    cout << "func > estimatorToObs: \n";
    vector<string> Euo;
    constructObs(obs, nfa, Euo);

    return obs;

}

vector<string> stateEstimateByEstObs(vector<ObsPart> & estObs, string & s) {
    int length = s.length();
    vector<string> initial = estObs[0].src;
    int count = 0;
    for(int i = 0; i < s.size(); ++i) {
        for(int j = 0; j < estObs.size(); ++j) {
            if (isalpha(s[i]) && ( i + 1 >= length || isalpha(s[i + 1]))) {
                if(equalString(estObs[j].src, initial) && s[i] == estObs[j].edge[0] && estObs[j].edge.size() == 1) {
                    initial = estObs[j].dst;
                    count = i;
                    break;
                }
            } else if(isalpha(s[i]) && !isalpha(s[i + 1])) {
                if(estObs[j].edge.size() == 2 && equalString(initial, estObs[j].src) && s[i] == estObs[j].edge[0] && s[i + 1] == estObs[j].edge[1]) {
                    ++i;
                    initial = estObs[j].dst;
                    count = i;
                    break;
                }
            }
        }
    }
    if(count == 0) {
        cout << "invalid input for " << s << "\n";
        return initial;
    } else if(count > 0 && count < s.size() - 1) {
            cout << "func > stateEstimateByEstObs terminate after\n";
            for(int k = 0; k <= count; ++k) {
                cout << s[k];
            }
            cout << "\nreach state";
            outputVectorString(initial);
            return initial;
    } else {
        cout << "the state reached by word " << s << " is\n";
        outputVectorString(initial);
        return initial;
    }
}

int main() {

    clock_t start, end;
    /**
     * 自己的例子 example 1
     */
    start = clock();
    cout << "please input the file name" << endl;
    string fileName;
    cin >> fileName;
    getchar();

//    std::string example1 = "example1/example1.txt";
//    std::string example1 = "random/random1000.txt";
    std::string eera1 = "example1/eera.txt";
    std::string eins1 = "example1/eins.txt";
    std::string euo1 = "example1/euo.txt";

//    std::string example1 = "example0/example0.txt";
//    std::string eera1 = "example0/eera.txt";
//    std::string eins1 = "example0/eins.txt";
//    std::string euo1 = "example0/euo.txt";
    // nfa
    std::vector<Part> nfa;

    // 字符集
    vector<string> Eera;
    vector<string> Eins;
    vector<string> Euo;
    vector<string> Eo;
    vector<string> E;

    Eera= inputEFromfile(Eera, eera1);
    Eins = inputEFromfile( Eins, eins1);
    Euo = inputEFromfile(Euo, euo1);
    nfa = constructPlantG(nfa, fileName);











    E = getEFromNFA(nfa, E);


    Eo = constructEoFromEEuo(Eo, E, Euo);

    // obs
    vector<ObsPart> obs;
    obs = constructObs(obs, nfa, Euo);

    // attacker observer
    vector<ObsPart> attObs;
    attObs = constructAttObs(attObs, obs, Eins, Eera);

    // operator observer
    vector<ObsPart> oprObs;
    oprObs = constructOprObs(oprObs, obs, Eo, Eins, Eera);

    // estimator
    vector<EstPart> estimator;
    estimator = constructEstimator(estimator, oprObs, attObs);

    // obs of estimator
    vector<ObsPart> estimatorObs;
    estimatorObs = estimatorToObs(estimatorObs, estimator);

    // test input word
    string word = "acc";
    vector<string> finalState;
    finalState = stateEstimateByEstObs(estimatorObs, word);
    end = clock();

//    // 要删的
//    cout << "\nplease input the file name" << endl;
//    cin >> fileName;
//    getchar();
//    // 要删的
    printf("The time consumed is: %f\n", (double)(end - start) / CLK_TCK);
    return 0;
}
