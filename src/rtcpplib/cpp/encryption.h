#ifndef RTCPP_ENCRYPTION_H
#define RTCPP_ENCRYPTION_H

namespace rtcpp {
    enum class EncryptionMode {
        AES128_SHA1_32,
        AES128_SHA1_80,
        AES192_SHA1_32,
        AES192_SHA1_80,
        AES256_SHA1_32,
        AES256_SHA1_80
    };

    enum class EncryptionType {
        DefaultSRTP = 0,
        UnEncryptedSRTP = 1,
        UnEncryptedSRTCP = 2,
        UnAuthenticatedSRTP = 4
    };
}

#endif //RTCPP_ENCRYPTION_H
