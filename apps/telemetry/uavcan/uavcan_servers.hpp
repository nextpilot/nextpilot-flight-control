/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once


#include <uavcan/protocol/dynamic_node_id_server/centralized.hpp>
#include <uavcan/protocol/file_server.hpp>
#include <uavcan/protocol/firmware_update_trigger.hpp>
#include <uavcan/protocol/node_info_retriever.hpp>
#include <uavcan/protocol/node_status_monitor.hpp>
#include <uavcan_posix/basic_file_server_backend.hpp>
#include <uavcan_posix/dynamic_node_id_server/file_event_tracer.hpp>
#include <uavcan_posix/dynamic_node_id_server/file_storage_backend.hpp>
#include <uavcan_posix/firmware_version_checker.hpp>

#include "uavcan_module.hpp"

/**
 * @file uavcan_servers.hpp
 *
 * Defines basic functionality of UAVCAN node.
 *
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 * @author David Sidrane <david_s5@nscdg.com>
 */

/**
 * A UAVCAN Server Sub node.
 */
class UavcanServers {
public:
    UavcanServers(uavcan::INode &node, uavcan::NodeInfoRetriever &node_info_retriever);
    ~UavcanServers() = default;

    int init();

    bool guessIfAllDynamicNodesAreAllocated() {
        return _server_instance.guessIfAllDynamicNodesAreAllocated();
    }

private:
    void unpackFwFromROMFS(const char *sd_path, const char *romfs_path);
    void migrateFWFromRoot(const char *sd_path, const char *sd_root_path);
    int  copyFw(const char *dst, const char *src);

    uavcan_posix::dynamic_node_id_server::FileEventTracer    _tracer;
    uavcan_posix::dynamic_node_id_server::FileStorageBackend _storage_backend;
    uavcan_posix::FirmwareVersionChecker                     _fw_version_checker;
    uavcan::dynamic_node_id_server::CentralizedServer        _server_instance; ///< server singleton pointer
    uavcan_posix::BasicFileServerBackend                     _fileserver_backend;
    uavcan::FirmwareUpdateTrigger                            _fw_upgrade_trigger;
    uavcan::BasicFileServer                                  _fw_server;

    uavcan::NodeInfoRetriever &_node_info_retriever;
};
