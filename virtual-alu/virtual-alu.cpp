#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <string_view>
#include <algorithm>
#include <cstdint>

class VirtualALU {
private:
    void print_alu_process(std::string& operator_name, int32_t a, int32_t b, int32_t res) {

    }
public:
    int32_t add(int32_t a, int32_t b) {

    }
    int32_t subtraction(int32_t a, int32_t b) {

    }
    int32_t multiplication(int32_t a, int32_t b) {

    }
    int32_t division(int32_t a, int32_t b) {

    }


    int32_t negate(int32_t num) {

    }
};

void safe_input(std::string& expr) {
    bool correct_expr;
    do {
        std::getline(std::cin, expr);

        if (expr == "exit") {
            return;
        }

        correct_expr = true;
        if (!std::all_of(expr.begin(), expr.end(), [](char c) { return std::string_view("0123456789+-*/() ").find(c) != std::string_view::npos; })) {
            correct_expr = false;
            std::cout << "Error: invalid math expression. Try again." << std::endl;
        }

    } while (!correct_expr);
}


template<typename T>
concept mathexpr = std::same_as<T, std::string> ||
std::same_as<T, std::vector<std::string>>;
bool math_expression_validation(const mathexpr auto& expr) {
    int operators_count = 0;
    int operands_count = 0;
    int open_parentheses_count = 0;
    int close_parentheses_count = 0;
    for (auto it = expr.begin(); it != expr.end(); it++) {
        if (std::string_view("+-*/").find(*it) != std::string_view::npos) {
            if constexpr (std::is_same_v<decltype(expr), const std::string&>) {
                if (*it == '-' && (it == expr.begin() || *(std::prev(it)) == '(')) {
                    continue;
                }
            }
            operators_count++;
        }
        else if (std::string_view("()").find(*it) != std::string_view::npos) {
            if (std::string_view("(").find(*it) != std::string_view::npos) {
                open_parentheses_count++;
            }
            else if (std::string_view(")").find(*it) != std::string_view::npos) {
                close_parentheses_count++;
            }
        }
        else {
            operands_count++;
        }
    }


    if (operands_count == 0) {
        return false;
    }
    else if (operands_count <= operators_count) {
        return false;
    }
    else if (open_parentheses_count != close_parentheses_count) {
        return false;
    }

    return true;
}



std::vector<std::string> shunting_yard_algorithm(std::string_view expr) {
    std::vector<std::string> output_arr;
    std::stack<char> operators_stack;

    for (auto it = expr.begin(); it != expr.end(); it++) {
        if (*it == ' ') continue;

        if (std::string_view("+-*/()").find(*it) != std::string_view::npos) {
            if (*it == ')') {
                while (!operators_stack.empty() && (operators_stack.top() != '(')) {
                    output_arr.push_back(std::string(1, operators_stack.top()));
                    operators_stack.pop();
                }
                if (!operators_stack.empty()) {
                    operators_stack.pop();
                    continue;
                }
                else {
                    return output_arr;
                }
            }
            else if (*it == '/' || *it == '*') {
                while (!operators_stack.empty() && (operators_stack.top() == '*' || operators_stack.top() == '/')) {
                    output_arr.push_back(std::string(1, operators_stack.top()));
                    operators_stack.pop();
                }
            }
            else if (*it == '+') {
                while (!operators_stack.empty() && operators_stack.top() != '(') {
                    output_arr.push_back(std::string(1, operators_stack.top()));
                    operators_stack.pop();
                }
            }
            else if (*it == '-') {
                if (it == expr.begin() || *(std::prev(it)) == '(') {
                    operators_stack.push('~');
                    continue;
                }
                else {
                    while (!operators_stack.empty() && operators_stack.top() != '(') {
                        output_arr.push_back(std::string(1, operators_stack.top()));
                        operators_stack.pop();
                    }
                }
            }
            operators_stack.push(*it);
            continue;
        }
        else {
            std::string number;
            while (it != expr.end() && (isdigit(*it) || *it == ' ')) {
                if (isdigit(*it)) {
                    number.push_back(*it);
                }
                it++;
            }
            output_arr.push_back(number);
            it--;
        }
    }

    while (!operators_stack.empty()) {
        output_arr.push_back(std::string(1, operators_stack.top()));
        operators_stack.pop();
    }

    return output_arr;
}



int32_t stack_machine(std::vector<std::string>& rpn) {
    std::stack<int32_t> result;
    VirtualALU alu;

    for (auto it = rpn.begin(); it != rpn.end(); it++) {
        if (*it == "~") {
            int32_t num = result.top();
            result.pop();
            result.push(alu.negate(num));
        }
        else if (*it == "+" || *it == "-" || *it == "*" || *it == "/") {
            int32_t a = result.top();
            result.pop();
            int32_t b = result.top();
            result.pop();
            if (*it == "+") {
                result.push(alu.add(a, b));
            }
            else if (*it == "-") {
                result.push(alu.subtraction(b, a));
            }
            else if (*it == "*") {
                result.push(alu.multiplication(a, b));
            }
            else if (*it == "/") {
                result.push(alu.division(b, a));
            }
        }
        else {
            result.push(std::stoi(*it));
        }
    }

    return result.top();
}



int main()
{
    std::cout << "Welcome to virtual-alu project!" << std::endl;

    while (true) {

        // First level of protection: check for valid characters and empty input. Use custom safe_input function instead of std::cin.
        std::string math_e;
        std::cout << "Enter a math problem. Enter 'exit' to close the program: " << std::endl;
        safe_input(math_e);
        if (math_e == "exit") {
            break;
        }

        // Second level of protection: check for mathematical correctness (e.g. balance of operands/operators and parentheses).
        std::vector<std::string> RPN;
        bool incorrect_expr = true;
        while (incorrect_expr) {
            RPN = shunting_yard_algorithm(math_e);
            if (math_expression_validation(math_e) && math_expression_validation(RPN)) {
                incorrect_expr = false;
            }
            else {
                std::cout << "Error: invalid math expression. Try again." << std::endl;
                safe_input(math_e);
            }
        }

        std::cout << '\n';



        // reverse polish notation output
        std::cout << "reverse polish notation: ";
        for (auto it = RPN.begin(); it != RPN.end(); it++) {
            std::cout << *it << ". ";
        }
        std::cout << '\n' << std::endl;



        std::cout << "result: " << stack_machine(RPN) << std::endl;
        std::cout << "----------------------------------------------------------------------------------------------\n" << std::endl;
    }

    return 0;
}