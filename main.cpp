#include <bits/stdc++.h>


using namespace std;
using namespace std::chrono;

#define pii pair<int,int>
struct vessel_info{
    int index;
    int size;
    int arrival_time;
    int processing_time;
    int weight;
    int mooring_time;
    int position;
    int break_pos;
    vessel_info(int index, int size, int arrival_time, int processing_time, int weight): 
            index(index), size(size), arrival_time(arrival_time), processing_time(processing_time), weight(weight) {};
};

vector<vessel_info> vessel;

long long calCost(){
    long long sum = 0;
    for (int i = 0; i < vessel.size(); ++i){
        sum += vessel[i].weight*(vessel[i].mooring_time - vessel[i].arrival_time);
    }
    return sum;
}

bool cmp1(vessel_info &V1, vessel_info &V2){
    if (V1.arrival_time < V2.arrival_time) return 1;
    else if (V1.arrival_time == V2.arrival_time) {
        return V1.weight > V2.weight;
    }
    else return 0;
}

bool cmp2(vessel_info &V1, vessel_info &V2){
    if (V1.index < V2.index) return 1;
    else return 0;
}

bool cmp3(vessel_info &V1, vessel_info &V2){
    if (V1.break_pos == V2.break_pos)
        return V1.mooring_time > V2.mooring_time;
    return V1.break_pos < V2.break_pos;
}

bool cmp4(vessel_info &V1, vessel_info &V2){
    if (V1.mooring_time == V2.mooring_time)
        return V1.weight > V2.weight;
    return V1.mooring_time > V2.mooring_time;
}

int berth_length;
int m,n;
set<int> berth_break;
vector<int> br(1,0);
int max_time = 0;

//=============================
vector<vector<pii>> Class1;
vector<vector<pii>> Class1new;
vector<pii> feasible_solution;
vector<double> feasible_prop;
vector<vector<pii>> Class3;
vector<vector<pii>> Class3_generator;
vector <double> cul_sum;
vector <int> feasible_direction;
int avt;

void deleteVector() {
    Class1.clear();
    Class1.resize(4);
    //Class1new.clear();
    Class1new.resize(4);
    feasible_solution.clear();
    feasible_prop.clear();
    Class3.clear();
    Class3.resize(4);
    Class3_generator.clear(); 
    Class3_generator.resize(4);
    cul_sum.clear();
    feasible_direction.clear();
    avt = 0;
}

//=============================

//=============================
int di[4] = {0,-1,-1,0};
int dj[4] = {-1,-1,0,0};
int space[5001][501];
map <string,pair<int,int>> dict;
void create_dict() {
    dict["1000"] = {1,0};
    dict["0100"] = {1,1};
    dict["0010"] = {1,2};
    dict["0001"] = {1,3};
    dict["1110"] = {3,3};
    dict["1101"] = {3,2};
    dict["1011"] = {3,1};
    dict["0111"] = {3,0};
    dict["0101"] = {2,-1};
    dict["1010"] = {2,-2};
}
//===============================
//tim giao diem
bool inBorder(int i,int j) {
    if (i < avt || i > max_time) return false;
    if (j < 0 || j > berth_length - 1) return false;
    return true;
}

pair<int,int> intersection(pair<int,int>A, pair<int,int>B, pair<int,int>C, pair<int,int>D){
    pair<int, int> res = {-1, -1};
    if (A.second < C.second || A.second > D.second) return res;
    if (C.first < B.first || C.first > A.first) return res;
    res.first = C.first;
    res.second = A.second;
    return res;
}

string identifyClass(int i, int j){
    string res = "";
    for (int k = 0; k < 4; ++k) {
        int new_i = i + di[k];
        int new_j = j + dj[k];
        if (inBorder(new_i,new_j)) {
            if (space[new_i][new_j] == 0) {
                res += '1';
            } else res += '0';
        }
        else res += '0';
    }
    return res;
}

void initLocation(){
    memset(space,0,sizeof(space));
}

