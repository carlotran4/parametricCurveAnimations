
#ifndef PARAMETRICCURVEVISUALIZER_FUNCTIONHANDLING_H
#define PARAMETRICCURVEVISUALIZER_FUNCTIONHANDLING_H

#include <string>
#include <vector>
#include <regex>
#include <cmath>

/*
 * Tests whether the inputted function for x(t) and y(t) is valid.
 *
 * @param function: The char array you want to check
 *
 * @return true if function is valid
 */
bool isValidFunction(char* function);

/**
 * Use the functions x(t) and y(t) and the value tArray[n]
 * to generate the values xArray[n] and yArray[n]
 * @param xArray Empty array to be filled with x values
 * @param yArray Empty array to be filled with y values
 * @param tArray Array filled with extrapolated t values from the lower to upper bound
 *               of the t-range specified by the user.
 * @param funY The function y(t)
 * @param funX The function x(t)
 * @param arraySize The size of the x,y,t arrays
 */
void generatePoints(double *xArray, double *yArray, const double *tArray, const char *funY, const char *funX, int arraySize);

/**
 * Given a range for t, fill each slot in the array such that
 * it has an equal increment and covers the values [range beginning, range end]
 * @param emptyTValArray Empty array of size arraySize.
 * @param range Range for t. Ex. [-10.5, 10.5]
 * @param arraySize Size of the emptyTValArray
 */
void fillTValues(double *emptyTValArray, const float* range, int arraySize);

/**
 * Find the index of a specific t-value in an array of t-values
 * @param tVal Value to find index for.
 * @param tValues Array of tValues.
 * @return Index of tVal.
 */
int indexOfTVal(float tVal, const double *tValues);

/**
 * Split a function f(t) into components. Ex. 50*(t+6) -> {50,*,(,t,+,6,)}
 * @param function A function f(t)
 * @return A vector of the components of f(t).
 */
std::vector<std::string> getComponents(const std::string& function);

/**
 * Returns a numerical value representing the strength of an operation
 * in respect to the order of operations. A higher number represents a more
 * powerful operator.
 * @param op Operator to get strength of
 * @return 4 for exponents, 3 for trig functions, 2 for multiplication/division
 * 1 for addition/subtraction.
 */
int power(const std::string& op);

/**
 * Implements the shunting yard algorithm to convert the inputted
 * parametric functions to postfix representation, which makes it easier
 * for computers to calculate values and removes order of operations issues.
 * @param components vector of inputted function, separated into components. Ex. {(,5,+,t,)*5}
 */
void toPostfix(std::vector<std::string>& components);

/**
 * Evaluate the function f(t) at t = tVal.
 * @param components Vector of f(t), seperated into components and in postfix notation.
 * @param tVal Value of t to evaluate at.
 * @return Result of evaluating f(t) at t = tVal.
 */
double getVal(std::vector<std::string> components, double tVal);
#endif //PARAMETRICCURVEVISUALIZER_FUNCTIONHANDLING_H
