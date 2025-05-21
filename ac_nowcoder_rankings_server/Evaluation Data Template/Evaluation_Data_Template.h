//
// Created by fsptd on 25-5-15.
//
#include <string>
#ifndef EVALUATION_DATA_TEMPLATE_H
#define EVALUATION_DATA_TEMPLATE_H

namespace ac_nowcoder_rankings_server {
    /**
     * @class Evaluation_Data_Template
     * @brief 该类用于存储和操作评测数据的模板信息。
     */
    class Evaluation_Data_Template {
    public:
        /**
         * @brief 获取内存使用量。
         * @return 返回内存使用量。
         */
        [[nodiscard]] long long int get_memory() const {
            return memory;
        }

        /**
         * @brief 设置内存使用量。
         * @param memory 内存使用量。
         */
        void set_memory(long long int memory) {
            this->memory = memory;
        }

        /**
         * @brief 获取代码长度。
         * @return 返回代码长度。
         */
        [[nodiscard]] long long int get_length() const {
            return length;
        }

        /**
         * @brief 设置代码长度。
         * @param length 代码长度。
         */
        void set_length(long long int length) {
            this->length = length;
        }

        /**
         * @brief 获取索引。
         * @return 返回索引。
         */
        [[nodiscard]] std::string get_index() const {
            return index;
        }

        /**
         * @brief 设置索引。
         * @param index 索引。
         */
        void set_index(const std::string &index) {
            this->index = index;
        }

        /**
         * @brief 获取编程语言。
         * @return 返回编程语言。
         */
        [[nodiscard]] std::string get_language() const {
            return language;
        }

        /**
         * @brief 设置编程语言。
         * @param language 编程语言。
         */
        void set_language(const std::string &language) {
            this->language = language;
        }

        /**
         * @brief 获取用户名。
         * @return 返回用户名。
         */
        [[nodiscard]] std::string get_user_name() const {
            return userName;
        }

        /**
         * @brief 设置用户名。
         * @param user_name 用户名。
         */
        void set_user_name(const std::string &user_name) {
            userName = user_name;
        }

        /**
         * @brief 获取用户ID。
         * @return 返回用户ID。
         */
        [[nodiscard]] long long int get_user_id() const {
            return userId;
        }

        /**
         * @brief 设置用户ID。
         * @param user_id 用户ID。
         */
        void set_user_id(long long int user_id) {
            userId = user_id;
        }

        /**
         * @brief 获取颜色等级。
         * @return 返回颜色等级。
         */
        [[nodiscard]] long long int get_color_level() const {
            return colorLevel;
        }

        /**
         * @brief 设置颜色等级。
         * @param color_level 颜色等级。
         */
        void set_color_level(long long int color_level) {
            colorLevel = color_level;
        }

        /**
         * @brief 获取状态信息。
         * @return 返回状态信息。
         */
        [[nodiscard]] std::string get_status_message() const {
            return statusMessage;
        }

        /**
         * @brief 设置状态信息。
         * @param status_message 状态信息。
         */
        void set_status_message(const std::string &status_message) {
            statusMessage = status_message;
        }

        /**
         * @brief 获取提交ID。
         * @return 返回提交ID。
         */
        [[nodiscard]] long long int get_submission_id() const {
            return submissionId;
        }

        /**
         * @brief 设置提交ID。
         * @param submission_id 提交ID。
         */
        void set_submission_id(long long int submission_id) {
            submissionId = submission_id;
        }

        /**
         * @brief 获取提交时间。
         * @return 返回提交时间。
         */
        [[nodiscard]] long long int get_submit_time() const {
            return submitTime;
        }

        /**
         * @brief 设置提交时间。
         * @param submit_time 提交时间。
         */
        void set_submit_time(long long int submit_time) {
            submitTime = submit_time;
        }

        /**
         * @brief 获取时间。
         * @return 返回时间。
         */
        [[nodiscard]] long long int get_time() const {
            return time;
        }

        /**
         * @brief 设置时间。
         * @param time 时间。
         */
        void set_time(long long int time) {
            this->time = time;
        }

        /**
         * @brief 获取问题ID。
         * @return 返回问题ID。
         */
        [[nodiscard]] long long int get_problem_id() const {
            return problemId;
        }

        /**
         * @brief 设置问题ID。
         * @param problem_id 问题ID。
         */
        void set_problem_id(long long int problem_id) {
            problemId = problem_id;
        }

        /**
         * @brief 构造函数，用于初始化所有成员变量。
         * @param memory 内存使用量。
         * @param length 代码长度。
         * @param index 索引。
         * @param language 编程语言。
         * @param user_name 用户名。
         * @param user_id 用户ID。
         * @param color_level 颜色等级。
         * @param status_message 状态信息。
         * @param submission_id 提交ID。
         * @param submit_time 提交时间。
         * @param time 时间。
         * @param problem_id 问题ID。
         */
        Evaluation_Data_Template(  long long int memory,   long long int length, const std::string &index, const std::string &language,
                                   const std::string &user_name, long long int user_id, long long int color_level,
                                   const std::string &status_message, long long int submission_id, long long int submit_time,
                                   long long int time, long long int problem_id)
            : memory(memory),
              length(length),
              index(index),
              language(language),
              userName(user_name),
              userId(user_id),
              colorLevel(color_level),
              statusMessage(status_message),
              submissionId(submission_id),
              submitTime(submit_time),
              time(time),
              problemId(problem_id) {
        }

        /**
         * @brief 默认构造函数。
         */
        Evaluation_Data_Template() = default;

        /**
         * @brief 默认析构函数。
         */
        ~Evaluation_Data_Template() = default;

    private:
        long long int memory;        ///< 内存使用量
        long long int length;        ///< 代码长度
        std::string index;           ///< 索引
        std::string language;        ///< 编程语言
        std::string userName;        ///< 用户名
        long long int userId;        ///< 用户ID
        long long int colorLevel;    ///< 颜色等级
        std::string statusMessage;   ///< 状态信息
        long long int submissionId;  ///< 提交ID
        long long int submitTime;    ///< 提交时间
        long long int time;          ///< 时间
        long long int problemId;     ///< 问题ID
    };
} // ac_nowcoder_rankings_server

#endif //EVALUATION_DATA_TEMPLATE_H
