#include "fsm_operator.hpp"

#include<iostream>
#include<vector>
#include<string>

uint32_t eval_func(int* arg) {
    return *arg;
}

void action_func(std::string* action) {
    std::cout << *action << "\n";
}


int main(int argc, char* argv[]) {

    sblib::FSMOperator<int, std::string> test;
    
    test.create_node("cut grass");
    test.set_eval_function(&eval_func);
    test.set_act_function(&action_func);
    
    test.create_node("eat breakfast");
    test.set_eval_function(&eval_func);
    test.set_act_function(&action_func);

    std::vector<std::string> child_names;
    child_names.push_back("cut grass");
    child_names.push_back("eat breakfast");

    test.create_node("wake up", &child_names);
    test.set_start_point();
    test.set_eval_function(&eval_func);
    test.set_act_function(&action_func);

    std::cout << test.print_FSM();

    std::string name = "wake up";

    test.start_operation(&name);

    int direction = 1;
    name = "eat breakfast";

    test.step(&direction, &name);
    
    return 0;
}