void generatorPoint() {
    for (int i = 0; i < 4; ++i) {
        for (auto point: Class3[i]) {
            int x = point.first;
            int y = point.second;
            if (i == 0){
                while (x >= 0 && space[x][y] == 0){
                    x--;
                }
                Class1[3].push_back({x + 1, y});
                Class3_generator[0].push_back({x + 1, y});
            }
            else if (i == 1){
                int flag = 1;
                while (y < berth_length && space[x][y] == 0){
                    y++;
                    if (berth_break.find(y) != berth_break.end()) {flag = 0; break;}
                }
                Class1[0].push_back({x, y - flag});
                Class3_generator[1].push_back({x, y - flag});
            }
            else if (i == 2){
                int flag = 1;

                while (y < berth_length && space[x][y] == 0){
                    y++;
                    
                    if (berth_break.find(y) != berth_break.end()) {flag = 0; break;}
                }
                Class1[3].push_back({x, y - flag});
                
                Class3_generator[2].push_back({x, y - flag});
            }
            else {
                while (x >= 0 && space[x][y] == 0){
                    x--;
                }
                Class1[0].push_back({x + 1, y});
                Class3_generator[3].push_back({x + 1, y});
            }
        }
        
    }
}

void assignClass(string res, int i, int j) {
    if (dict.find(res) != dict.end()) {
        pii val = dict[res];
        int cls = val.first;
        int idx = val.second;
        if (cls == 2) {
            if (idx == -1) {
                Class1[1].push_back({i,j});
                Class1[3].push_back({i,j});
            }
            else {
                Class1[0].push_back({i,j});
                Class1[2].push_back({i,j});
            }
        }        
        if (cls == 1) Class1[idx].push_back({i,j});
        else {
            Class3[idx].push_back({i,j});
            if (berth_break.find(j) != berth_break.end()){
                if (res == "0111") Class1[1].push_back({i, j});
                else if (res == "1011") Class1[0].push_back({i, j});
                else if (res == "1101") Class1[3].push_back({i, j});
                else Class1[2].push_back({i, j});
            }
        }
    }
    if (berth_break.find(j) != berth_break.end()){
        if (res == "1001" || res == "1011"){
            Class1[0].push_back({i,j});
        }
        if (res == "1001" || res == "1101"){
            Class1[3].push_back({i,j});
        }
        if (res == "0110"){
            Class1[1].push_back({i,j});
            Class1[2].push_back({i,j});
        }
    }
}

void sortVessel(int opt) {
    if (opt == 1) sort(vessel.begin(), vessel.end(), cmp1);
    else sort(vessel.begin(), vessel.end(), cmp2);
}

int toInt(string str){
    int k = str.size();
    int res = 0;
    int h;
    for (int i = 0 ; i < k; ++i){
        h = str[i] - '0';
        res = res * 10 + h;
    }
    return res;
}

void decode(string str){
    string rawstr = "";
    vector<int> raw_data;
    int i = 0;
    for (int k = 0; k < 4; k++){
        while ('0' <= str[i] && str[i] <= '9') rawstr += str[i++];
        raw_data.push_back(toInt(rawstr));   
        while (!('0' <= str[i] && str[i] <= '9') && str[i] != '\0') ++i;
        rawstr = "";
    }
    if (str[i] == '\0') raw_data.push_back(1);
    else {
        rawstr = "";
        while (str[i] != '\0') rawstr += str[i++];
        raw_data.push_back(toInt(rawstr));
    }
    vessel_info V(raw_data[0], raw_data[1], raw_data[2], raw_data[3], raw_data[4]);
    vessel.push_back(V);
}

void read(string fileName){
    string data;
    ifstream ifs;
    ifs.open(fileName);
    // read berth length
    getline(ifs, data);
    ifs >> berth_length;
    getline(ifs, data);
    getline(ifs,data);
    //read berth break
    getline(ifs, data);
    getline(ifs,data);
    while (data != ""){
        berth_break.insert(toInt(data));
        br.push_back(toInt(data));
        getline(ifs,data);
    }
    getline(ifs, data);
    while (getline(ifs, data)){
        decode(data);
    }
    br.push_back(berth_length);
}

