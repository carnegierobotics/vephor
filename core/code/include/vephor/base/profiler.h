/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "common.h"

#include <stack>
#include <chrono>

namespace vephor
{

class Profiler
{
public:
    using profiler_clock = std::chrono::steady_clock;
    using microseconds = std::chrono::microseconds;

    struct TrackedItem
    {
        float curr_sample_ms = 0;
        float accum_sample_ms = 0;
        float sample_sum_ms = 0;
        float sample_sum_sq_ms = 0;
        float sample_min_ms = 1e9;
        float sample_max_ms = -1e9;
        size_t samples = 0;
        std::string parent;
        std::string stack;
    };

    inline void pushSection(const std::string &name, const std::string &stack_name = "primary")
    {
        auto &active_items = active_item_stacks[stack_name];
        pushSectionInternal(name, active_items, stack_name);
    }

    inline void popSection(const std::string &stack_name = "primary")
    {
        auto &active_items = active_item_stacks[stack_name];
        popSectionInternal(active_items, stack_name);
    }

    inline void pushSectionAccumulate(const std::string &name, const std::string &stack_name = "primary")
    {
        auto &active_items = active_item_stacks[stack_name];
        pushSectionAccumulateInternal(name, active_items, stack_name);
    }

    inline void popSectionAccumulate(const std::string &stack_name = "primary")
    {
        auto &active_items = active_item_stacks[stack_name];
        popSectionAccumulateInternal(active_items, stack_name);
    }

    inline void finishSectionAccumulate(const std::string &name)
    {
        auto &tracked_item = tracked_items[name];
        auto dt_ms = tracked_item.accum_sample_ms;
        tracked_item.accum_sample_ms = 0;
        tracked_item.curr_sample_ms = dt_ms;
        tracked_item.sample_sum_ms += dt_ms;
        tracked_item.sample_sum_sq_ms += dt_ms * dt_ms;
        if (dt_ms < tracked_item.sample_min_ms)
        {
            tracked_item.sample_min_ms = dt_ms;
        }
        if (dt_ms > tracked_item.sample_max_ms)
        {
            tracked_item.sample_max_ms = dt_ms;
        }
        tracked_item.samples++;
    }

    inline const TrackedItem* getSection(const std::string &name) const
    {
        auto it = tracked_items.find(name);
        if (it == tracked_items.end())
            return NULL;
        return &it->second;
    }

    inline float getSectionTimeMS(const std::string &name) const
    {
        auto it = tracked_items.find(name);
        if (it == tracked_items.end())
            return 0.0f;

        float mean_ms = it->second.sample_sum_ms / static_cast<float>(it->second.samples);
        return mean_ms;
    }

    inline std::unordered_map<std::string, float> getSectionTimesMS() const
    {
        std::unordered_map<std::string, float> section_times_ms;

        for (const auto &[name, tracked_data] : tracked_items)
        {
            float mean_ms = tracked_data.sample_sum_ms / static_cast<float>(tracked_data.samples);
            section_times_ms[name] = mean_ms;
        }

        return section_times_ms;
    }
    inline void print()
    {
        auto section_times_ms = getSectionTimesMS();

        for (const auto &[name, time_ms] : section_times_ms)
        {
            v4print name, time_ms, "ms";
        }
    }
private:
    struct ActiveItem
    {
        std::string name;
        std::chrono::time_point<profiler_clock> start_time;
    };

    inline void pushSectionInternal(const std::string &name,
                                            std::stack<ActiveItem> &active_items,
                                            const std::string &stack_name)
    {
        const std::scoped_lock lock(item_lock);

        auto timing_start = profiler_clock::now();
        active_items.push({name, timing_start});
    }

    inline void popSectionInternal(std::stack<ActiveItem> &active_items, const std::string &stack_name)
    {
        const std::scoped_lock lock(item_lock);

        const auto &active_item = active_items.top();

        auto dt_ms = duration_cast<microseconds>(profiler_clock::now() - active_item.start_time).count() / 1e3;

        auto &tracked_item = tracked_items[active_item.name];
        tracked_item.curr_sample_ms = dt_ms;
        tracked_item.sample_sum_ms += dt_ms;
        tracked_item.sample_sum_sq_ms += dt_ms * dt_ms;
        if (dt_ms < tracked_item.sample_min_ms)
        {
            tracked_item.sample_min_ms = dt_ms;
        }
        if (dt_ms > tracked_item.sample_max_ms)
        {
            tracked_item.sample_max_ms = dt_ms;
        }
        tracked_item.samples++;

        active_items.pop();

        if (!active_items.empty())
        {
            tracked_item.parent = active_items.top().name;
        }

        tracked_item.stack = stack_name;
    }

    inline void pushSectionAccumulateInternal(const std::string &name,
                                            std::stack<ActiveItem> &active_items,
                                            const std::string &stack_name)
    {
        const std::scoped_lock lock(item_lock);

        auto timing_start = profiler_clock::now();
        active_items.push({name, timing_start});
    }

    inline void popSectionAccumulateInternal(std::stack<ActiveItem> &active_items, const std::string &stack_name)
    {
        const std::scoped_lock lock(item_lock);

        const auto &active_item = active_items.top();

        auto dt_ms = duration_cast<microseconds>(profiler_clock::now() - active_item.start_time).count() / 1e3;

        // ReSharper disable once CppUseStructuredBinding
        auto &tracked_item = tracked_items[active_item.name];
        tracked_item.accum_sample_ms += dt_ms;

        active_items.pop();

        if (!active_items.empty())
        {
            tracked_item.parent = active_items.top().name;
        }

        tracked_item.stack = stack_name;
    }

    std::unordered_map<std::string, std::stack<ActiveItem>> active_item_stacks;
    std::unordered_map<std::string, TrackedItem> tracked_items;
    std::mutex item_lock;
};

class ProfilerSection
{
public:
    ProfilerSection(
        Profiler* p_profiler, 
        const std::string &p_name, 
        const std::string &p_stack_name = "primary")
    : profiler(p_profiler), stack_name(p_stack_name)
    {
        p_profiler->pushSection(p_name, p_stack_name);
    }
    ~ProfilerSection()
    {
        profiler->popSection(stack_name);
    }
private:
    Profiler* profiler;
    std::string stack_name;
};

}