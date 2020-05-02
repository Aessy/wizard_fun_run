#pragma once

#include "box.h"
#include "log.h"
#include <json11/json11.hpp>

#include <boost/algorithm/string.hpp>

namespace jeagle
{

struct PlayerAsset
{
    std::vector<Box<int>> casting_boxes;
    std::vector<Box<int>> walking_boxes;

    static PlayerAsset from(json11::Json const& json)
    {
        PlayerAsset out;

        for (auto const& val : json["frames"].array_items())
        {
            auto const filename = val["filename"].string_value();
            if (boost::algorithm::starts_with(filename, "casting_"))
            {
                LOG_DEBUG("Found {}", filename);
                Box<int> box;
                auto const& frame = val["frame"];
                box.upper_left = {frame["x"].int_value(), frame["y"].int_value() };
                box.size = {frame["w"].int_value(), frame["h"].int_value()};
                out.casting_boxes.emplace_back(box);
            }
            else if (boost::algorithm::starts_with(filename, "walking_"))
            {
                LOG_DEBUG("Found {}", filename);
                Box<int> box;
                auto const& frame = val["frame"];
                box.upper_left = {frame["x"].int_value(), frame["y"].int_value() };
                box.size = {frame["w"].int_value(), frame["h"].int_value()};
                out.walking_boxes.emplace_back(box);
            }
        }

        return out;
    }
};

inline PlayerAsset load_player_asset_data()
{
    auto const maybe_json = json11_from_file("assets/character.json");

    if (maybe_json)
    {
        return PlayerAsset::from(*maybe_json);
    }
    else
    {
        LOG_ERROR("Failed to load player assets");
    }

    return {};
}

struct Player
{
    PlayerAsset asset_data {};

    enum class State
    {
          idle
        , casting_swing
        //, CASTING_BACKSWING
    };

    struct InputState
    {
        enum class MoveDirection
        {
            up, left, right, down, none
        };

        bool cast_requested {};
        MoveDirection direction;


    } input_state;

    Vector2f position {900, 500};
    Vector2f speed {};

    State state = State::idle;
    float state_accumulated_time = 0;

    struct AnimationController
    {
        State state;
        int millisecond_per_frame;
        std::vector<Box<int>> frames_in_state;
        std::size_t current_frame;
        std::string texture_key;

    } animation_controller;
};

} // namespace jeagle