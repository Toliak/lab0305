#include "BrokerResolver.h"

#include <boost/lexical_cast.hpp>

std::string BrokerFile::directoriesToString() const
{
    std::string result;
    for (const std::string &dir : directories) {
        result += dir + " ";
    }
    return result.substr(0, result.size() - 1);
}

void BrokerResolver::saveFileIfCorrect(const boost::filesystem::path &path)
{
    std::string filename = path.filename().string();
    boost::smatch result;
    if (filename.find(".old") != std::string::npos || !boost::regex_match(filename, result, filenameChecker)) {
        return;
    }

    fileCollection.emplace_front(BrokerFile{path.filename().string(), currentDirectories});

    std::string broker = fileCollection.front().directoriesToString();
    auto account = boost::lexical_cast<size_t>(result[2]);
    std::string date = result[3];

    auto iterator = dataCollection.find(BrokerData::Pair{broker, account});
    if (iterator != dataCollection.cend()) {
        iterator->second.files++;
        setIfMore(iterator->second.lastDate, date);
    } else {
        dataCollection[{broker, account}] = {1, date};
    }
}

void BrokerResolver::resolve(const boost::filesystem::path &path)
{
    for (const boost::filesystem::directory_entry &entry : boost::filesystem::directory_iterator{path}) {
        boost::filesystem::path resolved = boost::filesystem::canonical(entry);

        if (boost::filesystem::is_directory(resolved)) {
            currentDirectories.push_back(
                boost::filesystem::path{entry}.stem().string()          // save origin name
            );
            resolve(resolved);
        } else if (boost::filesystem::is_regular_file(resolved)) {
            saveFileIfCorrect(entry);
        }
    }

    if (!currentDirectories.empty()) {
        currentDirectories.pop_back();
    }
}

boost::regex BrokerResolver::filenameChecker{
    R"((\w+)_(\d{8})_([12]\d{3}(0[1-9]|1[0-2])(0[1-9]|[12]\d|3[01])).txt)"
};
