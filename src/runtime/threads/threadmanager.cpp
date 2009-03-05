//  Copyright (c) 2008-2009 Chirag Dekate, Hartmut Kaiser, Anshul Tandon
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/runtime/threads/thread_affinity.hpp>      // must be first header!
#include <hpx/runtime/threads/threadmanager.hpp>
#include <hpx/runtime/threads/thread.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/util/unlock_lock.hpp>
#include <hpx/util/logging.hpp>
#include <hpx/util/block_profiler.hpp>
#include <hpx/util/time_logger.hpp>
#include <hpx/performance_counters.hpp>

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/deadline_timer.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace threads
{
    ///////////////////////////////////////////////////////////////////////////
    namespace strings
    {
        char const* const thread_state_names[] = 
        {
            "init",
            "active",
            "pending",
            "suspended",
            "depleted",
            "terminated"
        };
    }

    char const* const get_thread_state_name(thread_state state)
    {
        if (state < init || state > terminated)
            return "unknown";
        return strings::thread_state_names[state];
    }

    ///////////////////////////////////////////////////////////////////////////
    threadmanager::threadmanager(util::io_service_pool& timer_pool,
            boost::function<void()> start_thread, boost::function<void()> stop, 
            boost::function<void(boost::exception_ptr const&)> on_error,
            std::size_t max_count)
      : max_count_(max_count), running_(false),
        timer_pool_(timer_pool), 
        start_thread_(start_thread), stop_(stop), on_error_(on_error),
        work_items_("work_items"), terminated_items_("terminated_items"), 
        new_tasks_("new_tasks"),
        thread_logger_("threadmanager::register_thread"),
        work_logger_("threadmanager::register_work"),
        add_new_logger_("threadmanager::add_new"),
        set_state_logger_("threadmanager::set_state")
#if HPX_DEBUG != 0
      , thread_count_(0)
#endif
    {
        LTM_(debug) << "threadmanager ctor";
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Fifo>
    inline void log_fifo_statistics(Fifo const& q)
    {
        LTIM_(fatal) << "~threadmanager: queue: "  << q.description_
                     << ", enqueue_spin_count: " << long(q.enqueue_spin_count_)
                     << ", dequeue_spin_count: " << long(q.dequeue_spin_count_);
    }

    threadmanager::~threadmanager() 
    {
        LTM_(debug) << "~threadmanager";
        if (!threads_.empty()) {
            if (running_) 
                stop();
            threads_.clear();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    thread_id_type threadmanager::register_thread(
        boost::function<thread_function_type> const& threadfunc, 
        char const* const description, thread_state initial_state, bool run_now)
    {
        util::block_profiler_wrapper<register_thread_tag> bp(thread_logger_);

        // verify parameters
        if (initial_state != pending && initial_state != suspended)
        {
            HPX_OSSTREAM strm;
            strm << "invalid initial state: " 
                 << get_thread_state_name(initial_state);
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::register_thread", HPX_OSSTREAM_GETSTRING(strm));
            return invalid_thread_id;
        }
        if (0 == description)
        {
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::register_thread", "description is NULL");
            return invalid_thread_id;
        }

        LTM_(info) << "register_thread: initial_state(" 
                   << get_thread_state_name(initial_state) << "), "
                   << std::boolalpha << "run_now(" << run_now << "), "
                   << "description(" << description << ")";

        // create the new thread
        std::auto_ptr<threads::thread> thrd (
            new threads::thread(threadfunc, initial_state, description));

        // lock data members while adding work
        {
            mutex_type::scoped_lock lk(mtx_);

            // add a new entry in the map for this thread
            thread_id_type id = thrd->get_thread_id();
            std::pair<thread_map_type::iterator, bool> p =
                thread_map_.insert(id, thrd.get());

            if (!p.second) {
                HPX_THROW_EXCEPTION(hpx::no_success, 
                    "threadmanager::register_thread", 
                    "Couldn't add new thread to the map of threads");
                return invalid_thread_id;
            }
        }

        // transfer ownership to map
        threads::thread* t = thrd.release();

        // only insert in the work-items queue if it is in pending state
        if (initial_state == pending) {
            // pushing the new thread in the pending queue thread
            work_items_.enqueue(t);
        }

        if (run_now) {
            // try to execute the new work item
            cond_.notify_all();
        }

        // return the thread_id of the newly created thread
        return t->get_thread_id();
    }

    ///////////////////////////////////////////////////////////////////////////
    void threadmanager::register_work(
        boost::function<thread_function_type> const& threadfunc, 
        char const* const description, thread_state initial_state, bool run_now)
    {
        util::block_profiler_wrapper<register_work_tag> bp(work_logger_);

        // verify parameters
        if (initial_state != pending && initial_state != suspended)
        {
            HPX_OSSTREAM strm;
            strm << "invalid initial state: " 
                 << get_thread_state_name(initial_state);
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::register_work", HPX_OSSTREAM_GETSTRING(strm));
            return;
        }
        if (0 == description)
        {
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::register_work", "description is NULL");
            return;
        }

        LTM_(info) << "register_work: initial_state(" 
                   << get_thread_state_name(initial_state) << "), "
                   << std::boolalpha << "run_now(" << run_now << "), "
                   << "description(" << description << ")";

    // create a new task
        new_tasks_.enqueue(
            task_description(threadfunc, initial_state, description));

        if (run_now) {
            // try to execute the new work item
            cond_.notify_all();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // add new threads if there is some amount of work available
    inline bool threadmanager::add_new(long add_count)
    {
        if (0 == add_count)
            return false;

        long added = 0;
        task_description task;
        while (add_count-- && new_tasks_.dequeue(&task)) 
        {
            // measure thread creation time
            util::block_profiler_wrapper<add_new_tag> bp(add_new_logger_);

            // create the new thread
            thread_state state = boost::get<1>(task);
            std::auto_ptr<threads::thread> thrd (
                new threads::thread(boost::get<0>(task), state, 
                    boost::get<2>(task)));

            // add the new entry to the map of all threads
            thread_id_type id = thrd->get_thread_id();
            std::pair<thread_map_type::iterator, bool> p =
                thread_map_.insert(id, thrd.get());

            if (!p.second) {
                HPX_THROW_EXCEPTION(hpx::no_success, 
                    "threadmanager::add_new", 
                    "Couldn't add new thread to the map of threads");
                return false;
            }

            // transfer ownership to map
            threads::thread* t = thrd.release();

            // only insert the thread into the work-items queue if it is in 
            // pending state
            if (state == pending) {
                // pushing the new thread into the pending queue 
                ++added;
                work_items_.enqueue(t);
                cond_.notify_all();         // wake up sleeping threads
            }
        }

        if (added) {
            LTM_(info) << "add_new: added " << added << " tasks to queues";
        }
        return added != 0;
    }

    inline bool threadmanager::add_new_if_possible()
    {
        if (new_tasks_.empty()) 
            return false;

        // create new threads from pending tasks (if appropriate)
        long add_count = -1;                  // default is no constraint

        // if the map doesn't hold max_count threads yet add some
        if (max_count_) {
            std::size_t count = thread_map_.size();
            if (max_count_ >= count + min_add_new_count) {
                add_count = max_count_ - count;
                if (add_count < min_add_new_count)
                    add_count = min_add_new_count;
            }
            else {
                return false;
            }
        }
        return add_new(add_count);
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool threadmanager::add_new_always()
    {
        if (new_tasks_.empty()) 
            return false;

        // create new threads from pending tasks (if appropriate)
        long add_count = -1;                  // default is no constraint

        // if we are desperate (no work in the queues), add some even if the 
        // map holds more than max_count
        if (max_count_) {
            std::size_t count = thread_map_.size();
            if (max_count_ >= count + min_add_new_count) {
                add_count = max_count_ - count;
                if (add_count < min_add_new_count)
                    add_count = min_add_new_count;
                if (add_count > max_add_new_count)
                    add_count = max_add_new_count;
            }
            else if (work_items_.empty()) {
                add_count = min_add_new_count;    // add this number of threads
                max_count_ += min_add_new_count;  // increase max_count
            }
            else {
                return false;
            }
        }
        return add_new(add_count);
    }

    ///////////////////////////////////////////////////////////////////////////
    // thread function registered for set_state if thread is currently active
    thread_state threadmanager::set_active_state(thread_id_type id, 
        thread_state newstate, thread_state_ex newstate_ex)
    {
        thread_state prevstate = set_state(id, newstate, newstate_ex);
        if (active == prevstate) {
            // re-schedule this task if thread is still active
            LTM_(info) << "set_active_state: " << "thread(" << id << "), "
                       << "is still active, scheduling new thread...";

            register_work(boost::bind(&threadmanager::set_active_state, this, 
                id, newstate, newstate_ex), "set state for active thread");
        }
        return terminated;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// The set_state function is part of the thread related API and allows
    /// to change the state of one of the threads managed by this threadmanager
    thread_state threadmanager::set_state(thread_id_type id, 
        thread_state new_state, thread_state_ex new_state_ex)
    {
        util::block_profiler_wrapper<set_state_tag> bp(set_state_logger_);

        // set_state can't be used to force a thread into active state
        if (new_state == active) {
            HPX_OSSTREAM strm;
            strm << "invalid new state: " << get_thread_state_name(new_state);
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::set_state", HPX_OSSTREAM_GETSTRING(strm));
            return unknown;
        }

        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        if (NULL == thrd->get())
            return terminated;     // this thread has been already terminated 

        // action depends on the current state
        thread_state previous_state = thrd->get_state();

        // nothing to do here if the state doesn't change
        if (new_state == previous_state)
            return new_state;

        // the thread to set the state for is currently running, so we 
        // schedule another thread to execute the pending set_state
        if (previous_state == active) {
            // schedule a new thread to set the state
            LTM_(info) << "set_state: " << "thread(" << id << "), "
                       << "is currently active, scheduling new thread...";

            register_work(boost::bind(&threadmanager::set_active_state, this, 
                id, new_state, new_state_ex), "set state for active thread");

            return active;     // done
        }
        else if (previous_state == terminated) {
            // If the thread has been terminated while this set_state was 
            // pending nothing has to be done anymore.
            return terminated;
        }

        // If the previous state was pending we are supposed to remove the
        // thread from the queue. But in order to avoid linearly looking 
        // through the queue we defer this to the thread function, which 
        // at some point will ignore this thread by simply skipping it 
        // (if it's not pending anymore). 

        LTM_(info) << "set_state: " << "thread(" << id << "), "
                   << "description(" << thrd->get_description() << "), "
                   << "new state(" << get_thread_state_name(new_state) << ")";

        // So all what we do here is to set the new state.
        thrd->set_state(new_state);
        thrd->set_state_ex(new_state_ex);
        if (new_state == pending) {
            work_items_.enqueue(thrd);
            cond_.notify_all();
        }
        return previous_state;
    }

    /// The get_state function is part of the thread related API and allows
    /// to query the state of one of the threads known to the threadmanager
    thread_state threadmanager::get_state(thread_id_type id) 
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_state() : terminated;
    }

    /// The get_description function is part of the thread related API and 
    /// allows to query the description of one of the threads known to the 
    /// threadmanager
    std::string threadmanager::get_description(thread_id_type id)
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_description() : "<unknown>";
    }

    /// This thread function is used by the at_timer thread below to trigger
    /// the required action.
    thread_state threadmanager::wake_timer_thread (thread_id_type id, 
        thread_state newstate, thread_state_ex newstate_ex, 
        thread_id_type timer_id) 
    {
        // first trigger the requested set_state 
        set_state(id, newstate, newstate_ex);

        // then re-activate the thread holding the deadline_timer
        set_state(timer_id, pending);
        return terminated;
    }

    /// This thread function initiates the required set_state action (on 
    /// behalf of one of the threadmanager#set_state functions).
    template <typename TimeType>
    thread_state threadmanager::at_timer (TimeType const& expire, 
        thread_id_type id, thread_state newstate, thread_state_ex newstate_ex)
    {
        // create timer firing in correspondence with given time
        boost::asio::deadline_timer t (timer_pool_.get_io_service(), expire);

        // create a new thread in suspended state, which will execute the 
        // requested set_state when timer fires and will re-awaken this thread, 
        // allowing the deadline_timer to go out of scope gracefully
        thread_self& self = get_self();
        thread_id_type wake_id = register_thread(boost::bind(
            &threadmanager::wake_timer_thread, this, id, newstate, newstate_ex,
            self.get_thread_id()), "wake_timer", suspended);

        // let the timer invoke the set_state on the new (suspended) thread
        t.async_wait(boost::bind(&threadmanager::set_state, this, wake_id, 
            pending, wait_timeout));

        // this waits for the thread to be executed when the timer fired
        self.yield(suspended);
        return terminated;
    }

    /// Set a timer to set the state of the given \a thread to the given 
    /// new value after it expired (at the given time)
    thread_id_type threadmanager::set_state (time_type const& expire_at, 
        thread_id_type id, thread_state newstate, thread_state_ex newstate_ex)
    {
        // this creates a new thread which creates the timer and handles the
        // requested actions
        thread_state (threadmanager::*f)(time_type const&, thread_id_type, 
                thread_state, thread_state_ex)
            = &threadmanager::at_timer<time_type>;

        return register_thread(
            boost::bind(f, this, expire_at, id, newstate, newstate_ex),
            "at_timer (expire at)");
    }

    /// Set a timer to set the state of the given \a thread to the given
    /// new value after it expired (after the given duration)
    thread_id_type threadmanager::set_state (duration_type const& from_now, 
        thread_id_type id, thread_state newstate, thread_state_ex newstate_ex)
    {
        // this creates a new thread which creates the timer and handles the
        // requested actions
        thread_state (threadmanager::*f)(duration_type const&, thread_id_type, 
                thread_state, thread_state_ex)
            = &threadmanager::at_timer<duration_type>;

        return register_thread(
            boost::bind(f, this, from_now, id, newstate, newstate_ex),
            "at_timer (from now)");
    }

    /// Retrieve the global id of the given thread
    naming::id_type threadmanager::get_thread_gid(thread_id_type id) 
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_gid() : naming::invalid_id;
    }

    // helper class for switching thread state in and out during execution
    class switch_status
    {
    public:
        switch_status (thread* t, thread_state new_state)
            : thread_(t), prev_state_(t->set_state(new_state))
        {}

        ~switch_status ()
        {
            thread_->set_state(prev_state_);
        }

        // allow to change the state the thread will be switched to after 
        // execution
        thread_state operator=(thread_state new_state)
        {
            return prev_state_ = new_state;
        }

        // allow to compare against the previous state of the thread
        bool operator== (thread_state rhs)
        {
            return prev_state_ == rhs;
        }

    private:
        thread* thread_;
        thread_state prev_state_;
    };

    ///////////////////////////////////////////////////////////////////////////
    inline bool threadmanager::cleanup_terminated()
    {
        if (!terminated_items_.empty()) {
            long delete_count = max_delete_count;   // delete only this much threads
            thread_id_type todelete;
            while (--delete_count && terminated_items_.dequeue(&todelete)) 
                thread_map_.erase(todelete);
        }
        return thread_map_.empty();
    }

    ///////////////////////////////////////////////////////////////////////////
    struct manage_counter_type
    {
        manage_counter_type()
          : status_(performance_counters::status_invalid_data)
        {
        }
        ~manage_counter_type()
        {
            if (performance_counters::status_invalid_data != status_) {
                error_code ec;
                remove_counter_type(info_, ec);   // ignore errors
            }
        }

        performance_counters::counter_status install(
            std::string const& name, performance_counters::counter_type type, 
            error_code& ec = throws)
        {
            BOOST_ASSERT(performance_counters::status_invalid_data == status_);
            info_.fullname_ = name;
            info_.type_ = type;
            status_ = add_counter_type(info_, ec);
            return status_;
        }

        performance_counters::counter_status status_;
        performance_counters::counter_info info_;
    };

    ///////////////////////////////////////////////////////////////////////////
    // main function executed by all OS threads managed by this threadmanager
    void threadmanager::tfunc(std::size_t num_thread)
    {
        // needs to be done as the first thing, otherwise logging won't work
        if (start_thread_)    // notify runtime system of started thread
            start_thread_();

        {
            manage_counter_type counter_type;
            if (0 == num_thread) {
                // register counter types
                error_code ec;
                counter_type.install("/queue/length", 
                    performance_counters::counter_raw, ec);   // doesn't throw
                if (ec) {
                    LTM_(info) << "tfunc(" << num_thread << "): failed to install "
                        "counter type '/queue/length': " << ec.get_message();
                }
            }

            LTM_(info) << "tfunc(" << num_thread << "): start";
            std::size_t num_px_threads = 0;
            try {
                num_px_threads = tfunc_impl(num_thread);
            }
            catch (hpx::exception const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught hpx::exception: " 
                        << e.what() << ", aborted thread execution";
                report_error(boost::current_exception());
                return;
            }
            catch (boost::system::system_error const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught boost::system::system_error: " 
                        << e.what() << ", aborted thread execution";
                report_error(boost::current_exception());
                return;
            }
            catch (std::exception const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught std::exception: " 
                        << e.what() << ", aborted thread execution";
                report_error(boost::current_exception());
                return;
            }
            catch (...) {
                LFATAL_ << "tfunc(" << num_thread << "): caught unexpected "
                    "exception, aborted thread execution";
                report_error(boost::current_exception());
                return;
            }

            LTM_(fatal) << "tfunc(" << num_thread << "): end, executed " 
                       << num_px_threads << " HPX threads";
        }

        if (stop_) 
            stop_();

        if (0 == num_thread) {
            // print queue statistics
            log_fifo_statistics(work_items_);
            log_fifo_statistics(terminated_items_);
            log_fifo_statistics(new_tasks_);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        // This try_lock_wrapper is essentially equivalent to the template 
        // boost::thread::detail::try_lock_wrapper with the one exception, that
        // the lock() function always calls base::try_lock(). This allows us to 
        // skip lock acquisition while exiting the condition variable.
        template<typename Mutex>
        class try_lock_wrapper
          : public boost::detail::try_lock_wrapper<Mutex>
        {
            typedef boost::detail::try_lock_wrapper<Mutex> base;

        public:
            explicit try_lock_wrapper(Mutex& m):
                base(m, boost::try_to_lock)
            {}

            void lock()
            {
                base::try_lock();       // this is different
            }
        };

        ///////////////////////////////////////////////////////////////////////
        // debug helper function, logs all suspended threads
        void dump_suspended_threads(boost::ptr_map<thread_id_type, threads::thread>& tm)
        {
            typedef boost::ptr_map<thread_id_type, threads::thread> thread_map_type;

            thread_map_type::const_iterator end = tm.end();
            for (thread_map_type::const_iterator it = tm.begin(); it != end; ++it)
            {
                threads::thread const* thrd = (*it).second;
                thread_state state = thrd->get_state();
                if (suspended == state)
                {
                    LTM_(error) << "suspended thread(" << (*it).first << "): "
                                << thrd->get_description();
                }
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////
    // This returns the current length of the queues (work items and new items)
    boost::int64_t threadmanager::get_queue_lengths() const
    {
        return work_items_.count_ + new_tasks_.count_;
    }

    ///////////////////////////////////////////////////////////////////////////
    struct manage_counter
    {
        ~manage_counter()
        {
            if (counter_) {
                error_code ec;
                remove_counter(info_, counter_, ec);
            }
        }

        performance_counters::counter_status install(
            std::string const& name, boost::function<boost::int64_t()> f, 
            error_code& ec = throws)
        {
            BOOST_ASSERT(!counter_);
            info_.fullname_ = name;
            return add_counter(info_, f, counter_, ec);
        }

        performance_counters::counter_info info_;
        naming::id_type counter_;
    };

    ///////////////////////////////////////////////////////////////////////////
    std::size_t threadmanager::tfunc_impl(std::size_t num_thread)
    {
#if HPX_DEBUG != 0
        ++thread_count_;
#endif
        std::size_t num_px_threads = 0;
        util::time_logger tl1("tfunc", num_thread);
        util::time_logger tl2("tfunc1", num_thread);
        util::time_logger tl3("tfunc2", num_thread);

        // the thread with number zero is the master
        bool is_master_thread = (0 == num_thread) ? true : false;
        set_affinity(num_thread);     // set affinity on Linux systems

        // register performance counters
        manage_counter queue_length_counter; 
        if (is_master_thread) {
            std::string name("/queue(threadmanager)/length");
            queue_length_counter.install(name, 
                boost::bind(&threadmanager::get_queue_lengths, this));
        }

        // run the work queue
        boost::coroutines::prepare_main_thread main_thread;
        while (true) {
            // Get the next PX thread from the queue
            thread* thrd = NULL;
            if (work_items_.dequeue(&thrd)) {
                tl1.tick();

                // Only pending PX threads will be executed.
                // Any non-pending PX threads are leftovers from a set_state() 
                // call for a previously pending PX thread (see comments above).
                thread_state state = thrd->get_state();

                LTM_(debug) << "tfunc(" << num_thread << "): "
                           << "thread(" << thrd->get_thread_id() << "), " 
                           << "description(" << thrd->get_description() << "), "
                           << "old state(" << get_thread_state_name(state) << ")";

                if (pending == state) {
                    // switch the state of the thread to active and back to 
                    // what the thread reports as its return value

                    switch_status thrd_stat (thrd, active);
                    if (thrd_stat == pending) {
                        // thread returns new required state
                        // store the returned state in the thread
                        tl3.tick();
                        thrd_stat = state = (*thrd)();
                        tl3.tock();
                        ++num_px_threads;
                    }

                }   // this stores the new state in the PX thread

                LTM_(debug) << "tfunc(" << num_thread << "): "
                           << "thread(" << thrd->get_thread_id() << "), "
                           << "description(" << thrd->get_description() << "), "
                           << "new state(" << get_thread_state_name(state) << ")";

                // Re-add this work item to our list of work items if the PX
                // thread should be re-scheduled. If the PX thread is suspended 
                // now we just keep it in the map of threads.
                if (state == pending) {
                    work_items_.enqueue(thrd);
                    cond_.notify_all();
                }

                // Remove the mapping from thread_map_ if PX thread is depleted 
                // or terminated, this will delete the PX thread as all 
                // references go out of scope.
                // FIXME: what has to be done with depleted PX threads?
                if (state == depleted || state == terminated) {
                    // all OS threads put their terminated PX threads into a 
                    // separate queue
                    thread_id_type id = thrd->get_thread_id();
                    terminated_items_.enqueue(id);
                }

                tl1.tock();
            }

            // only one dedicated OS thread is allowed to acquire the 
            // lock for the purpose of inserting the new threads into the 
            // thread-map and deleting all terminated threads
            if (is_master_thread) {
                {
                    // first clean up terminated threads
                    detail::try_lock_wrapper<mutex_type> lk(mtx_);
                    if (lk) {
                        // no point in having a thread waiting on the lock 
                        // while another thread is doing the maintenance
                        cleanup_terminated();

                        // now, add new threads from the queue of task descriptions
                        add_new_if_possible();    // calls notify_all
                    }
                }
            }

            // if nothing else has to be done either wait or terminate
            bool terminate = false;
            while (work_items_.empty()) {
                // No obvious work has to be done, so a lock won't hurt too much
                // but we lock only one of the threads, assuming this thread
                // will do the maintenance
                //
                // We prefer to exit this while loop (some kind of very short 
                // busy waiting) to blocking on this lock. Locking fails either
                // when a thread is currently doing thread maintenance, which
                // means there might be new work, or the thread owning the lock 
                // just falls through to the wait below (no work is available)
                // in which case the current thread (which failed to acquire 
                // the lock) will just retry to enter this loop.
                detail::try_lock_wrapper<mutex_type> lk(mtx_);
                if (!lk)
                    break;            // avoid long wait on lock

                // this thread acquired the lock, do maintenance and finally
                // call wait() if not work is available
                LTM_(info) << "tfunc(" << num_thread << "): queues empty"
                           << ", threads left: " << thread_map_.size();

                // stop running after all PX threads have been terminated
                if (!add_new_always() && !running_) {
                    // Before exiting each of the OS threads deletes the 
                    // remaining terminated PX threads 
                    if (cleanup_terminated()) {
                        // we don't have any registered work items anymore
                        cond_.notify_all();   // notify possibly waiting threads
                        terminate = true;
                        break;                // terminate scheduling loop
                    }

                    LTM_(info) << "tfunc(" << num_thread 
                               << "): threadmap not empty";
                }
                else {
                    cleanup_terminated();
                }

                // Wait until somebody needs some action (if no new work 
                // arrived in the meantime).
                // Ask again if queues are empty to avoid race conditions (we 
                // needed to lock anyways...), this way no notify_all() gets lost
                if (work_items_.empty())
                {
                    LTM_(info) << "tfunc(" << num_thread 
                               << "): queues empty, entering wait";

                    if (LHPX_ENABLED(error) && new_tasks_.empty())
                        detail::dump_suspended_threads(thread_map_);

                    bool timed_out = false;
                    {
                        namespace bpt = boost::posix_time;
                        tl2.tick();
                        timed_out = cond_.timed_wait(lk, bpt::milliseconds(5));
                        tl2.tock();
                    }

                    LTM_(info) << "tfunc(" << num_thread << "): exiting wait";

                    // make sure all pending new threads are properly queued
                    // but do that only if the lock has been acquired while 
                    // exiting the condition.wait() above
                    if ((lk && add_new_always()) || timed_out)
                        break;
                }
            }
            if (terminate)
                break;
        }

#if HPX_DEBUG != 0
        // the last OS thread is allowed to exit only if no more PX threads exist
        BOOST_ASSERT(0 != --thread_count_ || thread_map_.empty());
#endif
        return num_px_threads;
    }

    ///////////////////////////////////////////////////////////////////////////
    bool threadmanager::run(std::size_t num_threads) 
    {
        LTM_(info) << "run: creating " << num_threads << " OS thread(s)";

        if (0 == num_threads) {
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager::run", "number of threads is zero");
        }

        mutex_type::scoped_lock lk(mtx_);
        if (!threads_.empty() || running_) 
            return true;    // do nothing if already running

        LTM_(info) << "run: running timer pool"; 
        timer_pool_.run(false);

        running_ = false;
        try {
            // run threads and wait for initialization to complete
            running_ = true;
            while (num_threads-- != 0) {
                LTM_(info) << "run: create OS thread: " << num_threads; 

                // create a new thread
                threads_.push_back(new boost::thread(
                    boost::bind(&threadmanager::tfunc, this, num_threads)));

                // set the new threads affinity (on Windows systems)
                set_affinity(threads_.back(), num_threads);
            }

            // start timer pool as well
            timer_pool_.run(false);
        }
        catch (std::exception const& e) {
            LTM_(fatal) << "run: failed with:" << e.what(); 
            stop();
            threads_.clear();
            return false;
        }

        LTM_(info) << "run: running"; 
        return running_;
    }

    void threadmanager::stop (bool blocking)
    {
        LTM_(info) << "stop: blocking(" << std::boolalpha << blocking << ")"; 

        mutex_type::scoped_lock l(mtx_);
        if (!threads_.empty()) {
            if (running_) {
                LTM_(info) << "stop: set running_ = false"; 
                running_ = false;
                cond_.notify_all();         // make sure we're not waiting
            }

            if (blocking) {
                for (std::size_t i = 0; i < threads_.size(); ++i) 
                {
                    // make sure no OS thread is waiting
                    LTM_(info) << "stop: notify_all"; 
                    cond_.notify_all();

                    LTM_(info) << "stop(" << i << "): join"; 

                    // unlock the lock while joining
                    util::unlock_the_lock<mutex_type::scoped_lock> ul(l);
                    threads_[i].join();
                }
                threads_.clear();
            }

            LTM_(info) << "stop: stopping timer pool"; 
            timer_pool_.stop();             // stop timer pool as well
            if (blocking) 
                timer_pool_.join();
        }
    }

}}
