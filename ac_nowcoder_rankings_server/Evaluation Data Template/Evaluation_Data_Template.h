//
// Created by fsptd on 25-5-15.
//
#include <string>
#ifndef EVALUATION_DATA_TEMPLATE_H
#define EVALUATION_DATA_TEMPLATE_H

namespace ac_nowcoder_rankings_server {
    class Evaluation_Data_Template {
    public:
        [[nodiscard]] long long int get_memory() const {
            return memory;
        }

        void set_memory(long long int memory) {
            this->memory = memory;
        }

        [[nodiscard]] long long int get_length() const {
            return length;
        }

        void set_length(long long int length) {
            this->length = length;
        }

        [[nodiscard]] std::string get_index() const {
            return index;
        }

        void set_index(const std::string &index) {
            this->index = index;
        }

        [[nodiscard]] std::string get_language() const {
            return language;
        }

        void set_language(const std::string &language) {
            this->language = language;
        }

        [[nodiscard]] std::string get_user_name() const {
            return userName;
        }

        void set_user_name(const std::string &user_name) {
            userName = user_name;
        }

        [[nodiscard]] long long int get_user_id() const {
            return userId;
        }

        void set_user_id(long long int user_id) {
            userId = user_id;
        }

        [[nodiscard]] long long int get_color_level() const {
            return colorLevel;
        }

        void set_color_level(long long int color_level) {
            colorLevel = color_level;
        }

        [[nodiscard]] std::string get_status_message() const {
            return statusMessage;
        }

        void set_status_message(const std::string &status_message) {
            statusMessage = status_message;
        }

        [[nodiscard]] long long int get_submission_id() const {
            return submissionId;
        }

        void set_submission_id(long long int submission_id) {
            submissionId = submission_id;
        }

        [[nodiscard]] long long int get_submit_time() const {
            return submitTime;
        }

        void set_submit_time(long long int submit_time) {
            submitTime = submit_time;
        }

        [[nodiscard]] long long int get_time() const {
            return time;
        }

        void set_time(long long int time) {
            this->time = time;
        }

        [[nodiscard]] long long int get_problem_id() const {
            return problemId;
        }

        void set_problem_id(long long int problem_id) {
            problemId = problem_id;
        }

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

        Evaluation_Data_Template() = default;

        ~Evaluation_Data_Template() = default;

    private:
        long long int memory;
        long long int length;
        std::string index;
        std::string language;
        std::string userName;
        long long int userId;
        long long int colorLevel;
        std::string statusMessage;
        long long int submissionId;
        long long int submitTime;
        long long int time;
        long long int problemId;
    };
} // ac_nowcoder_rankings_server

#endif //EVALUATION_DATA_TEMPLATE_H
