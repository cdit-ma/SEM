#ifndef DOUBLEHASH_H
#define DOUBLEHASH_H
#include <QHash>

template<class Key, class Value> class DoubleHash
{
public:
    DoubleHash();

    void insert(const Key & key, const Value & value);
    void removeKey(const Key& key);
    void removeValue(const Value& value);

    QList<Key> getKeys();
    QList<Value> getValues();

    bool isEmpty();

    bool containsKey(const Key& key);
    bool containsValue(const Value& value);

    Value value(const Key& key);
    Key key(const Value& value);

private:
    QHash<Key,Value> forwardHash;
    QHash<Value,Key> backwardHash;
};

template<class Key, class Value>
DoubleHash<Key,Value>::DoubleHash()
{

}


template<class Key, class Value>
void DoubleHash<Key,Value>::insert(const Key &key, const Value &value)
{
    forwardHash.insert(key, value);
    backwardHash.insert(value, key);
}

template<class Key, class Value>
void DoubleHash<Key,Value>::removeKey(const Key &key)
{
    Value value = forwardHash[key];
    forwardHash.remove(key);
    backwardHash.remove(value);
}

template<class Key, class Value>
void DoubleHash<Key,Value>::removeValue(const Value &value)
{
    Key key = backwardHash[value];
    forwardHash.remove(key);
    backwardHash.remove(value);
}

template<class Key, class Value>
QList<Key> DoubleHash<Key,Value>::getKeys()
{
    return forwardHash.keys();
}

template<class Key, class Value>
QList<Value> DoubleHash<Key,Value>::getValues()
{
    return backwardHash.keys();
}

template<class Key, class Value>
bool DoubleHash<Key,Value>::isEmpty()
{
    return forwardHash.isEmpty();
}

template<class Key, class Value>
bool DoubleHash<Key,Value>::containsKey(const Key &key)
{
    return forwardHash.contains(key);
}

template<class Key, class Value>
bool DoubleHash<Key,Value>::containsValue(const Value &value)
{
    return backwardHash.contains(value);
}

template<class Key, class Value>
Value DoubleHash<Key,Value>::value(const Key &key)
{
    return forwardHash[key];
}

template<class Key, class Value>
Key DoubleHash<Key,Value>::key(const Value &value)
{
    return backwardHash[value];
}

#endif // DOUBLEHASH_H


