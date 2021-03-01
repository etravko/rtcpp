#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "emscripten.h"
#include "emscripten/bind.h"

#include "rtcpp.h"

const std::string MOUNT_POINT = "rtcpptmp";

bool mountFS(const std::string& path) {
    return EM_ASM_INT({
        try {
            let mountPoint = UTF8ToString($0);
            let path = UTF8ToString($1);

            if (!FS.analyzePath(mountPoint).exists) {
                FS.mkdir(mountPoint);    
            }
            
            FS.mount(NODEFS, { root: path }, mountPoint);

            return true;
        } catch (e) {
            console.log("Failed to mount folder " + e);
        }

        return false;

    }, MOUNT_POINT.c_str(), path.c_str());
}

bool unmountFS() {
    return EM_ASM_INT({
        try {
            let mountPoint = UTF8ToString($0);
            FS.unmount(mountPoint);            
        } catch (e) {
            console.log("Failed to unmount folder " + e);
        }

    }, MOUNT_POINT.c_str());
}

std::string getFSFileName(const std::string& name) {
    return MOUNT_POINT + "/" + name;
}

std::string analyzeRTPDump(const std::string path, const std::string fileName)
{
    std::string output;

    if (mountFS(path)) {
        try {
            rtcpp::FileDataSource ds(getFSFileName(fileName));
            output = rtcpp::AnalyzeRTPDump(ds);
        } catch (std::exception& e) {
            std::cout << "Failed to process data - " << e.what();
        }

        unmountFS();
    }

    return output;
}

std::string transformRTPDump(std::string path, std::string inFile, std::string outFile, std::string cfg) {
    std::stringstream result;

    if (mountFS(path)) {

        try {
            rtcpp::FileDataSource src(getFSFileName(inFile));
            rtcpp::FileDataSink snk(getFSFileName(outFile));
            rtcpp::TransformRTPDump(src, snk, cfg);
        } catch (std::exception& e) {
            result << "Failed to process data - " << e.what() 
                << " OutFile: " << getFSFileName(outFile) 
                << " InFile: " << getFSFileName(inFile);
        }

        unmountFS();
    } else {
        result << "Failed to mount FS";
    }

    return result.str();
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("analyzeRTPDump", &analyzeRTPDump);
    emscripten::function("transformRTPDump", &transformRTPDump);
}