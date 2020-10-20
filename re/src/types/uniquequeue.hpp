#ifndef UNIQUE_QUEUE_H
#define UNIQUE_QUEUE_H

#include <cassert>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
// REVIEW (Mitch): Add tests.
// REVIEW (Mitch): Take second hit at this class
//  Perfect forwarding should be considered
namespace re::types {
template<typename T> class unique_queue {
private:
    std::queue<T> m_queue;
    std::set<T> m_set;

public:
    // REVIEW (Mitch): Nodiscard?
    bool push(const T& t)
    {
        if(m_set.insert(t).second) {
            m_queue.push(t);
            return true;
        }
        return false;
    }

    void pop()
    {
        assert(!m_queue.empty());
        const T& val = front();

        typename std::set<T>::iterator it = m_set.find(val);
        assert(it != m_set.end());

        m_set.erase(it);
        m_queue.pop();
    }

    const T& front() const { return m_queue.front(); }

    [[nodiscard]] bool empty() const { return m_queue.empty(); }
};

} // namespace re::types
#endif // UNIQUE_QUEUE_H