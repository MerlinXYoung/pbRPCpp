/* 
 * File:   Queue.hpp
 * Author: Steven
 *
 * Created on April 3, 2013, 5:04 PM
 */

#ifndef QUEUE_HPP
#define	QUEUE_HPP

#include <queue>
#include <boost/thread/mutex.hpp>

namespace pbrpcpp {

    template< typename T>
    class Queue {
    public:

        Queue(const T& defValue = T())
        : closed_(false),
        defValue_(defValue) {

        }

        T take() {
            if (closed_) {
                return defValue_;
            }

            boost::unique_lock<boost::mutex> lock(mutex_);

            if (datas_.empty()) {
                cond_.wait(lock);
            }

            //maybe interrupted
            if (datas_.empty()) {
                return defValue_;
            }

            T data = datas_.front();
            datas_.pop();
            return data;
        }

        T peek() {
            if (closed_) {
                return defValue_;
            }

            boost::unique_lock<boost::mutex> lock(mutex_);

            if (datas_.empty()) {
                cond_.wait(lock);
            }

            //maybe interrupted
            if (datas_.empty()) {
                return defValue_;
            }

            return datas_.front();
        }

        void add(const T& data) {
            if (closed_) {
                return;
            }
            {
                boost::unique_lock<boost::mutex> gaurd(mutex_);

                datas_.push(data);
            }
            cond_.notify_all();
        }

        int size() {
            boost::unique_lock<boost::mutex> guard(mutex_);
            return datas_.size();
        }

        void close() {
            if (closed_) {
                return;
            }

            closed_ = true;
            cond_.notify_all();
        }

    private:
        bool closed_;
        T defValue_;
        boost::condition_variable cond_;
        boost::mutex mutex_;
        std::queue< T > datas_;
    };
}

#endif	/* QUEUE_HPP */