void Class3Intesection(){
    for (int i = 0; i < Class3[3].size(); ++i)
        for (int j = 0; j < Class3[1].size(); ++j){
            pii interPoint = intersection(Class3[3][i], Class3_generator[3][i], Class3[1][j], Class3_generator[1][j]);
            if (interPoint.first == -1) continue; else Class1[0].push_back(interPoint);
        }
    for (int i = 0; i < Class3[0].size(); ++i)
        for (int j = 0; j < Class3[2].size(); ++j){
            pii interPoint = intersection(Class3[0][i], Class3_generator[0][i], Class3[2][j], Class3_generator[2][j]);
            if (interPoint.first == -1) continue; else Class1[3].push_back(interPoint);
        }
}



bool checkLocation(pii point, int quadrant, int vessel_size, int time) {

    int x = point.first;
    int y = point.second;
    bool flag = true;
    if (quadrant == 1) {
        for (int i = x; i < x + time; ++i) {
            if (y - vessel_size < 0) return false;
            for (int j = y - 1; j >= y - vessel_size; j--) {
                if (berth_break.find(j) != berth_break.end()) {
                    if (y - vessel_size <= j) {
                        flag = false;
                        break;
                    }
                }
                if (space[i][j] > 0) {
                    flag = false;
                    break;
                }
            }
        }
    }
    else if (quadrant == 2) {
        for (int i = x; i > x - time; --i) {
            for (int j = y; j > y - vessel_size; j--) {
                if (berth_break.find(j) != berth_break.end()) {
                    if (y - vessel_size < j) {
                        flag = false;
                        break;
                    }
                }
                if (space[i][j] > 0) {
                    flag = false;
                    break;
                }
            }
        }
    }
    else if (quadrant == 3) {
        for (int i = x; i > x - time; i--) {
            if (y + vessel_size > berth_length) return false;
            for (int j = y + 1; j <= y + vessel_size; ++j) {
                if (berth_break.find(j) != berth_break.end()) {
                    if (y + vessel_size > j) {
                        flag = false;
                        break;
                    }
                }
                if (space[i][j] > 0) {
                    flag = false;
                    break;
                }
            }
        }
    }
    else if (quadrant == 4) {
        for (int i = x; i < x + time; ++i) {
            if (y + vessel_size > berth_length) return false;
            for (int j = y + 1; j <= y + vessel_size; ++j) {
                if (berth_break.find(j) != berth_break.end()) {
                    if (y + vessel_size > j) {
                        flag = false;
                        break;
                    }
                }
                if (space[i][j] > 0) {
                    flag = false;
                    break;
                }
            }
        }
    }
    return flag;
}


int generateCost(vessel_info V){
    int sum = 0;
    for (int i = 0; i < feasible_solution.size(); ++i){
        sum += V.weight*(feasible_solution[i].first-V.arrival_time);
    }
    if (sum == 0) return 0;
    for (int i = 0; i < feasible_solution.size(); ++i){
        int k = V.weight*(feasible_solution[i].first-V.arrival_time);
        if (i == 0) cul_sum.push_back(k * 1.0 / sum);
        else cul_sum.push_back(cul_sum[i - 1] + k * 1.0/sum);
    }
    srand(time(0));
    int num = 5;
    double r;
    while(num--) {
        r = (rand() % 9974) * 1.0 / 9974;
    }
    int idx = -1;
    for (int i = 0; i < cul_sum.size(); ++i) {
        if (cul_sum[i] > r) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        idx = cul_sum.size() - 1;
    }
    return idx;
}


int generateCost2(vessel_info V){
    int idx = 0, min = -1, k;
    for (int i = 0; i < feasible_solution.size(); ++i){
        k = V.weight*(feasible_solution[i].first-V.arrival_time);
        if (k < min){ min = k; idx = i;}
    }
    return idx;
}

int get_differ(int idx, int pos, int size) {
    int direction = feasible_direction[idx];
    int idx_up = upper_bound(br.begin(),br.end(),pos) - br.begin();
    int idx_lb = idx_up - 1;
    
    if (direction == 1) { //up
        if (br[idx_lb] == pos) {
            idx_up -= 1;
            idx_lb -= 1;
        }
    }
    return br[idx_up] - br[idx_lb] - size;
}

