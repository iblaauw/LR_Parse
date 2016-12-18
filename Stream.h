#pragma once

template <class TokenType>
class Stream
{
public:
    virtual TokenType GetNext() = 0;
};

