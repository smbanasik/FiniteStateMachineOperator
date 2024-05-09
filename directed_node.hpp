#ifndef SB_DIRECTED_NODE_HPP
#define SB_DIRECTED_NODE_HPP

#include <stdint.h>
#include <set>
#include <string>
#include <vector>

namespace sblib {
class DirectedNode {
public:
    DirectedNode(const uint32_t id, const std::string name) : id(id), name(name) {
        evaluation = nullptr;
        action = nullptr;
    }

    void add_child(DirectedNode* directed_edge) {

        if (child_check.find(directed_edge) == child_check.end()) {

            child_check.insert(directed_edge);
            children.emplace_back(directed_edge);
        }

    }

    uint32_t call_evaluation(void* param) {
        if (evaluation == nullptr)
            return 0;

        return evaluation(param);
    }

    void call_action(void* param) {
        if (action == nullptr)
            return;

        action(param);
    }

    // Set the function used by the evaluation function.
    // The function must:
    // - Return a uint32_t
    // - Recieve one parameter of the template type
    template<class T>
    void set_eval_function(uint32_t(*func)(T*)) {
        evaluation = (uint32_t(*)(void*))func;
    }

    // Set the function used by the action function.
    // The function must:
    // - Return void
    // - Recieve one parameter of the template type
    template<class T>
    void set_act_function(void (*func)(T*)) {
        action = (void(*)(void*))func;
    }

    const std::vector<DirectedNode*>* get_children() const {
        return const_cast<std::vector<DirectedNode*>*>(&children);
    }

    uint32_t get_id() const {
        return id;
    }

    const std::string* get_name() const {
        return const_cast<std::string*>(&name);
    }

private:
    uint32_t id;
    std::string name;
    std::set<DirectedNode*> child_check;
    std::vector<DirectedNode*> children;

    uint32_t(*evaluation)(void*); // Use parameter to make the choice
    void (*action)(void*); // Parameter could be used to modify object,
};
}
#endif