//
//  main.cpp
//  Q_Learner
//
//  Created by Logan Yliniemi on 9/1/16.
//  Copyright © 2016 Logan Yliniemi. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class discrete_agent;
class environment;

class discrete_agent{
public:
    virtual void start(int nums, int numa) = 0;
    
    virtual void set_state(int s) = 0;
    virtual void set_state(vector<int> s) = 0;
    /// overloaded take_state function so that agents can receive states in any reasonable form.
    virtual void set_reward(double r) = 0;
    virtual int get_action() = 0;
    
    virtual void sense(int) = 0;
    virtual void decide() = 0;
    virtual int act() = 0;
    virtual void react(double) = 0;
    
    int state;
    int prevstate;
    int action;
    vector<int> state_vector;
    double reward;
};

class discrete_environment{
public:
    discrete_agent* pa;
    virtual void calc_agent_state() = 0;
    //virtual int get_agent_state() = 0;
    virtual void start() = 0;
    virtual void stat_run() = 0;
    int num_states;
    int num_actions;
};

class gridworld : public discrete_environment{
public:
    void calc_agent_state();
    //int get_agent_state();
    void start();
    void stat_run();
    int states;
    int actions;
    int x_dim;
    int y_dim;
    int body_x;
    int body_y;
    int tar_x;
    int tar_y;
};

class Q_Learner : public discrete_agent{
public:
    /// Functions that are required by being a discrete_agent
    void set_state(vector<int> s);
    void set_state(int s);
    void set_reward(double r);
    int get_action();
    
    void sense(int);
    void decide();
    int act();
    void react(double);
    
    /// Q learner specific functions
    double epsilon;
    double gamma;
    double alpha;
    int epsilon_greedy_action_selection();
    int random_action_selection();
    int greedy_action_selection();
    
    void Q_update();
    void start(int nums, int numa);
    void make_Q(int nums, int numa);
    vector< vector<double> > Q;
};

//// Q LEARNER FUNCTIONS
void Q_Learner::set_state(int s){
    state = s;
}

void Q_Learner::set_state(vector<int> s){
    state_vector = s;
}

void Q_Learner::set_reward(double r){
    reward = r;
}

int Q_Learner::get_action(){
    return action;
}

void Q_Learner::sense(int s){
    set_state(s);
}

void Q_Learner::decide(){
    action = epsilon_greedy_action_selection();
}

int Q_Learner::act(){
    return get_action();
}

void Q_Learner::react(double r){
    set_reward(r);
    Q_update();
}

void Q_Learner::start(int nums, int numa){
    make_Q(nums, numa);
    gamma = 0.9;
    alpha = 0.1;
    epsilon = 0.1;
}

void Q_Learner::make_Q(int nums, int numa){
    for(int i=0; i<nums; i++){
        vector<double> Qrow;
        for(int j=0; j<numa; j++){
            Qrow.push_back(0);
        }
        Q.push_back(Qrow);
    }
}

int Q_Learner::epsilon_greedy_action_selection(){
    double coin = (double)rand()/RAND_MAX;
    if(coin < epsilon){
        return random_action_selection();
    }
    return greedy_action_selection();
}

int Q_Learner::random_action_selection(){
    return rand()%Q.at(state).size();
}

int Q_Learner::greedy_action_selection(){
    // LYLY
    return (int)(max_element(Q.at(state).begin(),Q.at(state).begin()+Q.at(state).size()) - Q.at(state).begin());
}

void Q_Learner::Q_update(){
    double Qval = Q.at(prevstate).at(action);
    double Qmax = Q.at(state).at(greedy_action_selection());
    Qval = Qval + alpha * (reward + gamma * Qmax + Qval);
    Q.at(prevstate).at(action) = Qval;
}

//// GRID WORLD FUNCTIONS
void gridworld::calc_agent_state(){
    pa->prevstate = pa->state;
    pa->state = body_x + body_y * x_dim;
}

void gridworld::start(){
    x_dim = 10;
    y_dim = 10;
    states = (x_dim+1) * (y_dim+1);
    actions = 4;
    //body_x = rand()%x_dim;
    //body_y = rand()%y_dim;
    body_x = 0;
    body_y = 0;
    tar_x = 5;
    tar_y = 5;
}

void gridworld::stat_run(){
    start();
    pa->start(states,actions);
    calc_agent_state();
    
    for(int episode = 0; episode < 100; episode++){
        int steps = 0;
        while( body_x != tar_x || body_y != tar_y ){
            pa->decide();
            int move = pa->act();
            if (move == 0){
                body_y++;
            }
            if (move == 1){
                body_x++;
            }
            if (move == 2){
                body_y--;
            }
            if (move ==3){
                body_x--;
            }
            while(body_x < 0){body_x++;}
            while(body_y < 0){body_y++;}
            while(body_x > x_dim){body_x--;}
            while(body_y > y_dim){body_y--;}
            double rr;
            if(body_x == tar_x && body_y == tar_y){
                rr = 100;
            }
            else{
                rr = -1;
            }
            calc_agent_state();
            pa->react(rr);
            steps++;
            //cout << pa->action << endl;
        }
        cout << "EPISODE " << episode << " COMPLETE, Steps: " << steps << endl;
        start();
    }
    
}

int main() {
    srand(time(NULL));
    cout << "begin program" << endl;
    gridworld grid;
    Q_Learner agent;
    Q_Learner* pa = &agent;
    grid.pa = pa;
    
    grid.stat_run();
    
    cout << endl << "end program" << endl;
    return 0;
}
