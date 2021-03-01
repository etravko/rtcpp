#include "rtpdecrypt.h"
#include <functional>
#include <cstdio>
#include "encryption.h"
#include <stdexcept>

namespace rtcpp {

class RTPLibrary {
public:
    RTPLibrary() {
        if (srtp_init() != srtp_err_status_ok) {
            throw std::runtime_error("Failed to initialize srtp library");
        }
    }

    ~RTPLibrary() {
        if (srtp_shutdown() != srtp_err_status_ok) {
            printf("Failed to deinitialize srtp library");
        }
    }

    RTPLibrary(const RTPLibrary&) = delete;
    RTPLibrary(RTPLibrary&&) = delete;
    RTPLibrary& operator=(const RTPLibrary&) = delete;
    RTPLibrary& operator=(RTPLibrary&&) = delete;
};

RTPLibrary& InitLibrary() {
    static RTPLibrary lib;
    return lib;
}

static struct {
    EncryptionMode mode;
    EncryptionType type;
    std::function<void(srtp_crypto_policy_t*)> f;
} MATCH_TABLE[] = {
        { EncryptionMode::AES128_SHA1_32, EncryptionType::DefaultSRTP,         srtp_crypto_policy_set_aes_cm_128_hmac_sha1_32 },
        { EncryptionMode::AES128_SHA1_32, EncryptionType::UnAuthenticatedSRTP, srtp_crypto_policy_set_aes_cm_128_null_auth },
        { EncryptionMode::AES128_SHA1_32, EncryptionType::UnEncryptedSRTP,  nullptr }, // not supported by library
        { EncryptionMode::AES128_SHA1_32, EncryptionType::UnEncryptedSRTCP,    srtp_crypto_policy_set_aes_cm_128_hmac_sha1_32 },

        { EncryptionMode::AES128_SHA1_80, EncryptionType::DefaultSRTP,         srtp_crypto_policy_set_rtp_default },
        { EncryptionMode::AES128_SHA1_80, EncryptionType::UnAuthenticatedSRTP, srtp_crypto_policy_set_aes_cm_128_null_auth },
        { EncryptionMode::AES128_SHA1_80, EncryptionType::UnEncryptedSRTP,     srtp_crypto_policy_set_null_cipher_hmac_sha1_80 },
        { EncryptionMode::AES128_SHA1_80, EncryptionType::UnEncryptedSRTCP,    srtp_crypto_policy_set_rtp_default },

        { EncryptionMode::AES256_SHA1_32, EncryptionType::DefaultSRTP,         srtp_crypto_policy_set_aes_cm_256_hmac_sha1_32 },
        { EncryptionMode::AES256_SHA1_32, EncryptionType::UnAuthenticatedSRTP, srtp_crypto_policy_set_aes_cm_256_null_auth },
        { EncryptionMode::AES256_SHA1_32, EncryptionType::UnEncryptedSRTP,  nullptr }, // not supported by library
        { EncryptionMode::AES256_SHA1_32, EncryptionType::UnEncryptedSRTCP,    srtp_crypto_policy_set_aes_cm_256_hmac_sha1_32 },

        { EncryptionMode::AES256_SHA1_80, EncryptionType::DefaultSRTP,         srtp_crypto_policy_set_aes_cm_256_hmac_sha1_80 },
        { EncryptionMode::AES256_SHA1_80, EncryptionType::UnAuthenticatedSRTP, srtp_crypto_policy_set_aes_cm_128_null_auth },
        { EncryptionMode::AES256_SHA1_80, EncryptionType::UnEncryptedSRTP,     srtp_crypto_policy_set_null_cipher_hmac_sha1_80 },
        { EncryptionMode::AES256_SHA1_80, EncryptionType::UnEncryptedSRTCP,    srtp_crypto_policy_set_aes_cm_256_hmac_sha1_80 }
};


void RTPDecrypt::InitPolicyCrypto(const StreamTransformCfg& cfg, srtp_policy_t& policy) {
    if (cfg.IsEncMode() && cfg.IsEncType() && cfg.IsEncKey()) {
        for (auto& item : MATCH_TABLE) {

            if (item.type == cfg.GetEncType() && item.mode == cfg.GetEncMode()) {
                if (item.f != nullptr) {
                    item.f(&policy.rtp);
                    // TODO: do it properly for rtcp
                    item.f(&policy.rtcp);

                    if (cfg.GetEncKey().size() == policy.rtp.cipher_key_len) {
                        policy.key = const_cast<uint8_t*>(cfg.GetEncKey().data());
                    } else {
                        throw std::runtime_error("Bad encryption key length");
                    }
                } else {
                    throw std::runtime_error("Such decryption configuration isn't supported");
                }

                return;
            }
        }
        throw  std::runtime_error("Requested SRTP configuration isn't supported for session");

    } else {
        throw std::runtime_error("Bad SRTP configuration for session");
    }
}

void RTPDecrypt::InitPolicy(uint32_t ssrc, const StreamTransformCfg& cfg) {
    policies_.push_back({});
    srtp_policy_t& policy = policies_.back();

    // wire up previous policy
    if (policies_.size() > 1) {
        policies_[policies_.size() - 2].next = &policy;
    }

    InitPolicyCrypto(cfg, policy);

    policy.ssrc.type = ssrc_specific;
    policy.ssrc.value = ssrc;

    ssrcs_.insert(ssrc);
}

RTPDecrypt::RTPDecrypt(TransformCfg& cfg)
    : session_{} {
    InitLibrary();

    for (const auto& [ssrc, streamCfg] : cfg) {
        if (streamCfg.IsEncryptionEnabled()) {
            InitPolicy(ssrc, streamCfg);
        }
    }

    if (!policies_.empty()) {
        if (srtp_create(&session_, &policies_.front()) != srtp_err_status_ok) {
            throw std::runtime_error("Failed to initialize encryption session");
        }
    }
}

int RTPDecrypt::Decrypt(uint32_t ssrc, void* p, uint32_t len) {
    int outLen = (int)len;

    if (ssrcs_.find(ssrc) != ssrcs_.end()) {
        srtp_err_status_t status = srtp_unprotect(session_, p, &outLen);
        if (status != srtp_err_status_ok) {
            throw std::runtime_error("Failed to decrypt packet");
        }

        return outLen;
    }

    return outLen;
}


}