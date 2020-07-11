/*
 * ThreadSafeMap.h
 *
 *  Created on: Jul 6, 2020
 *      Author: jeremy
 */

#ifndef THREADSAFEMAP_H_
#define THREADSAFEMAP_H_

#include <mutex>
#include <unordered_map>

template<class Key, class Value>
class ThreadSafeMap
{
    std::mutex m_;
    std::unordered_map<Key, Value> uo_;

public:
    Value get(Key const& k) {
        std::lock_guard<std::mutex> lock(m_);
        return uo_[k]; // Return a copy.
    }

    void set(const Key & k, const Value & v) {
        std::lock_guard<std::mutex> lock(m_);
        uo_[k] = v;
    }

    bool DoesKeyExist(const Key & k)
    {
    	std::unique_lock<std::mutex> lock(m_);
    	if (uo_.find(k) == uo_.end())
    	{
    		return false;
    	}
    	else
    	{
    		return true;
    	}
    }
};



#endif /* THREADSAFEMAP_H_ */
