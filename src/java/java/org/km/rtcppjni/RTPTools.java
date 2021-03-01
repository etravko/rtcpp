package org.km.rtcppjni;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

public class RTPTools {

    static {
        boolean tryLoadJar = false;
        try {
            System.loadLibrary("rtcppjni");
        } catch (UnsatisfiedLinkError e) {
            tryLoadJar = true;
        }

        if (tryLoadJar) {
            try {
                loadLibFromJar();
            } catch (IOException ioException) {
                throw new RuntimeException("Failed to load rtcppjni " + ioException.getMessage());
            }
        }
    }

    static void loadLibFromJar() throws IOException {
        InputStream fileStream = RTPTools.class.getResourceAsStream("/libs/rtcppjni.dll");
        Path tmpFilePath = Files.createTempFile("lib", ".dll");

        Files.copy(fileStream, tmpFilePath, StandardCopyOption.REPLACE_EXISTING);
        fileStream.close();

        File lib = tmpFilePath.toFile();
        lib.deleteOnExit();

        System.load(lib.getAbsolutePath());
    }

    public static native String AnalyzeRTPDump(ByteBuffer dump);
    public static native void TransformRTPDump(ByteBuffer dump, ByteBuffer transformed, String jsonCfg);

}

