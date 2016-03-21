#pragma once

#ifndef SINGLETON_H
#define SINGLETON_H

// http://www.nuonsoft.com/blog/2012/10/21/implementing-a-thread-safe-singleton-with-c11/

#include <mutex>
#include <memory>

template<class T>
class Singleton {
public:
  static T* instance() {
    std::call_once(m_once_flag, [] {m_instance.reset(new T);});
    return m_instance.get();
  }

private:
  Singleton() {
  }
  ;
  virtual ~Singleton() {
  }
  ;

  static std::unique_ptr<T> m_instance;
  static std::once_flag m_once_flag;
};

template<class T> std::unique_ptr<T> Singleton<T>::m_instance;
template<class T> std::once_flag Singleton<T>::m_once_flag;

template<class T>
class SingletonWithNullDeleter {
public:
  static T* instance() {
    std::call_once(m_once_flag, [] {m_instance = new T;});
    return m_instance;
  }

private:
  SingletonWithNullDeleter() {
  }
  ;
  virtual ~SingletonWithNullDeleter() {
  }
  ;

  static T* m_instance;
  static std::once_flag m_once_flag;
};

template<class T> T* SingletonWithNullDeleter<T>::m_instance = nullptr;
template<class T> std::once_flag SingletonWithNullDeleter<T>::m_once_flag;

#endif // SINGLETON_H
