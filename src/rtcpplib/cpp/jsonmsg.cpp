#include "jsonmsg.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "basen.hpp"
#include "encryption.h"
#include <map>
#include <set>

using namespace rtcpp;
using namespace rapidjson;

// example
// {"totalPackets":5993,"streams":[{"pt":100,"uid":"1924507848142749796","ssrc":448084401}]}
std::string rtcpp::BuildJSONAnalyzeReport(const RTPStreamAnalyzer &stats) {
    Document d;
    d.SetObject();

    d.AddMember("totalPackets", stats.TotalPackets(), d.GetAllocator());
    Value streams(kArrayType);

    for (const auto &stream : stats.Info()) {
        Value streamEntry(kObjectType);

        streamEntry.AddMember("pt", stream.second.pt, d.GetAllocator());
        streamEntry.AddMember("ssrc", stream.first, d.GetAllocator());

        Value uid;
        uid.SetString(std::to_string(stream.first).c_str(), d.GetAllocator());
        streamEntry.AddMember("uid", uid, d.GetAllocator());

        streams.PushBack(streamEntry, d.GetAllocator());
    }

    d.AddMember("streams", streams, d.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}

void ParsePT(StreamTransformCfg& cfg, GenericObject<false, GenericValue<UTF8<char>>>& json) {
    const char* PT_ID = "pt";
    if (json.HasMember(PT_ID)) {
        if (json[PT_ID].IsString()) {
            if (json[PT_ID].GetStringLength() > 0) {
                int pt = std::atoi(json[PT_ID].GetString());
                if (pt >= 0 && pt <= 127) {
                    cfg.SetPT((uint8_t)pt);
                } else {
                    throw std::runtime_error("bad value for pt in json");
                }
            } else {
                // ignore field
            }
        } else {
            throw std::runtime_error("failed to parse pt in json");
        }
    }
}

void ParseEncryptionType(StreamTransformCfg& cfg, GenericObject<false, GenericValue<UTF8<char>>>& json) {
    const char* ENC_TYPE_ID = "encryptionMode";

    const struct {
        std::string typeStr;
        EncryptionType typeEnum;
    } typeMatchTable[] = {
            {"Default", EncryptionType::DefaultSRTP },
            {"Unencrypted_SRTP", EncryptionType::UnEncryptedSRTP },
            {"Unencrypted_SRTCP", EncryptionType::UnEncryptedSRTCP },
            {"Unauthenticated_SRTP", EncryptionType::UnAuthenticatedSRTP },
    };

    if (json.HasMember(ENC_TYPE_ID)) {
        if (json[ENC_TYPE_ID].IsString()) {
            if (json[ENC_TYPE_ID].GetStringLength() > 0) {
                std::string encMode(json[ENC_TYPE_ID].GetString());

                for (const auto& typeItem : typeMatchTable) {
                    if (encMode.find(typeItem.typeStr) != std::string::npos) {
                        cfg.SetEncType(typeItem.typeEnum);
                    }
                }

                if (!cfg.IsEncType()) {
                    throw std::runtime_error("failed to parse encryption type");
                }
            } else {
                // ignore
            }
        } else {
            throw std::runtime_error("encryption type isn't a string");
        }
    }

}

void ParseEncryptionMode(StreamTransformCfg& cfg, GenericObject<false, GenericValue<UTF8<char>>>& json) {
    const char* ENC_MODE_ID = "encryptionType";

    const struct {
        std::string cipherStr;
        EncryptionMode cipherEnum;
    } cipherMatchTable[] = {
        {"AES128_SHA1_32", EncryptionMode::AES128_SHA1_32 },
        {"AES128_SHA1_80", EncryptionMode::AES128_SHA1_80 },
        {"AES192_SHA1_32", EncryptionMode::AES192_SHA1_32 },
        {"AES192_SHA1_80", EncryptionMode::AES192_SHA1_80 },
        {"AES256_SHA1_32", EncryptionMode::AES256_SHA1_32 },
        {"AES256_SHA1_80", EncryptionMode::AES256_SHA1_80 }
    };

    if (json.HasMember(ENC_MODE_ID)) {
        if (json[ENC_MODE_ID].IsString()) {
            if (json[ENC_MODE_ID].GetStringLength() > 0) {
                std::string encMode(json[ENC_MODE_ID].GetString());

                for (const auto &cipherItem : cipherMatchTable) {
                    if (encMode.find(cipherItem.cipherStr) != std::string::npos) {
                        cfg.SetEncMode(cipherItem.cipherEnum);
                        break;
                    }
                }

                if (!cfg.IsEncMode()) {
                    throw std::runtime_error("unknown encryption mode in json");
                }
            } else {
                // ignore
            }
        } else {
            throw std::runtime_error("encryption type isn't a string");
        }
    }
}

void ParseEncryptionKey(StreamTransformCfg& cfg, GenericObject<false, GenericValue<UTF8<char>>>& json) {
    const char* KEY_ID = "encryptionKey";

    if (json.HasMember(KEY_ID)) {
        if (json[KEY_ID].IsString()) {
            if (json[KEY_ID].GetStringLength() > 0) {
                std::string key(json[KEY_ID].GetString());

                std::vector<uint8_t> keyBytes;
                bn::decode_b64(key.begin(), key.end(), std::back_inserter(keyBytes));

                cfg.SetEncKey(std::move(keyBytes));
            } else {
                // ignore
            }

        } else {
            throw std::runtime_error("key isn't a string in json");
        }
    }
}

void ParseStreamTransformCfg(StreamTransformCfg& cfg, GenericObject<false, GenericValue<UTF8<char>>>& json) {
    ParsePT(cfg, json);
    ParseEncryptionKey(cfg, json);
    ParseEncryptionType(cfg, json);
    ParseEncryptionMode(cfg, json);
}

uint32_t ParseSSRC(const std::string& ssrcStr) {
    long long ssrc = std::atoll(ssrcStr.c_str());
    if (ssrc < 0 || ssrc > UINT32_MAX) {
        throw std::runtime_error("failed to parse SSRC in json");
    }

    return (uint32_t)ssrc;
}

/*
{
    "448084401": {
        "pt":"43",
        "encryptionType":"AES128_SHA1_80",
        "encryptionMode":"Unencrypted_SRTCP",
        "encryptionKey":"dfafdsa"
    }
}
 */
void rtcpp::ParseJSONTransformCfg(const std::string& msg, TransformCfg& transformCfg) {
    transformCfg.clear();

    Document d;
    d.Parse(msg.c_str());

    if (d.HasParseError()) {
        throw std::runtime_error("Failed to parse json message");
    }

    for (auto m = d.MemberBegin(); m != d.MemberEnd(); ++m) {
        uint32_t ssrc = 0;
        StreamTransformCfg streamCfg;

        if (m->name.IsString()) {
            ssrc = ParseSSRC(m->name.GetString());
            streamCfg.SetSSRC(ssrc);
        } else {
            throw std::runtime_error("Expected string as stream transform name in json");
        }

        if (m->value.IsObject()) {
            auto o = m->value.GetObject();
            ParseStreamTransformCfg(streamCfg, o);
        } else {
            throw std::runtime_error("Expected object as stream transform description in json");
        }

        auto it = transformCfg.find(ssrc);
        if (it != transformCfg.end()) {
            throw std::runtime_error("Duplicate ssrc found, while parsing json");
        }

        transformCfg.insert({ssrc, streamCfg});
    }

}