#pragma once

#include <iostream>
#include <mutex>

#if 0
template <typename T>
class Singleton
{
public:
    volatile T* getInstance()
    {
        if(m_pInstance == nullptr)
        {
            m_mutex.lock();
            if(m_pInstance == nullptr)
            {
                m_pInstance = new Singleton();
            }
            m_mutex.unlock();
        }

        return m_pInstance;
    }

private:
    Singleton()
    {}

    Singleton(const Singleton&) = delete;

    Singleton& operator=(const Singleton&) = delete;

private:
    static volatile T* m_pInstance;
    static std::mutex          m_mutex;
};

template <typename T>
volatile T* Singleton<T>::m_pInstance = nullptr;
template <typename T>
std::mutex          Singleton<T>::m_mutex;
#endif

template <typename T>
class Singleton
{
private:
    static T* m_pInstance;

public:
    static T& getInstance()
    {
        if (m_pInstance == nullptr)
        {
            m_pInstance = new T();
        }

        return *m_pInstance;
    }

private:
    Singleton()
    {}

    ~Singleton()
    {}

    Singleton(const Singleton& ) = delete;

    Singleton& operator=(const Singleton& ) = delete;

    static void Init()
    {
        m_pInstance = new T();
    }

    static void Destory()
    {
        delete m_pInstance;
    }
};

template <typename T>
T* Singleton<T>::m_pInstance = nullptr;
