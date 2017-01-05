#pragma once

#include <iostream>
#include <vector>
#include <memory>

class CFGParser
{
private:
    std::istream& input;
public:
    CFGParser(std::istream& input);

    void Parse();
};

