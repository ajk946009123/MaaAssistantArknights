#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "Utils/AsstMsg.h"
#include "Utils/AsstTypes.h"

typedef unsigned char uchar;

namespace cv
{
    class Mat;
}

namespace asst
{
    class Controller;
    class PackageTask;
    class RuntimeStatus;

    class Assistant
    {
    public:
        using TaskId = int;

        Assistant(AsstApiCallback callback = nullptr, void* callback_arg = nullptr);
        ~Assistant();

        // 设置实例级参数
        bool set_instance_option(InstanceOptionKey key, const std::string& value);
        // 连接adb
        bool connect(const std::string& adb_path, const std::string& address, const std::string& config);

        TaskId append_task(const std::string& type, const std::string& params);
        bool set_task_params(TaskId task_id, const std::string& params);

        // 开始执行任务队列
        bool start(bool block = true);
        // 停止任务队列并清空
        bool stop(bool block = true);
        // 是否正在运行
        bool running();

        std::vector<uchar> get_image() const;
        bool ctrler_click(int x, int y);
        std::string get_uuid() const;
        std::vector<TaskId> get_tasks_list() const;

    private:
        void working_proc();
        void msg_proc();
        static void task_callback(AsstMsg msg, const json::value& detail, void* custom_arg);

        void append_callback(AsstMsg msg, json::value detail);
        void clear_cache();
        bool inited() const noexcept;

        std::string m_uuid;

        std::shared_ptr<Controller> m_ctrler = nullptr;
        std::shared_ptr<RuntimeStatus> m_status = nullptr;

        bool m_thread_exit = false;
        std::list<std::pair<TaskId, std::shared_ptr<PackageTask>>> m_tasks_list;
        inline static TaskId m_task_id = 0; // 进程级唯一
        AsstApiCallback m_callback = nullptr;
        void* m_callback_arg = nullptr;

        bool m_thread_idle = true;
        mutable std::mutex m_mutex;
        std::condition_variable m_condvar;

        std::queue<std::pair<AsstMsg, json::value>> m_msg_queue;
        std::mutex m_msg_mutex;
        std::condition_variable m_msg_condvar;

        std::thread m_msg_thread;
        std::thread m_working_thread;
    };
}
