#include <cassert>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

template <typename T>
class unique_queue {
private:
    std::queue<T> m_queue;
    std::set<T> m_set;
public:
    bool push(const T& t) {
        if (m_set.insert(t).second) {
            m_queue.push(t);
            return true;
        }
        return false;
    }

    void pop() {
        assert(!m_queue.empty());
        const T& val = front();

        typename std::set<T>::iterator it = m_set.find(val);
        assert(it != m_set.end());

        m_set.erase(it);
        m_queue.pop();
    }

    const T& front() const {
        return m_queue.front();
    }

    bool empty() const {
        return m_queue.empty();
    }
};