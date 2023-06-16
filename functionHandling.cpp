#include <string>
#include <vector>
#include "functionHandling.h"
#include <regex>
#include <cmath>

#define TRIG_REGEX ".*cos|.*sin|.*tan"

bool isValidFunction(char* function){
    /*
     *TODO: Check for:
     * -closed parentheses
     * -no other letters besides t
     * -no divide by 0
     */
    return true;
}

//TODO: Implicit multiplication
//TODO: Optimize for time
void generatePoints(double *xArray, double *yArray, const double *tArray, const char *funY, const char *funX, int arraySize) {
    std::string funYString = funY; std::string funXString = funX;
    std::vector<std::string> xComponents; std::vector<std::string> yComponents;
    //split the function into tokens of operators or numbers
    xComponents = getComponents(funXString); yComponents = getComponents(funYString);
    //Use the shunting yard algorithm to convert the component lists into reverse polish notation (postfix)
    toPostfix(xComponents); toPostfix(yComponents);
    //using the given function vectors in RPN, fill the respective point array with its value
    for(int i = 0; i<arraySize; ++i){
        xArray[i] = getVal(xComponents, tArray[i]);
        yArray[i] = getVal(yComponents, tArray[i]);
    }
}

void fillTValues(double *emptyTValArray, const float* range, int arraySize) {
    // O(n) time complexity
    double step = (range[1]-range[0])/(double)arraySize;
    double val = range[0];
    for(int i = 0; i<arraySize;++i){
        emptyTValArray[i] = val;
        val+=step;
    }
}

int indexOfTVal(float tVal, const double *tValues, int arraySize) {
    int left = 0;
    int right = arraySize-1;
    int middle;
    while(left<=right){
        middle = (left+right)/2;
        if(tValues[middle]<=tVal && tValues[middle+1]>=tVal) return middle;
        if(tValues[middle]<tVal && tValues[middle+1]<tVal) left = middle+1;
        if(tValues[middle]>tVal) right = middle-1;
    }
    return -1;
}

std::vector<std::string> getComponents(const std::string& function){
    std::vector<std::string> components;
    int componentIndex = 0;
    for(int i = 0; i<function.length();++i){
        std::string letter = function.substr(i, 1);
        //get digits as tokens
        if(std::regex_match(letter, std::regex("[0-9.]"))){
            int digits = 1;
            components.push_back("");
            do{
                components[componentIndex] = function.substr(i, digits);
                ++digits;
            }
            while(std::regex_match(function.substr(i,digits),std::regex("^[0-9.]+$")) && (i+digits-1)<function.length());
            i+=digits-2;
            ++componentIndex;
        }

            //cos, sin, tan are operators
        else if(i<=function.size()-3 &&(letter == "s" || letter == "c" || function.substr(i, 3) == "tan")){
            ++componentIndex;
            components.push_back(function.substr(i, 3));
            i+=2;
        }

            //does not account for negative numbers or for implicit multiplication
        else if(letter == "-" || letter == "+" || letter =="*" || letter == "/" || letter == "(" || letter == ")" ||letter == "^"|| letter == "t"){
            ++componentIndex;
            components.push_back(letter);
        }
    }

    //Move negative signs into proper place
    for(int i = 0; i<components.size();++i){
        if(components[i] == "-" && i == 0){
            components[1] = "-" + components[1];
            components.erase(components.begin());
        }
        else if(components[i] == "-"){
            if(!std::regex_match(components[i-1], std::regex("[0-9.]")) && components[i-1] != ")" && components[i-1]!="t"){
                components[i+1] = "-"+components[i+1];
                components.erase(components.begin()+i);
                --i;
            }
        }
    }

    return components;
}

int power(const std::string& op){
    if(op == "^") return 4;
    if(std::regex_match(op,std::regex(TRIG_REGEX))) return 3;
    if(op == "*" || op == "/" ) return 2;
    if(op == "+"|| op == "-") return 1;
    else throw std::invalid_argument("Could not recognize operator ["+op+"]) ");
}

void toPostfix(std::vector<std::string>& components){
    std::vector<std::string> out;
    std::vector<std::string> operatorStack;

    while(!components.empty()){
        if(std::regex_match(components[0],std::regex("-*[0-9.]+"))||components[0]=="t"){
            out.push_back(components[0]);
            components.erase(components.begin());
        }
        else if(components[0] == ")"){
            while(operatorStack.back()!="("){
                out.push_back(operatorStack.back());
                operatorStack.pop_back();
            }
            operatorStack.pop_back();
            components.erase(components.begin());
        }
        else if(components[0] == "("){
            operatorStack.push_back(components[0]);
            components.erase(components.begin());
        }
        else if(!operatorStack.empty() && operatorStack.back()!= "(" && power(components[0])<=power(operatorStack.back())){
            do {
                out.push_back(operatorStack.back());
                operatorStack.pop_back();
            }
            while(!operatorStack.empty() && operatorStack.back()!= "(" &&power(components[0])<=power(operatorStack.back()));
            operatorStack.push_back(components[0]);
            components.erase(components.begin());
        }
        else{
            operatorStack.push_back(components[0]);
            components.erase(components.begin());
        }
    }

    while(!operatorStack.empty()){
        out.push_back(operatorStack.back());
        operatorStack.pop_back();
    }
    components = out;
}

//TODO: It might be more efficient to figure out the function, and then apply to whole array, as opposed
//      to finding for each of the 10k values.
double getVal(std::vector<std::string> components, double tVal){
    std::vector<double> stack;
    for(std::string &s: components){
        if(s == "t") s=std::to_string(tVal);
    }

    while(!components.empty()){
        if(std::regex_match(components.front(),std::regex("[0-9.-]+"))){
            stack.push_back(std::stod(components.front()));
            components.erase(components.begin());
        }
        else if(std::regex_match(components.front(), std::regex(TRIG_REGEX))){
            bool negative = false;
            //check negativity
            if(components.front().substr(0,1) == "-") {
                negative = true;
                components.front() = components.front().substr(1);
            }
            if(components.front() == "sin") stack.back() = sin(stack.back());
            if(components.front() == "cos") stack.back() = cos(stack.back());
            if(components.front() == "tan") stack.back() = tan(stack.back());

            if(negative) stack.back()*=-1.0;
            components.erase(components.begin());
        }
        else{
            double operand2 = stack.back();
            stack.pop_back();
            double operand1 = stack.back();
            stack.pop_back();
            double result;
            if(components.front() == "+") result = operand1+operand2;
            if(components.front() == "-") result = operand1-operand2;
            if(components.front() == "/") result = operand1/operand2;
            if(components.front() == "*") result = operand1*operand2;
            if(components.front() == "^") result = pow(operand1,operand2);

            stack.push_back(result);
            components.erase(components.begin());
        }
    }
    return stack.back();
}