int generateCost3(vessel_info V){
    int idx = 0, min = -1, k, dif;

    min = V.weight*(feasible_solution[0].first-V.arrival_time);
    
    dif = get_differ(0,feasible_solution[0].second,V.size);
    
    for (int i = 1; i < feasible_solution.size(); ++i){
        k = V.weight*(feasible_solution[i].first-V.arrival_time);
        if (k < min){ min = k; idx = i; dif = get_differ(i,feasible_solution[i].second,V.size);}
        else if (k == min){
            int k_diff = get_differ(i,feasible_solution[i].second,V.size);
            if (k_diff < dif) {dif = k_diff, idx = i;}
        }
    }
     
    return idx;
}

void fillColor(int idx,struct vessel_info V) {
    int direct = feasible_direction[idx];
    int vessel_size = V.size;
    int time_ = V.processing_time;
    int x = feasible_solution[idx].first;
    int y = feasible_solution[idx].second;
    
    if (direct == 1) {
        for (int i = x; i < x + time_; ++i) {
            for (int j = y - 1; j >= y - vessel_size; j--) {
                space[i][j] = V.index;       
            }
        }
        if (x + time_ > max_time) max_time = x + time_;
    }
    else if (direct == 2){
        for (int i = x; i > x - time_; --i) {
            for (int j = y - 1; j >= y - vessel_size; j--) {
                space[i][j] = V.index;       
            }
        }
        if (time_ >= max_time) max_time = time_;
    }
    else if (direct == 3){
        if (time_ >= max_time) max_time = time_;
        for (int i = x; i > x - time_; --i) {
            for (int j = y; j <= y + vessel_size; ++j) {
                space[i][j] = V.index;
            }
        }
    }
    else {
        for (int i = x; i < x + time_; ++i) {
            for (int j = y; j <= y + vessel_size; ++j) {
                space[i][j] = V.index;
            }
        }
        if (x + time_ > max_time) max_time = x + time_;
    }
    
}

void printPoint(vector<vector<pii>> a){
    for (int i = 0 ; i < 4; ++i)
        {cout << i << "----------------" << endl;
        for (int j = 0; j < a[i].size(); ++j)
            cout << a[i][j].first << ' ' << a[i][j].second << endl;
            cout << "---------------------" << endl;
        }
}

void printMap(){
    for (int i = 0; i < max_time + 1; ++i){
        for (int j = 0; j < berth_length; ++j) cout << space[i][j] << ' ';
        cout << endl;
    }
}

void printMap1(){
    for (int j = 0; j < berth_length; ++j){
        for (int i = 0; i < max_time + 1; ++i)
            cout << space[i][j] << ' ';
        cout << endl;
    }
}

void process() {
    initLocation();
    max_time = vessel[0].arrival_time;
    for (int k = 0; k < vessel.size(); ++k) {
        // cout << max_time << endl;
        // cout << "process the vessel: " << k + 1 << endl;
        deleteVector();
        // cout << "==================================" << endl;
        avt = vessel[k].arrival_time;
        if (max_time < avt) max_time = avt;
        for (int i = avt; i <= max_time; ++i) {
            for (int j = 0; j <= berth_length; ++j) {
                string res = identifyClass(i,j);
                assignClass(res,i,j);
            }
        } 
        generatorPoint();
        Class3Intesection();

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < Class1[i].size(); ++j) {
                bool check_loc = checkLocation(Class1[i][j],i+1,vessel[k].size,vessel[k].processing_time);

                if (check_loc) {
                    feasible_solution.push_back(Class1[i][j]);
                    feasible_direction.push_back(i+1);
                } 
            } 
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < Class1new[i].size(); ++j) {
                bool check_loc = checkLocation(Class1new[i][j],i+1,vessel[k].size,vessel[k].processing_time);
                if (check_loc) {
                    feasible_solution.push_back(Class1new[i][j]);
                    feasible_direction.push_back(i+1);
                } 
            } 
        }

        int idx = generateCost(vessel[k]);
        
        
        fillColor(idx,vessel[k]);
       
        if (feasible_direction[idx] == 1) {
            vessel[k].position = feasible_solution[idx].second - vessel[k].size;
            vessel[k].mooring_time = max(feasible_solution[idx].first,vessel[k].arrival_time);
        }
        else if (feasible_direction[idx] == 2){
            vessel[k].position = feasible_solution[idx].second - vessel[k].size;
            vessel[k].mooring_time = max(feasible_solution[idx].first - vessel[k].processing_time,vessel[k].arrival_time);
        }
        else if (feasible_direction[idx] == 3){
            vessel[k].position = feasible_solution[idx].second - vessel[k].size;
            vessel[k].mooring_time = max(feasible_solution[idx].first - vessel[k].processing_time,vessel[k].arrival_time);
        }
        else {
            vessel[k].mooring_time = max(feasible_solution[idx].first,vessel[k].arrival_time);
            vessel[k].position = feasible_solution[idx].second;
        } 
        //printMap1();
        int pos_check = lower_bound(br.begin(),br.end(),vessel[k].position) - br.begin();
        if (vessel[k].position < br[pos_check]) vessel[k].break_pos = pos_check - 1;
        else vessel[k].break_pos = pos_check;
    }
}

