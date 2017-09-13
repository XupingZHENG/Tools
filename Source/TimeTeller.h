#pragma once

#include <string>
#include <vector>
#include <map>

#include "Log.h"
#include "Misc.h"

class AutoTimerHandler
{
public:
    void record(const std::string& label, double t)
    {
        std::map<std::string, std::vector<double> >::iterator itr =
            mapLabelToValues.find(label);
        if (itr != mapLabelToValues.end())
        {
            itr->second.push_back(t);
        }
        else
        {
            std::vector<double> vals;
            vals.push_back(t);
            mapLabelToValues.insert(std::make_pair(label, vals));
        }
    }

    void clear()
    {
        mapLabelToValues.clear();
    }

    void report() const
    {
        LOG_INFO("Begin:");
        for (std::map<std::string, std::vector<double> >::const_iterator itr = mapLabelToValues.cbegin(), itrEnd = mapLabelToValues.cend();
            itr != itrEnd; ++itr)
        {
            const std::vector<double>& vals = itr->second;
            int count = (int)vals.size();
            double accum = 0;
            for (int i = 0; i < count; i++)
                accum += vals[i];
            LOG_INFO("{}: total {}, count {}, avg {}", itr->first, accum, count, count == 0 ? accum : accum / count);
        }
        LOG_INFO("End");
    }

private:
    std::map<std::string, std::vector<double> > mapLabelToValues;
};

extern AutoTimerHandler autoTimerHandler;

class AutoTimer
{
public:
    AutoTimer(const char* label_, AutoTimerHandler* handler_ = 0, bool printWhenDestruct_ = false) : 
        label(label_), handler(handler_), printWhenDestruct(printWhenDestruct_) {}
    
    AutoTimer(const std::string& label_, AutoTimerHandler* handler_ = 0, bool printWhenDestruct_ = false) :
        label(label_), handler(handler_), printWhenDestruct(printWhenDestruct_) {}

    ~AutoTimer()
    {
        t.end();
        if (handler)
            handler->record(label, t.elapsed());
        if (printWhenDestruct)
            LOG_INFO("Time elapsed in {}: {}", label, t.elapsed());
    }

private:
    Timer t;
    std::string label;
    AutoTimerHandler* handler;
    bool printWhenDestruct;
};