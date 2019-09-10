#include "BrokerResolver.h"

#include <boost/lexical_cast.hpp>

template<typename T>
void setIfMore(T &oldValue, const T &newValue)
{
    if (oldValue < newValue) {
        oldValue = newValue;
    }
}

boost::filesystem::path BrokerResolver::resolveIfSymlink(const boost::filesystem::path &path)
{
    if (!boost::filesystem::is_symlink(path)) {
        return path;
    }

    return resolveIfSymlink(
        boost::filesystem::read_symlink(path)
    );
}

void BrokerResolver::saveFileIfCorrect(const boost::filesystem::path &path)
{
    std::string filename = path.filename().string();
    boost::smatch result;
    if (!boost::regex_match(filename, result, filenameChecker)) {
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

        if (boost::filesystem::is_directory(resolveIfSymlink(entry))) {
            currentDirectories.push_back(
                boost::filesystem::path{entry}.stem().string()
            );
            resolve(entry);
        } else if (boost::filesystem::is_regular_file(entry)) {
            saveFileIfCorrect(entry);
        }
    }

    if (!currentDirectories.empty()) {
        currentDirectories.pop_back();
    }
}

boost::regex BrokerResolver::filenameChecker{
    R"((\w+)_(\d{8})_([12]\d{3}(0[1-9]|1[0-2])(0[1-9]|[12]\d|3[01])).(\w+))"
};
