#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <string_view>
#include <algorithm>
#include <cstdint>
#include <bitset>


class VirtualALU {
private:

    bool carry = 0;
    bool carry_second = 0;

    bool half_adder(bool a, bool b) {
        carry = a && b;
        return a ^ b;
    }

    bool full_adder(bool a, bool b) {
        bool result_first = a ^ b;
        bool result_second = result_first ^ carry;
        carry_second = result_first && carry;
        carry = a && b;
        carry = carry || carry_second;
        return result_second;
    }

    
    /*void print_alu_process(std::string operator_name, uint64_t a, uint64_t b, uint64_t res) {
        
    }*/

public:
    uint64_t addition(uint64_t a, uint64_t b, bool SUB) {
        if (SUB) b = ~b;
        carry = SUB;

        uint64_t result = 0;

        for (uint64_t i = 0; i < 64; ++i) {
            result |= full_adder(a & 1ULL , b & 1ULL) << i;
            a >>= 1;
            b >>= 1;
        }
        return result;
    }

    uint64_t subtraction(uint64_t a, uint64_t b) {
        return addition(a, b, 1);
    }

    uint64_t multiplication(uint64_t a, uint64_t b) {
        uint64_t result = 0;
        for (int i = 0; i < 64; i++) {
            if (b & 1ULL) result = addition(result, a, 0);
            a <<= 1;
            b >>= 1;
        }
        return result;
    }

    uint64_t division(uint64_t a, uint64_t b) {
        uint64_t remainder = 0;

        for (int i = 0; i < 64; ++i) {
            uint64_t a_mask = (a >> 63) & 1ULL;
            remainder <<= 1;
            remainder |= a_mask;
            a <<= 1;
            uint64_t temp = subtraction(remainder, b);
            if (temp >> 63 == 0) {
                remainder = temp;
                a |= 1ULL;
            }
        }
        return a;
    }


    uint64_t incrementator(uint64_t num) {
        carry = 1;
        uint64_t res = 0;
        for (int i = 0; i < 64; ++i) {
            uint64_t nummask = (num >> i) & 1ULL;
            bool sum = half_adder(nummask, carry) << i;
            res |= (uint64_t)sum << i;
        }
        return res;
    }
    uint64_t negate(uint64_t num) {
        return incrementator(~num);
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



std::bitset<64> stack_machine(std::vector<std::string>& rpn) {
    std::stack<uint64_t> nums;
    VirtualALU alu;

    for (auto it = rpn.begin(); it != rpn.end(); it++) {
        if (*it == "~") {
            uint64_t num = nums.top();
            nums.pop();
            nums.push(alu.negate(num));
        }
        else if (*it == "+" || *it == "-" || *it == "*" || *it == "/") {
            uint64_t a = nums.top();
            nums.pop();
            uint64_t b = nums.top();
            nums.pop();
            if (*it == "+") {
                nums.push(alu.addition(a, b, 0));
            }
            else if (*it == "-") {
                nums.push(alu.subtraction(b, a));
            }
            else if (*it == "*") {
                nums.push(alu.multiplication(a, b));
            }
            else if (*it == "/") {
                nums.push(alu.division(b, a));
            }
        }
        else {
            uint64_t num = std::stoull(*it);
            nums.push(uint64_t(num));
        }
    }
    return std::bitset<64>(nums.top());
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

        /*uint64_t res = stack_machine(RPN);
        std::string res_bin = res.to_string();
        unsigned long long res_dec_u = res.to_ullong();
        long long res_dec = static_cast<long long>(res_dec_u);
        std::cout << "result in 2-Bin: " << res_bin << std::endl;
        std::cout << "result in 10-Dec: " << res_dec << std::endl;*/
        std::cout << "result: " << stack_machine(RPN) << std::endl;
        std::cout << "----------------------------------------------------------------------------------------------\n" << std::endl;
    }
    return 0;
}