void write(string fileName){
    ofstream ofs;
    ofs.open(fileName);
    string open = "% Vessel index, mooring time $u_i$, starting berth position occupied $v_i$\n";
    ofs << open;
    for (int i = 0; i < vessel.size(); ++i){
        ofs << vessel[i].index << ' ' << vessel[i].mooring_time << ' ' << vessel[i].position << endl;
    }
    cout << "Cost: " << calCost() << endl;
    ofs.close();
}

pair<int,int> checkSwapVessel(vessel_info V1, vessel_info V2) {
    int tempCost = INT_MAX;
    pair<int,int> save{-1,-1};
    int pLeft = V1.mooring_time;
    int pUp = V1.position;
    
    int pRight = pLeft + V1.processing_time;
    int pBottom = V1.position + V1.size;
    
    int ptRight = pRight;
    int ptBottom = pBottom;
    
    while (pLeft > 0 && (space[pLeft][V1.position] == V1.index || space[pLeft][V1.position] == 0)) {
        pLeft--;
    }
    
    while (pUp > 0 && (space[V1.mooring_time][pUp] == V1.index || space[V1.mooring_time][pUp] == 0)) {
        if (berth_break.find(pUp) != berth_break.end()) break;
        pUp--;
    }
    
    while (ptRight < max_time && (space[ptRight][pBottom] == V1.index || space[ptRight][pBottom] == 0)) {
        ptRight++;
    }
    
    while (ptBottom < berth_length && (space[pRight][ptBottom] == V1.index || space[pRight][ptBottom] == 0)) {
        if (berth_break.find(ptBottom) != berth_break.end()) break;
        ptBottom++;
    }
    
    pRight = ptRight;
    pBottom = ptBottom;
    if (pLeft < V2.arrival_time) pLeft = V2.arrival_time;
    if (pRight - V2.processing_time < 0) return save;
    for (int i = pLeft; i <= V1.mooring_time; ++i) {
        for (int j = pUp; j <= max(pBottom - V2.size,0); ++j) {
            bool flag = true;
            for (int k = 0; k < V2.processing_time; k++) {
                for (int t = 0; t <= V2.size; t++) {
                    if (space[i + k][j + t] != V1.index && space[i + k][j + t] != 0) {
                        flag = false;
                        break;
                    } 
                }   
                if (flag == false) break;
            }
            if (flag == true) {
                int costNew = i - V2.arrival_time;
                if (costNew < tempCost) {
                    tempCost = costNew;
                    save = {i,j};
                }
                break;
            }
        } 
    }
    return save;
}

int checkSwapPhase(vessel_info V1,vessel_info V2,pair<int,int>& V1_save, pair<int,int>& V2_save) {
    
    V2_save = checkSwapVessel(V1,V2);
    V1_save = checkSwapVessel(V2,V1);

    if (V2_save.first < 0 || V2_save.second < 0 || V1_save.first < 0 || V1_save.second < 0) return -1;
    return (V2_save.first - V2.arrival_time) * V2.weight + (V1_save.first - V1.arrival_time) * V1.weight;
}

