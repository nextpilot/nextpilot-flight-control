/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "log_writer.h"

namespace nextpilot {
namespace logger {

LogWriter::LogWriter(Backend configured_backend, size_t file_buffer_size) :
    _backend(configured_backend) {
    if (configured_backend & BackendFile) {
        _log_writer_file_for_write = _log_writer_file = new LogWriterFile(file_buffer_size);

        if (!_log_writer_file) {
            PX4_ERR("LogWriterFile allocation failed");
        }
    }

    if (configured_backend & BackendMavlink) {
        _log_writer_mavlink_for_write = _log_writer_mavlink = new LogWriterMavlink();

        if (!_log_writer_mavlink) {
            PX4_ERR("LogWriterMavlink allocation failed");
        }
    }
}

bool LogWriter::init() {
    if (_log_writer_file) {
        if (!_log_writer_file->init()) {
            PX4_ERR("alloc failed");
            return false;
        }

        int ret = _log_writer_file->thread_start();

        if (ret) {
            PX4_ERR("failed to create writer thread (%i)", ret);
            return false;
        }
    }

    if (_log_writer_mavlink) {
        if (!_log_writer_mavlink->init()) {
            PX4_ERR("mavlink init failed");
            return false;
        }
    }

    return true;
}

LogWriter::~LogWriter() {
    if (_log_writer_file) {
        delete (_log_writer_file);
    }

    if (_log_writer_mavlink) {
        delete (_log_writer_mavlink);
    }
}

bool LogWriter::is_started(LogType type) const {
    bool ret = false;

    if (_log_writer_file) {
        ret = _log_writer_file->is_started(type);
    }

    if (_log_writer_mavlink && type == LogType::Full) {
        ret = ret || _log_writer_mavlink->is_started();
    }

    return ret;
}

bool LogWriter::is_started(LogType type, Backend query_backend) const {
    if (query_backend == BackendFile && _log_writer_file) {
        return _log_writer_file->is_started(type);
    }

    if (query_backend == BackendMavlink && _log_writer_mavlink && type == LogType::Full) {
        return _log_writer_mavlink->is_started();
    }

    return false;
}

void LogWriter::start_log_file(LogType type, const char *filename) {
    if (_log_writer_file) {
        _log_writer_file->start_log(type, filename);
    }
}

void LogWriter::stop_log_file(LogType type) {
    if (_log_writer_file) {
        _log_writer_file->stop_log(type);
    }
}

void LogWriter::start_log_mavlink() {
    if (_log_writer_mavlink) {
        _log_writer_mavlink->start_log();
    }
}

void LogWriter::stop_log_mavlink() {
    if (_log_writer_mavlink) {
        _log_writer_mavlink->stop_log();
    }
}

void LogWriter::thread_stop() {
    if (_log_writer_file) {
        _log_writer_file->thread_stop();
    }
}

int LogWriter::write_message(LogType type, void *ptr, size_t size, uint64_t dropout_start) {
    int ret_file = 0, ret_mavlink = 0;

    if (_log_writer_file_for_write) {
        ret_file = _log_writer_file_for_write->write_message(type, ptr, size, dropout_start);
    }

    if (_log_writer_mavlink_for_write && type == LogType::Full) {
        ret_mavlink = _log_writer_mavlink_for_write->write_message(ptr, size);
    }

    // file backend errors takes precedence
    if (ret_file != 0) {
        return ret_file;
    }

    return ret_mavlink;
}

void LogWriter::select_write_backend(Backend sel_backend) {
    if (sel_backend & BackendFile) {
        _log_writer_file_for_write = _log_writer_file;

    } else {
        _log_writer_file_for_write = nullptr;
    }

    if (sel_backend & BackendMavlink) {
        _log_writer_mavlink_for_write = _log_writer_mavlink;

    } else {
        _log_writer_mavlink_for_write = nullptr;
    }
}

}
} // namespace nextpilot::logger
