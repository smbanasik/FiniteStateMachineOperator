#ifndef SB_FSM_OPERATOR_HPP
#define SB_FSM_OPERATOR_HPP
// This class is intended as a framework to produce a finite state machine operator.
// The operator begins at the node start point with a condition. The operator evaluates the condition and moves
// to the next node. Upon moving to the next node, the operator performs an action function associated with the new node.
// Conditions fed to the node may be modified in between the action function (arrival) and the nodes evaluation function (departure).

// I see uses for this when developing rule sets for procedural generation, and for
// making decisions with artificial intelligence

#include <stdint.h>
#include <vector>
#include <string>


//#define SB_FSMO_USE_NAME_HASHMAP - see USEFUL NOTE
#ifdef SB_FSMO_USE_NAME_HASHMAP
#include <unordered_map>
#endif

#include "directed_node.hpp"

namespace sblib {
// Creates and operates a finite state machine
// Creation:
// Creation is handled by creating a node, setting it as the head, then
// modifying the head node.
// I recommend planning out the FSN first, then implementing it in a bottom-up
// manner, where nodes with no children are created first.
// Evaluation and action functions are optional.
// Nodes with no evaluation function will simply go to the first child available.
// Nodes with no action function will do nothing.
// Operation:
// Run the start_operation() function,
// Then, run the step function with the necessary parameter.
// It is up to the user to handle when a FSM ends.
// USEFUL NOTE: if you #define SB_FSMO_USE_NAME_HASHMAP, you may trade some memory for look up speed via names
// Note that names MUST be unique if you use this define. I recommend making them unique anyways.
template <class Eval, class Action>
class FSMOperator {

    // The finite state machine contains a node start point
    // and provides an interface for the graph.

    // Interface:
    // - adding nodes
    // - printing the FSM
    // - calling the node functions
    // Not included:
    // - removing nodes arbitrarily (shouldn't be included in the FSM)
    // - modifying nodes arbitrarily (should be set properly)

public:

    FSMOperator() {
        head = nullptr;
        start = 0;
        is_operating = false;
    };

    const DirectedNode* create_node(const std::string& name) {

        uint32_t id = storage.size();
        storage.emplace_back(id, name);
        this->head = &storage[id];
        return this->head;
    }

    // Create a node and add it to the FSM, set it as the parent,
    // and optionally with a set of existing children. If there are duplicates
    // in the optional set, they will be filtered out.
    const DirectedNode* create_node(const std::string& name, const std::vector<uint32_t>* children) {

        uint32_t id = storage.size();
        storage.emplace_back(id, name);

        if (children != nullptr) {
            add_children(&storage[id], children);
        }
        this->head = &storage[id];
        return this->head;
    }

    // Create a node and add it to the FSM, set it as the parent,
    // and optionally with a set of existing children. If there are duplicates
    // in the optional set, they will be filtered out.
    const DirectedNode* create_node(const std::string& name, const std::vector<std::string>* children) {

        uint32_t id = storage.size();
        storage.emplace_back(id, name);

#ifdef SB_FSMO_USE_NAME_HASHMAP
        // This requires that names are unique!
        this->name_map.insert(name, id);
#endif
        if (children != nullptr) {
            add_children(&storage[id], children);
        }
        this->head = &storage[id];
        return this->head;
    }


    // Create a node and add it to the FSM, set it as the parent,
    // and optionally with a set of existing children. If there are duplicates
    // in the optional set, they will be filtered out.
    const DirectedNode* create_node(const std::string& name, const std::vector<DirectedNode*>* children) {

        uint32_t id = storage.size();
        storage.emplace_back(id, name);

        if (children != nullptr) {
            add_children(&storage[id], children);
        }
        this->head = &storage[id];
        return this->head;
    }

    // Set this node as the head, which will be used when calling functions that modify the node
    void set_parent(const DirectedNode* node) {

        uint32_t idx = node->get_id();
        this->head = &storage[idx];
    }

    // Set the start point of the FSM to the current head
    void set_start_point() {
        start = head->get_id();
    }

    // Set the evaluation function of the current head
    void set_eval_function(uint32_t(*func)(Eval*)) {
        head->set_eval_function<Eval>(func);
    }

    // Set the action function of the current head
    void set_act_function(void (*func)(Action*)) {
        head->set_act_function<Action>(func);
    }

    // Add a child to the current head
    void add_child(const DirectedNode* node) {
        head->add_child(node);
    }

    // Print the finite state machine to a string (use the dot graph format)
    std::string print_FSM() {

        std::string graph = "digraph G { \n";

        for (int i = 0; i < storage.size(); i++) {

            DirectedNode* node = &storage[i];

            std::string node_name = "N" + std::to_string(node->get_id());

            graph = graph + node_name + "[shape=record, label=\"" + node_name;
            graph = graph + "|{" + *(node->get_name()) + "}\"];\n";

            for (int j = 0; j < node->get_children()->size(); j++) {

                std::string child_id = std::to_string(node->get_children()->at(j)->get_id());

                graph = graph + node_name + ":s -> N" + child_id + ":n [label=\"\"];\n";
            }
        }

        graph = graph + "}\n";

        return graph;
    }

    // Helpful function to determine if a FSM Operator has started
    // its operation phase
    bool get_is_operating() {
        return is_operating;
    }

    void start_operation(Action* action_parameter) {
        head = &storage[start];
        is_operating = true;
        head->call_action(action_parameter);
    }

    void step(Eval* eval_param, Action* action_param) {

        uint32_t idx = head->call_evaluation(eval_param);

        if (idx >= storage.size())
            idx = storage.size() - 1;

        head = (*head->get_children())[idx];
        head->call_action(action_param);
    }

private:
    
    static void add_children(DirectedNode* parent, const std::vector<DirectedNode*>* children) {

        for (int i = 0; i < children->size(); i++) {
            parent->add_child((*children)[i]);
        }
    }

    void add_children(DirectedNode* parent, const std::vector<uint32_t>* children) {

        for (int i = 0; i < children->size(); i++) {
            uint32_t child_idx = children[i];
            parent->add_child(&storage[child_idx]);
        }
    }
    
    void add_children(DirectedNode* parent, const std::vector<std::string>* children) {

        for (int i = 0; i < children->size(); i++) {

            std::string name = (children->at(i));
            uint32_t child_idx = 0;

#ifdef SB_FSMO_USE_NAME_HASHMAP
            child_idx = name_map.find(*name)
#else
            for (int j = 0; j < storage.size(); j++) {
                if (*(storage[j].get_name()) == name) {
                    child_idx = j;
                    break;
                }
            }
#endif
            parent->add_child(&storage[child_idx]);
        }
    }

    std::vector<DirectedNode> storage;

#ifdef SB_FSMO_USE_NAME_HASHMAP
    std::unordered_map<std::String, uint32_t> name_map;
#endif
    DirectedNode* head;
    uint32_t start;

    bool is_operating;
};
}
#endif