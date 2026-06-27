#include "turmite.hpp"
#include "util/turmite.hpp"
#include <CL/cl_platform.h>
#include <exception>
#include <string>

using namespace std;

// TODO descriptive error messages
namespace turmite {

void expectMore(string::const_iterator &cur, string::const_iterator &end) {
    if (cur == end)
        throw exception();
}

void consumeWhitespace(string::const_iterator &cur, string::const_iterator &end) {
    for (; cur != end; cur++) {
        char c = *cur;
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
    }
}

void consume(string::const_iterator &cur, string::const_iterator &end, char expected) {
    consumeWhitespace(cur, end);
    expectMore(cur, end);
    if (*cur++ != expected)
        throw exception();
}

void expectDigit(string::const_iterator &cur, string::const_iterator &end) {
    consumeWhitespace(cur, end);
    expectMore(cur, end);
    if (*cur < '0' || *cur > '9')
        throw exception();
}

template <cl_uchar max>
cl_uchar parseUChar(string::const_iterator &cur, string::const_iterator &end) {
    cl_uchar value = 0;

    consumeWhitespace(cur, end);
    expectDigit(cur, end);
    for (; cur != end; cur++) {
        char c = *cur;
        if (c < '0' || c > '9')
            break;
        cl_uchar digit = c - '0';
        if (value > (max - digit) / 10)
            throw exception();
        value = (10 * value) + digit;
    }

    return value;
}

turn_t parseTurn(string::const_iterator &cur, string::const_iterator &end) {
    consumeWhitespace(cur, end);
    expectMore(cur, end);
    char c = *cur++;

    if (c == '1' || c == '2' || c == '4' || c == '8')
        return c - '0';

    throw exception();
}

turmite_rule_t parseRule(string::const_iterator &cur, string::const_iterator &end) {
    consume(cur, end, '{');
    cell_t newValue = parseUChar<127>(cur, end);
    consume(cur, end, ',');
    turn_t turn = parseTurn(cur, end);
    consume(cur, end, ',');
    state_t nextState = parseUChar<255>(cur, end);
    consume(cur, end, '}');

    return {newValue, turn, nextState};
}

TurmiteRuleset parseRule(string &rule) {
    TurmiteRuleset rules{{}, 0, 0};
    string::const_iterator cur = rule.cbegin();
    string::const_iterator end = rule.cend();

    consume(cur, end, '{');
    while (1) { // States
        size_t nCellValues = 0;

        consume(cur, end, '{');
        while (1) { // Rules
            turmite_rule_t r = parseRule(cur, end);
            rules.rules.push_back(r);
            nCellValues++;

            consumeWhitespace(cur, end);
            expectMore(cur, end);
            if (*cur == '}')
                break;
            consume(cur, end, ',');
        }
        consume(cur, end, '}');

        if (!rules.nCellValues)
            rules.nCellValues = nCellValues;
        else if (nCellValues != rules.nCellValues)
            throw exception();

        rules.nStates++;

        consumeWhitespace(cur, end);
        expectMore(cur, end);
        if (*cur == '}')
            break;
        consume(cur, end, ',');
    }
    consume(cur, end, '}');

    // Verify
    for (auto &&rule : rules.rules)
        if (rule.newValue >= rules.nCellValues || rule.nextState >= rules.nStates)
            throw exception();

    return rules;
}

} // namespace turmite
