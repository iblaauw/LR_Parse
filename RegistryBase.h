#pragma once

#include <map>

#include "message_exception.h"

DECL_EXCEPTION(RegistryException);

template <class TValue>
class RegistryBase
{
public:
    using KeyType = int;
protected:
    std::map<KeyType, TValue> keyToVal;
    std::map<TValue, KeyType> valToKey;
    KeyType current = 0;

    using key_val_map = decltype(keyToVal);
    using val_key_map = decltype(valToKey);

public:
    using raw_val_iter = typename decltype(keyToVal)::const_iterator;
    using raw_key_iter = typename decltype(valToKey)::const_iterator;

    KeyType Register(const TValue& val)
    {
        KeyType key = current;

        auto iter = valToKey.find(val);
        if (iter != valToKey.end())
            throw RegistryException("Value has already been registered!");

        valToKey[val] = key;
        keyToVal[key] = val;
        current++;
        return key;
    }

    KeyType GetOrRegister(const TValue& val)
    {
        auto iter = valToKey.find(val);
        if (iter != valToKey.end())
            return iter->second;

        KeyType key = current;
        valToKey[val] = key;
        keyToVal[key] = val;
        current++;
        return key;
    }

    const TValue& GetValue(KeyType key) const
    {
        return keyToVal.at(key);
    }

    KeyType Get(const TValue& val) const
    {
        return valToKey.at(val);
    }

    template<class raw_iter>
    struct Iterator
    {
    private:
        raw_iter iter;
    public:
        Iterator(const raw_iter& iter) : iter(iter) {}

        const auto operator*() const -> const decltype(iter->second)&
        {
            return iter->second;
        }

        const auto operator->() const -> const decltype(iter->second)*
        {
            return &(iter->second);
        }

        Iterator& operator++()
        {
            iter++;
            return *this;
        }

        bool operator==(const Iterator& other) const
        {
            return iter == other.iter;
        }

        bool operator!=(const Iterator& other) const
        {
            return iter != other.iter;
        }
    };

    Iterator<raw_key_iter> begin() const { return Iterator<raw_key_iter>(valToKey.begin()); }
    Iterator<raw_key_iter> end() const { return Iterator<raw_key_iter>(valToKey.end()); }
    Iterator<raw_val_iter> begin_val() const { return Iterator<raw_val_iter>(keyToVal.begin()); }
    Iterator<raw_val_iter> end_val() const { return Iterator<raw_val_iter>(keyToVal.end()); }
};

