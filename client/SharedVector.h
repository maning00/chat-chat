//
// Created by 马宁 on 2019-07-09.
//

#ifndef CLIENT_SHAREDVECTOR_H
#define CLIENT_SHAREDVECTOR_H
#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

template <typename T>
class SharedVector
{
public:
    SharedVector();
    ~SharedVector();

    T& front();
    T& back();
    void pop_back();

    void push_back(const T& item);
    void push_back(T&& item);

    int size();
    bool empty();

private:
    std::vector<T> Vector_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template <typename T>
SharedVector<T>::SharedVector(){}

template <typename T>
SharedVector<T>::~SharedVector(){}

template <typename T>
T& SharedVector<T>::back()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (Vector_.empty())
    {
        cond_.wait(mlock);
    }
    return Vector_.back();
}

template <typename T>
T& SharedVector<T>::front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (Vector_.empty())
    {
        cond_.wait(mlock);
    }
    return Vector_.front();
}

template <typename T>
void SharedVector<T>::pop_back()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (Vector_.empty())
    {
        cond_.wait(mlock);
    }
    Vector_.pop_back();
}

template <typename T>
void SharedVector<T>::push_back(const T& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    Vector_.push_back(item);
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
void SharedVector<T>::push_back(T&& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    Vector_.push_back(std::move(item));
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
int SharedVector<T>::size()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    int size = Vector_.size();
    mlock.unlock();
    return size;
}

template <typename T>
bool SharedVector<T>::empty()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    bool empty = Vector_.empty();
    mlock.unlock();
    return empty;
}
#endif //CLIENT_SHAREDVECTOR_H
