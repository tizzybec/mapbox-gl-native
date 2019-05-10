#include <mbgl/util/filesystem.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/io.hpp>

#include "metadata.hpp"
#include "parser.hpp"

mbgl::optional<TestMetadata> TestMetadata::parseTestMetadata(const mbgl::filesystem::path& testPath) {
    TestMetadata metadata { testPath };

    if (!metadata.document.HasMember("metadata")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata': %s",
                           testPath.c_str());
        return metadata;
    }

    const mbgl::JSValue& metadataValue = metadata.document["metadata"];
    if (!metadataValue.HasMember("test")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata.test': %s",
                           testPath.c_str());
        return metadata;
    }

    const mbgl::JSValue& testValue = metadataValue["test"];

    if (testValue.HasMember("width")) {
        assert(testValue["width"].IsNumber());
        metadata.size.width = testValue["width"].GetInt();
    }

    if (testValue.HasMember("height")) {
        assert(testValue["height"].IsNumber());
        metadata.size.height = testValue["height"].GetInt();
    }

    if (testValue.HasMember("pixelRatio")) {
        assert(testValue["pixelRatio"].IsNumber());
        metadata.pixelRatio = testValue["pixelRatio"].GetDouble();
    }

    if (testValue.HasMember("allowed")) {
        assert(testValue["allowed"].IsNumber());
        metadata.allowed = testValue["allowed"].GetDouble();
    }

    if (testValue.HasMember("description")) {
        assert(testValue["description"].IsString());
        metadata.description = std::string{ testValue["description"].GetString(),
                                                testValue["description"].GetStringLength() };
    }

    if (testValue.HasMember("mapMode")) {
        assert(testValue["mapMode"].IsString());
        metadata.mapMode = testValue["mapMode"].GetString() == std::string("tile") ? mbgl::MapMode::Tile : mbgl::MapMode::Static;
    }

    if (testValue.HasMember("operations")) {
        assert(testValue["operations"].IsArray());
        metadata.hasOperations = true;
    }

    if (testValue.HasMember("debug")) {
        metadata.debug |= mbgl::MapDebugOptions::TileBorders;
    }

    if (testValue.HasMember("collisionDebug")) {
        metadata.debug |= mbgl::MapDebugOptions::Collision;
    }

    if (testValue.HasMember("showOverdrawInspector")) {
        metadata.debug |= mbgl::MapDebugOptions::Overdraw;
    }

    if (testValue.HasMember("crossSourceCollisions")) {
        assert(testValue["crossSourceCollisions"].IsBool());
        metadata.crossSourceCollisions = testValue["crossSourceCollisions"].GetBool();
    }

    if (testValue.HasMember("axonometric")) {
        assert(testValue["axonometric"].IsBool());
        metadata.axonometric = testValue["axonometric"].GetBool();
    }

    if (testValue.HasMember("skew")) {
        assert(testValue["skew"].IsArray());
        metadata.xSkew = testValue["skew"][0].GetDouble();
        metadata.ySkew = testValue["skew"][1].GetDouble();
    }

    // TODO: fadeDuration
    if (testValue.HasMember("fadeDuration")) {
        return {};
    }

    // TODO: addFakeCanvas
    if (testValue.HasMember("addFakeCanvas")) {
        return {};
    }

    return metadata;
}

TestMetadata::TestMetadata(const mbgl::filesystem::path& path_) : path(path_) {
    if (auto maybeJson = readJson(path.string())) {
        document = std::move(*maybeJson);
        localizeStyleURLs(document, document);
    }
}