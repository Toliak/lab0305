#pragma once

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <forward_list>

struct BrokerFile
{
    using DirsType = std::list<std::string>;

    std::string filename;
    DirsType directories;

    [[nodiscard]] std::string directoriesToString() const
    {
        std::string result;
        for (const std::string &dir : directories) {
            result += dir + " ";
        }
        return result.substr(0, result.size() - 1);
    }
};

struct BrokerData
{
    struct Pair
    {
        std::string broker;
        size_t account;

        bool operator==(const Pair &pair) const
        {
            return pair.account == account && pair.broker == broker;
        }

        struct Hash
        {
            size_t operator()(const Pair &pair) const
            {
                return std::hash<size_t>{}(pair.account) ^ std::hash<std::string>{}(pair.broker);
            }
        };
    };

    struct Data
    {
        size_t files = 0;
        std::string lastDate;
    };
};

class BrokerResolver
{
public:
    using Collection = std::forward_list<BrokerFile>;
    using DataCollection = std::unordered_map<BrokerData::Pair, BrokerData::Data, BrokerData::Pair::Hash>;
    static boost::regex filenameChecker;

    // Рекурсивный резольвер симлинков
    static boost::filesystem::path resolveIfSymlink(const boost::filesystem::path &path);

    // Запомнить информацию о файле, если он удовлетворяет условию
    void saveFileIfCorrect(const boost::filesystem::path &path);

    // Запомнить все файлы, удовлетворяющие условиям
    void resolve(const boost::filesystem::path &path);

    // Получить результат
    [[nodiscard]] const Collection &getFileCollection() const
    {
        return fileCollection;
    }

    // Получить результат по данным
    [[nodiscard]] const DataCollection &getDataCollection() const
    {
        return dataCollection;
    }
private:
    BrokerFile::DirsType currentDirectories;            // Список директорий, относительно текущей
    Collection fileCollection;                          // Результат
    DataCollection dataCollection;
};