void swapPhase(vessel_info& V1,vessel_info& V2, pair<int,int> V1_new, pair<int,int> V2_new) {
    for (int i = V1.mooring_time; i < V1.mooring_time + V1.processing_time; ++i) {
        for (int j = V1.position; j <= V1.position + V1.size; ++j) {
            space[i][j] = 0;
        }
    }   

    for (int i = V2.mooring_time; i < V2.mooring_time + V2.processing_time; ++i) {
        for (int j = V2.position; j <= V2.position + V2.size; ++j) {
            space[i][j] = 0;
        }
    }   

    V1.mooring_time = V1_new.first;
    V2.mooring_time = V2_new.first;
    V1.position = V1_new.second;
    V2.position = V2_new.second;

    
    for (int i = V1.mooring_time; i < V1.mooring_time + V1.processing_time; ++i) {
        for (int j = V1.position; j <= V1.position + V1.size; ++j) {
            space[i][j] = V1.index;
        }
    }   
    
    for (int i = V2.mooring_time; i < V2.mooring_time + V2.processing_time; ++i) {
        for (int j = V2.position; j <= V2.position + V2.size; ++j) {
            space[i][j] = V2.index;
        }
    }   
}

void searchPhase(){
    sort(vessel.begin(),vessel.end(),cmp3);
    for (int i = 0; i < vessel.size() - 1; ++i) {
        
        if (vessel[i].mooring_time == 0) continue;
        int j = i + 1;
        int maxReduceCost = INT_MAX;
        int locationReduceCost = -1;
        pair<int,int> V1_new;
        pair<int,int> V2_new;
        while (j < vessel.size() && vessel[i].break_pos ==  vessel[j].break_pos ) {
            if (vessel[i].weight < vessel[j].weight || vessel[i].arrival_time > vessel[j].mooring_time){
                ++j;
                continue;
            }
            if (vessel[j].mooring_time == 0) {
                break;
            }
            pair<int,int> V1_save;
            pair<int,int> V2_save;
            int r_cost = checkSwapPhase(vessel[i],vessel[j],V1_save,V2_save); 
            if (r_cost != -1) {
                if (r_cost < maxReduceCost && r_cost > 0){ 
                    maxReduceCost = r_cost;
                    locationReduceCost = j;
                    V1_new = V1_save;
                    V2_new = V2_save;
                }
            }
            j++;
        }
        if (maxReduceCost != INT_MAX) {
            swapPhase(vessel[i],vessel[locationReduceCost],V1_new,V2_new);
        }
        
    }
}

void searchPhaseAll(){
    sort(vessel.begin(),vessel.end(),cmp4);
    for (int i = 0; i < vessel.size() - 1; ++i) {
        
        if (vessel[i].mooring_time == 0) continue;
        int j = i + 1;
        int maxReduceCost = INT_MAX;
        int locationReduceCost = -1;
        pair<int,int> V1_new;
        pair<int,int> V2_new;
        while (j < vessel.size()) {
            if (vessel[i].weight < vessel[j].weight || vessel[i].arrival_time > vessel[j].mooring_time){
                ++j;
                continue;
            }
            if (vessel[j].mooring_time == 0) {
                break;
            }
            pair<int,int> V1_save;
            pair<int,int> V2_save;
            int r_cost = checkSwapPhase(vessel[i],vessel[j],V1_save,V2_save); 
            if (r_cost != -1) {
                if (r_cost < maxReduceCost && r_cost > 0){ 
                    maxReduceCost = r_cost;
                    locationReduceCost = j;
                    V1_new = V1_save;
                    V2_new = V2_save;
                }
            }
            j++;
        }
        if (maxReduceCost != INT_MAX) {
            swapPhase(vessel[i],vessel[locationReduceCost],V1_new,V2_new);
        }
        
    }
}

int main(){
    string fileIN, fileOUT;
    fileIN = "TestSet2/input5.txt";
    fileOUT = "Output2/output5.txt";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    double elapsed_seconds;
    start = std::chrono::system_clock::now();
    read(fileIN);
    sortVessel(1);
    create_dict();
    process();
    cout << "=========: " << calCost() << endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = duration_cast<microseconds> (end - start).count() * pow(10, -6);
    cout << "Time to processing: " << elapsed_seconds << "s\n";
    searchPhaseAll();
    sortVessel(2);
    write(fileOUT);
    end = std::chrono::system_clock::now();
    elapsed_seconds = duration_cast<microseconds> (end - start).count() * pow(10, -6);
    cout << "Time to processing: " << elapsed_seconds << "s\n";
    return 0;
}