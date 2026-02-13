#pragma once

#include <sdk/transport.hpp>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace sdk {

class cli {
public:
    static constexpr size_t kMaxLine   = 128;
    static constexpr size_t kMaxArgs   = 12;
    static constexpr size_t kQueueSize = 4;

    struct command {
        const char* name;
        int (*fn)(cli& c, int argc, char** argv);
        const char* help;
    };

public:
    explicit cli(transport& t)
        : t_(t) {}

    void set_echo(bool en) { echo_ = en; }
    void set_prompt(const char* p) { prompt_ = p ? p : ""; }
    void set_commands(const command* cmds, size_t n) { cmds_ = cmds; n_cmds_ = n; }

    void begin() {
        line_len_ = 0;
        clear_queue_();
        print_prompt();
    }

    void poll() {
        rx_step_();
        if (!executing_) {
            if (dequeue_line_(exec_line_)) {
                executing_ = true;
                execute_one_(exec_line_);
                executing_ = false;
                print_prompt();
            }
        }
    }

    void print_prompt() {
        write_str(prompt_);
    }

    void write_str(const char* s) {
        if (!s) return;
        write_bytes_(reinterpret_cast<const uint8_t*>(s), strlen(s));
    }

    void write_bytes(const uint8_t* b, size_t n) {
        write_bytes_(b, n);
    }

    void println(const char* s) {
        if (s) write_str(s);
        write_str("\r\n");
    }

    void help() {
        if (!cmds_) {
            println("no commands registered");
            return;
        }
        for (size_t i = 0; i < n_cmds_; ++i) {
            write_str(cmds_[i].name);
            write_str(" - ");
            write_str(cmds_[i].help ? cmds_[i].help : "");
            write_str("\r\n");
        }
    }

private:
    void rx_step_() {
        uint8_t buf[64];
        const size_t n = t_.available();
        if (n == 0) return;

        const size_t to_read = (n < sizeof(buf)) ? n : sizeof(buf);
        const size_t got = t_.read(buf, to_read);

        for (size_t i = 0; i < got; ++i) {
            on_char_(static_cast<char>(buf[i]));
        }
    }

    void on_char_(char ch) {
        if (ch == '\r' || ch == '\n') {
            if (last_was_cr_ && ch == '\n') {
                last_was_cr_ = false;
                return;
            }
            last_was_cr_ = (ch == '\r');

            if (echo_) write_str("\r\n");

            if (line_len_ > 0) {
                line_[line_len_] = '\0';
                enqueue_line_(line_);
                line_len_ = 0;
            } else {
                print_prompt();
            }
            return;
        }

        last_was_cr_ = false;

        if (ch == '\b' || ch == 0x7F) {
            if (line_len_ > 0) {
                line_len_--;
                line_[line_len_] = '\0';
                if (echo_) write_str("\b \b");
            }
            return;
        }

        if (ch < 0x20 || ch == 0x7F) {
            return;
        }

        if (line_len_ + 1 >= kMaxLine) {
            if (echo_) write_str("\a");
            return;
        }

        line_[line_len_++] = ch;
        line_[line_len_] = '\0';

        if (echo_) {
            uint8_t c = static_cast<uint8_t>(ch);
            write_bytes_(&c, 1);
        }
    }

    void execute_one_(char* line) {
        trim_inplace_(line);
        if (line[0] == '\0') return;

        int argc = 0;
        char* argv[kMaxArgs]{};

        split_args_(line, argc, argv);

        if (argc == 0) return;

        if (strcmp(argv[0], "help") == 0) {
            help();
            return;
        }

        if (!cmds_) {
            println("no commands registered");
            return;
        }

        for (size_t i = 0; i < n_cmds_; ++i) {
            if (strcmp(argv[0], cmds_[i].name) == 0) {
                (void)cmds_[i].fn(*this, argc, argv);
                return;
            }
        }

        write_str("unknown: ");
        println(argv[0]);
    }

    static void trim_inplace_(char* s) {
        if (!s) return;

        size_t len = strlen(s);
        size_t start = 0;
        while (start < len && (s[start] == ' ' || s[start] == '\t')) start++;

        size_t end = len;
        while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t')) end--;

        if (start != 0) {
            size_t j = 0;
            for (size_t i = start; i < end; ++i) s[j++] = s[i];
            s[j] = '\0';
        } else {
            s[end] = '\0';
        }
    }

    static void split_args_(char* line, int& argc, char** argv) {
        argc = 0;
        char* p = line;

        while (*p != '\0' && argc < static_cast<int>(kMaxArgs)) {
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '\0') break;

            argv[argc++] = p;

            while (*p != '\0' && *p != ' ' && *p != '\t') p++;
            if (*p == '\0') break;
            *p++ = '\0';
        }
    }

    bool enqueue_line_(const char* s) {
        if (queue_count_ == kQueueSize) {
            println("cli: queue full");
            return false;
        }

        const size_t len = strlen(s);
        const size_t copy_len = (len < (kMaxLine - 1)) ? len : (kMaxLine - 1);

        memcpy(queue_[queue_head_], s, copy_len);
        queue_[queue_head_][copy_len] = '\0';

        queue_head_ = (queue_head_ + 1u) % kQueueSize;
        queue_count_++;
        return true;
    }

    bool dequeue_line_(char* out) {
        if (queue_count_ == 0) return false;

        strncpy(out, queue_[queue_tail_], kMaxLine);
        out[kMaxLine - 1] = '\0';

        queue_tail_ = (queue_tail_ + 1u) % kQueueSize;
        queue_count_--;
        return true;
    }

    void clear_queue_() {
        queue_head_ = 0;
        queue_tail_ = 0;
        queue_count_ = 0;
        for (size_t i = 0; i < kQueueSize; ++i) queue_[i][0] = '\0';
    }

    void write_bytes_(const uint8_t* b, size_t n) {
        if (!b || n == 0) return;
        (void)t_.write(b, n);
    }

private:
    transport& t_;

    const command* cmds_ = nullptr;
    size_t n_cmds_ = 0;

    bool echo_ = true;
    const char* prompt_ = "> ";

    char line_[kMaxLine]{};
    size_t line_len_ = 0;
    bool last_was_cr_ = false;

    char queue_[kQueueSize][kMaxLine]{};
    uint32_t queue_head_ = 0;
    uint32_t queue_tail_ = 0;
    uint32_t queue_count_ = 0;

    bool executing_ = false;
    char exec_line_[kMaxLine]{};
};

} // namespace sdk
