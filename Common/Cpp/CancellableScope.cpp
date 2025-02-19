/*  Cancellable Scope
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <set>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "SpinLock.h"
#include "Pimpl.tpp"
#include "Exceptions.h"
#include "CancellableScope.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{


struct CancellableData{
    CancellableData()
        : cancelled(false)
    {}
    std::atomic<bool> cancelled;
    mutable SpinLock lock;
    std::exception_ptr exception;
};



Cancellable::Cancellable(){}
Cancellable::~Cancellable(){
    detach();
}
CancellableScope* Cancellable::scope() const{
    m_sanitizer.check_usage();
    return m_scope;
}
bool Cancellable::cancelled() const noexcept{
    m_sanitizer.check_usage();
    return m_impl->cancelled.load(std::memory_order_acquire);
}
bool Cancellable::cancel(std::exception_ptr exception) noexcept{
    m_sanitizer.check_usage();
    CancellableData& data(m_impl);
    SpinLockGuard lg(data.lock);
    if (exception && !data.exception){
        data.exception = std::move(exception);
    }
    if (data.cancelled.load(std::memory_order_acquire)){
        return true;
    }
    return data.cancelled.exchange(true);
}
void Cancellable::throw_if_cancelled() const{
    m_sanitizer.check_usage();
    const CancellableData& data(m_impl);
    if (!data.cancelled.load(std::memory_order_acquire)){
        return;
    }
    SpinLockGuard lg(data.lock);
    if (data.exception){
        std::rethrow_exception(data.exception);
    }else{
        throw OperationCancelledException();
    }
}
bool Cancellable::throw_if_cancelled_with_exception() const{
    m_sanitizer.check_usage();
    const CancellableData& data(m_impl);
    if (!data.cancelled.load(std::memory_order_acquire)){
        return false;
    }
    SpinLockGuard lg(data.lock);
    if (data.exception){
        std::rethrow_exception(data.exception);
    }
    return true;
}
void Cancellable::attach(CancellableScope& scope){
    m_sanitizer.check_usage();
    m_scope = &scope;
    scope += *this;
}
void Cancellable::detach() noexcept{
    m_sanitizer.check_usage();
    if (m_scope){
        *m_scope -= *this;
    }
}





struct CancellableScopeData{
    std::set<Cancellable*> children;

    std::mutex lock;
    std::condition_variable cv;
};



CancellableScope::CancellableScope(){}
CancellableScope::~CancellableScope(){
    detach();
}
bool CancellableScope::cancel(std::exception_ptr exception) noexcept{
    if (Cancellable::cancel(exception)){
        return true;
    }
    CancellableScopeData& data(m_impl);
    std::lock_guard lg(data.lock);
    for (Cancellable* child : data.children){
        child->cancel(exception);
    }
    data.children.clear();
    data.cv.notify_all();
    return false;
}
void CancellableScope::wait_for(std::chrono::milliseconds duration){
    m_sanitizer.check_usage();
    wait_until(current_time() + duration);
}
void CancellableScope::wait_until(WallClock stop){
    m_sanitizer.check_usage();
    throw_if_cancelled();
    CancellableScopeData& data(m_impl);
    {
        std::unique_lock<std::mutex> lg(data.lock);
        data.cv.wait_until(
            lg, stop,
            [=]{
                return current_time() >= stop || cancelled();
            }
        );
    }
    throw_if_cancelled();
}
void CancellableScope::wait_until_cancel(){
    m_sanitizer.check_usage();
    throw_if_cancelled();
    CancellableScopeData& data(m_impl);
    {
        std::unique_lock<std::mutex> lg(data.lock);
        data.cv.wait(
            lg,
            [=]{
                return cancelled();
            }
        );
    }
    throw_if_cancelled();
}
void CancellableScope::operator+=(Cancellable& cancellable){
    m_sanitizer.check_usage();
    CancellableScopeData& data(m_impl);
    std::lock_guard<std::mutex> lg(data.lock);
    throw_if_cancelled();
    data.children.insert(&cancellable);
}
void CancellableScope::operator-=(Cancellable& cancellable){
    m_sanitizer.check_usage();
    CancellableScopeData& data(m_impl);
    std::lock_guard<std::mutex> lg(data.lock);
    data.children.erase(&cancellable);
}






}
