#pragma once
#include <QDebug>
#include <functional>
#include <utility>
#include <QWeakPointer>
#include <QSharedPointer>


template <typename Object, typename ...Args>
class CallbackWithOwner
{
public:
  CallbackWithOwner(QWeakPointer<Object> owner, std::function<void(Args...)> c)
    : m_owner(owner), m_callback(c)
  {
  }


  void call(Args... args)
  {
      if (!isValid()) {
          qDebug() << "Callback owner was destroyed";
          return;
      }

      return m_callback(args...);
  }

  bool isValid()
  {
    auto owner = m_owner.toStrongRef();
    return !owner.isNull();
  }

  std::function<void(Args...)> rawCallback()
  {
      return m_callback;
  }

private:
  QWeakPointer<Object> m_owner;
  std::function<void(Args...)> m_callback;
};
